#include "GUIVirtueGump.h"
#include "OrionUO.h"
#include "Managers/MouseManager.h"

CGUIVirtueGump::CGUIVirtueGump(u16 graphic, int x, int y)
    : CGUIDrawObject(GOT_VIRTUE_GUMP, graphic, graphic, 0, x, y)
{
}

CGUIVirtueGump::~CGUIVirtueGump()
{
}

bool CGUIVirtueGump::Select()
{
    bool select = false;
    CGLTexture *th = g_Orion.ExecuteGump(Graphic);
    if (th != nullptr)
    {
        Core::TMousePos pos = g_MouseManager.GetPosition();
        int x = pos.x - m_X;
        int y = pos.y - m_Y;
        select = (x >= 0 && y >= 0 && x < th->Width && y < th->Height);
    }
    return select;
}
