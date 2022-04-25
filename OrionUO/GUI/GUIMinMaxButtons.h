#pragma once

#include "BaseGUI.h"
#include "Core/Input.h"
#include "Core/Optional.h"

class CGUIMinMaxButtons : public CBaseGUI
{
public:
    //!Минимальное значение
    int MinValue = 0;

    //!Максимальное значение
    int MaxValue = 0;

    //!Текущее значение
    int Value = 0;

    //!Имеет текст
    bool HaveText = false;

    //!Позиция текста
    SLIDER_TEXT_POSITION TextPosition = STP_RIGHT;

    //!Шрифт текста
    u8 Font = 0;

    //!Цвет текста
    u16 TextColor = 0;

    //!Юникод текст
    bool Unicode = true;

    //!Ширина текста
    int TextWidth = 0;

    //!Ориентация текста
    TEXT_ALIGN_TYPE Align = TS_LEFT;

    //!Флаги текста
    u16 TextFlags = 0;

    //!Координата текста по оси X
    int TextX = 0;

    //!Координата текста по оси Y
    int TextY = 0;

    //!Базовый шаг смещения
    int BaseScrollStep = 1;

    //!Текущий шаг смещения
    int ScrollStep = 1;

    //!Время последней итерации прокрутки
    u32 LastScrollTime = 0;

    //!Стандартное смещение текста
    int DefaultTextOffset = 2;

private:
    //!Текстура текста
    CGLTextTexture Text{ CGLTextTexture() };

    //!Метод прокрутки (NoScroll/Max/Min)
    int m_ScrollMode{ 0 };

public:
    CGUIMinMaxButtons(
        int serial, u16 graphic, int x, int y, int minValue, int maxValue, int value);
    virtual ~CGUIMinMaxButtons();

    virtual bool IsPressedOuthit() { return true; }

    virtual Core::Vec2<i32> GetSize() { return Core::Vec2<i32>(36, 18); }

    //Прокрутка
    virtual void Scroll(int delay);

    //Нажатие
    virtual void OnClick(Core::Optional<Core::TMousePos> a_position = Core::Optional<Core::TMousePos>());

    //!Обновить текст
    void UpdateText();

    //!Установить параметры текста
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

    virtual void Draw(bool checktrans = false);
    virtual bool Select();
};
