#include "RendererImpl.h"
#include "gles3loader.h"
#include "framework/image.h"
#include "framework/palette.h"
#include "library/strings.h"
#include "framework/logger.h"
#include "GLImageData.h"
#include "Texture.h"
#include "FBOData.h"

#include <string>

namespace OpenApoc
{
	// Shader programs for images. Note the absence of version string - it is appended
	// in the Program class constructor.
	static const char* CachedProgram_vertexSource = {
		"layout(location = 0) in vec2 position;												  \n"
		"layout(location = 1) in vec3 texcoord_in;											  \n"
		"layout(location = 2) in int paletteIdx_in;											  \n"
		"out vec3 texcoord;																	  \n"
		"flat out int paletteIdx;															  \n"
		"uniform vec2 screenSize;															  \n"
		"uniform bool flipY;																  \n"
		"void main() {																		  \n"
		"	paletteIdx = paletteIdx_in;														  \n"
		"	texcoord = texcoord_in;															  \n"
		"   vec2 tmpPos = position * 2.0;"
		"   tmpPos.x /= screenSize.x;"
		"   tmpPos.y /= screenSize.y;"
		"   if (flipY) { gl_Position = vec4(tmpPos.x - 1.0, 1.0 - tmpPos.y, 0.0, 1.0); } \n"
		"   else { gl_Position = vec4(tmpPos.x - 1.0, tmpPos.y - 1.0, 0.0, 1.0); }\n"
		"} 																					  \n"
	};

	static const char* CachedProgram_fragmentSource = {
		"precision mediump float;																	  \n"
		"precision mediump int;																		  \n"
		"in vec3 texcoord;																			  \n"
		"flat in int paletteIdx;																	  \n"
		"uniform mediump usampler2DArray texIdxCache;												  \n"
		"uniform mediump sampler2DArray texRGBACache;												  \n"
		"uniform mediump sampler2D palette;															  \n"
		"out vec4 out_colour;																		  \n"
		"void main() {																				  \n"
		"	if (paletteIdx < 0)																		  \n"
		"	{																						  \n"
		"       ivec3 texSize = textureSize(texRGBACache, 0);										  \n"
		"       vec3 normCoord = vec3((2.0 * texcoord.x + 1.0) / (2.0 * float(texSize.x)),	          \n"
		"           (2.0 * texcoord.y + 1.0) / (2.0 * float(texSize.y)), texcoord.z);				  \n"
		"		out_colour = texture(texRGBACache, normCoord);										  \n"
		"	}																						  \n"
		"	else																					  \n"
		"	{																						  \n"
		"		uint idx = texelFetch(texIdxCache, ivec3(texcoord.x, texcoord.y, texcoord.z), 0).r;	  \n"
		"		if (idx == 0u) discard;																  \n"
		"		out_colour = texelFetch(palette, ivec2(idx, paletteIdx), 0);						  \n"
		"	}																						  \n"
		"}																							  \n"
	};

	static const char* ImmediateProgram_vertexSource = {
		"layout(location = 0) in vec2 position;												  \n"
		"layout(location = 1) in vec3 texcoord_in;											  \n"
		"layout(location = 2) in int paletteIdx_in;											  \n"
		"out vec2 texcoord;																	  \n"
		"flat out int paletteIdx;															  \n"
		"uniform vec2 screenSize;															  \n"
		"uniform bool flipY;																  \n"
		"void main() {																		  \n"
		"	paletteIdx = paletteIdx_in;														  \n"
		"	texcoord = texcoord_in.xy;													      \n"
		"   vec2 tmpPos = position * 2.0;"
		"   tmpPos.x /= screenSize.x;"
		"   tmpPos.y /= screenSize.y;"
		"   if (flipY) { gl_Position = vec4(tmpPos.x - 1.0, 1.0 - tmpPos.y, 0.0, 1.0); } \n"
		"   else { gl_Position = vec4(tmpPos.x - 1.0, tmpPos.y - 1.0, 0.0, 1.0); }\n"
		"} 																					  \n"
	};

	static const char* ImmediateProgram_fragmentSource = {
		"precision mediump float;																	  \n"
		"precision mediump int;																		  \n"
		"in vec2 texcoord;																			  \n"
		"flat in int paletteIdx;																	  \n"
		"uniform mediump usampler2D texIdx;												  \n"
		"uniform mediump sampler2D texRGBA;												  \n"
		"uniform mediump sampler2D palette;															  \n"
		"out vec4 out_colour;																		  \n"
		"void main() {																				  \n"
		"	if (paletteIdx < 0)																		  \n"
		"	{																						  \n"
		"       ivec2 texSize = textureSize(texRGBA, 0);										  \n"
		"       vec2 normCoord = vec2((2.0 * texcoord.x + 1.0) / (2.0 * float(texSize.x)), \n"
		"           (2.0 * texcoord.y + 1.0) / (2.0 * float(texSize.y)));\n"
		"		out_colour = texture(texRGBA, normCoord);										  \n"
		"	}																						  \n"
		"	else																					  \n"
		"	{																						  \n"
		"		uint idx = texelFetch(texIdx, ivec2(texcoord.x, texcoord.y), 0).r;	  \n"
		"		if (idx == 0u) discard;																  \n"
		"		out_colour = texelFetch(palette, ivec2(idx, paletteIdx), 0);						  \n"
		"	}																						  \n"
		"}																							  \n"
	};

