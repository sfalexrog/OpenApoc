#include "minigload.h"

#if !USE_LOADER
#define GL_GLEXT_PROTOTYPES
#endif
#include "gldefs/gl2.h"
#include "gldefs/gl2ext.h"

#include <SDL.h>
#include <string>
#include <sstream>
#include <unordered_set>
#include <assert.h>

static bool initialized = false;
static bool extsLoaded = false;

static bool extsFetched = false;
static std::unordered_set<std::string> extensions;

static int versionMajor = -1;
static int versionMinor = -1;

#define PTRTYPE(name) PFN ## name ## PROC

namespace gl
{
#if USE_LOADER
#define GLAPIPROC(ret, name, params, ptrtype) typedef ret (GL_APIENTRY *  PTRTYPE(name)) params; PTRTYPE(name) name = 0;
#define GLAPIFUNC(ret, name, params, ptrtype) typedef ret (GL_APIENTRY *  PTRTYPE(name)) params; PTRTYPE(name) name = 0;
#include "gldefs/gl2.h.inl"
#else
#define GLAPIPROC(ret, name, params, ptrtype) ptrtype name = gl ## name;
#define GLAPIFUNC(ret, name, params, ptrtype) ptrtype name = gl ## name;
#include "gldefs/gl2.h.inl"
#endif

#undef GLAPIPROC
#undef GLAPIFUNC
	namespace ext {
#define GLAPIPROCEXT(ret, name, params, ptrtype, extname) typedef ret (GL_APIENTRY *  PTRTYPE(name)) params; PTRTYPE(name) name = 0;
#define GLAPIFUNCEXT(ret, name, params, ptrtype, extname) typedef ret (GL_APIENTRY *  PTRTYPE(name)) params; PTRTYPE(name) name = 0;
#include "gldefs/gl2ext.h.inl"
#undef GLAPIPROCEXT
#undef GLAPIFUNCEXT
	}

	namespace sys {
		bool loadGLCore()
		{
			if (initialized) { return initialized; }
			int failed = 0;
#ifdef USE_LOADER
// A word of warning: the EGL (or OpenGL, for that matter) specification is not required to pass NULL if a procedure is not found.
#define GLAPIPROC(ret, name, params, ptrtype) do { name = (ptrtype) SDL_GL_GetProcAddress("gl" #name); if (!gl::name) {failed++;} } while( 0 );
#define GLAPIFUNC(ret, name, params, ptrtype) do { name = (ptrtype) SDL_GL_GetProcAddress("gl" #name); if (!gl::name) {failed++;} } while( 0 );
#include "gldefs/gl2.h.inl"
#else
// TODO: write non-loader functions?
#endif
			initialized = failed == 0;

			if (initialized)
			{
				std::stringstream verSS((const char*)gl::GetString(gl::VERSION));
				std::string OpenGlMagic;
				std::string EsMagic;
				char dotMagic;
				verSS >> OpenGlMagic >> EsMagic >> versionMajor >> dotMagic >> versionMinor;
				assert(OpenGlMagic == "OpenGL");
				assert(EsMagic == "ES");
			}
			return initialized;
		}

		bool IsVersionGEQ(int major, int minor)
		{
			if (versionMajor > major) return true;
			if (versionMinor < minor) return false;
			if (versionMinor >= minor) return true;
			return false;
		}

		std::unordered_set<std::string> loadExtensions()
		{
			if (!initialized)
			{
				loadGLCore();
			}
			if (extsFetched)
			{
				return extensions;
			}
			std::string extString((const char*)gl::GetString(gl::EXTENSIONS));
			int start = 0;
			int end;
			while ( (end = extString.find(" ", start)) != std::string::npos)
			{
				extensions.insert(extString.substr(start, end - start));
				start = end + 1;
			}
#define GLAPIPROCEXT(ret, name, params, ptrtype, extname) do { if (extensions.find(extname) != extensions.end()) { ext::name = (ptrtype) SDL_GL_GetProcAddress("gl" #name);}	} while(0);
#define GLAPIFUNCEXT(ret, name, params, ptrtype, extname) do { if (extensions.find(extname) != extensions.end()) { ext::name = (ptrtype) SDL_GL_GetProcAddress("gl" #name);}	} while(0);
#include "gldefs/gl2ext.h.inl"
#undef GLAPIPROCEXT
#undef GLAPIFUNCEXT
			return extensions;
		}
		std::unordered_set<std::string> getExtensions()
		{
			if (!extsFetched)
			{
				loadExtensions();
			}
			return extensions;
		}

		bool isExtensionSupported(const char* extName)
		{
			if (!extsFetched)
			{
				loadExtensions();
			}
			return extensions.find(extName) != extensions.end();
		}

	}
}
