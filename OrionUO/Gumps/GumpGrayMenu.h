// MIT License
// Copyright (C) August 2016 Hotride

#pragma once

#include "Gump.h"

class CGumpGrayMenu : public CGump
{
public:
    enum
    {
        ID_GGM_CANCEL = 1000,
        ID_GGM_CONTINUE = 1001
    };

    CGumpGrayMenu(u32 serial, u32 id, short x, short y);
    virtual ~CGumpGrayMenu();

    void SendMenuResponse(int index);

    GUMP_BUTTON_EVENT_H;
};
