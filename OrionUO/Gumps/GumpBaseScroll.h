#pragma once

#include "Gump.h"

class CGumpBaseScroll : public CGump
{
public:
    int Height = 0;
    int ScrollerOffsetY = 0;
    int ScissorOffsetHeight = 0;
    bool HaveBackgroundLines = false;

protected:
    static const u32 ID_GBS_HTMLGUMP = 0xFFFFFFF0;
    static const u32 ID_GBS_BUTTON_MINIMIZE = 0xFFFFFFF1;
    static const u32 ID_GBS_BUTTON_RESIZE = 0xFFFFFFF2;

    CGUIButton *m_Minimizer{ nullptr };
    CGUIScrollBackground *m_Background{ nullptr };
    CGUIHTMLGump *m_HTMLGump{ nullptr };
    CGUIResizeButton *m_Resizer{ nullptr };

    int m_MinHeight{ 160 };

    virtual void UpdateHeight();

public:
    CGumpBaseScroll(
        GUMP_TYPE type,
        u32 serial,
        u16 graphic,
        int height,
        short x,
        short y,
        bool haveMinimizer,
        int scrollerOffsetY = 0,
        bool haveBackgroundLines = false,
        int scissorOffsetHeight = 0);
    virtual ~CGumpBaseScroll();

    GUMP_RESIZE_START_EVENT_H;
    GUMP_RESIZE_EVENT_H;
    GUMP_RESIZE_END_EVENT_H;

private:
    int m_resizeStartHeight = 0;
    Core::Vec2<i32> m_resizeStartMouse;
};
