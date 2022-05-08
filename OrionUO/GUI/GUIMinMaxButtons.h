#pragma once

#include "BaseGUI.h"
#include "Core/Minimal.h"
#include "Core/Input.h"
#include "Core/Optional.h"
#include "GLEngine/GLTextTexture.h"

class CGUIMinMaxButtons : public CBaseGUI
{
public:
    int MinValue                      = 0;
    int MaxValue                      = 0;
    int Value                         = 0;
    bool HaveText                     = false;
    SLIDER_TEXT_POSITION TextPosition = STP_RIGHT;
    u8 Font                           = 0;
    u16 TextColor                     = 0;
    bool Unicode                      = true;
    int TextWidth                     = 0;
    TEXT_ALIGN_TYPE Align             = TS_LEFT;
    u16 TextFlags                     = 0;
    int TextX                         = 0;
    int TextY                         = 0;
    int BaseScrollStep                = 1;
    int ScrollStep                    = 1;
    u32 LastScrollTime                = 0;
    int DefaultTextOffset             = 2;

private:
    CGLTextTexture Text{ CGLTextTexture() };
    int m_ScrollMode{ 0 };

public:
    CGUIMinMaxButtons(int serial, u16 graphic, int x, int y, int minValue, int maxValue, int value);
    virtual ~CGUIMinMaxButtons();

    virtual bool IsPressedOuthit() { return true; }
    virtual Core::Vec2<i32> GetSize() { return Core::Vec2<i32>(36, 18); }
    virtual void Scroll(int delay);
    virtual void
    OnClick(Core::Optional<Core::TMousePos> a_position = Core::Optional<Core::TMousePos>());
    void UpdateText();
    void SetTextParameters(
        bool haveText,
        SLIDER_TEXT_POSITION textPosition,
        u8 font,
        u16 color,
        bool unicode,
        int textWidth         = 0,
        TEXT_ALIGN_TYPE align = TS_LEFT,
        u16 textFlags         = 0);
    virtual void PrepareTextures();
    virtual void Draw(bool checktrans = false);
    virtual bool Select();
};
