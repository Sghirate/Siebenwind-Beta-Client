#pragma once

#if ORION_WITH_OPTICK
#include <optick.h>
#define PROFILER_BEGIN_FRAME(...) OPTICK_FRAME(__VA_ARGS__)
#define PROFILER_END_FRAME(...)
#define PROFILER_EVENT(...) OPTICK_EVENT(__VA_ARGS__)
//#elif ORION_WITH_TRACY
//#include <tracy.hpp>
#elif ORION_WITH_TRACY
#undef min
#undef max
#include <Tracy.hpp>
#define PROFILER_BEGIN_FRAME(...) FrameMark
#define PROFILER_END_FRAME(...)
#define PROFILER_EVENT(...) ZoneScoped
#else
#define PROFILER_BEGIN_FRAME(...)
#define PROFILER_END_FRAME(...)
#define PROFILER_EVENT(...)
#endif
