#include "GUITilepic.h"
#include "../OrionUO.h"

CGUITilepic::CGUITilepic(u16 graphic, u16 color, int x, int y)
    : CGUIDrawObject(GOT_TILEPIC, 0, graphic, color, x, y)
{
}

CGUITilepic::~CGUITilepic()
{
}

Core::Vec2<i32> CGUITilepic::GetSize()
{
    Core::Vec2<i32> size;

    CGLTexture *th = g_Orion.ExecuteStaticArt(Graphic);

    if (th != nullptr)
    {
        size.x = th->Width;
        size.y = th->Height;
    }

    return size;
}

void CGUITilepic::PrepareTextures()
{
    g_Orion.ExecuteStaticArt(Graphic);
}

void CGUITilepic::Draw(bool checktrans)
{
    CGLTexture *th = g_Orion.ExecuteStaticArt(Graphic);

    if (th != nullptr)
    {
        SetShaderMode();

        th->Draw(m_X, m_Y, checktrans);
    }
}

bool CGUITilepic::Select()
{
    //if (CGUIDrawObject::Select())
    //	return true;
    CGLTexture *th = g_Orion.m_StaticDataIndex[Graphic].Texture;

    if (th != nullptr)
    {
        return th->Select(m_X, m_Y, !CheckPolygone);
    }

    return false;
}
