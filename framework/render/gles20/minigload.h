#pragma once

#include <string>
#include <unordered_set>

#if defined( _WIN32 ) || defined ( EXPLICIT_INCLUDE_PLATFORMDEFS_H )
#include "gldefs/platformdefs.h"
#else
#include <GLES2/gl2platform.h
#endif

#if defined( _WIN32 ) || defined( EXPLICIT_USE_LOADER )
#define USE_LOADER 1
#else
#define USE_LOADER 0
#endif

#define LOAD_EXTENSIONS 1

namespace gl
{
//#if USE_LOADER
#define GLAPIPROC(ret, name, params, ptrtype) extern ret (GL_APIENTRY * name) params;
#define GLAPIFUNC(ret, name, params, ptrtype) extern ret (GL_APIENTRY * name) params;
//#else
//#define GLAPIPROC(ret, name, params, ptrtype) inline ret name params;
//#define GLAPIFUNC(ret, name, params, ptrtype) inline ret name params;
//#endif
# include "gldefs/gl2.h.inl"
#undef GLAPIPROC
#undef GLAPIFUNC

	enum {
#define GLENUMVAL(name, val) name = val,
#include "gldefs/gl2.h.enum.inl"
#undef GLENUMVAL
	};

#if LOAD_EXTENSIONS
	namespace ext {
#define GLAPIPROCEXT(ret, name, params, ptrtype, extname) extern ret (GL_APIENTRY * name) params;
#define GLAPIFUNCEXT(ret, name, params, ptrtype, extname) extern ret (GL_APIENTRY * name) params;
#include "gldefs/gl2ext.h.inl"
#undef GLAPIPROCEXT
#undef GLAPIFUNCEXT
		enum {
#define GLENUMVAL(name, val) name = val,
#include "gldefs/gl2ext.h.enum.inl"
#undef GLENUMVAL
		};
	};
#endif
	namespace sys {
		bool loadGLCore();
		std::unordered_set<std::string> getExtensions();
		std::unordered_set<std::string> loadExtensions();
		bool isExtensionSupported(const char* extName);
		bool IsVersionGEQ(int major, int minor);
	}
}
