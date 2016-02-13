cmake_minimum_required(VERSION 2.8)

option(BACKTRACE_ON_ERROR "Print backtrace on logging an error (Requires libunwind on linux, no extra dependencies on windows)" ON)
option(DIALOG_ON_ERROR "Pop up a dialog box showing errors" ON)
set(RENDERER "GL30" CACHE STRING "Renderer to use, one of \"GL30\"")

unset(FRAMEWORK_SOURCES)
unset(FRAMEWORK_INCLUDE_DIRS)
unset(FRAMEWORK_LIBRARIES)

find_package(PkgConfig)

pkg_check_modules(PC_SDL2 REQUIRED sdl2>=2.0)
if (NOT PC_SDL2_FOUND)
		message(FATAL_ERROR "sdl2 not found")
endif()
foreach (SDL2_LIB ${PC_SDL2_LIBRARIES})
		message("Searching for ${SDL2_LIB} in ${PC_SDL2_LIBRARY_DIRS}")
		find_library(SDL2_LIBRARY_PATH-${SDL2_LIB} ${SDL2_LIB} HINTS ${PC_SDL2_LIBRARY_DIRS})
		if (NOT SDL2_LIBRARY_PATH-${SDL2_LIB})
				message(FATAL_ERROR "sdl2 library ${SDL2_LIB} not found in ${PC_SDL2_LIBRARY_DIRS}")
		endif()
		message("Found ${SDL2_LIB} at ${SDL2_LIBRARY_PATH-${SDL2_LIB}}")
		list(APPEND FRAMEWORK_LIBRARIES ${SDL2_LIB})
		list(APPEND FRAMEWORK_INCLUDE_DIRS ${PC_SDL2_INCLUDE_DIRS})
endforeach()

aux_source_directory(framework FRAMEWORK_SOURCES)

if (RENDERER STREQUAL "GL30")
	list(APPEND FRAMEWORK_SOURCES framework/render/gl30/ogl_3_0_renderer.cpp)
	find_package(OpenGL)
	if (NOT OPENGL_FOUND)
			message(FATAL_ERROR "Failed to find OpenGL library")
	endif()
	list(APPEND FRAMEWORK_INCLUDE_DIRS ${OPENGL_INCLUDE_DIR})
	list(APPEND FRAMEWORK_LIBRARIES ${OPENGL_gl_LIBRARY})

else()
	message(FATAL_ERROR "Unrecognised RENDERER \"${RENDERER}\"")
endif()

include(imageloader)
list(APPEND FRAMEWORK_SOURCES ${IMAGELOADER_SOURCES})
list(APPEND FRAMEWORK_INCLUDE_DIRS ${IMAGELOADER_INCLUDE_DIRS})
list(APPEND FRAMEWORK_LIBRARIES ${IMAGELOADER_LIBRARIES})

include(soundbackend)
list(APPEND FRAMEWORK_SOURCES ${SOUNDBACKEND_SOURCES})
list(APPEND FRAMEWORK_INCLUDE_DIRS ${SOUNDBACKEND_INCLUDE_DIRS})
list(APPEND FRAMEWORK_LIBRARIES ${SOUNDBACKEND_LIBRARIES})


if(DIALOG_ON_ERROR)
		add_definitions(-DERROR_DIALOG)
endif()

if(BACKTRACE_ON_ERROR)
		pkg_check_modules(PC_UNWIND libunwind)
		if (NOT PC_UNWIND_FOUND)
				#Ubuntu 12.04 libunwind doesn't have a pkgconfig - try 'current' paths anyway
				find_path(UNWIND_INCLUDE_DIR libunwind.h HINTS ${FRAMEWORK_INCLUDE_DIRS})
				if (NOT UNWIND_INCLUDE_DIR)
						message(FATAL_ERROR "Libunwind not found")
				endif()
				list(APPEND FRAMEWORK_LIBRARIES unwind dl)
				#HACK - this assumes the library path is already searched?
		else()
			find_path(UNWIND_INCLUDE_DIR libunwind.h HINTS ${PC_UNWIND_INCLUDEDIR})
			list(APPEND FRAMEWORK_INCLUDE_DIRS ${UNWIND_INCLUDE_DIR})
			list(APPEND FRAMEWORK_LIBRARIES ${PC_UNWIND_LIBRARIES} dl)
		endif()
		add_definitions(-DBACKTRACE_LIBUNWIND)
		#FIXME: Add Windows support for cmake? (BACKTRACE_WINDOWS?)
endif()

pkg_check_modules(PC_PHYSFS REQUIRED physfs>=2.0.0)

find_path(PHYSFS_INCLUDE_DIR physfs.h HINTS ${PC_PHYSFS_INCLUDEDIR})
list(APPEND FRAMEWORK_INCLUDE_DIRS ${PHYSFS_INCLUDE_DIR})


foreach (PHYSFS_LIBRARY ${PC_PHYSFS_LIBRARIES})
	find_library(PHYSFS_LIBRARY_PATH ${PHYSFS_LIBRARY} HINTS
			${PC_PHYSFS_LIBRARY_DIRS})
	if (NOT PHYSFS_LIBRARY_PATH)
			message(FATAL_ERROR "Failed to find physfs library ${PHYSFS_LIBRARY} in ${PC_PHYSFS_LIBRARY_DIRS}")
	endif()
	list(APPEND PHYSFS_LIBRARIES ${PHYSFS_LIBRARY_PATH})
endforeach(PHYSFS_LIBRARY)

pkg_check_modules(PC_GLES2 REQUIRED glesv2)

foreach (PHYSFS_LIBRARY ${PC_PHYSFS_LIBRARIES})
	find_library(PHYSFS_LIBRARY_PATH ${PHYSFS_LIBRARY} HINTS
			${PC_PHYSFS_LIBRARY_DIRS})
	if (NOT PHYSFS_LIBRARY_PATH)
			message(FATAL_ERROR "Failed to find physfs library ${PHYSFS_LIBRARY} in ${PC_PHYSFS_LIBRARY_DIRS}")
	endif()
	list(APPEND PHYSFS_LIBRARIES ${PHYSFS_LIBRARY_PATH})
endforeach(PHYSFS_LIBRARY)

list(APPEND FRAMEWORK_LIBRARIES ${PHYSFS_LIBRARIES})

pkg_check_modules(PC_GLES2 REQUIRED glesv2)

foreach (GLES2_LIBRARY ${PC_GLES2_LIBRARIES})
	find_library(GLES2_LIBRARY_PATH ${GLES2_LIBRARY} HINTS
			${PC_GLES2_LIBRARY_DIRS})
	if (NOT GLES2_LIBRARY_PATH)
			message(FATAL_ERROR "Failed to find gles2 library ${GLES2_LIBRARY} in ${PC_GLES2_LIBRARY_DIRS}")
	endif()
	list(APPEND GLES2_LIBRARIES ${GLES2_LIBRARY_PATH})
endforeach(GLES2_LIBRARY)
