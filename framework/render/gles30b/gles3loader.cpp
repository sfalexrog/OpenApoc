#include "gles3loader.h"

#ifdef OPENAPOC_GL32_RENDER
#include "gl32/gl_core_3_2.inl"
#else

#if !defined(OPENAPOC_USE_GLES_LOADER)
#define GL_GLEXT_PROTOTYPES
#endif

#ifdef OPENAPOC_SANE_PLATFORM
#include <GLES3/gl2.h>
#include <GLES3/gl3.h>
#else
#include "gldefs/platformdefs.h"
#endif

#include <SDL.h>
#include <string>
#include <unordered_set>

static bool initialized;

namespace gl
{
#if defined(OPENAPOC_USE_GLES_LOADER)
#define GLAPIPROC(ret, name, params, ptrtype) typedef ret (GL_APIENTRY *  ptrtype) params; ptrtype name = 0;
#define GLAPIFUNC(ret, name, params, ptrtype) typedef ret (GL_APIENTRY *  ptrtype) params; ptrtype name = 0;
#include "gldefs/gl3.h.inl"
#undef GLAPIPROC
#undef GLAPIFUNC
#else
#define GLAPIPROC(ret, name, params, ptrtype) ptrtype name = gl ## name;
#define GLAPIFUNC(ret, name, params, ptrtype) ptrtype name = gl ## name;
#include "gldefs/gl2.h.inl"
#endif
	namespace sys 
	{
		bool LoadFunctions()
		{
			if (initialized) { return initialized; }
			int failed = 0;
#if defined(OPENAPOC_USE_GLES_LOADER)
			// A word of warning: the EGL (or OpenGL, for that matter) specification is not required to pass NULL if a procedure is not found.
#define GLAPIPROC(ret, name, params, ptrtype) do { name = (ptrtype) SDL_GL_GetProcAddress("gl" #name); if (!gl::name) {failed++;} } while( 0 );
#define GLAPIFUNC(ret, name, params, ptrtype) do { name = (ptrtype) SDL_GL_GetProcAddress("gl" #name); if (!gl::name) {failed++;} } while( 0 );
#include "gldefs/gl3.h.inl"
#undef GLAPIPROC
#undef GLAPIFUNC
#endif
			initialized = failed == 0;
			return initialized;
		}
	}

}

#endif /*OPENAPOC_GL32_RENDER*/