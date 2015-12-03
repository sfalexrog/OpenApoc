#include "library/sp.h"
#include "library/sp.h"

#include "framework/framework.h"
#include "game/boot.h"
#include "game/resources/gamecore.h"
#include "game/city/city.h"
#include "framework/renderer.h"
#include "framework/renderer_interface.h"
#include "framework/sound_interface.h"
#include "framework/sound.h"
#include "framework/trace.h"

//#include <allegro5/allegro5.h>
#include <SDL.h>
#include <iostream>
#include <string>

// Use physfs to get prefs dir
#include <physfs.h>
#include <SDL_syswm.h>

#ifdef _WIN32
//#include <allegro5/allegro_windows.h>
#endif

#ifdef OPENAPOC_GLES

#include "framework/render/gles20/EGLContext.h"

#endif /*OPENAPOC_GLES*/

using namespace OpenApoc;

namespace
{

#ifndef DATA_DIRECTORY
#define DATA_DIRECTORY "./data"
#endif

#ifndef RENDERERS
#ifdef _WIN32
#pragma message("WARNING: Using default renderer list")
#else
#warning RENDERERS not set - using default list
#endif
#define RENDERERS "GL_3_0:GL_2_0"
#endif

static std::map<UString, UString> defaultConfig = {
#ifdef PANDORA
    {"Visual.ScreenWidth", "800"},
    {"Visual.ScreenHeight", "480"},
    {"Visual.FullScreen", "true"},
#else
    {"Visual.ScreenWidth", "1600"},
    {"Visual.ScreenHeight", "900"},
    {"Visual.FullScreen", "false"},
#endif
    {"Language", "en_gb"},
    {"GameRules", "XCOMAPOC.XML"},
    {"Resource.LocalDataDir", "/storage/emulated/0/openapoc/data"},
    {"Resource.SystemDataDir", "/storage/emulated/0/openapoc"},
    {"Resource.LocalCDPath", "/storage/emulated/0/openapoc/cd.iso"},
    {"Resource.SystemCDPath", "/storage/emulated/0/openapoc/cd.iso"},
    {"Visual.Renderers", RENDERERS},
    {"Audio.Backends", "SDLRaw:allegro:null"},
    {"Audio.GlobalGain", "20"},
    {"Audio.SampleGain", "20"},
    {"Audio.MusicGain", "20"},
    {"Framework.ThreadPoolSize", "0"},
};

std::map<UString, std::unique_ptr<OpenApoc::RendererFactory>> *registeredRenderers = nullptr;
std::map<UString, std::unique_ptr<OpenApoc::SoundBackendFactory>> *registeredSoundBackends =
    nullptr;
};

namespace OpenApoc
{

void registerRenderer(RendererFactory *factory, UString name)
{
	if (!registeredRenderers)
		registeredRenderers = new std::map<UString, std::unique_ptr<OpenApoc::RendererFactory>>();
	registeredRenderers->emplace(name, std::unique_ptr<RendererFactory>(factory));
}

void registerSoundBackend(SoundBackendFactory *factory, UString name)
{
	if (!registeredSoundBackends)
		registeredSoundBackends =
		    new std::map<UString, std::unique_ptr<OpenApoc::SoundBackendFactory>>();
	registeredSoundBackends->emplace(name, std::unique_ptr<SoundBackendFactory>(factory));
}

class JukeBoxImpl : public JukeBox
{
	Framework &fw;
	unsigned int position;
	std::vector<sp<MusicTrack>> trackList;
	JukeBox::PlayMode mode;

  public:
	JukeBoxImpl(Framework &fw) : fw(fw), mode(JukeBox::PlayMode::Loop) {}
	virtual ~JukeBoxImpl() { this->stop(); }

