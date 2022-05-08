#pragma once

#include "Gump.h"

class CGumpScreenMain : public CGump
{
    CGUIButton *m_Arrow{ nullptr };

public:
    CGumpScreenMain();
    virtual ~CGumpScreenMain();

    CEntryText *m_PasswordFake{ nullptr };

     void PrepareContent()  override;
     void UpdateContent()  override;
     void InitToolTip()  override;

    GUMP_BUTTON_EVENT_H;
    GUMP_TEXT_ENTRY_EVENT_H;
};