	class Program
	{
	public:
		GLuint prog;
		static GLuint CreateShader(GLenum type, const UString source)
		{
			GLuint shader = gl::CreateShader(type);
			auto sourceString = source.str();
			const GLchar *string = sourceString.c_str();
			GLint stringLength = sourceString.length();
			gl::ShaderSource(shader, 1, &string, &stringLength);
			gl::CompileShader(shader);
			GLint compileStatus;
			gl::GetShaderiv(shader, gl::COMPILE_STATUS, &compileStatus);
			if (compileStatus == gl::TRUE_)
				return shader;

			GLint logLength;
			gl::GetShaderiv(shader, gl::INFO_LOG_LENGTH, &logLength);

			std::unique_ptr<char[]> log(new char[logLength]);
			gl::GetShaderInfoLog(shader, logLength, NULL, log.get());

			LogError("Shader compile error: %s", log.get());

			gl::DeleteShader(shader);
			return 0;
		}
		Program(const UString vertexSource, const UString fragmentSource) : prog(0)
		{
			UString GLESVersionString;

			if (std::string((const char*) gl::GetString(gl::VERSION)).find("OpenGL ES") != std::string::npos)
			{
				GLESVersionString = "#version 300 es\n";
			}
			else
			{
				GLESVersionString = "#version 330\n";
			}
			

			GLuint vShader = CreateShader(gl::VERTEX_SHADER, GLESVersionString + vertexSource);
			if (!vShader)
			{
				LogError("Failed to compile vertex shader");
				return;
			}
			GLuint fShader = CreateShader(gl::FRAGMENT_SHADER, GLESVersionString + fragmentSource);
			if (!fShader)
			{
				LogError("Failed to compile fragment shader");
				gl::DeleteShader(vShader);
				return;
			}

			prog = gl::CreateProgram();
			gl::AttachShader(prog, vShader);
			gl::AttachShader(prog, fShader);

			gl::DeleteShader(vShader);
			gl::DeleteShader(fShader);

			gl::LinkProgram(prog);

			GLint linkStatus;
			gl::GetProgramiv(prog, gl::LINK_STATUS, &linkStatus);
			if (linkStatus == gl::TRUE_)
				return;

			GLint logLength;
			gl::GetProgramiv(prog, gl::INFO_LOG_LENGTH, &logLength);

			std::unique_ptr<char[]> log(new char[logLength]);
			gl::GetProgramInfoLog(prog, logLength, NULL, log.get());

			LogError("Program link error: %s", log.get());

			gl::DeleteProgram(prog);
			prog = 0;
			return;
		}

		void Uniform(GLuint loc, Colour c)
		{
			gl::Uniform4f(loc, c.r / 255.0f, c.g / 255.0f, c.b / 255.0f, c.a / 255.0f);
		}

		void Uniform(GLuint loc, Vec2<float> v) { gl::Uniform2f(loc, v.x, v.y); }
		void Uniform(GLuint loc, Vec2<int> v)
		{
			// FIXME: Float conversion
			gl::Uniform2f(loc, v.x, v.y);
		}
		void Uniform(GLuint loc, float v) { gl::Uniform1f(loc, v); }
		void Uniform(GLuint loc, int v) 
		{ 
			assert(loc < 1024);
			gl::Uniform1i(loc, v); 
		}

		void Uniform(GLuint loc, bool v) { gl::Uniform1f(loc, (v ? 1.0f : 0.0f)); }

		~Program()
		{
			if (prog)
				gl::DeleteProgram(prog);
		}
	};

	class CachingProgram : public Program
	{
	public:
		GLint positionLoc;
		GLint texcoordLoc;
		GLint paletteIdxLoc;
		GLint texIdxLoc;
		GLint texRGBALoc;
		GLint paletteLoc;
		GLint screenSizeLoc;
		GLint flipYLoc;
		Vec2<int> currentScreenSize;
		bool currentFlipY;
		GLint currentTexIdxUnit;
		GLint currentTexRGBAUnit;
		GLint currentTexPaletteUnit;

		CachingProgram()
			: Program(CachedProgram_vertexSource, CachedProgram_fragmentSource),
			currentScreenSize(0, 0), currentFlipY(0), currentTexIdxUnit(0), currentTexRGBAUnit(0),
			currentTexPaletteUnit(0)
		{
			positionLoc = gl::GetAttribLocation(prog, "position");
			assert(positionLoc == 0);
			texcoordLoc = gl::GetAttribLocation(prog, "texcoord_in");
			assert(texcoordLoc == 1);
			paletteIdxLoc = gl::GetAttribLocation(prog, "paletteIdx_in");
			assert(paletteIdxLoc == 2);
			texIdxLoc = gl::GetUniformLocation(prog, "texIdxCache");
			if (texIdxLoc < 0) { LogError("'texIdxCache' uniform not found in shader"); }
			texRGBALoc = gl::GetUniformLocation(prog, "texRGBACache");
			if (texRGBALoc < 0) { LogError("'texRGBACache' uniform not found in shader"); }
			paletteLoc = gl::GetUniformLocation(prog, "palette");
			if (paletteLoc < 0) { LogError("'palette' uniform not found in shader"); }
			screenSizeLoc = gl::GetUniformLocation(prog, "screenSize");
			if (screenSizeLoc < 0) { LogError("'screenSize' uniform not found in shader"); }
			flipYLoc = gl::GetUniformLocation(prog, "flipY");
			if (flipYLoc < 0) { LogError("'flipY' uniform not found in shader"); }
		}
		void setUniforms(Vec2<int> screenSize, bool flipY, GLint texIdxUnit, GLint texRGBAUnit, GLint texPaletteUnit)
		{
			if (screenSize != currentScreenSize)
			{
				currentScreenSize = screenSize;
				Uniform(screenSizeLoc, screenSize);
			}
			if (texIdxUnit != currentTexIdxUnit)
			{
				currentTexIdxUnit = texIdxUnit;
				Uniform(texIdxLoc, texIdxUnit);
			}
			if (texRGBAUnit != currentTexRGBAUnit)
			{
				currentTexRGBAUnit = texRGBAUnit;
				Uniform(texRGBALoc, texRGBAUnit);
			}
			if (texPaletteUnit != currentTexPaletteUnit)
			{
				currentTexPaletteUnit = texPaletteUnit;
				Uniform(paletteLoc, texPaletteUnit);
			}
			if (flipY != currentFlipY)
			{
				currentFlipY = flipY;
				Uniform(flipYLoc, flipY);
			}
		}
	};

