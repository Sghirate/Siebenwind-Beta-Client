#pragma once

#include "Core/Minimal.h"
#include <deque>

class CContainerStackItem
{
public:
    u32 Serial       = 0;
    short X          = 0;
    short Y          = 0;
    short MinimizedX = 0;
    short MinimizedY = 0;
    bool Minimized   = false;
    bool LockMoving  = false;

    CContainerStackItem(
        int serial,
        short x,
        short y,
        short minimizedX,
        short minimizedY,
        bool minimized,
        bool lockMoving);
    ~CContainerStackItem() {}
};

extern std::deque<CContainerStackItem> g_ContainerStack;

extern u32 g_CheckContainerStackTimer;
