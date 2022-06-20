cmake_minimum_required(VERSION 3.15)
find_program(CMAKE_C_COMPILER NAMES $ENV{CC} clang PATHS ENV PATH NO_DEFAULT_PATH)
find_program(CMAKE_CXX_COMPILER NAMES $ENV{CXX} clang++ PATHS ENV PATH NO_DEFAULT_PATH)

project(OrionUO)

#
# Custom Build Options
#
option(ENABLE_LTO "Enables Link Time Optimization" OFF)
option(ORION_BUILD_MODULES "Enables Modular Project Structure" OFF)

list(APPEND CMAKE_MODULE_PATH
  ${CMAKE_SOURCE_DIR}/CMake
)

# Enable folders for IDE
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Folders to include
set(INC_BASE)
set(INC_DEBUG)
set(INC_RELEASE)

# Libraries to link
set(LIBS_BASE)
set(LIBS_DEBUG)
set(LIBS_RELEASE)

# Library binaries to copy
set(BINS_BASE)
set(BINS_DEBUG)
set(BINS_RELEASE)

# Defines to apply
set(DEFINES_BASE)
set(DEFINES_DEBUG)
set(DEFINES_RELEASE)

# CMake Customizations
include(cotire)
include(CheckAndAddFlag)
include(CompileDefinitions)
include(Compiler)
include(CheckCCompilerFlag)
include(CCache)
include(ClangFormat)
#include(WindowsDeps)
include(Revision)
include(EmbeddedResources)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING
      "Build type (Release/Debug/RelWithDebInfo/MinSizeRel)" FORCE)
endif()

#
# Dependencies
#
include(CheckCXXSourceRuns)

set(EXTERNAL_DIR "${CMAKE_SOURCE_DIR}/external")
set(EXTRA_DIR "${EXTERNAL_DIR}/extra")

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	set(ORION_ARCH "x64")
	set(ORION_64BITS 1)
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
	set(ORION_ARCH "x86")
	set(ORION_64BITS 0)
endif()

# tracy
if (ORION_WINDOWS OR ORION_LINUX)
	option(TRACY_ENABLE "" ON)
	option(TRACY_ON_DEMAND "" ON)
	add_subdirectory("${EXTERNAL_DIR}/tracy")
	# set(INC_BASE
	# 	"${INC_BASE}"
	# 	"${EXTERNAL_DIR}/tracy/tracy")
	set(DEFINES_BASE
		"${DEFINES_BASE}"
		"ORION_WITH_TRACY")
	set(ORION_WITH_TRACY ON)
endif()

# GLEW
add_subdirectory("${EXTERNAL_DIR}/GLEW/build/cmake")
set(LIBS_BASE
	"${LIBS_BASE}"
	"glew")
set(DEFINES_BASE
	"${DEFINES_BASE}"
	"ORION_WITH_GLEW")
set(ORION_WITH_GLEW ON)

# SDL2
add_subdirectory("${EXTERNAL_DIR}/SDL2")
set(INC_BASE
	"${INC_BASE}"
	"${EXTERNAL_DIR}/GLEW/include")
set(LIBS_BASE
	"${LIBS_BASE}"
	"SDL2")
set(DEFINES_BASE
	"${DEFINES_BASE}"
	"ORION_WITH_SDL2")
set(ORION_WITH_SDL2 ON)

if(ORION_WINDOWS)
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")

	# PSAPI
	set(LIBS_BASE
		"${LIBS_BASE}"
		"${EXTERNAL_DIR}/psapi/${ORION_ARCH}/lib/psapi.lib")
	set(DEFINES_BASE
		"${DEFINES_BASE}"
		"ORION_WITH_PSASI")
	set(ORION_WITH_PSASI ON)
else()
	#find_package(SDL2 REQUIRED)
	#find_package(GLEW REQUIRED)
	# Set the preferred opengl library to GLVND if there is GLVND and Legacy.
	# Only if there is no GLVND Legacy will be used.
	# The that is the default with cmake-3.11. Legacy is deprecated.
	# See cmake --help-policy CMP0072
	set(OpenGL_GL_PREFERENCE GLVND)
endif()

find_package(OpenGL REQUIRED)

include_directories(${EXTRA_DIR})
include_directories(${OPENGL_INCLUDE_DIR})
include_directories(${SDL2_INCLUDE_DIRS})
include_directories(${SDL2_IMAGE_INCLUDE_DIR})
include_directories(${GLEW_INCLUDE_DIR})
include_directories(${INC_BASE})
include_directories($<$<CONFIG:Debug>:${INC_DEBUG}>
					$<$<CONFIG:Release>:${INC_RELEASE}>)
#
# Project Files
#

configure_file(
  "${PROJECT_SOURCE_DIR}/OrionUO/GitRevision.h.in"
  "${PROJECT_BINARY_DIR}/GitRevision.h"
)
include_directories("${PROJECT_BINARY_DIR}/")

add_subdirectory(OrionUO)
