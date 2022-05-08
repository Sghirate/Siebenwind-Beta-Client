#pragma once

#include "Core/Minimal.h"
#include "Gump.h"
#include "Platform.h"
#include <string>

class CGumpNotify : public CGump
{
    u8 Variant       = 0;
    short Width      = 0;
    short Height     = 0;
    std::string Text = "";

private:
    static const int ID_GN_BUTTON_OK = 1;

    void Process();

public:
    CGumpNotify(short x, short y, u8 variant, short width, short height, std::string text);
    virtual ~CGumpNotify();

    static const int ID_GN_STATE_LOGOUT       = 1;
    static const int ID_GN_STATE_NOTIFICATION = 2;

    GUMP_BUTTON_EVENT_H;
     void OnKeyDown(const Core::KeyEvent& ev)  override;
};
