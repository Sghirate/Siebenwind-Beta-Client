#include "GLTextTexture.h"
#include "GLEngine/GLEngine.h"

CGLTextTexture::CGLTextTexture()

{
}

CGLTextTexture::~CGLTextTexture()
{
}

void CGLTextTexture::Clear()
{
    CGLTexture::Clear();
    LinesCount = 0;
}

void CGLTextTexture::Draw(int x, int y, bool checktrans)
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
