#pragma once

#include "library/vec.h"
#include "library/sp.h"
#include "gles3loader.h"

namespace OpenApoc
{

	class Image;
	class Palette;

	class Texture
	{
	public:
		enum FilterType {
			FT_NEAREST,
			FT_LINEAR
		};
	private:
		enum TexType {
			TEXTURE_2D_INDEX,
			TEXTURE_2D_ARRAY_INDEX,
			TEXTURE_2D_RGBA,
			TEXTURE_2D_ARRAY_RGBA
		} _textureType;

		
		GLuint _textureIdx;
		Vec3<int> _size;
		
	public:
		Texture(Vec2<int> size, TexType textureType, FilterType filterType = FT_NEAREST);
		Texture(Vec3<int> size, TexType textureType, FilterType filterType = FT_NEAREST);
		// Create a 2D texture for an indexed (paletted) image
		static sp<Texture> createIndexTexture(Vec2<int> size);
		// Create a 2D texture array for an indexed (paletted) image
		static sp<Texture> createIndexTexture(Vec3<int> size);
		// Create a 2D texture for RGBA (32-bit) image
		static sp<Texture> createRGBATexture(Vec2<int> size, FilterType filterType = FT_NEAREST);
		// Create a 2D texture array for RGBA (32-bit) image
		static sp<Texture> createRGBATexture(Vec3<int> size, FilterType filterType = FT_NEAREST);
		// Upload the image to a 2D texture, rotating if needed
		void uploadImage(sp<Image> image, Vec2<int> at, bool rotate = false);
		// Upload the image to a 2D array texture, rotating if needed
		void uploadImage(sp<Image> image, Vec3<int> at, bool rotate = false);
		// Upload the palette to a palette texture
		void uploadImage(sp<Palette> palette, Vec2<int> at);
		// Bind this texture to a texture unit
		void bind(int textureUnit);
		// Get this texture's index
		GLuint id() { return _textureIdx; };
		~Texture();
		// Texture limits (determined experimentally at the start)
		class Limits
		{
		public:
			static GLuint MAX_TEXTURE_WIDTH;
			static GLuint MAX_TEXTURE_HEIGHT;
			static GLuint MAX_TEXTURE_DEPTH;
		};
	};
}