	class ImmediateProgram : public Program
	{
	public:
		GLint positionLoc;
		GLint texcoordLoc;
		GLint paletteIdxLoc;
		GLint texIdxLoc;
		GLint texRGBALoc;
		GLint paletteLoc;
		GLint screenSizeLoc;
		GLint flipYLoc;
		Vec2<int> currentScreenSize;
		bool currentFlipY;
		GLint currentTexIdxUnit;
		GLint currentTexRGBAUnit;
		GLint currentTexPaletteUnit;
		ImmediateProgram()
			: Program(ImmediateProgram_vertexSource, ImmediateProgram_fragmentSource),
			currentScreenSize(0, 0), currentFlipY(0), currentTexIdxUnit(0), currentTexRGBAUnit(0),
			currentTexPaletteUnit(0)
		{
			positionLoc = gl::GetAttribLocation(prog, "position");
			assert(positionLoc == 0);
			texcoordLoc = gl::GetAttribLocation(prog, "texcoord_in");
			assert(texcoordLoc == 1);
			paletteIdxLoc = gl::GetAttribLocation(prog, "paletteIdx_in");
			assert(paletteIdxLoc == 2);
			texIdxLoc = gl::GetUniformLocation(prog, "texIdx");
			if (texIdxLoc < 0) { LogError("'texIdx' uniform not found in shader"); }
			texRGBALoc = gl::GetUniformLocation(prog, "texRGBA");
			if (texRGBALoc < 0) { LogError("'texRGBA' uniform not found in shader"); }
			paletteLoc = gl::GetUniformLocation(prog, "palette");
			if (paletteLoc < 0) { LogError("'palette' uniform not found in shader"); }
			screenSizeLoc = gl::GetUniformLocation(prog, "screenSize");
			if (screenSizeLoc < 0) { LogError("'screenSize' uniform not found in shader"); }
			flipYLoc = gl::GetUniformLocation(prog, "flipY");
			if (flipYLoc < 0) { LogError("'flipY' uniform not found in shader"); }
		}
		void setUniforms(Vec2<int> screenSize, bool flipY, GLint texIdxUnit, GLint texRGBAUnit, GLint texPaletteUnit)
		{
			if (screenSize != currentScreenSize)
			{
				currentScreenSize = screenSize;
				Uniform(screenSizeLoc, screenSize);
			}
			if (texIdxUnit != currentTexIdxUnit)
			{
				currentTexIdxUnit = texIdxUnit;
				Uniform(texIdxLoc, texIdxUnit);
			}
			if (texRGBAUnit != currentTexRGBAUnit)
			{
				currentTexRGBAUnit = texRGBAUnit;
				Uniform(texRGBALoc, texRGBAUnit);
			}
			if (texPaletteUnit != currentTexPaletteUnit)
			{
				currentTexPaletteUnit = texPaletteUnit;
				Uniform(paletteLoc, texPaletteUnit);
			}
			if (flipY != currentFlipY)
			{
				currentFlipY = flipY;
				Uniform(flipYLoc, flipY);
			}
		}
	};


	/**
	 *  Caching manager
	 */
	class TextureCache
	{
		struct Rectangle
		{
			int left, top, right, bottom;
			Rectangle() : top(0), bottom(0), left(0), right(0) {}
			Rectangle(int l, int t, int r, int b) : top(t), left(l), right(r), bottom(b) {}
		};

		struct Node {
			std::unique_ptr<Node> child[2];
			Rectangle rc;
			sp<Image> prect;
			sp<Texture> targetTexture;
			int page;

			enum fitting {
				DOES_NOT_FIT,
				FITS_PERFECTLY,
				FITS_PERFECTLY_ROTATED,
				FITS_WITH_SPACE,
				FITS_WITH_SPACE_ROTATED
			};

			fitting fits(sp<Image> img)
			{
				int rcWidth = rc.right - rc.left;
				int rcHeight = rc.bottom - rc.top;
				int imWidth = img->size.x;
				int imHeight = img->size.y;
				if ((rcWidth == imWidth) && (rcHeight == imHeight))
				{
					return FITS_PERFECTLY;
				}
				if ((rcWidth == imHeight) && (rcHeight == imWidth))
				{
					return FITS_PERFECTLY_ROTATED;
				}
				if ((rcWidth >= imWidth) && (rcHeight >= imHeight))
				{
					return FITS_WITH_SPACE;
				}
				if ((rcWidth >= imHeight) && (rcHeight >= imWidth))
				{
					return FITS_WITH_SPACE_ROTATED;
				}
				return DOES_NOT_FIT;
			}

			Node(Rectangle freeSpace) :rc(freeSpace) {}

