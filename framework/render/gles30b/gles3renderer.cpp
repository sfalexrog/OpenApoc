#include "library/sp.h"
#include "framework/renderer.h"

#include "framework/logger.h"

#include "gles3loader.h"
#include "RendererImpl.h"
#include <SDL.h>
#include "Texture.h"

namespace OpenApoc
{

	bool Renderer::preWindowCreateHook(int &display_flags)
	{
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 0);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 0);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 0);
		display_flags |= SDL_WINDOW_OPENGL;
		return true;
	}

	/**
	 *	Find out texture limits the "hard" way.
	 *  Creates a texture with the largest possible (sane?) dimensions, and then checks for
	 *  gl::GetError() == gl::OUT_OF_MEMORY. Since the context is probably useless after that, it will
	 *  have to be recreated again and again until the "sane" values are found.
	 *
	 */
	static void populateLimits(SDL_Window *win)
	{
		LogInfo("Trying to find the memory limits by stress-testing the system");
		SDL_GLContext testContext = SDL_GL_CreateContext(win);
		LogInfo("Querying OpenGL for maximum texture sizes...");
		int maxTextureWidth;
		int maxTextureHeight;
		int maxTextureDepth;
		gl::GetIntegerv(gl::MAX_TEXTURE_SIZE, &maxTextureWidth);
		maxTextureHeight = maxTextureWidth;
		gl::GetIntegerv(gl::MAX_ARRAY_TEXTURE_LAYERS, &maxTextureDepth);
		LogInfo("Got maxTextureWidth == maxTextureHeight == %d, maxTextureDepth == %d", maxTextureWidth, maxTextureDepth);
		LogInfo("(that would translate to %d megabytes of memory for RGBA textures)", (maxTextureWidth / 1024) * (maxTextureHeight / 1024) * maxTextureDepth * 4);
		LogInfo("Will now try and push the limits!");
		SDL_GL_DeleteContext(testContext);
		bool limitsFound = false;
		// Try our luck with a sane amount of layers first
		maxTextureDepth = std::min(maxTextureDepth, 32);
		maxTextureWidth = std::min(maxTextureWidth, 16384);
		maxTextureHeight = std::min(maxTextureHeight, 16384);
		sp<Image> testImage(new RGBImage(Vec2<unsigned int>(256, 256)));
		while (!limitsFound)
		{
			testContext = SDL_GL_CreateContext(win);
			GLenum error = gl::GetError();
			if (error)
			{
				LogWarning("Got GL errors in the queue, flushing... (queued error: %d)", error);
				while (error = gl::GetError())
				{
					LogWarning("Still flushing... (%d)", error);
				}
			}
			LogInfo("Trying %d x %d x %d RGBA texture...", maxTextureWidth, maxTextureHeight, maxTextureDepth);
			sp<Texture> testTexture = Texture::createRGBATexture(Vec3<int>(maxTextureWidth, maxTextureHeight, maxTextureDepth));
			error = gl::GetError();
			if (error)
			{
				LogWarning("There were errors during texture creation!");
			}
			else
			{
				// Try uploading something to it at some random places
				LogInfo("Uploading the texture to some random places...");
				testTexture->uploadImage(testImage, Vec3<int>(0, 0, 0));
				error = gl::GetError();
				testTexture->uploadImage(testImage, Vec3<int>(maxTextureWidth - testImage->size.x, maxTextureHeight - testImage->size.y, 0));
				error = gl::GetError();
				testTexture->uploadImage(testImage, Vec3<int>(0, 0, maxTextureDepth - 1));
				error = gl::GetError();
				testTexture->uploadImage(testImage, Vec3<int>(maxTextureWidth - testImage->size.x, maxTextureHeight - testImage->size.y, maxTextureDepth - 1));
				error = gl::GetError();
			}
			if (error)
			{
				if (error == gl::OUT_OF_MEMORY)
				{
					LogInfo("Got Out Of Memory error, shrinking texture and moving on");
					maxTextureDepth /= 2;
					if (maxTextureDepth < 2)
					{
						LogInfo("Layer count is too small, shrinking texture size");
						maxTextureDepth = 32;
						maxTextureHeight /= 2;
						maxTextureWidth /= 2;
						if ((maxTextureHeight < 1024) || (maxTextureHeight < 1024))
						{
							LogError("Your hardware just sucks. Throw it away and get a real video card.");
						}
					}
					// Will probably generate another error, but meh
					testTexture.reset();
					SDL_GL_DeleteContext(testContext);
					continue;
				}
				LogWarning("Got error, but it is not gl::OUT_OF_MEMORY: %d", error);
				while (error = gl::GetError())
				{
					LogWarning("Flushing error queue: %d", error);
					testTexture.reset();
					SDL_GL_DeleteContext(testContext);
					// Try shrinking texture anyway
					maxTextureDepth /= 2;
					if (maxTextureDepth < 2)
					{
						LogError("You know, there's probably something wrong elsewhere");
					}
				}
			}
			else
			{
				LogInfo("Curiously enough, no error was found with texture size of %d x %d x %d", maxTextureWidth, maxTextureHeight, maxTextureDepth);
				LogInfo("Cutting maxTextureDepth in half just in case");
				maxTextureDepth /= 2;
				Texture::Limits::MAX_TEXTURE_WIDTH = maxTextureWidth;
				Texture::Limits::MAX_TEXTURE_HEIGHT = maxTextureHeight;
				Texture::Limits::MAX_TEXTURE_DEPTH = maxTextureDepth;
				limitsFound = true;
				testTexture.reset();
				SDL_GL_DeleteContext(testContext);
			}
		}

	}

	Renderer::Renderer(SDL_Window *win)
	{
		// Proceed with actual context creation
		SDL_GLContext ctx = SDL_GL_CreateContext(win);
		if (!ctx)
		{
			LogError("Could not create context: %s", SDL_GetError());
		}
		// Output the context parameters
		LogInfo("Created OpenGL context, parameters:");
		int value;
		SDL_GL_GetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, &value);
		std::string profileType;
		switch (value)
		{
		case SDL_GL_CONTEXT_PROFILE_ES:
			profileType = "ES";
			break;
		case SDL_GL_CONTEXT_PROFILE_CORE:
			profileType = "Core";
			break;
		case SDL_GL_CONTEXT_PROFILE_COMPATIBILITY:
			profileType = "Compatibility";
			break;
		default:
			profileType = "Unknown";
		}
		LogInfo("  Context profile: %s", profileType.c_str());
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
		bool functionsLoaded = gl::sys::loadGLCore();
		if (!functionsLoaded)
		{
			LogError("Failed to load GL functions");
		}
		SDL_GL_DeleteContext(ctx);
		populateLimits(win);
		ctx = SDL_GL_CreateContext(win);
		SDL_GL_MakeCurrent(win, ctx); // for good measure?
		SDL_GL_SetSwapInterval(1);
		pimpl.reset(new RendererImpl());
		pimpl->window = win;
		pimpl->context = ctx;
		LogWarning("(not actually a warning, just flushing logs)");
	}

	Renderer::~Renderer()
	{
		auto ctxCopy = pimpl->context;
		auto winCopy = pimpl->window;
		pimpl.reset();
		SDL_GL_MakeCurrent(winCopy, nullptr);
		SDL_GL_DeleteContext(ctxCopy);
	}

	void Renderer::setSurface(sp<Surface> s) { pimpl->setSurface(s); }
	sp<Surface> Renderer::getSurface() { return pimpl->getSurface(); }

	void Renderer::swapBuffers() { SDL_GL_SwapWindow(pimpl->window); }

	void Renderer::clear(Colour c) { pimpl->clear(c); }
	void Renderer::setPalette(sp<Palette> p) { pimpl->setPalette(p); }
	sp<Palette> Renderer::getPalette() { return pimpl->getPalette(); }
	void Renderer::draw(sp<Image> i, Vec2<float> position) { pimpl->draw(i, position); }
	void Renderer::drawRotated(sp<Image> i, Vec2<float> center, Vec2<float> position, float angle)
	{
		pimpl->drawRotated(i, center, position, angle);
	}
	void Renderer::drawScaled(sp<Image> i, Vec2<float> position, Vec2<float> size, Scaler scaler)
	{
		pimpl->drawScaled(i, position, size, scaler);
	}
	void Renderer::drawTinted(sp<Image> i, Vec2<float> position, Colour tint)
	{
		pimpl->drawTinted(i, position, tint);
	}
	void Renderer::drawFilledRect(Vec2<float> position, Vec2<float> size, Colour c)
	{
		pimpl->drawFilledRect(position, size, c);
	}
	void Renderer::drawRect(Vec2<float> position, Vec2<float> size, Colour c, float thickness)
	{
		pimpl->drawRect(position, size, c, thickness);
	}
	void Renderer::drawLine(Vec2<float> p1, Vec2<float> p2, Colour c, float thickness)
	{
		pimpl->drawLine(p1, p2, c, thickness);
	}
	void Renderer::flush() { pimpl->flush(); }
	UString Renderer::getName() { return "GLES3.0b Renderer"; }

	sp<Surface> Renderer::getDefaultSurface() { return pimpl->getDefaultSurface(); }

}

