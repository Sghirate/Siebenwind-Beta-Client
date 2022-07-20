#include "GLHTMLTextTexture.h"
#include "Managers/MouseManager.h"

CGLHTMLTextTexture::CGLHTMLTextTexture()
{
}

CGLHTMLTextTexture::~CGLHTMLTextTexture()
{
    m_WebLinkRect.clear();
}

u16 CGLHTMLTextTexture::WebLinkUnderMouse(int x, int y)
{
    Core::TMousePos pos = g_MouseManager.GetPosition();
    x                   = pos.x - x;
    y                   = pos.y - y;
    for (auto it = m_WebLinkRect.begin(); it != m_WebLinkRect.end(); ++it)
    {
        if (y >= (*it).StartY && y < (*it).StartY + (*it).EndY)
        {
            if (x >= (*it).StartX && x < (*it).StartX + (*it).EndX)
            {
                return it->LinkID;
            }
        }
    }

    return 0;
}
