#include "Texture.h"
#include "framework/image.h"
#include "framework/logger.h"
#include "framework/palette.h"
#include "GLImageData.h"

namespace OpenApoc
{

	GLuint Texture::Limits::MAX_TEXTURE_WIDTH;
	GLuint Texture::Limits::MAX_TEXTURE_HEIGHT;
	GLuint Texture::Limits::MAX_TEXTURE_DEPTH;

	Texture::Texture(Vec2<int> size, TexType textureType, FilterType filterType) : _textureType(textureType), _size(size, 0)
	{
		assert(textureType == TEXTURE_2D_INDEX || textureType == TEXTURE_2D_RGBA);
		GLenum filtering;
		switch (filterType)
		{
		case FT_NEAREST:
			filtering = gl::NEAREST;
			break;
		case FT_LINEAR:
			filtering = gl::LINEAR;
			break;
		}

		gl::GenTextures(1, &_textureIdx);
		gl::ActiveTexture(gl::TEXTURE0);
		gl::BindTexture(gl::TEXTURE_2D, _textureIdx);
		gl::TexParameteri(gl::TEXTURE_2D, gl::TEXTURE_MIN_FILTER, filtering);
		gl::TexParameteri(gl::TEXTURE_2D, gl::TEXTURE_MAG_FILTER, filtering);
		gl::TexParameteri(gl::TEXTURE_2D, gl::TEXTURE_WRAP_S, gl::CLAMP_TO_EDGE);
		gl::TexParameteri(gl::TEXTURE_2D, gl::TEXTURE_WRAP_T, gl::CLAMP_TO_EDGE);

		GLenum internalformat;
		switch (textureType)
		{
		case TEXTURE_2D_INDEX:
			internalformat = gl::R8UI;
			break;
		case TEXTURE_2D_RGBA:
			internalformat = gl::RGBA8;
			break;
		default:
			LogError("Wrong texture type!");
		}
		gl::TexStorage2D(gl::TEXTURE_2D, 1, internalformat, size.x, size.y);
	}

	Texture::Texture(Vec3<int> size, TexType textureType, FilterType filterType) : _textureType(textureType), _size(size)
	{
		assert(textureType == TEXTURE_2D_ARRAY_INDEX || textureType == TEXTURE_2D_ARRAY_RGBA);
		GLenum filtering;
		GLenum error = gl::GetError();
		switch (filterType)
		{
		case FT_NEAREST:
			filtering = gl::NEAREST;
			break;
		case FT_LINEAR:
			filtering = gl::LINEAR;
			break;
		}

		gl::GenTextures(1, &_textureIdx);
		gl::ActiveTexture(gl::TEXTURE0);
		gl::BindTexture(gl::TEXTURE_2D_ARRAY, _textureIdx);
		assert((error = gl::GetError()) == 0);
		gl::TexParameteri(gl::TEXTURE_2D_ARRAY, gl::TEXTURE_MIN_FILTER, filtering);
		gl::TexParameteri(gl::TEXTURE_2D_ARRAY, gl::TEXTURE_MAG_FILTER, filtering);
		gl::TexParameteri(gl::TEXTURE_2D_ARRAY, gl::TEXTURE_WRAP_S, gl::CLAMP_TO_EDGE);
		gl::TexParameteri(gl::TEXTURE_2D_ARRAY, gl::TEXTURE_WRAP_T, gl::CLAMP_TO_EDGE);
		assert((error = gl::GetError()) == 0);
		GLenum internalformat;
		switch (textureType)
		{
		case TEXTURE_2D_ARRAY_INDEX:
			internalformat = gl::R8UI;
			break;
		case TEXTURE_2D_ARRAY_RGBA:
			internalformat = gl::RGBA8;
			break;
		default:
			LogError("Wrong texture type!");
		}
		gl::TexStorage3D(gl::TEXTURE_2D_ARRAY, 1, internalformat, size.x, size.y, size.z);
	}
	
	sp<Texture> Texture::createIndexTexture(Vec2<int> size)
	{
		return mksp<Texture>(size, TEXTURE_2D_INDEX);
	
	}
	
	sp<Texture> Texture::createRGBATexture(Vec2<int> size, FilterType filterType)
	{
		return mksp<Texture>(size, TEXTURE_2D_RGBA, filterType);
	
	}

