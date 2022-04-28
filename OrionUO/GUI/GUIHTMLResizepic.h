#pragma once

#include "GUIResizepic.h"

class CGUIHTMLResizepic : public CGUIResizepic
{
private:
    class CGUIHTMLGump *m_HTMLGump{ nullptr };

public:
    CGUIHTMLResizepic(
        class CGUIHTMLGump *htmlGump,
        int serial,
        u16 graphic,
        int x,
        int y,
        int width,
        int height);
    virtual ~CGUIHTMLResizepic();

    void Scroll(bool up, int delay);
    virtual bool IsControlHTML() { return true; }
};
