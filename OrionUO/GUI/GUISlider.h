#pragma once

#include "BaseGUI.h"

class CGUISlider : public CBaseGUI
{
public:
    //!ИД картинки в выбранном состоянии
    u16 GraphicSelected = 0;

    //!ИД картинки в зажатом состоянии
    u16 GraphicPressed = 0;

    //!ИД картинки фона
    u16 BackgroundGraphic = 0;

    //!Сборный фон
    bool CompositeBackground = false;

    //!Вертикальный или горизонтальный
    bool Vertical = false;

    //!Длина
    int Length = 0;

    //!Смещение в процентах
    float Percents = 0.0f;

    //!Смещение
    int Offset = 0;

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

    //!Текст в юникоде
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

    //!Шаг скроллера
    int ScrollStep = 15;

    //!Время последнего скроллинга
    u32 LastScrollTime = 0;

    //!Стандартное смещение текста
    int DefaultTextOffset = 2;

private:
    //!Текстура текста
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

    //!Скроллинг
    virtual void OnScroll(bool up, int delay);

    //!Нажатие на компоненту
    virtual void OnClick(int x, int y);

    //!Обновить текст
    void UpdateText();

    //!Пересчитать смещения
    virtual void CalculateOffset();

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

    virtual u16 GetDrawGraphic();

    virtual void Draw(bool checktrans = false);
    virtual bool Select();

    virtual void OnMouseEnter();
    virtual void OnMouseExit();
};