			Node* insert(sp<Image> img)
			{
				if (prect == img)
				{
					return this;
				}
				// We are not in a leaf
				if (child[0] || child[1])
				{
					/*if (!prect.lock())
					{
						LogWarning("Cache discrepancy: cache entry exists for nonexistent entry!");
						// Try to salvage the situation by uploading the image here
						auto fit = fits(img);
						if (fit != DOES_NOT_FIT)
						{
							prect = img;
							if (fit == FITS_PERFECTLY_ROTATED || fit == FITS_WITH_SPACE_ROTATED)
							{
								targetTexture->uploadImage(img, Vec3<int>(rc.top, rc.left, page), true);
							}
							else
							{
								targetTexture->uploadImage(img, Vec3<int>(rc.top, rc.left, page));
							}
							if (fit == FITS_WITH_SPACE || fit == FITS_WITH_SPACE_ROTATED)
							{
								LogWarning("Uploaded smaller image than possible (possible waste of space)");
							}
							return this;
						}
					}*/
					auto newNode = child[1]->insert(img);
					if (newNode != nullptr) { return newNode; }
					return child[0]->insert(img);
				}
				else
				{
					// There is already an image there
					if (prect)
					{
						return nullptr;
					}
					auto fit = fits(img);
					
					switch (fit)
					{
					case DOES_NOT_FIT:
						// There is not enough space to fit the image
						return nullptr;
					case FITS_PERFECTLY:
						// Perfect fit: just upload the image!
						prect = img;
						targetTexture->uploadImage(img, Vec3<int>(rc.left , rc.top, page));
						return this;
					case FITS_PERFECTLY_ROTATED:
						prect = img;
						targetTexture->uploadImage(img, Vec3<int>(rc.left, rc.top, page), true);
						return this;
					case FITS_WITH_SPACE:
						{
							int dw = (rc.right - rc.left) - img->size.x;
							int dh = (rc.bottom - rc.top) - img->size.y;
							if (dw > dh)
							{
								child[0].reset(new Node(Rectangle(rc.left, rc.top, rc.left + img->size.x, rc.bottom)));
								child[1].reset(new Node(Rectangle(rc.left + img->size.x, rc.top, rc.right, rc.bottom)));
							}
							else
							{
								child[0].reset(new Node(Rectangle(rc.left, rc.top, rc.right, rc.top + img->size.y)));
								child[1].reset(new Node(Rectangle(rc.left, rc.top + img->size.y, rc.right, rc.bottom)));
							}
							child[0]->page = page;
							child[0]->targetTexture = targetTexture;
							child[1]->page = page;
							child[1]->targetTexture = targetTexture;
							return child[0]->insert(img);
						}
					case FITS_WITH_SPACE_ROTATED:
						{
							// This one is a bit tricky: we can't rotate the image just yet,
							// so if the image needs to be rotated, we just make the split
							// in such a way that it will FIT_PERFECTLY_ROTATED.
							int sizeX = img->size.y;
							int sizeY = img->size.x;
							int dw = (rc.right - rc.left) - sizeX;
							int dh = (rc.bottom - rc.top) - sizeY;
							if (dw > dh)
							{
								child[0].reset(new Node(Rectangle(rc.left, rc.top, rc.left + sizeX, rc.bottom)));
								child[1].reset(new Node(Rectangle(rc.left + sizeX, rc.top, rc.right, rc.bottom)));
							}
							else
							{
								child[0].reset(new Node(Rectangle(rc.left, rc.top, rc.right, rc.top + sizeY)));
								child[1].reset(new Node(Rectangle(rc.left, rc.top + sizeY, rc.right, rc.bottom)));
							}
							// Make sure we're all on the same page
							child[0]->page = page;
							child[0]->targetTexture = targetTexture;
							child[1]->page = page;
							child[1]->targetTexture = targetTexture;
							return child[0]->insert(img);
						}
					}
					// We should never end up here, but hey, just in case
					return nullptr;
				}
			}

			~Node()
			{
			}
		};

		std::vector<Node*> _pages;
		sp<Texture> _cacheTexture;
		Vec2<int> _pageSize;
		int _maxPages;
	public:
		TextureCache(sp<Texture> targetTexture) : _cacheTexture(targetTexture)
		{
			auto texSize = _cacheTexture->size();
			_pageSize = Vec2<int>(texSize.x, texSize.y);
			_maxPages = texSize.z;
			assert(_maxPages > 0);
			_pages = std::vector<Node*>(_maxPages);
			// Pre-create root nodes
			int pageIdx = 0;
			for (auto &page : _pages)
			{
				page = new Node(Rectangle(0, 0, _pageSize.x - 1, _pageSize.y - 1));
				page->page = pageIdx++;
				page->targetTexture = _cacheTexture;
			}
		}

		int putImage(sp<Image> img)
		{
			// Only cache relatively small images (we will run out of space very quickly otherwise)
			int maxCacheableSize = std::min(Texture::Limits::MAX_TEXTURE_WIDTH / 4, 256u);
			bool isCacheable = (img->size.x < maxCacheableSize) && (img->size.y < maxCacheableSize);

			int pageNum = 0;
			GLImageData *imageData = static_cast<GLImageData*>(img->rendererPrivateData.get());
			if (!imageData)
			{
				img->rendererPrivateData.reset(new GLImageData(img));
			}
			// Mark "large" images as uncacheable
			if (!isCacheable)
			{
				imageData->setCachedState(CacheState::CachedState::CACHE_UNCACHEABLE);
			}
			switch (imageData->getCacheState())
			{
				case CacheState::CachedState::CACHE_CACHED:
					return imageData->texelCoords[0].z;
				case CacheState::CachedState::CACHE_UNCACHEABLE:
					return -1;
				default:
					break;
			}
			while (!_pages[pageNum]->insert(img))
			{
				pageNum++;
				if (pageNum == _pages.size())
				{
					LogWarning("Could not place image into cache: cache is full");
					imageData->setCachedState(CacheState::CachedState::CACHE_UNCACHEABLE);
					return -1;
				}
			}
			imageData->setCachedState(CacheState::CachedState::CACHE_CACHED);
			return pageNum;
		}
		~TextureCache()
		{
			for (auto& page : _pages)
			{
				delete page;
			}
		}

	};

