#include "GLImageData.h"
#include "Texture.h"
#include "framework/image.h"
#include "framework/logger.h"
#include "framework/palette.h"

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
		else
		{
			LogError("Unknown image type!");
		}
	}

	GLImageData::GLImageData(sp<Palette> parent)
	{
		ImageTag = IMG_PALETTE;
		if (!PaletteDataExt::paletteTexture)
		{
			PaletteDataExt::paletteTexture = Texture::createRGBATexture(
				Vec2<int>(std::max(parent->colours.size(), (size_t)256), 
				MAX_STORED_PALETTES));
			PaletteDataExt::idxCount = 0;
		}
		paletteData = new PaletteData();
		paletteData->size = parent->colours.size();
		PaletteDataExt::paletteTexture->uploadImage(parent, Vec2<int>(0, PaletteDataExt::idxCount));
		paletteData->index = PaletteDataExt::idxCount;
		PaletteDataExt::idxCount++;
		assert(PaletteDataExt::idxCount < MAX_STORED_PALETTES);
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
			PaletteDataExt::idxCount--;
			// This __shouldn't__ happen, but just to be sure...
			if (PaletteDataExt::idxCount < 0) 
			{ 
				LogWarning("Texture index underrun!");
				PaletteDataExt::idxCount = 0;
			}
			break;
		}
	}
}
