#include "RenderWorldObject.h"
#include "Managers/ConfigManager.h"
#include "ScreenStages/GameScreen.h"

CRenderWorldObject::CRenderWorldObject(
    RENDER_OBJECT_TYPE renderType,
    int serial,
    u16 graphic,
    u16 color,
    int x,
    int y,
    char z)
    : CRenderObject(serial, graphic, color, x, y)
    , RenderType(renderType)
    , m_Z(z)
{
    m_DrawTextureColor[0] = 0xFF;
    m_DrawTextureColor[1] = 0xFF;
    m_DrawTextureColor[2] = 0xFF;
    m_DrawTextureColor[3] = 0x00;

    UpdateRealDrawCoordinates();
}

CRenderWorldObject::~CRenderWorldObject()
{
    RemoveRender();
}

bool CRenderWorldObject::ProcessAlpha(int maxAlpha)
{
    if (!g_ConfigManager.RemoveOrCreateObjectsWithBlending)
    {
        m_DrawTextureColor[3] = (u8)maxAlpha;

        return (maxAlpha != 0);
    }

    bool result = false;
    int alpha = (int)m_DrawTextureColor[3];

    if (alpha > maxAlpha)
    {
        alpha -= ALPHA_STEP;

        if (alpha < maxAlpha)
        {
            alpha = maxAlpha;
        }

        result = true;
    }
    else if (alpha < maxAlpha)
    {
        alpha += ALPHA_STEP;

        if (alpha > maxAlpha)
        {
            alpha = maxAlpha;
        }

        result = true;
    }

    m_DrawTextureColor[3] = (u8)alpha;

    return result;
}

void CRenderWorldObject::RemoveRender()
{
    if (m_PrevXY != nullptr)
    {
        m_PrevXY->m_NextXY = m_NextXY;
    }

    if (m_NextXY != nullptr)
    {
        m_NextXY->m_PrevXY = m_PrevXY;
    }

    m_NextXY = nullptr;
    m_PrevXY = nullptr;
}

bool CRenderWorldObject::RemovedFromRender()
{
    return m_NextXY == nullptr && m_PrevXY == nullptr;
}

CLandObject *CRenderWorldObject::GetLand()
{

    for (CRenderWorldObject *land = this; land != nullptr; land = land->m_NextXY)
    {
        if (land->IsLandObject())
        {
            return (CLandObject *)land;
        }
    }

    for (CRenderWorldObject *land = this->m_PrevXY; land != nullptr; land = land->m_PrevXY)
    {
        if (land->IsLandObject())
        {
            return (CLandObject *)land;
        }
    }

    return nullptr;
}

void CRenderWorldObject::SetZ(char val)
{
    m_Z = val;
    UpdateRealDrawCoordinates();
}

void CRenderWorldObject::UpdateRealDrawCoordinates()
{
    RealDrawX = (m_X - m_Y) * 22;
    RealDrawY = (m_X + m_Y) * 22 - (m_Z * 4);
    Changed = true;
}

void CRenderWorldObject::UpdateDrawCoordinates()
{
    DrawX = RealDrawX - g_RenderBounds.WindowDrawOffsetX;
    DrawY = RealDrawY - g_RenderBounds.WindowDrawOffsetY;
    Changed = false;
    UpdateTextCoordinates();
    FixTextCoordinates();
}
