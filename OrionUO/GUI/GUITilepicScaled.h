#pragma once

#include "GUITilepic.h"

class CGUITilepicScaled : public CGUITilepic
{
    int Width = 0;
    int Height = 0;

public:
    CGUITilepicScaled(u16 graphic, u16 color, int x, int y, int width, int height);
    virtual ~CGUITilepicScaled();

    virtual void Draw(bool checktrans = false);
    virtual bool Select() { return false; }
};
