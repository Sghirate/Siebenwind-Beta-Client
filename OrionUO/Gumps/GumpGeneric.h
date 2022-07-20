#pragma once

#include "Gump.h"
#include "Platform.h"

class CGumpGeneric : public CGump
{
public:
    u32 MasterGump = 0;

    CGumpGeneric(u32 serial, short x, short y, u32 id);
    virtual ~CGumpGeneric();

    void AddText(
        int index,
        const std::wstring &text,
        CBaseGUI *start = nullptr,
        bool backbroundCanBeColored = false);

    void SendGumpResponse(int index);

    virtual void InitToolTip();

    GUMP_BUTTON_EVENT_H;
    GUMP_DIRECT_HTML_LINK_EVENT_H;

    virtual bool OnLeftMouseButtonDoubleClick();
     void OnTextInput(const Core::TextEvent &ev)  override;
     void OnKeyDown(const Core::KeyEvent &ev)  override;
};
