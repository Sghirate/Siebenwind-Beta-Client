#pragma once

#include "Gump.h"

class CGumpScreenCharacterList : public CGump
{
public:
    CGumpScreenCharacterList();
    virtual ~CGumpScreenCharacterList();

    virtual void UpdateContent();
    virtual void InitToolTip();

    GUMP_BUTTON_EVENT_H;
    GUMP_TEXT_ENTRY_EVENT_H;

     bool OnLeftMouseButtonDoubleClick()  override;
};
