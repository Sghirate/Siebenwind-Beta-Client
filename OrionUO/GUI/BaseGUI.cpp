#include "BaseGUI.h"

CBaseGUI::CBaseGUI(
    GUMP_OBJECT_TYPE type, int serial, u16 graphic, u16 color, int x, int y)
    : CRenderObject(serial, graphic, color, x, y)
    , Type(type)
{
}

CBaseGUI::~CBaseGUI()
{
}
