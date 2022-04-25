#pragma once
/*
#if defined(ORION_WINDOWS)
#if defined(_MSC_VER)
#pragma warning(disable : 4800) //forcing value to bool 'true' or 'false' (performance warning)
#endif
#if !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <windows.h>
#endif

#include <SDL_thread.h>
#include <thread>

#include <stdio.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdlib.h>

#include <string>
using std::string;
using std::wstring;

#include <vector>
using std::vector;

#include <deque>
using std::deque;

#include <map>
using std::map;

#include <unordered_map>
using std::pair;
using std::unordered_map;

#include "../Definitions.h"
*/
extern SDL_threadID g_MainThread;
const int PACKET_VARIABLE_SIZE = 0;

int CalculatePercents(int max, int current, int maxValue);

inline float deg2radf(float degr)
{
    return degr * (float)(M_PI / 180.0f);
}

inline float rad2degf(float radians)
{
    return (float)(radians * 180.0f / M_PI);
}

inline int RandomInt(int n)
{
    return (rand() % n);
}

inline int RandomIntMinMax(int n, int m)
{
    return (rand() % (m - n) + n);
}

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
