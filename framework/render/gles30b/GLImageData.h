#pragma once

#include "framework/renderer.h"
#include "library/sp.h"
#include "library/vec.h"
#include <array>

namespace OpenApoc
{
	class Texture;

	class RGBImage;
	class Palette;
	class PaletteImage;

	class FBOData;

	/**
	 *	Extended palette data
	 */
	class PaletteDataExt
	{
	public:
		static sp<Texture> paletteTexture;
		static int idxCount;
	};

	namespace CacheState
	{
		enum CachedState {
			CACHE_NOT_CACHED,
			CACHE_CACHED,
			CACHE_UNCACHEABLE
		};
	}

	struct RGBData
	{
		CacheState::CachedState cachedState;
		Vec2<int> cachePos;
		Vec2<float> size;
		sp<Texture> tex;
	};

	struct PaletteData
	{
		int size;
		int index;
	};

	struct IndexData
	{
		CacheState::CachedState cachedState;
		Vec3<int> cachePos;
		Vec2<float> size;
		sp<Texture> tex;
	};

	struct SurfaceData
	{
		CacheState::CachedState cachedState;
		sp<FBOData> fboData;
	};

	class GLImageData : public RendererImageData
	{

	private:
		wp<Image> _parent;
		static sp<Texture> paletteTexture;
	public:
		enum {
			IMG_RGBA,
			IMG_PALETTE,
			IMG_INDEX,
			IMG_SURFACE
		} ImageTag;

		union 
		{
			RGBData *rgbData;
			PaletteData *paletteData;
			IndexData *indexData;
			SurfaceData *surfaceData;
		};

		// Coordinates order: top left, top right, bottom left, bottom right
		std::array<Vec3<int>, 4> texelCoords;

		GLImageData(sp<Image> parent);
		GLImageData(sp<Surface> parent, bool defaultFbo = false);
		GLImageData(sp<Palette> parent);
		CacheState::CachedState getCacheState();
		void setCachedState(CacheState::CachedState cachedState);
		~GLImageData();
	};
}