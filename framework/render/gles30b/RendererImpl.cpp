#include "RendererImpl.h"

#include "FBOData.h"

#include "framework/logger.h"

namespace OpenApoc
{
	RendererImpl::RendererImpl()
	{
		cRenderer.reset(new CachingRenderer());
		LogWarning("Stopping here");
		while (1) {};
	}

	void RendererImpl::setSurface(sp<Surface> s)
	{
		if (currentSurface == s)
		{
			return;
		}
		cRenderer->flush();
		currentSurface = s;
		if (!s->rendererPrivateData)
		{
			s->rendererPrivateData.reset(new FBOData(s->size));
		}
		FBOData *fbo = static_cast<FBOData*>(s->rendererPrivateData.get());
		gl::BindFramebuffer(gl::FRAMEBUFFER, fbo->fbo);
		//currentBoundFBO = fbo->fbo;
		gl::Viewport(0, 0, s->size.x, s->size.y);
	}
}