#pragma once

#include "Core/Minimal.h"
#include "GUIButton.h"

class CGUIHTMLButton : public CGUIButton
{
private:
    class CGUIHTMLGump* m_HTMLGump{ nullptr };

public:
    CGUIHTMLButton(
        class CGUIHTMLGump* htmlGump,
        int serial,
        u16 graphic,
        u16 graphicSelected,
        u16 graphicPressed,
        int x,
        int y);
    virtual ~CGUIHTMLButton();

    virtual void SetShaderMode();
    void Scroll(bool up, int delay);
    virtual bool IsControlHTML() { return true; }
};