	void RendererImpl::setSurface(sp<Surface> s)
	{
		if (currentSurface == s)
		{
			return;
		}
		// Make sure we're not supposed to render anything
		flushCache();
		currentSurface = s;
		if (!s->rendererPrivateData)
		{
			s->rendererPrivateData.reset(new GLImageData(s));
		}
		GLImageData *surfaceData = static_cast<GLImageData*>(s->rendererPrivateData.get());
		FBOData *fbo = surfaceData->surfaceData->fboData.get();
		gl::BindFramebuffer(gl::FRAMEBUFFER, fbo->fbo);
		currentBoundFBO = fbo->fbo;
		gl::Viewport(0, 0, s->size.x, s->size.y);
	}

	void RendererImpl::enqueue(const SpriteData& data)
	{
		for (int i = 0; i < 4; i++)
		{
			_mappedVertices[_mappedCurrent + i].position = data.vertex[i].position;
			_mappedVertices[_mappedCurrent + i].texCoord = data.vertex[i].texCoord;
			_mappedVertices[_mappedCurrent + i].paletteIdx = data.vertex[i].paletteIdx;
		}
		_mappedCurrent += 4;
		_elemCount += 5;
	}

	RendererImpl::SpriteData RendererImpl::transform(sp<Image> image, Vec2<float> position, Vec2<float> size, Vec2<float> center, float angle)
	{
		SpriteData data;
		// WARNING: This assumes the image already has its renderer-specific data set
		GLImageData *imageData = static_cast<GLImageData*>(image->rendererPrivateData.get());
		if (!imageData)
		{
			image->rendererPrivateData.reset(new GLImageData(image));
			imageData = static_cast<GLImageData*>(image->rendererPrivateData.get());
		}
		GLImageData *paletteData = static_cast<GLImageData*>(currentPalette->rendererPrivateData.get());
		int paletteIdx;
		if (paletteData)
		{
			paletteIdx = (imageData->ImageTag == GLImageData::IMG_INDEX) ? paletteData->paletteData->index : -1;
		}
		else
		{
			paletteIdx = -1;
		}
			
		for (int i = 0; i < 4; i++)
		{
			data.vertex[i].texCoord = imageData->texelCoords[i];
			data.vertex[i].paletteIdx = paletteIdx;
		}

		if (angle != 0.0f)
		{
			data.vertex[0].position = Vec2<float>(0.0f, 0.0f);
			data.vertex[1].position = Vec2<float>(size.x, 0.0f);
			data.vertex[2].position = Vec2<float>(0.0f, size.y);
			data.vertex[3].position = Vec2<float>(size.x, size.y);
			auto rotMatrix = glm::rotate(angle, Vec3<float>(0.0f, 0.0f, 1.0f));
			for (auto &v : data.vertex)
			{
				v.position -= center;
				auto transformed = rotMatrix * glm::vec4(v.position.x, v.position.y, 0.0f, 1.0f);
				v.position.x = transformed.x;
				v.position.y = transformed.y;
				v.position += center;
				v.position += position;
			}
		}
		else
		{
			data.vertex[0].position = position;
			data.vertex[1].position = position + Vec2<float>(size.x, 0.0f);
			data.vertex[2].position = position + Vec2<float>(0.0f, size.y);
			data.vertex[3].position = position + Vec2<float>(size.x, size.y);
		}
		return data;
	}

	/**
	 *	Attempt to place image in cache, returning true if it's successful
	 *  This function will also create rendererPrivateData for the image if it's missing
	 *  for some reason.
	 */
	bool RendererImpl::cacheImage(sp<Image> image)
	{
		GLImageData *data = static_cast<GLImageData*>(image->rendererPrivateData.get());
		if (!data)
		{
			image->rendererPrivateData.reset(new GLImageData(image));
			data = static_cast<GLImageData*>(image->rendererPrivateData.get());
		}
		if (data->getCacheState() == CacheState::CACHE_CACHED) return true;
		if (data->getCacheState() == CacheState::CACHE_UNCACHEABLE) return false;

		// Put the image into the appropriate cache
		switch (data->ImageTag)
		{
		case GLImageData::IMG_INDEX:
			_idxCache->putImage(image);
			break;
		case GLImageData::IMG_RGBA:
			_rgbaCache->putImage(image);
			break;
		case GLImageData::IMG_SURFACE:
			// Surfaces don't go into cache
			return false;
			break;
		}
		return data->getCacheState() == CacheState::CACHE_CACHED;
	}


	/**
	 *	Cache size limits
	 */
	static const int INDEX_TEXTURE_MULTIPLIER = 2;
	static const int MAX_PALETTE_SIZE = 256;
	static const int MAX_PALETTES_COUNT = 1024;
	// Number of cached vertices; each primitive takes 4 vertices, so it's a grand total of 16380 cached primitives.
	static const int MAX_CACHED_VERTICES = 65520;
	// Each primitive requires 4 indices and a PRIMITIVE_RESTART
	static const int MAX_CACHED_INDICES = MAX_CACHED_VERTICES * 5 / 4;
	static const int PRIMITIVE_RESTART_INDEX = 65535;

