// MIT License
// Copyright (C) August 2016 Hotride

#pragma once

#include "Gump.h"

class CGumpBuff : public CGump
{
private:
    const int BUFF_ITEM_STEP_OFFSET_X = 3;
    const int BUFF_ITEM_STEP_OFFSET_Y = 3;
    const int MUNIMUM_ICON_ALPHA = 80;
    const int ALPHA_CHANGE_KOEFF = 600;
    const int USE_ALPHA_BLENDING_WHEN_TIMER_LESS = 10000;

    const int ID_GB_NEXT_WINDOW_DIRECTION = 1;
    const int ID_GB_LOCK_MOVING = 2;
    const int ID_GB_BUFF_ITEM = 10;

    void GetGumpStatus(
        Core::Vec2<i32> &ball,
        Core::Vec2<i32> &items,
        bool &useX,
        bool &decX,
        bool &decY,
        Core::Vec2<i32> &startGump,
        Core::Vec2<i32> &endGump);

public:
    CGumpBuff(short x, short y);
    virtual ~CGumpBuff();

    virtual bool CanBeDisplayed();

    void AddBuff(u16 id, u16 timer, const std::wstring &text);

    void DeleteBuff(u16 id);

    void UpdateBuffIcons();

    virtual void InitToolTip();

    virtual void PrepareContent();

    virtual void UpdateContent();

    GUMP_BUTTON_EVENT_H;
};
