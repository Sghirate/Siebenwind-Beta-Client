// MIT License
// Copyright (C) August 2016 Hotride

#pragma once

#include "GUIPolygonal.h"
class CGUIHTMLResizepic;
class CGUIHTMLButton;
class CGUIHTMLHitBox;
class CGUIScissor;
class CGUIHTMLSlider;

class CGUIHTMLGump : public CGUIPolygonal
{
public:
    bool HaveBackground = false;
    bool HaveScrollbar = false;
    Core::Vec2<i32> DataSize = Core::Vec2<i32>();
    Core::Vec2<i32> DataOffset = Core::Vec2<i32>();
    Core::Vec2<i32> CurrentOffset = Core::Vec2<i32>();
    Core::Vec2<i32> AvailableOffset = Core::Vec2<i32>();

private:
    void CalculateDataSize(CBaseGUI *item, int &startX, int &startY, int &endX, int &endY);

public:
    CGUIHTMLGump(
        int serial,
        u16 graphic,
        int x,
        int y,
        int width,
        int height,
        bool haveBackground,
        bool haveScrollbar);
    virtual ~CGUIHTMLGump();

    CGUIHTMLResizepic *m_Background{ nullptr };
    CGUIHTMLButton *m_ButtonUp{ nullptr };
    CGUIHTMLButton *m_ButtonDown{ nullptr };
    CGUIHTMLHitBox *m_HitBoxLeft{ nullptr };
    CGUIHTMLHitBox *m_HitBoxRight{ nullptr };
    CGUIHTMLSlider *m_Slider{ nullptr };
    CGUIScissor *m_Scissor{ nullptr };

    void Initalize(bool menu = false);
    void UpdateHeight(int height);
    void ResetDataOffset();
    void CalculateDataSize();
    virtual void PrepareTextures();
    virtual bool EntryPointerHere();
    virtual bool Select();
    virtual void Scroll(bool up, int delay);
    virtual bool IsHTMLGump() { return true; }
};
