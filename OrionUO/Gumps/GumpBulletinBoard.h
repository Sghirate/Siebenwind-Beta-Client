#pragma once

#include "Gump.h"

class CGumpBulletinBoard : public CGump
{
public:
    CGumpBulletinBoard(u32 serial, short x, short y, const std::string &name);
    virtual ~CGumpBulletinBoard();

    CGUIHTMLGump *m_HTMLGump{ nullptr };

    GUMP_BUTTON_EVENT_H;

    virtual bool OnLeftMouseButtonDoubleClick();
};
