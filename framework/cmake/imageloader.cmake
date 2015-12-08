cmake_minimum_required(VERSION 2.8)

find_package(PkgConfig)

option (BUILD_ALLEGRO_IMAGELOADER "Build an allegro image loader backend" OFF)
option (BUILD_LODEPNG_IMAGELOADER "Build a lodepng image loader backend" ON)
option (BUILD_SDL2_IMAGELOADER "Build a sdl2 image loader backend" ON)

set (IMAGELOADERS "")
set (IMAGELOADER_SOURCES "")
set (IMAGELOADER_LIBRARIES "")
set (IMAGELOADER_INCLUDES "")

if(BUILD_ALLEGRO_IMAGELOADER)
		list(APPEND IMAGELOADERS allegro)
		list(APPEND IMAGELOADER_SOURCES "framework/imageloader/allegro_image.cpp")
		list(APPEND FRAMEWORK_ALLEGRO_LIBRARIES allegro_image allegro allegro_physfs)
endif()

if (BUILD_LODEPNG_IMAGELOADER)
		list(APPEND IMAGELOADERS lodepng)
		list(APPEND IMAGELOADER_SOURCES "framework/imageloader/lodepng_image.cpp" "framework/imageloader/lodepng.cpp")
endif()

if (BUILD_SDL2_IMAGELOADER)
	list(APPEND IMAGELOADERS sdl2)
	list(APPEND IMAGELOADER_SOURCES "framework/imageloader/sdl_image.cpp")
	pkg_check_modules(PC_SDL2_IMAGE REQUIRED SDL2_image>=2.0)
	foreach (SDL2_IMAGE_LIB ${PC_SDL2_IMAGE_LIBRARIES})
			find_library(SDL2_IMAGE_LIBRARY_PATH-${SDL2_IMAGE_LIB} ${SDL2_IMAGE_LIB} HINTS ${PC_SDL2_IMAGE_LIBRARY_DIRS})
			if (NOT SDL2_IMAGE_LIBRARY_PATH-${SDL2_IMAGE_LIB})
					message(FATAL_ERROR "sdl2_image library ${SDL2_IMAGE_LIB} not found in ${PC_SDL2_IMAGE_LIBRARY_DIRS}")
			endif()
			list(APPEND IMAGELOADER_LIBRARIES ${SDL2_IMAGE_LIBRARY_PATH-${SDL2_IMAGE_LIB}})
			message("Added lib ${SDL2_IMAGE_LIBRARY_PATH-${SDL2_IMAGE_LIB}}")
	endforeach()
	list(APPEND IMAGELOADER_INCLUDE ${PC_SDL2_IMAGE_INCLUDE_DIRS})
endif()

if (NOT IMAGELOADERS)
		message(FATAL_ERROR "No image loaders specified")
endif()

message("Building image loaders: ${IMAGELOADERS}")
