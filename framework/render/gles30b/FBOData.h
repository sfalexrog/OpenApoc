#pragma once

#include "framework/renderer.h"
#include "gles3loader.h"

namespace OpenApoc
{
	class Texture;

	class FBOData : public RendererImageData
	{
	public:
		GLuint fbo;
		sp<Texture> tex;
		GLuint depthBuffer;
		Vec2<float> size;
		FBOData(GLuint fbo) : fbo(fbo), tex(nullptr), size(0, 0) { };
		FBOData(Vec2<int> size);
		~FBOData();
	};
}