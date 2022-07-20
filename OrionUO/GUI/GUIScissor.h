#pragma once

#include "GUIPolygonal.h"

class CGump;

class CGUIScissor : public CGUIPolygonal
{
    int BaseX = 0;
    int BaseY = 0;

public:
    CGump* GumpParent = nullptr;

    CGUIScissor(
        bool enabled,
        int baseX  = 0,
        int baseY  = 0,
        int x      = 0,
        int y      = 0,
        int width  = 0,
        int height = 0);
    virtual ~CGUIScissor();

    virtual void Draw(bool checktrans = false);
};
