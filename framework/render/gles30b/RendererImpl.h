#pragma once

#include "library/sp.h"
#include "framework/image.h"
#include "framework/renderer.h"
#include "CachingRenderer.h"

struct SDL_Window;
typedef void* SDL_GLContext;

namespace OpenApoc
{

	class RendererImpl
	{
	private:
		friend class Renderer;

		sp<CachingRenderer> cRenderer;

		SDL_Window *window;
		SDL_GLContext context;
		
		sp<Surface> currentSurface;
		sp<Palette> currentPalette;

		friend class RendererSurfaceBinding;
		void setSurface(sp<Surface> s);

		sp<Surface> getSurface() { return currentSurface; }
		sp<Surface> defaultSurface;

	public:
		RendererImpl();

		void clear(Colour c) { /* STUB */ }
		void setPalette(sp<Palette> p) { currentPalette = p; }
		sp<Palette> getPalette() { return currentPalette; }
		void draw(sp<Image> i, Vec2<float> position) { cRenderer->draw(i, position); }
		void drawRotated(sp<Image> i, Vec2<float> center, Vec2<float> position, float angle) {/* STUB */}
		void drawScaled(sp<Image> i, Vec2<float> position, Vec2<float> size, Renderer::Scaler scaler) {/* STUB */}
		void drawFilledRect(Vec2<float> position, Vec2<float> size, Colour c) {}
		void drawTinted(sp<Image> i, Vec2<float> position, Colour tint) {}
		void drawRect(Vec2<float> position, Vec2<float> size, Colour c, float thickness) {}
		void drawLine(Vec2<float> p1, Vec2<float> p2, Colour c, float thickness) {}
		void flush() { cRenderer->flush(); }
		sp<Surface> getDefaultSurface() { return defaultSurface; }
	};
}