	sp<Texture> Texture::createRGBATexture(Vec3<int> size, FilterType filterType)
	{
		return mksp<Texture>(size, TEXTURE_2D_ARRAY_RGBA, filterType);
	}

	sp<Texture> Texture::createIndexTexture(Vec3<int> size)
	{
		return mksp<Texture>(size, TEXTURE_2D_ARRAY_INDEX);
	}
	
	void Texture::uploadImage(sp<Image> image, Vec2<int> at, bool rotate)
	{
		bind(0);
		GLenum format;
		GLImageData *imageData;
		if (image->rendererPrivateData)
		{
			imageData = static_cast<GLImageData*>(image->rendererPrivateData.get());
		}
		else
		{
			image->rendererPrivateData.reset(imageData = new GLImageData(image));
		}
		switch (_textureType)
		{
		case TEXTURE_2D_INDEX:
			assert(imageData->ImageTag == GLImageData::IMG_INDEX);
			gl::PixelStorei(gl::UNPACK_ALIGNMENT, 1);
			format = gl::R8UI;
			break;
		case TEXTURE_2D_RGBA:
			assert(imageData->ImageTag == GLImageData::IMG_PALETTE || imageData->ImageTag == GLImageData::IMG_RGBA);
			gl::PixelStorei(gl::UNPACK_ALIGNMENT, 4);
			format = gl::RGBA8;
			break;
		default:
			LogError("Attempted to upload image to an incompatible texture!");
		}
		switch (imageData->ImageTag)
		{
		case GLImageData::IMG_INDEX:
			{
				PaletteImage *pImage = static_cast<PaletteImage*>(image.get());
				PaletteImageLock pl(sp<PaletteImage>(pImage), ImageLockUse::Read);
				if (rotate)
				{
					uint8_t* sourceImage = reinterpret_cast<uint8_t*>(pl.getData());
					up<unsigned char[]> rotatedImage(new unsigned char[image->size.x * image->size.y]);
					// FIXME: There should be a better/faster way to do it!
					for (int y = 0; y < image->size.y; y++)
					{
						for (int x = 0; x < image->size.x; x++)
						{
							rotatedImage[image->size.y - y - 1 + x * image->size.x] = sourceImage[x + image->size.x * y];
						}
					}
					gl::TexSubImage2D(gl::TEXTURE_2D, 0, at.x, at.y, image->size.y, image->size.x, format, gl::UNSIGNED_BYTE, rotatedImage.get());
				}
				else
				{
					gl::TexSubImage2D(gl::TEXTURE_2D, 0, at.x, at.y, image->size.x, image->size.y, format, gl::UNSIGNED_BYTE, pl.getData());
				}

			}
			break;
		case GLImageData::IMG_RGBA:
			{
				RGBImage *rgbImage = static_cast<RGBImage*>(image.get());
				RGBImageLock rl(sp<RGBImage>(rgbImage), ImageLockUse::Read);
				if (rotate)
				{
					Colour *sourceImage = reinterpret_cast<Colour*>(rl.getData());
					up<Colour[]> rotatedImage(new Colour[image->size.x * image->size.y]);
					// FIXME: There should be a better/faster way to do it!
					for (int y = 0; y < image->size.y; y++)
					{
						for (int x = 0; x < image->size.x; x++)
						{
							rotatedImage[image->size.y - y - 1 + x * image->size.x] = sourceImage[x + image->size.x * y];
						}
					}
					gl::TexSubImage2D(gl::TEXTURE_2D, 0, at.x, at.y, image->size.y, image->size.x, format, gl::UNSIGNED_BYTE, rotatedImage.get());
				}
				else
				{
					gl::TexSubImage2D(gl::TEXTURE_2D, 0, at.x, at.y, image->size.x, image->size.y, format, gl::UNSIGNED_BYTE, rl.getData());
				}
			}
			break;
		}
	}

	void Texture::uploadImage(sp<Palette> palette, Vec2<int> at)
	{
		bind(0);
		assert(_textureType == TEXTURE_2D_RGBA);
		gl::PixelStorei(gl::UNPACK_ALIGNMENT, 4);
		gl::TexSubImage2D(gl::TEXTURE_2D, 0, at.x, at.y, palette->colours.size(), 1, gl::RGBA8, gl::UNSIGNED_BYTE, palette->colours.data());
	}
	
	Texture::~Texture()
	{
		gl::DeleteTextures(1, &_textureIdx);
	}
	
