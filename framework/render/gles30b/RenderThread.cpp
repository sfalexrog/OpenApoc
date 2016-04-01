#include "RenderThread.h"
#include "framework/image.h"
#include "framework/renderer.h"
#include "framework/logger.h"

#include "gles3loader.h"

#include <boost/lockfree/spsc_queue.hpp>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <SDL.h>

#include "RendererImpl.h"

#include "framework/trace.h"

namespace OpenApoc
{
	// Command for the renderer - basically carbon copy of JonnyH's 
	// RendererCmd from his ThreadedRenderer branch
	class RenderCmd
	{
	private:
		
	public:

		RenderCmd() {}

		enum Cmd {
			Clear,
			SetPalette,
			Draw,
			DrawRotated,
			DrawScaled,
			DrawTinted,
			DrawFilledRect,
			DrawRect,
			DrawLine,
			Flush,
			Flip,
			SetSurface,
			Quit
		} cmd;

		Colour c;
		Colour tint;
		sp<Palette> palette;
		sp<Image> image;
		sp<Surface> surface;
		// FIXME: Should probably have a union of sorts of all these arguments?
		Vec2<float> p1;
		Vec2<float> position;
		Vec2<float> p2;
		Vec2<float> center;
		Vec2<float> size;
		float thickness;
		float angle;
		Renderer::Scaler scaler;

		static RenderCmd clear(Colour c)
		{
			RenderCmd cmd;
			cmd.cmd = Cmd::Clear;
			cmd.c = c;
			return cmd;
		}

		static RenderCmd setPalette(sp<Palette> p)
		{
			RenderCmd cmd;
			cmd.cmd = Cmd::SetPalette;
			cmd.palette = p;
			return cmd;
		}

		static RenderCmd draw(sp<Image> i, Vec2<float> position)
		{
			RenderCmd cmd;
			cmd.cmd = Cmd::Draw;
			cmd.image = i;
			cmd.position = position;
			return cmd;
		}

		static RenderCmd drawRotated(sp<Image> i, Vec2<float> center, Vec2<float> position, float angle)
		{
			RenderCmd cmd;
			cmd.cmd = Cmd::DrawRotated;
			cmd.image = i;
			cmd.center = center;
			cmd.position = position;
			cmd.angle = angle;
			return cmd;
		}

		static RenderCmd drawScaled(sp<Image> i, Vec2<float> position, Vec2<float> size, Renderer::Scaler scaler)
		{
			RenderCmd cmd;
			cmd.cmd = Cmd::DrawScaled;
			cmd.image = i;
			cmd.position = position;
			cmd.size = size;
			cmd.scaler = scaler;
			return cmd;
		}
		static RenderCmd drawFilledRect(Vec2<float> position, Vec2<float> size, Colour c)
		{
			RenderCmd cmd;
			cmd.cmd = Cmd::DrawFilledRect;
			cmd.position = position;
			cmd.size = size;
			cmd.c = c;
			return cmd;

		}
		static RenderCmd drawTinted(sp<Image> i, Vec2<float> position, Colour tint)
		{
			RenderCmd cmd;
			cmd.cmd = Cmd::DrawTinted;
			cmd.image = i;
			cmd.position = position;
			cmd.tint = tint;
			return cmd;

		}
		static RenderCmd drawRect(Vec2<float> position, Vec2<float> size, Colour c, float thickness)
		{
			RenderCmd cmd;
			cmd.cmd = Cmd::DrawRect;
			cmd.position = position;
			cmd.size = size;
			cmd.c = c;
			cmd.thickness = thickness;
			return cmd;
		}
		static RenderCmd drawLine(Vec2<float> p1, Vec2<float> p2, Colour c, float thickness)
		{
			RenderCmd cmd;
			cmd.cmd = Cmd::DrawLine;
			cmd.p1 = p1;
			cmd.p2 = p2;
			cmd.c = c;
			cmd.thickness = thickness;
			return cmd;
		}
		static RenderCmd flush()
		{
			RenderCmd cmd;
			cmd.cmd = Cmd::Flush;
			return cmd;
		}
		static RenderCmd flip()
		{
			RenderCmd cmd;
			cmd.cmd = Cmd::Flip;
			return cmd;
		}
		static RenderCmd setSurface(sp<Surface> s)
		{
			RenderCmd cmd;
			cmd.cmd = Cmd::SetSurface;
			cmd.surface = s;
			return cmd;
		}

