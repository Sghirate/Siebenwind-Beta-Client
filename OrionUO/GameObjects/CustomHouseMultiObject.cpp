#include "CustomHouseMultiObject.h"

CCustomHouseMultiObject::CCustomHouseMultiObject(
    u16 graphic, u16 color, short x, short y, char z, int flags)
    : CMultiObject(graphic, x, y, z, flags)
{
    Color = color;
}

CCustomHouseMultiObject::~CCustomHouseMultiObject()
{
}
