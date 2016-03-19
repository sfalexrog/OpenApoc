#include "GLImageData.h"
#include "Texture.h"
#include "framework/image.h"
#include "framework/logger.h"
#include "framework/palette.h"

#include "FBOData.h"

const int MAX_STORED_PALETTES = 1024;

namespace OpenApoc
{

	sp<Texture> PaletteDataExt::paletteTexture;
	int PaletteDataExt::idxCount;

	/**
	 *  Create and store renderer-specific image data
	 *  This should be the only place in the renderer where we use dynamic_casts.
	 *  The image will be "tagged" (i.e. its type will be stored) and will have some
	 *  format-specific data stored with it. Accessing this data should only be done by
	 *  first checking the tag of the image.
	 */
	GLImageData::GLImageData(sp<Image> image) : _parent(image)
	{
		sp<RGBImage> rgbImage = std::dynamic_pointer_cast<RGBImage>(image);
		sp<PaletteImage> indexImage = std::dynamic_pointer_cast<PaletteImage>(image);
		sp<Surface> surfaceImage = std::dynamic_pointer_cast<Surface>(image);

		if (rgbImage)
		{
			ImageTag = IMG_RGBA;
			rgbData = new RGBData();
			rgbData->cachedState = CacheState::CACHE_NOT_CACHED;
			rgbData->size = image->size;
		}
		else if (indexImage)
		{
			ImageTag = IMG_INDEX;
			indexData = new IndexData();
			indexData->cachedState = CacheState::CACHE_NOT_CACHED;
			indexData->size = image->size;
		}
		else if (surfaceImage)
		{
			ImageTag = IMG_SURFACE;
			surfaceData = new SurfaceData();
			surfaceData->fboData.reset(new FBOData(image->size));
			// Hardware surfaces are not cacheable (yet?)
			surfaceData->cachedState = CacheState::CACHE_UNCACHEABLE;
		}
		else
		{
			LogError("Unknown image type!");
		}
	}

	/**
	 *	Create FBO-specific image data.
	 *  This is a special case data creation, with support for
	 *  default FBO creation.
	 *  This code is actually duplicated in the default constructor.
	 */
	GLImageData::GLImageData(sp<Surface> parent, bool defaultFbo)
	{
		ImageTag = IMG_SURFACE;
		surfaceData = new SurfaceData();
		surfaceData->cachedState = CacheState::CACHE_UNCACHEABLE;
		if (defaultFbo)
		{
			surfaceData->fboData.reset(new FBOData(0));	
		}
		else
		{
			surfaceData->fboData.reset(new FBOData(parent->size));
		}
	}

	GLImageData::GLImageData(sp<Palette> parent)
	{
		ImageTag = IMG_PALETTE;
		auto error = gl::GetError();
		if (!PaletteDataExt::paletteTexture)
		{
			PaletteDataExt::paletteTexture = Texture::createRGBATexture(
				Vec2<int>(std::max(parent->colours.size(), (size_t)256), 
				MAX_STORED_PALETTES));
			PaletteDataExt::idxCount = 0;
		}
		error = gl::GetError(); assert(error == 0);
		paletteData = new PaletteData();
		paletteData->size = parent->colours.size();
		PaletteDataExt::paletteTexture->uploadImage(parent, Vec2<int>(0, PaletteDataExt::idxCount));
		paletteData->index = PaletteDataExt::idxCount;
		PaletteDataExt::idxCount++;
		assert(PaletteDataExt::idxCount < MAX_STORED_PALETTES);
	}

	CacheState::CachedState GLImageData::getCacheState()
	{
		switch (ImageTag)
		{
		case IMG_RGBA:
			return rgbData->cachedState;
		case IMG_INDEX:
			return indexData->cachedState;
		case IMG_PALETTE:
			// FIXME: Is this even necessary?
			return CacheState::CachedState::CACHE_UNCACHEABLE;
		case IMG_SURFACE:
			return CacheState::CachedState::CACHE_UNCACHEABLE;
		}
		return CacheState::CachedState::CACHE_UNCACHEABLE;
	}

	void GLImageData::setCachedState(CacheState::CachedState cachedState)
	{
		switch (ImageTag)
		{
		case IMG_RGBA:
			rgbData->cachedState = cachedState;
			break;
		case IMG_INDEX:
			indexData->cachedState = cachedState;
			break;
		case IMG_SURFACE:
			// Do nothing, we can't cache them anyway
			break;
		case IMG_PALETTE:
			// Again, do nothing, we don't exactly "cache" palettes.
			break;
		}
	}

	GLImageData::~GLImageData()
	{
		// TODO: Is this really necessary? I mean, the allocator should "know" the pointer type... right?
		switch (ImageTag)
		{
		case IMG_RGBA:
			delete rgbData;
			break;
		case IMG_INDEX:
			delete indexData;
			break;
		case IMG_PALETTE:
			delete paletteData;
			//PaletteDataExt::idxCount--;
			// This __shouldn't__ happen, but just to be sure...
			if (PaletteDataExt::idxCount < 0) 
			{ 
				LogWarning("Texture index underrun!");
				PaletteDataExt::idxCount = 0;
			}
			break;
		case IMG_SURFACE:
			delete surfaceData;
		}
	}
}
