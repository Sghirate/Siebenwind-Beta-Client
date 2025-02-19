#pragma once

#include "Gump.h"

class CGumpSelectColor : public CGump
{
protected:
    enum
    {
        ID_GSC_BUTTON_OKAY = 1,
        ID_GSC_SLIDER = 2,
        ID_GSC_COLORS = 10,
    };

    SELECT_COLOR_GUMP_STATE m_State;
    u8 m_ColorRef = 1;
    int m_SelectedIndex = 10;

    CGUISlider *m_Slider{ nullptr };
    CGUIDataBox *m_DataBox{ nullptr };

public:
    CGumpSelectColor(u32 serial, short x, short y, SELECT_COLOR_GUMP_STATE state);
    virtual ~CGumpSelectColor();

     void UpdateContent()  override;

    GUMP_BUTTON_EVENT_H;
    GUMP_SLIDER_CLICK_EVENT_H;
    GUMP_SLIDER_MOVE_EVENT_H;

    virtual void OnSelectColor(u16 color);
};
