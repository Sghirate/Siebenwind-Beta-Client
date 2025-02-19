#pragma once

#include "Core/Minimal.h"

#define USE_DEBUG_TRACE_FUNCTION 0

#if USE_DEBUG_TRACE_FUNCTION != 0
#define DEBUG_TRACE_FUNCTION                                                                       \
    do                                                                                             \
    {                                                                                              \
        fprintf(stdout, "CALL: %s\n", __FUNCTION__);                                               \
    } while (0)
#else
#define DEBUG_TRACE_FUNCTION
#endif

#define ORION_NOT_IMPLEMENTED                                                                      \
    fprintf(stdout, "NOT_IMPLEMENTED: %s:%d: %s\n", __FILE__, __LINE__, __FUNCTION__);

#define DEBUGGING_OUTPUT 1

#if DEBUGGING_OUTPUT == 1
void DebugDump(u8 *data, int size);
#else
#define DebugDump(buf, size)
#endif

#define RELEASE_POINTER(ptr)                                                                       \
    if (ptr != nullptr)                                                                            \
    {                                                                                              \
        delete ptr;                                                                                \
        ptr = nullptr;                                                                             \
    }

#define IN_RANGE(name, id1, id2) ((name) >= (id1) && (name) <= (id2))
#define OUT_RANGE(name, id1, id2) ((name) < (id1) || (name) > (id2))
