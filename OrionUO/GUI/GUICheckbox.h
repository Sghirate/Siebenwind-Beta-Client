#pragma once

#include "GUIDrawObject.h"
#include "../plugin/enumlist.h"

class CGUICheckbox : public CGUIDrawObject
{
public:
    //!ИД картинки в нажатом состоянии
    u16 GraphicChecked = 0;

    //!ИД картинки в выбранном состоянии
    u16 GraphicSelected = 0;

    //!ИД картинки в отключенном состоянии
    u16 GraphicDisabled = 0;

    //!Состояние компоненты
    bool Checked = false;

    //!Позиция текста
    SLIDER_TEXT_POSITION TextPosition = STP_RIGHT;

    //!Координата текста по оси X
    int TextX = 0;

    //!Координата текста по оси Y
    int TextY = 0;

    //!Стандартное смещение текста
    int DefaultTextOffset = 2;

protected:
    CGLTextTexture Text{ CGLTextTexture() };

    void UpdateTextPosition();

public:
    CGUICheckbox(
        int serial,
        u16 graphic,
        u16 graphicChecked,
        u16 graphicDisabled,
        int x,
        int y);
    virtual ~CGUICheckbox();

    //!Установить текст
    void SetTextParameters(
        u8 font,
        const std::wstring &text,
        u16 color,
        SLIDER_TEXT_POSITION textPosition = STP_RIGHT,
        int textWidth = 0,
        TEXT_ALIGN_TYPE align = TS_LEFT,
        u16 textFlags = 0);
    void SetTextParameters(
        u8 font,
        const std::string &text,
        u16 color,
        SLIDER_TEXT_POSITION textPosition = STP_RIGHT,
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