	RendererImpl::RendererImpl()
	{
		GLenum error = gl::GetError(); assert(error == 0);
		// Prepare cache textures
		int scratchTexSize = std::min(Texture::Limits::MAX_TEXTURE_WIDTH, 1024u);
		_cachingProgram.reset(new CachingProgram());
		_immediateProgram.reset(new ImmediateProgram());
		error = gl::GetError(); assert(error == 0);
		_idxTexture = Texture::createIndexTexture(Vec3<int>(
			Texture::Limits::MAX_TEXTURE_WIDTH, 
			Texture::Limits::MAX_TEXTURE_HEIGHT, 
			Texture::Limits::MAX_TEXTURE_DEPTH * INDEX_TEXTURE_MULTIPLIER));
		_rgbaTexture = Texture::createRGBATexture(Vec3<int>(
			Texture::Limits::MAX_TEXTURE_WIDTH,
			Texture::Limits::MAX_TEXTURE_HEIGHT,
			Texture::Limits::MAX_TEXTURE_DEPTH));
		//_paletteTexture = Texture::createRGBATexture(Vec2<int>(MAX_PALETTE_SIZE, MAX_PALETTES_COUNT));
		_scratchIndexTexture = Texture::createIndexTexture(Vec2<int>(scratchTexSize, scratchTexSize));
		_scratchRGBATexture = Texture::createRGBATexture(Vec2<int>(scratchTexSize, scratchTexSize));

		_idxCache.reset(new TextureCache(_idxTexture));
		_rgbaCache.reset(new TextureCache(_rgbaTexture));

		error = gl::GetError(); assert(error == 0);
		// Enable required options
#ifdef OPENAPOC_GL32_RENDER
		// TODO: Since the renderer will probably use mostly a common subset of GLES3 and GL3.2,
		// check if these parameters are working
		gl::Enable(gl::PRIMITIVE_RESTART);
		gl::PrimitiveRestartIndex(PRIMITIVE_RESTART_INDEX);
#else
		gl::Enable(gl::PRIMITIVE_RESTART_FIXED_INDEX);
#endif
		error = gl::GetError(); assert(error == 0);

		// Create default rendering surface

		GLint viewport[4];
		gl::GetIntegerv(gl::VIEWPORT, viewport);
		assert(viewport[0] == 0 && viewport[1] == 0);
		LogInfo("Viewport {%d,%d,%d,%d}", viewport[0], viewport[1], viewport[2], viewport[3]);

		gl::Enable(gl::BLEND);
		gl::BlendFunc(gl::SRC_ALPHA, gl::ONE_MINUS_SRC_ALPHA);

		// Generate buffers for our caches
		gl::GenVertexArrays(1, &cacheVAO);
		error = gl::GetError(); assert(error == 0);

		gl::BindVertexArray(cacheVAO);
		gl::GenBuffers(1, &cacheVBO);
		gl::BindBuffer(gl::ARRAY_BUFFER, cacheVBO);
		error = gl::GetError(); assert(error == 0);

		gl::BufferData(gl::ARRAY_BUFFER, sizeof(VertexData) * MAX_CACHED_VERTICES, nullptr, gl::STREAM_DRAW);
		_mappedVertices = reinterpret_cast<VertexData*>(gl::MapBufferRange(
			gl::ARRAY_BUFFER, 0, sizeof(VertexData) * MAX_CACHED_VERTICES, 
			gl::MAP_WRITE_BIT | gl::MAP_INVALIDATE_RANGE_BIT | gl::MAP_INVALIDATE_BUFFER_BIT | gl::MAP_FLUSH_EXPLICIT_BIT));

		gl::GenBuffers(1, &elementsVBO);
		gl::BindBuffer(gl::ELEMENT_ARRAY_BUFFER, elementsVBO);
		gl::BufferData(gl::ELEMENT_ARRAY_BUFFER, sizeof(GLshort) * MAX_CACHED_INDICES, nullptr, gl::STATIC_DRAW);
		_mappedElements = reinterpret_cast<GLushort*>(gl::MapBufferRange(
			gl::ELEMENT_ARRAY_BUFFER, 0, sizeof(GLshort) * MAX_CACHED_INDICES,
			gl::MAP_WRITE_BIT));
		error = gl::GetError(); assert(error == 0);

		for (int i = 0; i < (MAX_CACHED_INDICES / 5); i++)
		{
			for (GLushort v = 0; v < 4; v++)
			{
				_mappedElements[5 * i + v] = GLushort(4 * i +  v);
			}
			_mappedElements[5 * i + 4] = PRIMITIVE_RESTART_INDEX;
		}

		gl::UnmapBuffer(gl::ELEMENT_ARRAY_BUFFER);

		gl::EnableVertexAttribArray(0);
		gl::EnableVertexAttribArray(1);
		gl::EnableVertexAttribArray(2);
		error = gl::GetError(); assert(error == 0);

		gl::VertexAttribPointer(0, 2, gl::FLOAT, gl::FALSE_, sizeof(VertexData), 0);
		gl::VertexAttribPointer(1, 3, gl::FLOAT, gl::FALSE_, sizeof(VertexData), reinterpret_cast<void*>(sizeof(Vec2<float>)));
		gl::VertexAttribIPointer(2, 1, gl::INT, sizeof(VertexData), reinterpret_cast<void*>(sizeof(Vec2<float>) + sizeof(Vec3<float>)));
		error = gl::GetError(); assert(error == 0);

		_mappedBegin = 0;
		_mappedCurrent = 0;
		_mapOffset = 0;
		_mappedEnd = MAX_CACHED_VERTICES;
		_elemOffset = 0;
		_elemCount = 0;

		defaultSurface = mksp<Surface>(Vec2<int>(viewport[2], viewport[3]));
		defaultSurface->rendererPrivateData.reset(new GLImageData(defaultSurface, true));
		setSurface(defaultSurface);

		error = gl::GetError(); assert(error == 0);
	}

