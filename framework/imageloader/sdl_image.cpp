#include "library/sp.h"
#include "framework/imageloader_interface.h"
#include "framework/logger.h"
#include "library/vec.h"

#include <physfs.h>
#include <SDL_image.h>

#include <string>

using namespace OpenApoc;

namespace
{

class SDLImageLoader : public OpenApoc::ImageLoader
{
  private:
	SDL_PixelFormat *tgtPixFormat;
	SDL_Surface *dummySurface;

  public:
	SDLImageLoader()
	{
		// FIXME: Compile with PNG support?
#ifndef __ANDROID__
		IMG_Init(IMG_INIT_PNG);
#endif
		// Create an empty SDL_Surface with 32bpp to use as a base for image loading
		dummySurface = SDL_CreateRGBSurface(0, 1, 1, 32, 0xff, 0xff00, 0xff0000, 0xff000000);
		tgtPixFormat = dummySurface->format;
	}
	virtual ~SDLImageLoader()
	{
		/* IMG_Quit();*/
		SDL_FreeSurface(dummySurface);
	} // Required only if loading PNG, JPG or TIF?

	virtual sp<OpenApoc::Image> loadImage(UString path) override
	{
#ifdef __ANDROID__
		if (path.str().find(".PNG") != std::string::npos)
		{
			LogInfo("SDL_Image loader does not support PNG files right now.");
			return nullptr;
		}
#endif
		// TODO: Use SDL_RWops with PhysFS!
		PHYSFS_file *source = PHYSFS_openRead(path.c_str());
		if (!source)
		{
			LogWarning("Could not open file %s", path.c_str());
			return nullptr;
		}
		int fsize = PHYSFS_fileLength(source);
		std::unique_ptr<char[]> buffer(new char[fsize]);
		PHYSFS_readBytes(source, buffer.get(), fsize);
		PHYSFS_close(source);
		SDL_RWops *memIO = SDL_RWFromConstMem(buffer.get(), fsize);
		SDL_Surface *bmp = IMG_Load_RW(memIO, 1);
		if (!bmp)
		{
			LogInfo("Failed to read image %s", path.c_str());
			LogInfo("IMG_Load_RW Error: %s", IMG_GetError());
			return nullptr;
		}

		SDL_Surface *src = SDL_ConvertSurface(bmp, tgtPixFormat, 0);
		if (!src)
		{
			LogWarning("Failed to convert surface \"%s\" to RGBA", path.c_str());
			return nullptr;
		}
		SDL_FreeSurface(bmp);

		OpenApoc::Vec2<int> size{src->w, src->h};
		auto img = std::make_shared<OpenApoc::RGBImage>(size);
		OpenApoc::RGBImageLock dst{img, OpenApoc::ImageLockUse::Write};

		SDL_LockSurface(src);
		char *srcLinePtr = reinterpret_cast<char *>(src->pixels);

		for (int y = 0; y < size.y; y++)
		{
			OpenApoc::Colour *c = reinterpret_cast<OpenApoc::Colour *>(srcLinePtr);
			for (int x = 0; x < size.x; x++)
			{
				dst.set(OpenApoc::Vec2<int>{x, y}, c[x]);
			}
			srcLinePtr += src->pitch;
		}
		SDL_UnlockSurface(src);
		SDL_FreeSurface(src);
		return img;
	}

	virtual UString getName() override { return "SDL_ImageLoader"; }
};

class SDLImageLoaderFactory : public OpenApoc::ImageLoaderFactory
{
  public:
	virtual OpenApoc::ImageLoader *create() override { return new SDLImageLoader(); }
	virtual ~SDLImageLoaderFactory() {}
};
//#ifndef ANDROID
OpenApoc::ImageLoaderRegister<SDLImageLoaderFactory>
    register_at_load_allegro_image("SDL_ImageLoader");
//#endif
}; // anonymous namespace
