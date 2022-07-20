#pragma once

#include "GUISlider.h"

class CGUIHTMLSlider : public CGUISlider
{
private:
    class CGUIHTMLGump *m_HTMLGump{ nullptr };

public:
    CGUIHTMLSlider(
        class CGUIHTMLGump *htmlGump,
        int serial,
        u16 graphic,
        u16 graphicSelected,
        u16 graphicPressed,
        u16 backgroundGraphic,
        bool compositeBackground,
        bool vertical,
        int x,
        int y,
        int length,
        int minValue,
        int maxValue,
        int value);
    virtual ~CGUIHTMLSlider();
    virtual void CalculateOffset();
    virtual bool IsControlHTML() { return true; }
};
