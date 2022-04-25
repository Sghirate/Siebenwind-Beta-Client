#pragma once

#include "Gump.h"
#include "../Platform.h"

class CGumpNotify : public CGump
{
    u8 Variant = 0;
    short Width = 0;
    short Height = 0;
    string Text = "";

private:
    static const int ID_GN_BUTTON_OK = 1;

    void Process();

public:
    CGumpNotify(short x, short y, u8 variant, short width, short height, string text);
    virtual ~CGumpNotify();

    static const int ID_GN_STATE_LOGOUT = 1;
    static const int ID_GN_STATE_NOTIFICATION = 2;

    GUMP_BUTTON_EVENT_H;
    virtual void OnKeyDown(const KeyEvent &ev) override;
};
