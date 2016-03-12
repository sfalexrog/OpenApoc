#include "CachingRenderer.h"
#include "gles3loader.h"
#include "framework/image.h"
#include "framework/palette.h"
#include "library/strings.h"
#include "framework/logger.h"
#include "GLImageData.h"

namespace OpenApoc
{
	static const char* CachedProgram_vertexSource = {
		"#version 300 es																	  \n"
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
		"	vec2 tmpPos = position;															  \n"
		"	tmpPos = tmpPos / screenSize;													  \n"
		"	tmpPos = tmpPos - vec2(0.5, 0.5);												  \n"
		"	if (flipY) gl_Position = vec4((tmpPos.x * 2.0), -(tmpPos.y * 2.0), 0.0, 1.0);	  \n"
		"	else gl_Position = vec4(((tmpPos.x * 2.0), (tmpPos.y * 2.0), 0.0, 1.0));		  \n"
		"} 																					  \n"
	};

	static const char* CachedProgram_fragmentSource = {
		"#version 300 es																			  \n"
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
		"		out_colour = texture(texRGBACache, texcoord);										  \n"
		"	}																						  \n"
		"	else																					  \n"
		"	{																						  \n"
		"		uint idx = texelFetch(texIdxCache, ivec3(texcoord.x, texcoord.y, texcoord.z), 0).r;	  \n"
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
			GLuint vShader = CreateShader(gl::VERTEX_SHADER, vertexSource);
			if (!vShader)
			{
				LogError("Failed to compile vertex shader");
				return;
			}
			GLuint fShader = CreateShader(gl::FRAGMENT_SHADER, fragmentSource);
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
		void Uniform(GLuint loc, int v) { gl::Uniform1i(loc, v); }

		void Uniform(GLuint loc, bool v) { gl::Uniform1f(loc, (v ? 1.0f : 0.0f)); }

		~Program()
		{
			if (prog)
				gl::DeleteProgram(prog);
		}
	};

	class CachedProgram : public Program
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

		CachedProgram()
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

	/**
	 *  Caching manager
	 */



	/**
	 *	Enqueues the draw call.
	 *  Actual draw calls happen in the flush() call.
	 */
	void CachingRenderer::draw(sp<Image> image, Vec2<float> position)
	{
		int vtxStart = 4 * _primitivesCount;
		GLImageData *data = static_cast<GLImageData*>(image->rendererPrivateData.get());
		if (!data)
		{
			image->rendererPrivateData.reset(new GLImageData(image));
		}

		switch (data->ImageTag)
		{
		case GLImageData::IMG_INDEX:
			// First try to cache the image
			if (data->indexData->cachedState == CacheState::CACHE_NOT_CACHED)
			{

			}
			// If it can't be cached, oh well
			if (data->indexData->cachedState == CacheState::CACHE_UNCACHEABLE)
			{
				flush();
				drawImmediate(image, position);
			}
			break;
		case GLImageData::IMG_RGBA:
			// Again, first try to cache the image
			if (data->rgbData->cachedState == CacheState::CACHE_NOT_CACHED)
			{

			}
			if (data->rgbData->cachedState == CacheState::CACHE_UNCACHEABLE)
			{
				flush();
				drawImmediate(image, position);
			}
			break;
		}

		_cachedVertices[vtxStart].position = position;
		_cachedVertices[vtxStart + 1].position = position + Vec2<float>(0, image->size.y);
		_cachedVertices[vtxStart + 2].position = position + Vec2<float>(image->size.x, 0);
		_cachedVertices[vtxStart + 3].position = position + Vec2<float>(image->size);

		switch (data->ImageTag)
		{
		case GLImageData::IMG_INDEX:
			// TODO
			break;
		case GLImageData::IMG_RGBA:
			// TODO
			break;
		}
	}

	void CachingRenderer::drawImmediate(sp<Image> image, Vec2<float> position)
	{
		// TODO
	}

	void CachingRenderer::flush()
	{

	}
	
}