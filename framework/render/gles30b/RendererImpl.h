#pragma once

#include "library/vec.h"
#include "library/sp.h"
#include <array>

#include "framework/renderer.h"

#include "gles3loader.h"

struct SDL_Window;
typedef void* SDL_GLContext;

namespace OpenApoc
{

	class Image;
	class Texture;
	class Program;
	class TextureCache;

	class RGBImage;
	class PaletteImage;
	class Surface;

	struct RGBData;
	struct IndexData;
	struct SurfaceData;

	class RendererImpl
	{
	private:
		friend class Renderer;
		SDL_Window *window;
		SDL_GLContext context;

		sp<Surface> currentSurface;
		sp<Palette> currentPalette;

		friend class RendererSurfaceBinding;
		void setSurface(sp<Surface> s);
		GLuint currentBoundFBO;

		sp<Surface> getSurface() { return currentSurface; }
		sp<Surface> defaultSurface;
	public:
		struct VertexData
		{
			Vec3<float> position;
			Vec3<float> texCoord;
			int paletteIdx;
		};
	private:
		struct SpriteData
		{
			std::array<VertexData, 4> vertex;
		};

		//static_assert(sizeof(VertexData) == 24, "Unexpected VertexData size");

		sp<Program> _cachingProgram;
		sp<Program> _immediateProgram;

		sp<Texture> _idxTexture;
		sp<Texture> _rgbaTexture;
		//sp<Texture> _paletteTexture;
		sp<Texture> _scratchRGBATexture;
		sp<Texture> _scratchIndexTexture;

		sp<TextureCache> _idxCache;
		sp<TextureCache> _rgbaCache;

		GLuint cacheVAO;
		GLuint immediateVAO;

		GLuint cacheVBO;
		GLuint immediateVBO;

		GLuint elementsVBO;

		VertexData* _mappedVertices;
		GLushort* _mappedElements;

		int _mappedBegin;
		int _mappedCurrent;
		int _mappedEnd;
		int _mapOffset;
		int _elemOffset;
		int _elemCount;

		float _primOrder;

		void enqueue(const SpriteData& data);
		SpriteData transform(sp<Image> image, Vec2<float> position, Vec2<float> size, Vec2<float> center = { 0.0f, 0.0f }, float angle = 0.0f);

		bool cacheImage(sp<Image> image);

		void drawImmediate(sp<Image> image, SpriteData &data);
		void drawImmediate(sp<RGBImage> image, RGBData *rdata, SpriteData &data);
		void drawImmediate(sp<PaletteImage> image, IndexData *idata, SpriteData &data);
		void drawImmediate(sp<Surface> image, SurfaceData *sdata, SpriteData &data);
		void drawImmediateExecute(const SpriteData &data);
		void flushCache();
		void flushImmediate();

		void resetFrame();

	public:
		RendererImpl();
		void clear(Colour c);
		void draw(sp<Image> image, Vec2<float> position);
		void setPalette(sp<Palette> p);
		sp<Palette> getPalette();
		void drawRotated(sp<Image> i, Vec2<float> center, Vec2<float> position, float angle);
		void drawScaled(sp<Image> i, Vec2<float> position, Vec2<float> size, Renderer::Scaler scaler);
		void drawFilledRect(Vec2<float> position, Vec2<float> size, Colour c) {}
		void drawTinted(sp<Image> i, Vec2<float> position, Colour tint) {}
		void drawRect(Vec2<float> position, Vec2<float> size, Colour c, float thickness) {}
		void drawLine(Vec2<float> p1, Vec2<float> p2, Colour c, float thickness) {}
		void flush();
		sp<Surface> getDefaultSurface() { return defaultSurface; }		
	};
}