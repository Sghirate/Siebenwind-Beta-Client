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
