#pragma once

#include "Core/Minimal.h"
#include "Gump.h"
#include <string>

class CGumpMenu : public CGump
{
public:
    std::string Text = "";
    bool TextChanged = false;

protected:
    virtual void CalculateGumpState();

public:
    CGumpMenu(u32 serial, u32 id, short x, short y);
    virtual ~CGumpMenu();

    static const int ID_GM_HTMLGUMP = 1000;

    CGUIText* m_TextObject{ nullptr };

    void SendMenuResponse(int index);

    virtual void PrepareContent();

    virtual bool OnLeftMouseButtonDoubleClick();
};
