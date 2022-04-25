#pragma once

#include "Core/Minimal.h"
#include "MultiObject.h"

class CCustomHouseMultiObject : public CMultiObject
{
public:
    CCustomHouseMultiObject(u16 graphic, u16 color, short x, short y, char z, int flags);
    virtual ~CCustomHouseMultiObject();
    virtual bool IsCustomHouseMulti() { return true; }
};
