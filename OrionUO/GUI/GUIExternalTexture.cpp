#include "GUIExternalTexture.h"
#include "GLEngine/GLEngine.h"
#include "GLEngine/GLTexture.h"
#include "Globals.h"
#include "Managers/ColorManager.h"

CGUIExternalTexture::CGUIExternalTexture(
    CGLTexture* texture, bool deleteTextureOnDestroy, int x, int y, int drawWidth, int drawHeight)
    : CBaseGUI(GOT_EXTERNALTEXTURE, 0, 0, 0, x, y)
    , m_Texture(texture)
    , DeleteTextureOnDestroy(deleteTextureOnDestroy)
    , DrawWidth(drawWidth)
    , DrawHeight(drawHeight)
{
}

CGUIExternalTexture::~CGUIExternalTexture()
{
    if (DeleteTextureOnDestroy && m_Texture)
    {
        delete (m_Texture);
        m_Texture = nullptr;
    }
}

Core::Vec2<i32> CGUIExternalTexture::GetSize()
{
    Core::Vec2<i32> size;
    if (m_Texture != nullptr)
    {
        if (DrawWidth != 0)
            size.x = DrawWidth;
        else
            size.x = m_Texture->Width;

        if (DrawHeight != 0)
            size.y = DrawHeight;
        else
            size.y = m_Texture->Height;
    }
    return size;
}

void CGUIExternalTexture::SetShaderMode()
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

void CGUIExternalTexture::Draw(bool checktrans)
{
    if (m_Texture != nullptr)
    {
        SetShaderMode();

        if ((DrawWidth != 0) || (DrawHeight != 0))
        {
            CGLTexture tex;
            tex.Texture = m_Texture->Texture;
            if (DrawWidth != 0)
                tex.Width = DrawWidth;
            else
                tex.Width = m_Texture->Width;

            if (DrawHeight != 0)
                tex.Height = DrawHeight;
            else
                tex.Height = m_Texture->Height;

            g_GL.GL1_Draw(tex, m_X, m_Y);
            tex.Texture = 0;
        }
        else
        {
            m_Texture->Draw(m_X, m_Y, checktrans);
        }
    }
}

bool CGUIExternalTexture::Select()
{
    return m_Texture ? m_Texture->Select(m_X, m_Y, !CheckPolygone) : false;
}