		static RenderCmd quit()
		{
			RenderCmd cmd;
			cmd.cmd = Cmd::Quit;
			return cmd;
		}
	};

	// We can't have this queue be a part of the class interface
	// Have at most 65536 enqueued draw calls - *should* be enough for one frame
	static boost::lockfree::spsc_queue<RenderCmd, boost::lockfree::capacity<65536> > renderQueue;

	void RenderThread::RenderFunction(SDL_Window *window, SDL_GLContext context)
	{
		Trace::setThreadName("Render");
		{
			std::unique_lock<std::mutex> lk(readyMutex);
			LogInfo("Rendering thread started");
			// Grab the context and attach it to the current thread
			// SDL_GL_MakeCurrent takes care of the thread-local setup
			LogInfo("Grabbing OpenGL context...");
			SDL_GL_MakeCurrent(window, context);
			// Load 
			LogInfo("Loading OpenGL functions...");
			auto functionsLoaded = gl::sys::LoadFunctions();
			if (!functionsLoaded)
			{
				LogError("Failed to load GL functions");
			}
			LogInfo("  GL vendor: %s", gl::GetString(gl::VENDOR));
			LogInfo("  GL version: %s", gl::GetString(gl::VERSION));
			LogInfo("  GL renderer: %s", gl::GetString(gl::RENDERER));
			LogInfo("  GLSL version: %s", gl::GetString(gl::SHADING_LANGUAGE_VERSION));

			std::unique_ptr<TraceObj> statusObj(new TraceObj("rendering"));

			LogInfo("Loading Renderer implementation...");
			renderer.reset(new RendererImpl);
			renderer->window = window;
			renderer->context = context;

			currentSurface = defaultSurface = renderer->defaultSurface;


			LogInfo("Showtime");
			ready = true;
			lk.unlock();
		}
		readyCV.notify_all();
		RenderCmd cmd;
		while (true)
		{
			while (renderQueue.pop(cmd))
			{
				switch (cmd.cmd)
				{
				case RenderCmd::Cmd::Quit:
				{
					LogInfo("Stopping rendering thread");
					return;
				}
				case RenderCmd::Cmd::Clear:
					renderer->clear(cmd.c);
					break;
				case RenderCmd::Cmd::SetPalette:
					renderer->setPalette(cmd.palette);
					break;
				case RenderCmd::Cmd::Draw:
					renderer->draw(cmd.image, cmd.position);
					break;
				case RenderCmd::Cmd::DrawRotated:
					renderer->drawRotated(cmd.image, cmd.center, cmd.position, cmd.angle);
					break;
				case RenderCmd::Cmd::DrawScaled:
					renderer->drawScaled(cmd.image, cmd.position, cmd.size, cmd.scaler);
					break;
				case RenderCmd::Cmd::DrawTinted:
					renderer->drawTinted(cmd.image, cmd.position, cmd.tint);
					break;
				case RenderCmd::Cmd::DrawFilledRect:
					renderer->drawFilledRect(cmd.position, cmd.size, cmd.c);
					break;
				case RenderCmd::Cmd::DrawRect:
					renderer->drawRect(cmd.position, cmd.size, cmd.c, cmd.thickness);
					break;
				case RenderCmd::Cmd::DrawLine:
					renderer->drawLine(cmd.p1, cmd.p2, cmd.c, cmd.thickness);
					break;
				case RenderCmd::Cmd::Flush:
					renderer->flush();
					break;
				case RenderCmd::Cmd::Flip:
				{
					renderer->flush();
					renderer->resetFrame();
					SDL_GL_SwapWindow(window);
					break;
				}
				case RenderCmd::Cmd::SetSurface:
					renderer->setSurface(cmd.surface);
					break;
				}
			}
			// In theory, we should wait for the
			// queue to be filled, but I'm not a big fan
			// of spamming notify_all(), so let's just have a
			// delay to prevent busy-waiting
			SDL_Delay(1);
		}
	}

