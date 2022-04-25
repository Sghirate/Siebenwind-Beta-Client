#include "GUITilepicScaled.h"
#include "../OrionUO.h"

CGUITilepicScaled::CGUITilepicScaled(
    u16 graphic, u16 color, int x, int y, int width, int height)
    : CGUITilepic(graphic, color, x, y)
    , Width(width)
    , Height(height)
{
}

CGUITilepicScaled::~CGUITilepicScaled()
{
}

void CGUITilepicScaled::Draw(bool checktrans)
{
    CGLTexture *th = g_Orion.ExecuteStaticArt(Graphic);
    if (th != nullptr)
    {
        SetShaderMode();
        g_GL_Draw(*th, m_X, m_Y);
    }
}
