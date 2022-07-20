#pragma once

#include "RenderStaticObject.h"

class CMultiObject : public CRenderStaticObject
{
public:
    bool OnTarget = false;
    u16 OriginalGraphic = 0;
    int State = 0;

    CMultiObject(u16 graphic, short x, short y, char z, int flags);
    virtual ~CMultiObject();
    virtual void UpdateGraphicBySeason();
    virtual void Draw(int x, int y);
    virtual void Select(int x, int y);
    bool IsMultiObject() { return true; }
    virtual bool IsCustomHouseMulti() { return false; }
};
