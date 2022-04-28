#include "GUIButtonTileart.h"
#include "../OrionUO.h"

CGUIButtonTileart::CGUIButtonTileart(
    int serial,
    u16 graphic,
    u16 graphicSelected,
    u16 graphicPressed,
    int x,
    int y,
    u16 tileGraphic,
    u16 tileColor,
    int tileX,
    int tileY)
    : CGUIButton(serial, graphic, graphicSelected, graphicPressed, x, y)
    , TileGraphic(tileGraphic)
    , TileColor(tileColor)
    , TileX(tileX)
    , TileY(tileY)
{
    Type = GOT_BUTTONTILEART;
}

CGUIButtonTileart::~CGUIButtonTileart()
{
}

Core::Vec2<i32> CGUIButtonTileart::GetSize()
{
    Core::Vec2<i32> gumpSize = CGUIDrawObject::GetSize();
    Core::Vec2<i32> tileSize;

    CGLTexture *th = g_Orion.ExecuteStaticArt(TileGraphic);

    if (th != nullptr)
    {
        tileSize.x = th->Width;
        tileSize.y = th->Height;
    }

    int startX = m_X;
    int endX = m_X + gumpSize.x;

    if (TileX < startX)
    {
        startX = TileX;
    }

    if (TileX + tileSize.x > endX)
    {
        endX = TileX + tileSize.x;
    }

    int startY = m_Y;
    int endY = m_Y + gumpSize.y;

    if (TileY < startY)
    {
        startY = TileY;
    }

    if (TileY + tileSize.y > endY)
    {
        endY = TileY + tileSize.y;
    }

    return Core::Vec2<i32>(abs(endX) - abs(startX), abs(endY) - abs(startY));
}

void CGUIButtonTileart::PrepareTextures()
{
    CGUIButton::PrepareTextures();

    g_Orion.ExecuteStaticArt(TileGraphic);
}

void CGUIButtonTileart::Draw(bool checktrans)
{
    CGUIDrawObject::Draw(checktrans);

    CGLTexture *th = g_Orion.ExecuteStaticArt(TileGraphic);

    if (th != nullptr)
    {
        SetShaderMode();

        th->Draw(m_X, m_Y, checktrans);
    }
}

bool CGUIButtonTileart::Select()
{
    if (CGUIDrawObject::Select())
    {
        return true;
    }

    CGLTexture *th = g_Orion.ExecuteStaticArt(TileGraphic);

    if (th != nullptr)
    {
        return th->Select(m_X, m_Y, !CheckPolygone);
    }

    return false;
}
