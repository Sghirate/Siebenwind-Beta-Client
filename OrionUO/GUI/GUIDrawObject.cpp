#include "GUIDrawObject.h"
#include "../OrionUO.h"
#include "../Managers/ColorManager.h"

CGUIDrawObject::CGUIDrawObject(
    GUMP_OBJECT_TYPE type, int serial, u16 graphic, u16 color, int x, int y)
    : CBaseGUI(type, serial, graphic, color, x, y)
{
}

CGUIDrawObject::~CGUIDrawObject()
{
}

Core::Vec2<i32> CGUIDrawObject::GetSize()
{
    Core::Vec2<i32> size;
    CGLTexture *th = g_Orion.ExecuteGump(GetDrawGraphic());

    if (th != nullptr)
    {
        size.x = th->Width;
        size.y = th->Height;
    }

    return size;
}

void CGUIDrawObject::SetShaderMode()
{

    if (Color != 0)
    {
        if (PartialHue)
        {
            glUniform1iARB(g_ShaderDrawMode, SDM_PARTIAL_HUE);
        }
        else
        {
            glUniform1iARB(g_ShaderDrawMode, SDM_COLORED);
        }

        g_ColorManager.SendColorsToShader(Color);
    }
    else
    {
        glUniform1iARB(g_ShaderDrawMode, SDM_NO_COLOR);
    }
}

void CGUIDrawObject::PrepareTextures()
{
    g_Orion.ExecuteGump(Graphic);
}

void CGUIDrawObject::Draw(bool checktrans)
{
    CGLTexture *th = g_Orion.ExecuteGump(GetDrawGraphic());

    if (th != nullptr)
    {
        SetShaderMode();

        th->Draw(m_X, m_Y, checktrans);
    }
}

bool CGUIDrawObject::Select()
{
    CGLTexture *th = g_Orion.ExecuteGump(Graphic);

    if (th != nullptr)
    {
        return th->Select(m_X, m_Y, !CheckPolygone);
    }

    return false;
}
