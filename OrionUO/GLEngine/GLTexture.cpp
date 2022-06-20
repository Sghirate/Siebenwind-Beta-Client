#include "GLTexture.h"
#include "GLEngine/GLEngine.h"
#include "Managers/MouseManager.h"

CGLTexture::CGLTexture()
{
}

CGLTexture::~CGLTexture()
{
    Clear();
}

void CGLTexture::Draw(int x, int y, bool checktrans)
{
    if (Texture != 0)
    {
        if (checktrans)
        {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            g_GL_Draw(*this, x, y);
            glDisable(GL_BLEND);
            glEnable(GL_STENCIL_TEST);
            g_GL_Draw(*this, x, y);
            glDisable(GL_STENCIL_TEST);
        }
        else
        {
            g_GL_Draw(*this, x, y);
        }
    }
}

void CGLTexture::Draw(int x, int y, int width, int height, bool checktrans)
{
    if (Texture != 0)
    {
        if (width == 0)
        {
            width = Width;
        }

        if (height == 0)
        {
            height = Height;
        }

        if (checktrans)
        {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            g_GL_DrawStretched(*this, x, y, width, height);
            glDisable(GL_BLEND);
            glEnable(GL_STENCIL_TEST);
            g_GL_DrawStretched(*this, x, y, width, height);
            glDisable(GL_STENCIL_TEST);
        }
        else
        {
            g_GL_DrawStretched(*this, x, y, width, height);
        }
    }
}

void CGLTexture::DrawRotated(int x, int y, float angle)
{
    if (Texture != 0)
    {
        g_GL_DrawRotated(*this, x, y, angle);
    }
}

void CGLTexture::DrawTransparent(int x, int y, bool stencil)
{
    if (Texture != 0)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(1.0f, 1.0f, 1.0f, 0.25f);
        g_GL_Draw(*this, x, y);
        glDisable(GL_BLEND);

        if (stencil)
        {
            glEnable(GL_STENCIL_TEST);
            g_GL_Draw(*this, x, y);
            glDisable(GL_STENCIL_TEST);
        }
    }
}

void CGLTexture::Clear()
{
    Width  = 0;
    Height = 0;
    m_hitMap.Reset();

    if (Texture != 0)
    {
        glDeleteTextures(1, &Texture);
        Texture = 0;
    }

    if (VertexBuffer != 0)
    {
        glDeleteBuffers(1, &VertexBuffer);
        VertexBuffer = 0;
    }

    if (MirroredVertexBuffer != 0)
    {
        glDeleteBuffers(1, &MirroredVertexBuffer);
        MirroredVertexBuffer = 0;
    }
}

bool CGLTexture::Select(int x, int y, bool pixelCheck)
{
    Core::TMousePos pos = g_MouseManager.GetPosition();
    x                   = pos.x - x;
    y                   = pos.y - y;
    if (x >= 0 && y >= 0 && x < Width && y < Height)
        return (!pixelCheck) || m_hitMap.Get((y * Width) + x);
    return false;
}