	virtual void play(std::vector<UString> tracks, JukeBox::PlayMode mode) override
	{
		this->trackList.clear();
		this->position = 0;
		this->mode = mode;
		for (auto &track : tracks)
		{
			auto musicTrack = fw.data->load_music(track);
			if (!musicTrack)
				LogError("Failed to load music track \"%s\" - skipping", track.c_str());
			else
				this->trackList.push_back(musicTrack);
		}
		this->progressTrack(this);
		this->fw.soundBackend->playMusic(progressTrack, this);
	}
	static void progressTrack(void *data)
	{
		JukeBoxImpl *jukebox = static_cast<JukeBoxImpl *>(data);
		if (jukebox->trackList.size() == 0)
		{
			LogWarning("Trying to play empty jukebox");
			return;
		}
		if (jukebox->position >= jukebox->trackList.size())
		{
			LogInfo("End of jukebox playlist");
			return;
		}
		LogInfo("Playing track %u (%s)", jukebox->position,
		        jukebox->trackList[jukebox->position]->getName().c_str());
		jukebox->fw.soundBackend->setTrack(jukebox->trackList[jukebox->position]);

		jukebox->position++;
		if (jukebox->mode == JukeBox::PlayMode::Loop)
			jukebox->position = jukebox->position % jukebox->trackList.size();
	}
	virtual void stop() override { fw.soundBackend->stopMusic(); }
};

class FrameworkPrivate
{
  private:
	friend class Framework;
	bool quitProgram;

	SDL_DisplayMode screenMode;
	SDL_Window *window;
	SDL_GLContext context;

	//ALLEGRO_EVENT_QUEUE *eventAllegro;
	std::list<Event *> eventQueue;
	//ALLEGRO_MUTEX *eventMutex; // TODO: Find an equivalent in SDL?

	StageStack ProgramStages;
	sp<Surface> defaultSurface;
};

Framework::Framework(const UString programName, const std::vector<UString> cmdline)
    : p(new FrameworkPrivate), programName(programName)
{
	TRACE_FN;
	LogInfo("Starting framework");
	PHYSFS_init(programName.c_str());
#ifdef ANDROID
	SDL_SetHint(SDL_HINT_ANDROID_SEPARATE_MOUSE_AND_TOUCH, "1");
#endif
	// Initialize subsystems separately?
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		LogError("Cannot init SDL2");
		LogError("SDL error: %s", SDL_GetError());
		p->quitProgram = true;
		return;
	}
/*
	if (!al_install_keyboard() || !al_install_mouse())
	{
		LogError(" Cannot init Allegro plugins");
		p->quitProgram = true;
		return;
	}
*/
	LogInfo("Loading config\n");
	p->quitProgram = false;
	UString settingsPath(PHYSFS_getPrefDir(PROGRAM_ORGANISATION, PROGRAM_NAME));
	settingsPath += "/settings.cfg";
	Settings.reset(new ConfigFile(settingsPath, defaultConfig));

	for (auto &option : cmdline)
	{
		auto splitString = option.split('=');
		if (splitString.size() != 2)
		{
			LogError("Failed to parse command line option \"%s\" - ignoring", option.c_str());
			continue;
		}
		else
		{
			LogInfo("Setting option \"%s\" to \"%s\" from command line", splitString[0].c_str(),
			        splitString[1].c_str());
			Settings->set(splitString[0], splitString[1]);
		}
	}

	int threadPoolSize = Settings->getInt("Framework.ThreadPoolSize");
	if (threadPoolSize > 0)
	{
		LogInfo("Set thread pool size to %d", threadPoolSize);
	}
	else if (std::thread::hardware_concurrency() != 0)
	{
		threadPoolSize = std::thread::hardware_concurrency();
		LogInfo("Set thread pool size to reported HW concurrency of %d", threadPoolSize);
	}
	else
	{
		threadPoolSize = 2;
		LogInfo("Failed to get HW concurrency, falling back to pool size %d", threadPoolSize);
	}

	this->threadPool.reset(new ThreadPool(threadPoolSize));

	std::vector<UString> resourcePaths;
	resourcePaths.push_back(Settings->getString("Resource.SystemCDPath"));
	resourcePaths.push_back(Settings->getString("Resource.LocalCDPath"));
	resourcePaths.push_back(Settings->getString("Resource.SystemDataDir"));
	resourcePaths.push_back(Settings->getString("Resource.LocalDataDir"));

	this->data.reset(new Data(resourcePaths));

	auto testFile = this->data->fs.open("MUSIC");
	if (!testFile)
	{
		LogError("Failed to open \"music\" from the CD - likely the cd couldn't be loaded or paths "
		         "are incorrect if using an extracted CD image");
	}

	auto testFile2 = this->data->fs.open("FileDoesntExist");
	if (testFile2)
	{
		LogError("Succeded in opening \"FileDoesntExist\" - either you have the weirdest filename "
		         "preferences or something is wrong");
	}
/*
	p->eventAllegro = al_create_event_queue();
	p->eventMutex = al_create_mutex_recursive();
*/
	srand(static_cast<unsigned int>(SDL_GetTicks()));