	void RendererImpl::clear(Colour c)
	{
		gl::ClearColor(c.r / 255.0, c.g / 255.0, c.b / 255.0, c.a / 255.0);
		gl::Clear(gl::COLOR_BUFFER_BIT | gl::DEPTH_BUFFER_BIT);
		auto error = gl::GetError();
		assert(error == 0);
	}


	/**
	 *	Enqueues the draw call.
	 *  Actual draw calls happen in the flush() call.
	 */
	void RendererImpl::draw(sp<Image> image, Vec2<float> position)
	{
		if (cacheImage(image))
		{
			SpriteData data = transform(image, position, image->size);
			enqueue(data);
			if (_mappedCurrent == _mappedEnd) flushCache();
		}
		else
		{
			SpriteData data = transform(image, position, image->size);
			flushCache();
			drawImmediate(image, data);
		}
		auto error = gl::GetError();
		assert(error == 0);
	}

	void RendererImpl::setPalette(sp<Palette> p)
	{
		if (!p->rendererPrivateData)
		{
			p->rendererPrivateData.reset(new GLImageData(p));
		}
		currentPalette = p;
		auto error = gl::GetError();
		assert(error == 0);
	}

	void RendererImpl::drawRotated(sp<Image> i, Vec2<float> center, Vec2<float> position, float angle)
	{
		if (!i->rendererPrivateData)
		{
			i->rendererPrivateData.reset(new GLImageData(i));
		}
		auto error = gl::GetError();
		assert(error == 0);
		SpriteData data = transform(i, position, i->size, center, angle);
		if (cacheImage(i))
		{
			enqueue(transform(i, position, i->size, center, angle));
			if (_mappedCurrent == _mappedEnd) flushCache();
		}
		else
		{
			flushCache();
			drawImmediate(i, data);
		}
		error = gl::GetError(); assert(error == 0);
	}

	void RendererImpl::drawScaled(sp<Image> i, Vec2<float> position, Vec2<float> size, Renderer::Scaler scaler)
	{
		if (!i->rendererPrivateData)
		{
			i->rendererPrivateData.reset(new GLImageData(i));
		}
		auto error = gl::GetError();
		assert(error == 0);
		SpriteData data = transform(i, position, size);
		if (cacheImage(i))
		{
			enqueue(data);
			if (_mappedCurrent == _mappedEnd) flushCache();
		}
		else
		{
			flushCache();
			drawImmediate(i, data);
		}
		error = gl::GetError(); assert(error == 0);
	}

	sp<Palette> RendererImpl::getPalette()
	{
		return currentPalette;
	}

	void RendererImpl::flush()
	{
		flushCache();
	}

	void RendererImpl::drawImmediate(sp<Image> image, SpriteData data)
	{
		GLImageData *gdata = static_cast<GLImageData*>(image->rendererPrivateData.get());
		switch (gdata->ImageTag)
		{
		case GLImageData::IMG_RGBA:
			drawImmediate(std::static_pointer_cast<RGBImage>(image), gdata->rgbData, data);
			break;
		case GLImageData::IMG_INDEX:
			drawImmediate(std::static_pointer_cast<PaletteImage>(image), gdata->indexData, data);
			break;
		case GLImageData::IMG_SURFACE:
			drawImmediate(std::static_pointer_cast<Surface>(image), gdata->surfaceData, data);
			break;
		}
	}

	void RendererImpl::drawImmediate(sp<RGBImage> image, RGBData *rdata, SpriteData data)
	{
		_scratchRGBATexture->uploadImage(image, Vec2<int>(0, 0));
		auto error = gl::GetError();
		assert(error == 0);
		GLImageData *gdata = static_cast<GLImageData*>(image->rendererPrivateData.get());
		for (int i = 0; i < 4; i++)
		{
			data.vertex[i].texCoord = gdata->texelCoords[i];
		}
		assert(_mappedCurrent == 0);
		_scratchRGBATexture->bind(1);
		drawImmediateExecute(data);
		error = gl::GetError(); assert(error == 0);
	}

	void RendererImpl::drawImmediate(sp<PaletteImage> image, IndexData *idata, SpriteData data)
	{
		_scratchIndexTexture->uploadImage(image, Vec2<int>(0, 0));
		auto error = gl::GetError();
		assert(error == 0);
		GLImageData *gdata = static_cast<GLImageData*>(image->rendererPrivateData.get());
		for (int i = 0; i < 4; i++)
		{
			data.vertex[i].texCoord = gdata->texelCoords[i];
		}
		assert(_mappedCurrent == 0);
		drawImmediateExecute(data);
		error = gl::GetError(); assert(error == 0);
	}

