#pragma once

#include "Gump.h"

class CGumpRacialAbility : public CGump
{
private:
    const int ID_GS_LOCK_MOVING = 1;

public:
    CGumpRacialAbility(int serial, int x, int y);
    virtual ~CGumpRacialAbility();

    static void OnAbilityUse(u16 index);

    virtual void InitToolTip();

    GUMP_BUTTON_EVENT_H;

    virtual bool OnLeftMouseButtonDoubleClick();
};