	Display_Initialise();
	Audio_Initialise();
	/*
	al_register_event_source(p->eventAllegro, al_get_display_event_source(p->screen));
	al_register_event_source(p->eventAllegro, al_get_keyboard_event_source());
	al_register_event_source(p->eventAllegro, al_get_mouse_event_source());
	// TODO: Check if it's safe to remove the #ifdef?
#ifdef ANDROID
	al_install_touch_input();
	al_register_event_source(p->eventAllegro, al_get_touch_input_event_source());
#endif
	 */

}

Framework::~Framework()
{
	TRACE_FN;
	LogInfo("Destroying framework");
	// Kill gamecore and program stages first, so any resources are cleaned before
	// allegro is de-inited
	state.reset();
	gamecore.reset();
	p->ProgramStages.Clear();
	LogInfo("Saving config");
	SaveSettings();

	LogInfo("Shutdown");
	Display_Shutdown();
	Audio_Shutdown();
	/*
	al_destroy_event_queue(p->eventAllegro);
	al_destroy_mutex(p->eventMutex);*/

	LogInfo("SDL shutdown");
	/*al_uninstall_mouse();
	al_uninstall_keyboard();

	al_uninstall_system();*/
	PHYSFS_deinit();
	SDL_Quit();
}

void Framework::Run()
{
	int frame = 0;
	TRACE_FN;
	LogInfo("Program loop started");

	p->ProgramStages.Push(std::make_shared<BootUp>(*this));

	this->renderer->setPalette(this->data->load_palette("xcom3/ufodata/PAL_06.DAT"));

	while (!p->quitProgram)
	{
		frame++;
		TraceObj obj{"Frame", {{"frame", Strings::FromInteger(frame)}}};

		ProcessEvents();

		StageCmd cmd;
		if (p->ProgramStages.IsEmpty())
		{
			break;
		}
		{
			TraceObj updateObj("Update");
			p->ProgramStages.Current()->Update(&cmd);
		}
		switch (cmd.cmd)
		{
			case StageCmd::Command::CONTINUE:
				break;
			case StageCmd::Command::REPLACE:
				p->ProgramStages.Pop();
				p->ProgramStages.Push(cmd.nextStage);
				break;
			case StageCmd::Command::PUSH:
				p->ProgramStages.Push(cmd.nextStage);
				break;
			case StageCmd::Command::POP:
				p->ProgramStages.Pop();
				break;
			case StageCmd::Command::QUIT:
				p->quitProgram = true;
				p->ProgramStages.Clear();
				break;
		}
		{
			TraceObj objBind{"RendererSurfaceBinding"};
			RendererSurfaceBinding b(*this->renderer, p->defaultSurface);
		}
		{
			TraceObj objClear{"clear"};
			this->renderer->clear();
		}
		if (!p->ProgramStages.IsEmpty())
		{
			TraceObj updateObj("Render");
			p->ProgramStages.Current()->Render();
			{
				TraceObj flipObj("Flip");
#ifndef OPENAPOC_GLES
				SDL_GL_SwapWindow(p->window);
#else
				#ifdef _MSC_VER
				GLContext::GetInstance()->Swap();
				#else
				SDL_GL_SwapWindow(p->window);
				#endif
#endif
			}
		}
	}
}

void Framework::ProcessEvents()
{
	TRACE_FN;
	if (p->ProgramStages.IsEmpty())
	{
		p->quitProgram = true;
		return;
	}

	// Convert Allegro events before we process
	// TODO: Consider threading the translation
		TranslateSDLEvents();

	//al_lock_mutex(p->eventMutex);

	while (p->eventQueue.size() > 0 && !p->ProgramStages.IsEmpty())
	{
		Event *e;
		e = p->eventQueue.front();
		p->eventQueue.pop_front();
		if (!e)
		{
			LogError("Invalid event on queue");
			continue;
		}
		switch (e->Type)
		{
			case EVENT_WINDOW_CLOSED:
				delete e;
				//al_unlock_mutex(p->eventMutex);
				ShutdownFramework();
				return;
				break;
			default:
				p->ProgramStages.Current()->EventOccurred(e);
				break;
		}
		delete e;
	}

	//al_unlock_mutex(p->eventMutex);
}

