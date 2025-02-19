#include "LandObject.h"
#include "Globals.h"
#include "OrionUO.h"
#include "SelectedObject.h"

CLandObject::CLandObject(int serial, u16 graphic, u16 color, short x, short y, char z)
    : CMapObject(ROT_LAND_OBJECT, serial, 0, color, x, y, z)
    , MinZ(z)
    , AverageZ(z)
{
    OriginalGraphic = graphic;
    UpdateGraphicBySeason();

    m_DrawTextureColor[3] = 0xFF;

    LAND_TILES &tile = g_Orion.m_LandData[graphic];

    IsStretched = ((tile.TexID == 0u) && ::IsWet(tile.Flags));

    m_rect.set(0, 0, 0, 0);
    m_Normals->set(0.0, 0.0, 0.0);

#if UO_DEBUG_INFO != 0
    g_LandObjectsCount++;
#endif
}

CLandObject::~CLandObject()
{
    if (PositionBuffer != 0)
    {
        glDeleteBuffers(1, &PositionBuffer);
        PositionBuffer = 0;
    }

    if (VertexBuffer != 0)
    {
        glDeleteBuffers(1, &VertexBuffer);
        VertexBuffer = 0;
    }

    if (NormalBuffer != 0)
    {
        glDeleteBuffers(1, &NormalBuffer);
        NormalBuffer = 0;
    }
}

void CLandObject::UpdateGraphicBySeason()
{
    Graphic = g_Orion.GetLandSeasonGraphic(OriginalGraphic);
    NoDrawTile = (Graphic == 2);
}

int CLandObject::GetDirectionZ(int direction)
{
    switch (direction)
    {
        case 1:
            return (m_rect.h / 4);
        case 2:
            return (m_rect.w / 4);
        case 3:
            return (m_rect.y / 4);
        default:
            break;
    }

    return m_Z;
}

int CLandObject::CalculateCurrentAverageZ(int direction)
{
    int result = GetDirectionZ(((u8)(direction >> 1) + 1) & 3);

    if ((direction & 1) != 0)
    {
        return result;
    }

    return (result + GetDirectionZ(direction >> 1)) >> 1;
}

void CLandObject::UpdateZ(int zTop, int zRight, int zBottom)
{
    if (IsStretched)
    {
        Serial = ((m_Z + zTop + zRight + zBottom) / 4);

        m_rect.x = m_Z * 4 + 1;
        m_rect.y = zTop * 4;
        m_rect.w = zRight * 4;
        m_rect.h = zBottom * 4 + 1;

        if (abs(m_Z - zRight) <= abs(zBottom - zTop))
        {
            AverageZ = (m_Z + zRight) >> 1;
        }
        else
        {
            AverageZ = (zBottom + zTop) >> 1;
        }

        MinZ = m_Z;

        if (zTop < MinZ)
        {
            MinZ = zTop;
        }

        if (zRight < MinZ)
        {
            MinZ = zRight;
        }

        if (zBottom < MinZ)
        {
            MinZ = zBottom;
        }
    }
}

void CLandObject::Draw(int x, int y)
{
    if (m_Z <= g_MaxGroundZ)
    {
        u16 objColor = 0;

        if (g_DeveloperMode == DM_DEBUGGING && g_SelectedObject.Object == this)
        {
            objColor = SELECT_LAND_COLOR;
        }

#if UO_DEBUG_INFO != 0
        g_RenderedObjectsCountInGameWindow++;
#endif

        if (!IsStretched)
        {
            g_Orion.DrawLandArt(Graphic, objColor, x, y);
        }
        else
        {
            g_Orion.DrawLandTexture(this, objColor, x, y);
        }
    }
}

void CLandObject::Select(int x, int y)
{
    if (m_Z <= g_MaxGroundZ)
    {
        if (!IsStretched)
        {
            if (g_Orion.LandPixelsInXY(Graphic, x, y))
            {
                g_SelectedObject.Init(this);
            }
        }
        else
        {
            if (g_Orion.LandTexturePixelsInXY(x, y + (m_Z * 4), m_rect))
            {
                g_SelectedObject.Init(this);
            }
        }
    }
}
