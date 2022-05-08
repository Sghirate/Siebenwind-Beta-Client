#pragma once

#include "Gump.h"

class CGumpScreenServer : public CGump
{
public:
    CGumpScreenServer();
    virtual ~CGumpScreenServer();

     void UpdateContent()  override;
     void InitToolTip()  override;

    GUMP_BUTTON_EVENT_H;
    GUMP_TEXT_ENTRY_EVENT_H;
};
