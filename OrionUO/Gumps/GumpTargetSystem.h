#pragma once

#include "Gump.h"
#include <string>

class CGumpTargetSystem : public CGump
{
    std::string OldName = "";

private:
    const int ID_GSB_BUFF_GUMP   = 1;
    const int ID_GSB_LOCK_MOVING = 2;

    CGUIGumppic* m_Body{ nullptr };
    CGUIDataBox* m_DataBox{ nullptr };

public:
    CGumpTargetSystem(u32 serial, short x, short y);
    virtual ~CGumpTargetSystem();

    virtual bool CanBeDisplayed();

    virtual void PrepareContent();
    virtual void UpdateContent();

    GUMP_BUTTON_EVENT_H;

    void OnLeftMouseDown();
    bool OnLeftMouseButtonDoubleClick();
};
