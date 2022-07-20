#pragma once

#include "BaseTypes.h"
#include "Vec.h"
#include "Rect.h"

#define UNUSED(x) (void)x
#define ToColorR(x) ((x)&0xff)
#define ToColorG(x) ((x >> 8) & 0xff)
#define ToColorB(x) ((x >> 16) & 0xff)
#define ToColorA(x) ((x >> 24) & 0xff)

namespace Core
{
template<typename T>
T Max(T a, T b) { return b < a ? a : b; }
template<typename T>
T Min(T a, T b) { return b > a ? a : b; }
} // namespace Core

inline u32 unpack32(u8 *buf)
{
    return (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];
}

inline u16 unpack16(u8 *buf)
{
    return (buf[0] << 8) | buf[1];
}

inline void pack32(u8 *buf, u32 x)
{
    buf[0] = u8(x >> 24);
    buf[1] = u8((x >> 16) & 0xff);
    buf[2] = u8((x >> 8) & 0xff);
    buf[3] = u8(x & 0xff);
}

inline void pack16(u8 *buf, u16 x)
{
    buf[0] = x >> 8;
    buf[1] = x & 0xff;
}

inline int CalculatePercents(int max, int current, int maxValue)
{
    if (max > 0)
    {
        max = Core::Min((current * 100) / max, 100);
        if (max > 1)
            max = (maxValue * max) / 100;
    }
    return max;
}

inline float deg2radf(float degr)
{
    return degr * (float)(M_PI / 180.0f);
}

inline float rad2degf(float radians)
{
    return (float)(radians * 180.0f / M_PI);
}

#define IN_RANGE(name, id1, id2) ((name) >= (id1) && (name) <= (id2))
#define OUT_RANGE(name, id1, id2) ((name) < (id1) || (name) > (id2))