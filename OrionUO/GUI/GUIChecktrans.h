#pragma once

#include "GUIPolygonal.h"

class CGUIChecktrans : public CGUIPolygonal
{
public:
    CGUIChecktrans(int x, int y, int width, int height);
    virtual ~CGUIChecktrans();

    virtual void Draw(bool checktrans = false);
    virtual bool Select() { return false; }
};
