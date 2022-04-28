#include "GUIScissor.h"
#include "../Gumps/Gump.h"

CGUIScissor::CGUIScissor(bool enabled, int baseX, int baseY, int x, int y, int width, int height)
    : CGUIPolygonal(GOT_SCISSOR, x, y, width, height)
    , BaseX(baseX)
    , BaseY(baseY)
{
    Enabled = enabled;
}

CGUIScissor::~CGUIScissor()
{
}

void CGUIScissor::Draw(bool checktrans)
{
    if (Enabled)
    {
        int x = m_X;
        int y = m_Y;
        if (GumpParent != nullptr)
        {
            x -= GumpParent->GumpRect.pos.x;
            y -= GumpParent->GumpRect.pos.y;
        }
        g_GL.PushScissor(BaseX + x, BaseY + y, Width, Height);
    }
    else
    {
        g_GL.PopScissor();
    }
}
