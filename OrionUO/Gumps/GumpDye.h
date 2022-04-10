// MIT License
// Copyright (C) August 2016 Hotride

#pragma once

#include "GumpSelectColor.h"

class CGumpDye : public CGumpSelectColor
{
private:
    u16 GetCurrentColor();
    CGUITilepic *m_Tube{ nullptr };

public:
    CGumpDye(u32 serial, i16 x, i16 y, u16 graphic);
    virtual ~CGumpDye();

    virtual void UpdateContent();
    virtual void OnSelectColor(u16 color);

    GUMP_BUTTON_EVENT_H;
    GUMP_SLIDER_CLICK_EVENT_H;
    GUMP_SLIDER_MOVE_EVENT_H;
};
