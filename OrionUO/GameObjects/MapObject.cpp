#include "MapObject.h"

CMapObject::CMapObject(
    RENDER_OBJECT_TYPE type, int serial, u16 graphic, u16 color, short x, short y, char z)
    : CRenderWorldObject(type, serial, graphic, color, x, y, z)
{
}

CMapObject::~CMapObject()
{
#if UO_DEBUG_INFO != 0
    if (RenderType == ROT_LAND_OBJECT)
    {
        g_LandObjectsCount--;
    }
    else if (RenderType == ROT_STATIC_OBJECT)
    {
        g_StaticsObjectsCount--;
    }
#endif //UO_DEBUG_INFO!=0
}
