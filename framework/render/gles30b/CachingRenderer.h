#pragma once

#include <array>
#include "library/vec.h"
#include "library/sp.h"


const int MAX_CACHED_VERTICES = 65535;

// Each primitive consists of 4 vertices and a PRIMITIVE_RESTART value
const int MAX_CACHED_PRIMITIVES = MAX_CACHED_VERTICES / 5;

const int PRIMITIVE_RESTART_INDEX = 65536;

namespace OpenApoc
{

	class Image;
	class Texture;
	class Program;

	class CachingRenderer
	{
	private:
		struct VertexData
		{
			Vec2<float> position;
			Vec3<float> texCoord;
			int paletteIdx;
		};

		sp<Program> _cachingProgram;
		sp<Program> _immediateProgram;

		sp<Texture> _idxTexture;
		sp<Texture> _rgbaTexture;
		sp<Texture> _paletteTexture;
		sp<Texture> _scratchRGBATexture;
		sp<Texture> _scratchIndexTexture;

		std::array<VertexData, MAX_CACHED_VERTICES> _cachedVertices;
		std::array<short, MAX_CACHED_VERTICES> _cachedIndices;
		int _primitivesCount;
		void drawImmediate(sp<Image> image, Vec2<float> position);
	public:
		CachingRenderer();
		void draw(sp<Image> image, Vec2<float> position);
		void flush();
	};
}