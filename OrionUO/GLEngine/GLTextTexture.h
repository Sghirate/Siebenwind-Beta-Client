#pragma once

#include "Core/Minimal.h"
#include "GLTexture.h"

struct WEB_LINK_RECT
{
    u16 LinkID;
    int StartX;
    int StartY;
    int EndX;
    int EndY;
};

class CGLTextTexture : public CGLTexture
{
public:
    int LinesCount = 0;

    CGLTextTexture();
    virtual ~CGLTextTexture();

    bool Empty() { return (Texture == 0); }
    virtual void Clear();
    virtual void Draw(int x, int y, bool checktrans = false);
    virtual void ClearWebLink() {}
    virtual void AddWebLink(WEB_LINK_RECT& wl) {}
    virtual u16 WebLinkUnderMouse(int x, int y) { return 0; }
};