void Framework::PushEvent(Event *e)
{
	//al_lock_mutex(p->eventMutex);
	p->eventQueue.push_back(e);
	//al_unlock_mutex(p->eventMutex);
}

void Framework::TranslateSDLEvents()
{
	SDL_Event e;
	Event *fwE;

	// FIXME: That's not the right way to figure out the primary finger!
	int primaryFingerID = -1;
	SDL_Finger *primaryFinger = SDL_GetTouchFinger(SDL_GetTouchDevice(0), 0);
	if (primaryFinger)
	{
		primaryFingerID = primaryFinger->id;
	}

	while (SDL_PollEvent(&e))
	{
		switch (e.type)
		{
			case SDL_QUIT:
				fwE = new Event();
				fwE->Type = EVENT_WINDOW_CLOSED;
				PushEvent(fwE);
				break;
			case SDL_JOYDEVICEADDED:
			case SDL_JOYDEVICEREMOVED:
				//al_reconfigure_joysticks();
				// FIXME: Do nothing?
				break;
				// FIXME: SDL does not have native timer events
			/*case ALLEGRO_EVENT_TIMER:
				fwE = new Event();
				fwE->Type = EVENT_TIMER_TICK;
				fwE->Data.Timer.TimerObject = e.timer.source;
				PushEvent(fwE);
				break;*/
			case SDL_KEYDOWN:
				fwE = new Event();
				fwE->Type = EVENT_KEY_DOWN;
				fwE->Data.Keyboard.KeyCode = e.key.keysym.sym;
				fwE->Data.Keyboard.UniChar = e.key.keysym.sym;
				fwE->Data.Keyboard.Modifiers = e.key.keysym.mod;
				PushEvent(fwE);
				break;
			case SDL_KEYUP:
				fwE = new Event();
				fwE->Type = EVENT_KEY_UP;
				fwE->Data.Keyboard.KeyCode = e.key.keysym.sym;
				fwE->Data.Keyboard.UniChar = e.key.keysym.sym;
				fwE->Data.Keyboard.Modifiers = e.key.keysym.mod;
				PushEvent(fwE);
				break;
				// FIXME: handle SDL_TEXTINPUT?
				/*
			case ALLEGRO_EVENT_KEY_CHAR:
				fwE = new Event();
				fwE->Type = EVENT_KEY_PRESS;
				fwE->Data.Keyboard.KeyCode = e.keyboard.keycode;
				fwE->Data.Keyboard.UniChar = e.keyboard.unichar;
				fwE->Data.Keyboard.Modifiers = e.keyboard.modifiers;
				PushEvent(fwE);
				break;*/
			case SDL_MOUSEMOTION:
				fwE = new Event();
				fwE->Type = EVENT_MOUSE_MOVE;
				fwE->Data.Mouse.X = e.motion.x;
				fwE->Data.Mouse.Y = e.motion.y;
				fwE->Data.Mouse.DeltaX = e.motion.xrel;
				fwE->Data.Mouse.DeltaY = e.motion.yrel;
				fwE->Data.Mouse.WheelVertical = 0;   // These should be handled
				fwE->Data.Mouse.WheelHorizontal = 0; // in a separate event
				fwE->Data.Mouse.Button = e.motion.state;
				PushEvent(fwE);
				break;
			case SDL_MOUSEWHEEL:
				// FIXME: Check these values for sanity
				fwE = new Event();
				fwE->Type = EVENT_MOUSE_MOVE;
				// Since I'm using some variables that are not used anywhere else,
				// this code should be in its own small block.
				{
					int mx, my;
					fwE->Data.Mouse.Button = SDL_GetMouseState(&mx, &my);
					fwE->Data.Mouse.X =	mx;
					fwE->Data.Mouse.Y = my;
					fwE->Data.Mouse.DeltaX = 0; // FIXME: This might cause problems?
					fwE->Data.Mouse.DeltaY = 0;
					fwE->Data.Mouse.WheelVertical = e.wheel.y;
					fwE->Data.Mouse.WheelHorizontal = e.wheel.x;
				}
				PushEvent(fwE);
				break;
			case SDL_MOUSEBUTTONDOWN:
				fwE = new Event();
				fwE->Type = EVENT_MOUSE_DOWN;
				fwE->Data.Mouse.X = e.button.x;
				fwE->Data.Mouse.Y = e.button.y;
				fwE->Data.Mouse.DeltaX = 0; // FIXME: This might cause problems?
				fwE->Data.Mouse.DeltaY = 0;
				fwE->Data.Mouse.WheelVertical = 0;
				fwE->Data.Mouse.WheelHorizontal = 0;
				fwE->Data.Mouse.Button = SDL_BUTTON(e.button.button);
				PushEvent(fwE);
				break;
			case SDL_MOUSEBUTTONUP:
				fwE = new Event();
				fwE->Type = EVENT_MOUSE_UP;
				fwE->Data.Mouse.X = e.button.x;
				fwE->Data.Mouse.Y = e.button.y;
				fwE->Data.Mouse.DeltaX = 0; // FIXME: This might cause problems?
				fwE->Data.Mouse.DeltaY = 0;
				fwE->Data.Mouse.WheelVertical = 0;
				fwE->Data.Mouse.WheelHorizontal = 0;
				fwE->Data.Mouse.Button = SDL_BUTTON(e.button.button);
				PushEvent(fwE);
				break;
			case SDL_FINGERDOWN:
				fwE = new Event();
				fwE->Type = EVENT_FINGER_DOWN;
				fwE->Data.Finger.X = e.tfinger.x * Display_GetWidth();
				fwE->Data.Finger.Y = e.tfinger.y * Display_GetHeight();
				fwE->Data.Finger.DeltaX = e.tfinger.dx * Display_GetWidth();
				fwE->Data.Finger.DeltaY = e.tfinger.dy * Display_GetHeight();
				fwE->Data.Finger.Id = e.tfinger.fingerId;
				fwE->Data.Finger.IsPrimary = e.tfinger.fingerId == primaryFingerID; // FIXME: Try to remember the ID of the first touching finger!
				PushEvent(fwE);
				break;
			case SDL_FINGERUP:
				fwE = new Event();
				fwE->Type = EVENT_FINGER_UP;
				fwE->Data.Finger.X = e.tfinger.x * Display_GetWidth();
				fwE->Data.Finger.Y = e.tfinger.y * Display_GetHeight();
				fwE->Data.Finger.DeltaX = e.tfinger.dx * Display_GetWidth();
				fwE->Data.Finger.DeltaY = e.tfinger.dy * Display_GetHeight();
				fwE->Data.Finger.Id = e.tfinger.fingerId;
				fwE->Data.Finger.IsPrimary = e.tfinger.fingerId == primaryFingerID; // FIXME: Try to remember the ID of the first touching finger!
				PushEvent(fwE);
				break;
			case SDL_FINGERMOTION:
				fwE = new Event();
				fwE->Type = EVENT_FINGER_MOVE;
				fwE->Data.Finger.X = e.tfinger.x * Display_GetWidth();
				fwE->Data.Finger.Y = e.tfinger.y * Display_GetHeight();
				fwE->Data.Finger.DeltaX = e.tfinger.dx * Display_GetWidth();
				fwE->Data.Finger.DeltaY = e.tfinger.dy * Display_GetHeight();
				fwE->Data.Finger.Id = e.tfinger.fingerId;
				fwE->Data.Finger.IsPrimary = e.tfinger.fingerId == primaryFingerID; // FIXME: Try to remember the ID of the first touching finger!
				PushEvent(fwE);
				break;
			case SDL_WINDOWEVENT:
				// Window events get special treatment
				switch (e.window.event)
				{
					case SDL_WINDOWEVENT_RESIZED:
						// FIXME: Do we care about SDL_WINDOWEVENT_SIZE_CHANGED?
						fwE = new Event();
						fwE->Type = EVENT_WINDOW_RESIZE;
						fwE->Data.Display.X = 0;
						fwE->Data.Display.Y = 0;
						fwE->Data.Display.Width = e.window.data1;
						fwE->Data.Display.Height = e.window.data2;
						fwE->Data.Display.Active = true;
						break;
					case SDL_WINDOWEVENT_HIDDEN:
					case SDL_WINDOWEVENT_MINIMIZED:
					case SDL_WINDOWEVENT_LEAVE:
						// FIXME: Check if we should react this way for each of those events
						// FIXME: Check if we're missing some of the events
						fwE = new Event();
						fwE->Type = EVENT_WINDOW_DEACTIVATE;
						fwE->Data.Display.X = 0;
						fwE->Data.Display.Y = 0;
						// FIXME: Is this even necessary?
						SDL_GetWindowSize(p->window,
							&(fwE->Data.Display.Width),
							&(fwE->Data.Display.Height));
						fwE->Data.Display.Active = false;
						PushEvent(fwE);
						break;
					case SDL_WINDOWEVENT_SHOWN:
					case SDL_WINDOWEVENT_EXPOSED:
					case SDL_WINDOWEVENT_RESTORED:
					case SDL_WINDOWEVENT_ENTER:
						// FIXME: Should we handle all these events as "aaand we're back" events?
						fwE = new Event();
						fwE->Type = EVENT_WINDOW_ACTIVATE;
						fwE->Data.Display.X = 0;
						fwE->Data.Display.Y = 0;
						// FIXME: Is this even necessary?
						SDL_GetWindowSize(p->window,
										  &(fwE->Data.Display.Width),
										  &(fwE->Data.Display.Height));
						fwE->Data.Display.Active = false;
						PushEvent(fwE);
						break;
					case SDL_WINDOWEVENT_CLOSE:
						// Closing a window will be a "quit" event.
						e.type = SDL_QUIT;
						SDL_PushEvent(&e);
						break;
				}
				break;
			/*case ALLEGRO_EVENT_DISPLAY_RESIZE:
				fwE = new Event();
				fwE->Type = EVENT_WINDOW_RESIZE;
				fwE->Data.Display.X = 0;
				fwE->Data.Display.Y = 0;
				fwE->Data.Display.Width = al_get_display_width(p->screen);
				fwE->Data.Display.Height = al_get_display_height(p->screen);
				fwE->Data.Display.Active = true;
				PushEvent(fwE);
				break;
			case ALLEGRO_EVENT_DISPLAY_SWITCH_IN:
				fwE = new Event();
				fwE->Type = EVENT_WINDOW_ACTIVATE;
				fwE->Data.Display.X = 0;
				fwE->Data.Display.Y = 0;
				fwE->Data.Display.Width = al_get_display_width(p->screen);
				fwE->Data.Display.Height = al_get_display_height(p->screen);
				fwE->Data.Display.Active = true;
				PushEvent(fwE);
				break;
			case ALLEGRO_EVENT_DISPLAY_SWITCH_OUT:
				fwE = new Event();
				fwE->Type = EVENT_WINDOW_DEACTIVATE;
				fwE->Data.Display.X = 0;
				fwE->Data.Display.Y = 0;
				fwE->Data.Display.Width = al_get_display_width(p->screen);
				fwE->Data.Display.Height = al_get_display_height(p->screen);
				fwE->Data.Display.Active = false;
				PushEvent(fwE);
				break;*/
				// TODO: Actually register touches as touches, not as mouse events.
				/*
			case ALLEGRO_EVENT_TOUCH_BEGIN:
				fwE = new Event();
				fwE->Type = EVENT_FINGER_DOWN;
				fwE->Data.Finger.Id = e.touch.id;
				fwE->Data.Finger.IsPrimary = e.touch.primary;
				fwE->Data.Finger.X = e.touch.x;
				fwE->Data.Finger.Y = e.touch.y;
				fwE->Data.Finger.DeltaX = e.touch.dx;
				fwE->Data.Finger.DeltaY = e.touch.dy;
				PushEvent(fwE);
				break;
			case ALLEGRO_EVENT_TOUCH_END:
				fwE = new Event();
				fwE->Type = EVENT_FINGER_UP;
				fwE->Data.Finger.Id = e.touch.id;
				fwE->Data.Finger.IsPrimary = e.touch.primary;
				fwE->Data.Finger.X = e.touch.x;
				fwE->Data.Finger.Y = e.touch.y;
				fwE->Data.Finger.DeltaX = e.touch.dx;
				fwE->Data.Finger.DeltaY = e.touch.dy;
				PushEvent(fwE);
				break;
			case ALLEGRO_EVENT_TOUCH_MOVE:
				fwE = new Event();
				fwE->Type = EVENT_FINGER_MOVE;
				fwE->Data.Finger.Id = e.touch.id;
				fwE->Data.Finger.IsPrimary = e.touch.primary;
				fwE->Data.Finger.X = e.touch.x;
				fwE->Data.Finger.Y = e.touch.y;
				fwE->Data.Finger.DeltaX = e.touch.dx;
				fwE->Data.Finger.DeltaY = e.touch.dy;
				PushEvent(fwE);
				break;*/
			default:
				fwE = new Event();
				fwE->Type = EVENT_UNDEFINED;
				PushEvent(fwE);
				break;
		}
	}
}

