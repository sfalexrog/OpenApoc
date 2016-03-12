#pragma once

#include "framework/renderer.h"
#include "library/sp.h"
#include "library/vec.h"

namespace OpenApoc
{
	class Texture;

	class RGBImage;
	class Palette;
	class PaletteImage;

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

	class GLImageData : public RendererImageData
	{

	private:
		wp<Image> _parent;
		static sp<Texture> paletteTexture;
	public:
		enum {
			IMG_RGBA,
			IMG_PALETTE,
			IMG_INDEX
		} ImageTag;

		union 
		{
			RGBData *rgbData;
			PaletteData *paletteData;
			IndexData *indexData;
		};
		GLImageData(sp<Image> parent);
		GLImageData(sp<Palette> parent);
		~GLImageData();
	};
}