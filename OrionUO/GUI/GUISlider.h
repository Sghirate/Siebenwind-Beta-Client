#pragma once

#include "BaseGUI.h"
#include "Core/Minimal.h"
#include "GLEngine/GLTextTexture.h"

class CGUISlider : public CBaseGUI
{
public:
    u16 GraphicSelected = 0;
    u16 GraphicPressed = 0;
    u16 BackgroundGraphic = 0;
    bool CompositeBackground = false;
    bool Vertical = false;
    int Length = 0;
    float Percents = 0.0f;
    int Offset = 0;
    int MinValue = 0;
    int MaxValue = 0;
    int Value = 0;
    bool HaveText = false;
    SLIDER_TEXT_POSITION TextPosition = STP_RIGHT;
    u8 Font = 0;
    u16 TextColor = 0;
    bool Unicode = true;
    int TextWidth = 0;
    TEXT_ALIGN_TYPE Align = TS_LEFT;
    u16 TextFlags = 0;
    int TextX = 0;
    int TextY = 0;
    int ScrollStep = 15;
    u32 LastScrollTime = 0;
    int DefaultTextOffset = 2;

private:
    CGLTextTexture Text{ CGLTextTexture() };

public:
    CGUISlider(
        int serial,
        u16 graphic,
        u16 graphicSelected,
        u16 graphicPressed,
        u16 backgroundGraphic,
        bool compositeBackground,
        bool vertical,
        int x,
        int y,
        int length,
        int minValue,
        int maxValue,
        int value);
    virtual ~CGUISlider();

    virtual bool IsPressedOuthit() { return true; }
    virtual Core::Vec2<i32> GetSize();
    virtual void OnScroll(bool up, int delay);
    virtual void OnClick(int x, int y);
    void UpdateText();
    virtual void CalculateOffset();
    void SetTextParameters(
        bool haveText,
        SLIDER_TEXT_POSITION textPosition,
        u8 font,
        u16 color,
        bool unicode,
        int textWidth = 0,
        TEXT_ALIGN_TYPE align = TS_LEFT,
        u16 textFlags = 0);
    virtual void PrepareTextures();
    virtual u16 GetDrawGraphic();
    virtual void Draw(bool checktrans = false);
    virtual bool Select();
    virtual void OnMouseEnter();
    virtual void OnMouseExit();
};
