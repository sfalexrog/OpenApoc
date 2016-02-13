#include "framework/renderer.h"
#include "library/sp.h"

namespace OpenApoc
{

RendererSurfaceBinding::RendererSurfaceBinding(Renderer &r, sp<Surface> s)
    : prevBinding(r.getSurface()), r(r)
{
	r.setSurface(s);
}

RendererSurfaceBinding::~RendererSurfaceBinding() { r.setSurface(prevBinding); }

RendererImageData::~RendererImageData() {}

}; // namespace OpenApoc
