#pragma once

#if WITH_OPTICK
#include <optick.h>
#define PROFILER_FRAME(...) OPTICK_FRAME(__VA_ARGS__)
#define PROFILER_EVENT(...) OPTICK_EVENT(__VA_ARGS__)
#else
#define PROFILER_FRAME(...)
#define PROFILER_EVENT(...)
#endif
