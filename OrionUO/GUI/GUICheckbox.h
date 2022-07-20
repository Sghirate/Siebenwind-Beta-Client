#pragma once

#include "Core/Minimal.h"
#include "GLEngine/GLTextTexture.h"
#include "GUIDrawObject.h"
#include "plugin/enumlist.h"
#include <string>

class CGUICheckbox : public CGUIDrawObject
{
public:
    u16 GraphicChecked                = 0;
    u16 GraphicSelected               = 0;
    u16 GraphicDisabled               = 0;
    bool Checked                      = false;
    SLIDER_TEXT_POSITION TextPosition = STP_RIGHT;
    int TextX                         = 0;
    int TextY                         = 0;
    int DefaultTextOffset             = 2;

protected:
    CGLTextTexture Text{ CGLTextTexture() };

    void UpdateTextPosition();

public:
    CGUICheckbox(int serial, u16 graphic, u16 graphicChecked, u16 graphicDisabled, int x, int y);
    virtual ~CGUICheckbox();

    void SetTextParameters(
        u8 font,
        const std::wstring& text,
        u16 color,
        SLIDER_TEXT_POSITION textPosition = STP_RIGHT,
        int textWidth                     = 0,
        TEXT_ALIGN_TYPE align             = TS_LEFT,
        u16 textFlags                     = 0);
    void SetTextParameters(
        u8 font,
        const std::string& text,
        u16 color,
        SLIDER_TEXT_POSITION textPosition = STP_RIGHT,
        int textWidth                     = 0,
        TEXT_ALIGN_TYPE align             = TS_LEFT,
        u16 textFlags                     = 0);

    virtual void PrepareTextures();
    virtual u16 GetDrawGraphic();

    virtual void Draw(bool checktrans = false);
    virtual bool Select();

    virtual void OnMouseEnter();
    virtual void OnMouseExit();
};
