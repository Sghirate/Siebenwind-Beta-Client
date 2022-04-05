#pragma once

#if defined(ORION_WINDOWS)

#include "../../external/GLEW/include/glew.h"
#include "../../external/GLEW/include/wglew.h"

#else

#define NO_SDL_GLEXT
#include <GL/glew.h>
#if defined(__APPLE__)
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#endif // ORION_WINDOWS
