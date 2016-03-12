#include "FBOData.h"
#include "Texture.h"
#include "framework/trace.h"

namespace OpenApoc
{
	FBOData::FBOData(Vec2<int> size)
	{
		TRACE_FN;
		tex = Texture::createRGBATexture(size);
		tex->bind(0);
		gl::GenRenderbuffers(1, &depthBuffer);
		gl::BindRenderbuffer(gl::RENDERBUFFER, depthBuffer);
		gl::RenderbufferStorage(gl::RENDERBUFFER, gl::DEPTH_COMPONENT24, size.x, size.y);

		gl::GenFramebuffers(1, &fbo);
		gl::BindFramebuffer(gl::DRAW_FRAMEBUFFER, fbo);

		gl::FramebufferTexture2D(gl::DRAW_FRAMEBUFFER, gl::COLOR_ATTACHMENT0, gl::TEXTURE_2D, tex->id(), 0);
		gl::FramebufferRenderbuffer(gl::DRAW_FRAMEBUFFER, gl::DEPTH_ATTACHMENT, gl::RENDERBUFFER, depthBuffer);

		assert(gl::CheckFramebufferStatus(gl::DRAW_FRAMEBUFFER) == gl::FRAMEBUFFER_COMPLETE);
	}

	FBOData::~FBOData()
	{
		// tex is managed by a SharedPtr, so no need to explicitly destroy it here.
		if (depthBuffer)
		{
			gl::DeleteRenderbuffers(1, &depthBuffer);
		}
		if (fbo)
		{
			gl::DeleteFramebuffers(1, &fbo);
		}
	}
}