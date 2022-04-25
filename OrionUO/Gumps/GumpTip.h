#pragma once

#include "GumpBaseScroll.h"

class CGumpTip : public CGumpBaseScroll
{
    bool Updates = false;
    CGUIButton *m_ButtonPrevGump{ nullptr };
    CGUIButton *m_ButtonNextGump{ nullptr };

    virtual void UpdateHeight();

public:
    CGumpTip(u32 serial, short x, short y, const std::string &str, bool updates);
    virtual ~CGumpTip();

    void SendTipRequest(u8 flag);

    GUMP_BUTTON_EVENT_H;

    virtual bool OnLeftMouseButtonDoubleClick() override;
};
