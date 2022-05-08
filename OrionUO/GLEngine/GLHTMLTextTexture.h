#pragma once

#include "Core/Minimal.h"
#include "GLEngine/GLTextTexture.h"
#include <deque>

class CGLHTMLTextTexture : public CGLTextTexture
{
private:
    std::deque<WEB_LINK_RECT> m_WebLinkRect;

public:
    CGLHTMLTextTexture();
    virtual ~CGLHTMLTextTexture();

    void ClearWebLink() { m_WebLinkRect.clear(); }

    void AddWebLink(WEB_LINK_RECT& wl) { m_WebLinkRect.push_back(wl); }

    u16 WebLinkUnderMouse(int x, int y);
};