	RenderThread::RenderThread(SDL_Window *window, SDL_GLContext context) : ready(false), win(window), ctx(context)
	{
		// Prepare to transfer the context to the rendering thread
		SDL_GL_MakeCurrent(window, nullptr);
		rndThread = std::thread(&RenderThread::RenderFunction, std::ref(*this), window, context);
		// Wait for renderer to become ready
		std::unique_lock<std::mutex> lk(readyMutex);
		readyCV.wait(lk, [&]{return ready; });
		SDL_Delay(16);
	}

	RenderThread::~RenderThread()
	{
		renderQueue.push(RenderCmd::quit());
		rndThread.join();
	}

	void RenderThread::clear(Colour c) 
	{ 
		renderQueue.push(RenderCmd::clear(c)); 
	} 
	void RenderThread::draw(sp<Image> image, Vec2<float> position) 
	{ 
		renderQueue.push(RenderCmd::draw(image, position)); 
	} 
	void RenderThread::setPalette(sp<Palette> p) 
	{ 
		// Store the palette, since getting the current palette is not thread-safe
		currentPalette = p;
		renderQueue.push(RenderCmd::setPalette(p)); 
	} 
	sp<Palette> RenderThread::getPalette()
	{
		// Return the stored palette, instead of the one that's currently in use
		return currentPalette;
	}
	void RenderThread::drawRotated(sp<Image> i, Vec2<float> center, Vec2<float> position, float angle)
	{ 
		renderQueue.push(RenderCmd::drawRotated(i, center, position, angle)); 
	}
	void RenderThread::drawScaled(sp<Image> i, Vec2<float> position, Vec2<float> size, Renderer::Scaler scaler) 
	{ 
		renderQueue.push(RenderCmd::drawScaled(i, position, size, scaler)); 
	}
	void RenderThread::drawFilledRect(Vec2<float> position, Vec2<float> size, Colour c)
	{ 
		renderQueue.push(RenderCmd::drawFilledRect(position, size, c)); 
	}
	void RenderThread::drawTinted(sp<Image> i, Vec2<float> position, Colour tint) 
	{
		renderQueue.push(RenderCmd::drawTinted(i, position, tint)); 
	}
	void RenderThread::drawRect(Vec2<float> position, Vec2<float> size, Colour c, float thickness)
	{ 
		renderQueue.push(RenderCmd::drawRect(position, size, c, thickness)); 
	}
	void RenderThread::drawLine(Vec2<float> p1, Vec2<float> p2, Colour c, float thickness) 
	{ 
		renderQueue.push(RenderCmd::drawLine(p1, p2, c, thickness)); 
	}
	void RenderThread::flush()
	{
		renderQueue.push(RenderCmd::flush());
	}
	void RenderThread::swapBuffers()
	{
		renderQueue.push(RenderCmd::flip());
	}
	sp<Surface> RenderThread::getDefaultSurface()
	{
		// FIXME: Might not be thread-safe?
		while (!defaultSurface);
		return defaultSurface;
	}
	sp<Surface> RenderThread::getSurface()
	{
		// This is DEFINITELY not thread-safe, so return the "stored" value
		return currentSurface;
	}
	void RenderThread::setSurface(sp<Surface> s)
	{
		// Store the surface for getSurface() call
		currentSurface = s;
		renderQueue.push(RenderCmd::setSurface(s));
	}

	void RenderThread::waitForRender()
	{
		// FIXME: This results in a busy-wait
		while (!renderQueue.empty());
	}

}