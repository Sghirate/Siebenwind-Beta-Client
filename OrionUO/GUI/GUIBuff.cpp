#include "GUIBuff.h"
#include "../OrionUO.h"

CGUIBuff::CGUIBuff(u16 graphic, int timer, const std::wstring &text)
    : CGUIDrawObject(GOT_BUFF, 0, graphic, 0, 0, 0)
    , Timer(timer)
    , Text(text)
{
}

CGUIBuff::~CGUIBuff()
{
}

void CGUIBuff::Draw(bool checktrans)
{

    CGLTexture *th = g_Orion.ExecuteGump(GetDrawGraphic());
    if (th != nullptr)
    {
        glColor4ub(0xFF, 0xFF, 0xFF, Alpha);
        th->Draw(m_X, m_Y, checktrans);
    }
}
