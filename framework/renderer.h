#pragma once
#include "library/colour.h"
#include "library/sp.h"
#include "library/strings.h"
#include "library/vec.h"
#include <memory>

// Declare this so we don't include SDL.h everywhere
struct SDL_Window;

namespace OpenApoc
{

class Image;
class Palette;
class Surface;

class RendererImageData
{
  public:
	virtual ~RendererImageData();
};

class RendererImpl;

class Renderer
{
  private:
	up<RendererImpl> pimpl;
	friend class RendererSurfaceBinding;
	void setSurface(sp<Surface> s);
	sp<Surface> getSurface();

  public:
	enum class Scaler
	{
		Nearest,
		Linear,
	};

	// The renderer may need to set some GL attributes before the window creation and/or modify the
	// flags passed in to the create window call, so needs a hook here...

	static bool preWindowCreateHook(int &display_flags);

	Renderer(SDL_Window *win);
	~Renderer();

	void swapBuffers();

	void clear(Colour c = Colour{0, 0, 0, 0});
	void setPalette(sp<Palette> p);
	sp<Palette> getPalette();
	void draw(sp<Image> i, Vec2<float> position);
	void drawRotated(sp<Image> i, Vec2<float> center, Vec2<float> position, float angle);
	void drawScaled(sp<Image> i, Vec2<float> position, Vec2<float> size,
	                Scaler scaler = Scaler::Linear);
	void drawTinted(sp<Image> i, Vec2<float> position, Colour tint);
	void drawFilledRect(Vec2<float> position, Vec2<float> size, Colour c);
	// drawRect() is expected to grow inwards, so {position, position + size} specifies the bounds
	// of the rect no matter the thickness
	void drawRect(Vec2<float> position, Vec2<float> size, Colour c, float thickness = 1.0);
	void drawLine(Vec2<float> p1, Vec2<float> p2, Colour c, float thickness = 1.0);
	void flush();
	UString getName();

	sp<Surface> getDefaultSurface();
};

class RendererSurfaceBinding
{
  private:
	// Disallow copy
	RendererSurfaceBinding(const RendererSurfaceBinding &) = delete;
	sp<Surface> prevBinding;
	Renderer &r;

  public:
	RendererSurfaceBinding(Renderer &r, sp<Surface> surface);
	~RendererSurfaceBinding();
};

}; // namespace openapoc