	void Texture::uploadImage(sp<Image> image, Vec3<int> at, bool rotate)
	{
		bind(0);
		GLenum format;
		GLImageData *imageData;
		if (!image->rendererPrivateData)
		{
			imageData = new GLImageData(image);
			image->rendererPrivateData.reset(imageData);
		}
		else
		{
			imageData = static_cast<GLImageData*>(image->rendererPrivateData.get());
		}
		switch (_textureType)
		{
		case TEXTURE_2D_ARRAY_INDEX:
			assert(imageData->ImageTag == GLImageData::IMG_INDEX);
			gl::PixelStorei(gl::UNPACK_ALIGNMENT, 1);
			format = gl::RED_INTEGER;
			break;
		case TEXTURE_2D_ARRAY_RGBA:
			assert(imageData->ImageTag == GLImageData::IMG_PALETTE || imageData->ImageTag == GLImageData::IMG_RGBA);
			gl::PixelStorei(gl::UNPACK_ALIGNMENT, 4);
			format = gl::RGBA;
			break;
		default:
			LogError("Attempted to upload image to an incompatible texture!");
		}
		switch (imageData->ImageTag)
		{
		case GLImageData::IMG_INDEX:
		{
			sp<PaletteImage> pImage = std::static_pointer_cast<PaletteImage>(image);
			PaletteImageLock pl(pImage, ImageLockUse::Read);
			if (rotate)
			{
				uint8_t* sourceImage = reinterpret_cast<uint8_t*>(pl.getData());
				up<unsigned char[]> rotatedImage(new unsigned char[image->size.x * image->size.y]);
				// FIXME: There should be a better/faster way to do it!
				for (int y = 0; y < image->size.y; y++)
				{
					for (int x = 0; x < image->size.x; x++)
					{
						rotatedImage[image->size.y - y - 1 + x * image->size.x] = sourceImage[x + image->size.x * y];
					}
				}
				gl::TexSubImage3D(gl::TEXTURE_2D_ARRAY, 0, at.x, at.y, at.z, image->size.y, image->size.x, 1, format, gl::UNSIGNED_BYTE, rotatedImage.get());
			}
			else
			{
				gl::TexSubImage3D(gl::TEXTURE_2D_ARRAY, 0, at.x, at.y, at.z, image->size.x, image->size.y, 1, format, gl::UNSIGNED_BYTE, pl.getData());
				auto error = gl::GetError();
				if (error)
				{
					LogWarning("Error during texture upload: %d", error);
				}
			}

		}
		break;
		case GLImageData::IMG_RGBA:
		{
			sp<RGBImage> rgbImage = std::static_pointer_cast<RGBImage>(image);
			RGBImageLock rl(rgbImage, ImageLockUse::Read);
			if (rotate)
			{
				Colour *sourceImage = reinterpret_cast<Colour*>(rl.getData());
				up<Colour[]> rotatedImage(new Colour[image->size.x * image->size.y]);
				// FIXME: There should be a better/faster way to do it!
				for (int y = 0; y < image->size.y; y++)
				{
					for (int x = 0; x < image->size.x; x++)
					{
						rotatedImage[image->size.y - y - 1 + x * image->size.x] = sourceImage[x + image->size.x * y];
					}
				}
				gl::TexSubImage3D(gl::TEXTURE_2D_ARRAY, 0, at.x, at.y, at.z, image->size.y, image->size.x, 1, format, gl::UNSIGNED_BYTE, rotatedImage.get());
			}
			else
			{
				gl::TexSubImage3D(gl::TEXTURE_2D_ARRAY, 0, at.x, at.y, at.z, image->size.x, image->size.y, 1, format, gl::UNSIGNED_BYTE, rl.getData());
				auto error = gl::GetError();
				if (error)
				{
					LogWarning("Error during texture upload: %d", error);
				}
			}
		}
		break;
		}
	}

	void Texture::bind(int textureUnit)
	{
		gl::ActiveTexture(gl::TEXTURE0 + textureUnit);
		GLenum binding;
		switch (_textureType)
		{
		case TEXTURE_2D_ARRAY_INDEX:
		case TEXTURE_2D_ARRAY_RGBA:
			binding = gl::TEXTURE_2D_ARRAY;
			break;
		case TEXTURE_2D_INDEX:
		case TEXTURE_2D_RGBA:
			binding = gl::TEXTURE_2D;
		}
		gl::BindTexture(binding, _textureIdx);
	}
		
}
