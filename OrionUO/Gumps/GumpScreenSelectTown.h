#pragma once

#include "Gump.h"

class CGumpScreenSelectTown : public CGump
{
private:
    std::vector<Core::Vec2<i32>> m_TownButtonText;
    CGUIHTMLGump *m_HTMLGump{ nullptr };
    CGUIText *m_Description{ nullptr };

public:
    CGumpScreenSelectTown();
    virtual ~CGumpScreenSelectTown();

    virtual void UpdateContent();

    GUMP_BUTTON_EVENT_H;
    GUMP_TEXT_ENTRY_EVENT_H;
};