void Framework::ShutdownFramework()
{
	LogInfo("Shutdown framework");
	p->ProgramStages.Clear();
	p->quitProgram = true;
}

void Framework::SaveSettings()
{
	// Just to keep the filename consistant
	UString settingsPath(PHYSFS_getPrefDir(PROGRAM_ORGANISATION, PROGRAM_NAME));
	settingsPath += "/settings.cfg";
	Settings->save(settingsPath);
}

void Framework::Display_Initialise()
{
	TRACE_FN;
	LogInfo("Init display");
	int display_flags = SDL_WINDOW_OPENGL;
#ifdef OPENAPOC_GLES
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
#else
#ifdef SDL_OPENGL_CORE
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
#endif

#if ALLEGRO_VERSION > 5 || (ALLEGRO_VERSION == 5 && ALLEGRO_SUB_VERSION >= 1)
	display_flags |= ALLEGRO_OPENGL_3_0 | ALLEGRO_PROGRAMMABLE_PIPELINE;
#endif
#endif
	// Request context version 3.0
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
	// Request RGBA5551 - change if needed
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 1);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);


	int scrW = Settings->getInt("Visual.ScreenWidth");
	int scrH = Settings->getInt("Visual.ScreenHeight");
	bool scrFS = Settings->getBool("Visual.FullScreen");

	if (scrW < 640 || scrH < 480)
	{
		LogError(
		    "Requested display size of {%d,%d} is lower than {640,480} and probably won't work",
		    scrW, scrH);
	}

	if (scrFS)
	{
		display_flags |= SDL_WINDOW_FULLSCREEN;
	}

	p->window = SDL_CreateWindow("OpenApoc",
								 SDL_WINDOWPOS_UNDEFINED,
								 SDL_WINDOWPOS_UNDEFINED,
								 scrW,
								 scrH,
								 display_flags);

	if (!p->window)
	{
		LogError("Failed to create window");
		;
		exit(1);
	}

	// FIXME: This is very, very Windows-specific.
	// Then again, GLContext should not be needed in the future (allegro should take care of EGL)