	void RendererImpl::drawImmediate(sp<Surface> image, SurfaceData *sdata, SpriteData data)
	{
		assert(_mappedCurrent == 0);
		auto error = gl::GetError();
		assert(error == 0);

		data.vertex[0].texCoord = Vec3<float>(0, 0, 0);
		data.vertex[1].texCoord = Vec3<float>(image->size.x - 1, 0, 0);
		data.vertex[2].texCoord = Vec3<float>(0, image->size.y - 1, 0);
		data.vertex[3].texCoord = Vec3<float>(image->size.x - 1, image->size.y - 1, 0);

		sdata->fboData->tex->bind(1);
		if (PaletteDataExt::paletteTexture)
			PaletteDataExt::paletteTexture->bind(2);
		assert(_mappedCurrent == 0);
		drawImmediateExecute(data);
		error = gl::GetError(); assert(error == 0);
	}

	void RendererImpl::drawImmediateExecute(SpriteData data)
	{
		auto error = gl::GetError();
		assert(error == 0);
		gl::UseProgram(_immediateProgram->prog);
		error = gl::GetError(); assert(error == 0);
		sp<ImmediateProgram> iProgram = std::static_pointer_cast<ImmediateProgram>(_immediateProgram);
		_scratchIndexTexture->bind(0);
		if (PaletteDataExt::paletteTexture)
			PaletteDataExt::paletteTexture->bind(2);
		bool flip = currentBoundFBO != 0;
		iProgram->setUniforms(currentSurface->size, true, 0, 1, 2);
		error = gl::GetError(); assert(error == 0);
		enqueue(data);
		gl::FlushMappedBufferRange(gl::ARRAY_BUFFER, /*_mapOffset * sizeof(VertexData)*/ 0, _mappedCurrent * sizeof(VertexData));
		error = gl::GetError(); assert(error == 0);
		gl::UnmapBuffer(gl::ARRAY_BUFFER);
		error = gl::GetError(); assert(error == 0);
		gl::DrawElements(gl::TRIANGLE_STRIP, _elemCount, gl::UNSIGNED_SHORT, reinterpret_cast<void*>(_elemOffset * sizeof(GLushort)));
		error = gl::GetError(); assert(error == 0);
		_elemOffset += _elemCount;
		_elemCount = 0;
		_mapOffset += _mappedCurrent;
		_mappedEnd -= _mappedCurrent;
		_mappedCurrent = 0;
		if (_mapOffset + 4096 >= MAX_CACHED_VERTICES)
		{
			gl::BufferData(gl::ARRAY_BUFFER, sizeof(VertexData) * MAX_CACHED_VERTICES, nullptr, gl::STREAM_DRAW);
			_mappedEnd = MAX_CACHED_VERTICES;
			_mapOffset = 0;
			_elemOffset = 0;
		}
		_mappedVertices = (VertexData*)gl::MapBufferRange(gl::ARRAY_BUFFER, sizeof(VertexData) * _mapOffset, sizeof(VertexData) * _mappedEnd, gl::MAP_WRITE_BIT | gl::MAP_INVALIDATE_RANGE_BIT | gl::MAP_FLUSH_EXPLICIT_BIT);
		error = gl::GetError(); assert(error == 0);
	}

	/**
	 *	Perform the actual draw calls.
	 *  This will temporarily unbind the buffers, execute the draw calls and rebind the buffers,
	 *  orphaning if necessary.
	 */
	void RendererImpl::flushCache()
	{
		// Do not perform empty flushes
		if (_mappedCurrent == 0) return;
		sp<CachingProgram> program = std::static_pointer_cast<CachingProgram>(_cachingProgram);
		gl::UseProgram(program->prog);
		auto error = gl::GetError(); assert(error == 0);
		gl::FlushMappedBufferRange(gl::ARRAY_BUFFER, /*_mapOffset * sizeof(VertexData)*/ 0, sizeof(VertexData) * _mappedCurrent);
		error = gl::GetError(); assert(error == 0);
		gl::UnmapBuffer(gl::ARRAY_BUFFER);
		error = gl::GetError(); assert(error == 0);
		_idxTexture->bind(0);
		_rgbaTexture->bind(1);
		//_paletteTexture->bind(2);
		if (PaletteDataExt::paletteTexture)
		{
			PaletteDataExt::paletteTexture->bind(2);
		}
		bool flip = currentBoundFBO != 0;
		program->setUniforms(currentSurface->size, false, 0, 1, 2);

		gl::DrawElements(gl::TRIANGLE_STRIP, _elemCount, gl::UNSIGNED_SHORT, reinterpret_cast<void*>(_elemOffset * sizeof(GLushort)));
		error = gl::GetError(); assert(error == 0);
		_elemOffset += _elemCount;
		_elemCount = 0;
		_mappedEnd -= _mappedCurrent;
		_mappedBegin = 0;
		_mapOffset += _mappedCurrent;
		_mappedCurrent = 0;
		// Orphan buffer and request a new one if this one is running low on elements
		if (_mapOffset + 4096 >= MAX_CACHED_VERTICES)
		{
			gl::BufferData(gl::ARRAY_BUFFER, sizeof(VertexData) * MAX_CACHED_VERTICES, nullptr, gl::STREAM_DRAW);
			_mappedEnd = MAX_CACHED_VERTICES;
			_mapOffset = 0;
			_elemOffset = 0;
		}
		_mappedVertices = (VertexData*) gl::MapBufferRange(gl::ARRAY_BUFFER, sizeof(VertexData) * _mapOffset, sizeof(VertexData) * _mappedEnd, gl::MAP_WRITE_BIT | gl::MAP_INVALIDATE_RANGE_BIT | gl::MAP_FLUSH_EXPLICIT_BIT);
	}
	
}