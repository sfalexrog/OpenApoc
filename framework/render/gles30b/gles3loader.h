#pragma once

#include <string>
#include <unordered_set>

#include "gldefs/platformdefs.h"

#define OPENAPOC_USE_GLES_LOADER

namespace gl
{
#define GLAPIPROC(ret, name, params, ptrtype) extern ret (GL_APIENTRY * name) params;
#define GLAPIFUNC(ret, name, params, ptrtype) extern ret (GL_APIENTRY * name) params;
# include "gldefs/gl3.h.inl"
#undef GLAPIPROC
#undef GLAPIFUNC

	enum {
#define GLENUMVAL(name, val) name = val,
#include "gldefs/gl3.h.enum.inl"
#undef GLENUMVAL
	};

	namespace sys
	{
		bool loadGLCore();
	}

}