#ifdef _MSC_VER
	// FIXME: Retrieve the window's hWnd and pass it to GLContext
	GLContext::GetInstance()->Init(al_get_win_window_handle(p->screen));
#else
	p->context = SDL_GL_CreateContext(p->window);
	if (!p->context)
	{
		LogError("Could not create GL context! [SDLError: %s]", SDL_GetError());
		SDL_DestroyWindow(p->window);
		exit(1);
	}
	// Output the context parameters
	LogInfo("Created OpenGL context, parameters:");
	int value;
	SDL_GL_GetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, &value);
	LogInfo("  Context profile: %s", (value == SDL_GL_CONTEXT_PROFILE_ES) ? "ES" : "Core");
	int ctxMajor, ctxMinor;
	SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &ctxMajor);
	SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &ctxMinor);
	LogInfo("  Context version: %d.%d", ctxMajor, ctxMinor);
	int bitsRed, bitsGreen, bitsBlue, bitsAlpha;
	SDL_GL_GetAttribute(SDL_GL_RED_SIZE, &bitsRed);
	SDL_GL_GetAttribute(SDL_GL_GREEN_SIZE, &bitsGreen);
	SDL_GL_GetAttribute(SDL_GL_BLUE_SIZE, &bitsBlue);
	SDL_GL_GetAttribute(SDL_GL_ALPHA_SIZE, &bitsAlpha);
	LogInfo("  RGBA bits: %d-%d-%d-%d", bitsRed, bitsGreen, bitsBlue, bitsAlpha);
