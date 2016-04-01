#pragma once

#include <SDL.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <boost/lockfree/spsc_queue.hpp>
#include "library/sp.h"
#include "RendererImpl.h"

namespace OpenApoc
{
	class Palette;

	class RenderThread
	{
		std::mutex queueMutex;
		std::condition_variable queueCV;

		bool ready;
		std::mutex readyMutex;
		std::condition_variable readyCV;

		SDL_Window *win;
		SDL_GLContext ctx;

		std::thread rndThread;
		up<RendererImpl> renderer;

		// Store the current palette, since the one in the renderer could be different
		sp<Palette> currentPalette;
		sp<Surface> currentSurface;
		sp<Surface> defaultSurface;

		void RenderFunction(SDL_Window *window, SDL_GLContext context);
	public:
		RenderThread(SDL_Window *window, SDL_GLContext context);
		~RenderThread();

		void clear(Colour c);
		void draw(sp<Image> image, Vec2<float> position);
		void setPalette(sp<Palette> p);
		sp<Palette> getPalette();
		void drawRotated(sp<Image> i, Vec2<float> center, Vec2<float> position, float angle);
		void drawScaled(sp<Image> i, Vec2<float> position, Vec2<float> size, Renderer::Scaler scaler);
		void drawFilledRect(Vec2<float> position, Vec2<float> size, Colour c);
		void drawTinted(sp<Image> i, Vec2<float> position, Colour tint);
		void drawRect(Vec2<float> position, Vec2<float> size, Colour c, float thickness);
		void drawLine(Vec2<float> p1, Vec2<float> p2, Colour c, float thickness);
		void flush();
		void swapBuffers();
		sp<Surface> getDefaultSurface();
		sp<Surface> getSurface();
		void setSurface(sp<Surface> s);

		void waitForRender();

	};
}