#endif
	// FIXME: Are we _really_ using SDL/Allegro blending functions?
	//al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);

	//al_hide_mouse_cursor(p->screen);

	SDL_ShowCursor(SDL_DISABLE);

	for (auto &rendererName : Settings->getString("Visual.Renderers").split(':'))
	{
		auto rendererFactory = registeredRenderers->find(rendererName);
		if (rendererFactory == registeredRenderers->end())
		{
			LogInfo("Renderer \"%s\" not in supported list", rendererName.c_str());
			continue;
		}
		Renderer *r = rendererFactory->second->create();
		if (!r)
		{
			LogInfo("Renderer \"%s\" failed to init", rendererName.c_str());
			continue;
		}
		this->renderer.reset(r);
		LogInfo("Using renderer: %s", this->renderer->getName().c_str());
		break;
	}
	if (!this->renderer)
	{
		LogError("No functional renderer found");
		abort();
	}
	this->p->defaultSurface = this->renderer->getDefaultSurface();
}

void Framework::Display_Shutdown()
{
	TRACE_FN;
	LogInfo("Shutdown Display");
	p->defaultSurface.reset();
	renderer.reset();

	//al_unregister_event_source(p->eventAllegro, al_get_display_event_source(p->screen));
	//al_destroy_display(p->screen);
	SDL_DestroyWindow(p->window);
}

int Framework::Display_GetWidth()
{
	int width;
	SDL_GetWindowSize(p->window, &width, 0);
	return width;
}

int Framework::Display_GetHeight()
{
	int height;
	SDL_GetWindowSize(p->window, 0, &height);
	return height;
}

Vec2<int> Framework::Display_GetSize()
{
	return Vec2<int>(this->Display_GetWidth(), this->Display_GetHeight());
}

void Framework::Display_SetTitle(UString NewTitle)
{
	// FIXME: SDL only sets window title.
	//al_set_app_name(NewTitle.c_str());
	SDL_SetWindowTitle(p->window, NewTitle.c_str());
	//al_set_window_title(p->screen, NewTitle.c_str());
}

void Framework::Display_SetIcon()
{
#ifdef _WIN32
	SDL_SysWMInfo info;
	SDL_VERSION(&info.version);
	SDL_GetWindowWMInfo(p->window, &info);
	HINSTANCE handle = GetModuleHandle(NULL);
	HICON icon = LoadIcon(handle, L"ALLEGRO_ICON");
	//HWND hwnd = al_get_win_window_handle(p->screen);
	HWND hwnd = info.info.win.window;
	SetClassLongPtr(hwnd, GCLP_HICON, (LONG_PTR)icon);
#else
// TODO: Figure out how this works
// al_set_display_icon(p->screen, ...);
	// NOTE: this should be a call to SDL_SetWindowIcon(p->window, icon);
	// where icon should be a SDL_Surface* with our desired icon.
#endif
}

void Framework::Audio_Initialise()
{
	TRACE_FN;
	LogInfo("Initialise Audio");

	for (auto &soundBackendName : Settings->getString("Audio.Backends").split(':'))
	{
		auto backendFactory = registeredSoundBackends->find(soundBackendName);
		if (backendFactory == registeredSoundBackends->end())
		{
			LogInfo("Sound backend %s not in supported list", soundBackendName.c_str());
			continue;
		}
		SoundBackend *backend = backendFactory->second->create();
		if (!backend)
		{
			LogInfo("Sound backend %s failed to init", soundBackendName.c_str());
			continue;
		}
		this->soundBackend.reset(backend);
		LogInfo("Using sound backend %s", soundBackendName.c_str());
		break;
	}
	if (!this->soundBackend)
	{
		LogError("No functional sound backend found");
	}
	this->jukebox.reset(new JukeBoxImpl(*this));

	/* Setup initial gain */
	this->soundBackend->setGain(SoundBackend::Gain::Global,
	                            static_cast<float>(this->Settings->getInt("Audio.GlobalGain")) /
	                                20.0f);
	this->soundBackend->setGain(SoundBackend::Gain::Music,
	                            static_cast<float>(this->Settings->getInt("Audio.MusicGain")) /
	                                20.0f);
	this->soundBackend->setGain(SoundBackend::Gain::Sample,
	                            static_cast<float>(this->Settings->getInt("Audio.SampleGain")) /
	                                20.0f);
}

void Framework::Audio_Shutdown()
{
	TRACE_FN;
	LogInfo("Shutdown Audio");
	this->jukebox.reset();
	this->soundBackend.reset();
}

sp<Stage> Framework::Stage_GetPrevious() { return p->ProgramStages.Previous(); }

sp<Stage> Framework::Stage_GetPrevious(sp<Stage> From) { return p->ProgramStages.Previous(From); }

}; // namespace OpenApoc
