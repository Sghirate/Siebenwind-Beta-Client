#pragma once

#include "BaseGUI.h"
#include "../TextEngine/EntryText.h"

class CGUITextEntry : public CBaseGUI
{
public:
    //!Цвет текста в выбранном состоянии
    u16 ColorSelected = 0;

    //!Цвет текста в состоянии фокуса
    u16 ColorFocused = 0;

    //!Текст в юникоде
    bool Unicode = false;

    //!Шрифт
    u8 Font = 0;

    //!Ориентация текста
    TEXT_ALIGN_TYPE Align = TS_LEFT;

    //!Флаги текста
    u16 TextFlags = 0;

    //!Производить выбор объекта по серийнику может быть выбран фоном, хит-боксом и т.п.)
    bool CheckOnSerial = false;

    //!Только для чтения
    bool ReadOnly = false;

    //!Флаг фокуса
    bool Focused = false;

    //!Флаг использования глобального цвета
    bool UseGlobalColor = false;

    //!R-компонента глобального OGL цвета
    u8 GlobalColorR = 0;

    //!G-компонента глобального OGL цвета
    u8 GlobalColorG = 0;

    //!B-компонента глобального OGL цвета
    u8 GlobalColorB = 0;

    //!Альфа-канал глобального OGL цвета
    u8 GlobalColorA = 0;

    //!R-компонента глобального OGL цвета когда компонента выбрана
    u8 GlobalColorSelectedR = 0;

    //!G-компонента глобального OGL цвета когда компонента выбрана
    u8 GlobalColorSelectedG = 0;

    //!B-компонента глобального OGL цвета когда компонента выбрана
    u8 GlobalColorSelectedB = 0;

    //!Альфа-канал глобального OGL цвета когда компонента выбрана
    u8 GlobalColorSelectedA = 0;

    //!R-компонента глобального OGL цвета когда компонента находится в фокусе
    u8 GlobalColorFocusedR = 0;

    //!G-компонента глобального OGL цвета когда компонента находится в фокусе
    u8 GlobalColorFocusedG = 0;

    //!B-компонента глобального OGL цвета когда компонента находится в фокусе
    u8 GlobalColorFocusedB = 0;

    //!Альфа-канал глобального OGL цвета когда компонента находится в фокусе
    u8 GlobalColorFocusedA = 0;

    //!Смещение текста когда компонента находится в фокусе
    char FocusedOffsetY = 0;

    CGUITextEntry(
        int serial,
        u16 color,
        u16 colorSelected,
        u16 colorFocused,
        int x,
        int y,
        int maxWidth = 0,
        bool unicode = true,
        u8 font = 0,
        TEXT_ALIGN_TYPE align = TS_LEFT,
        u16 textFlags = 0,
        int maxLength = 0);
    virtual ~CGUITextEntry();

    //!Объект класса для ввода текста
    class CEntryText m_Entry;

    virtual void PrepareTextures();

    virtual bool EntryPointerHere();

    virtual Core::Vec2<i32> GetSize();

    //!Установить глобальный OGL цвет перед отрисовкой текста
    void SetGlobalColor(bool use, int color, int selected, int focused);

    //!Нажатие на компоненту
    void OnClick(CGump *gump, int x, int y);

    virtual void OnMouseEnter();
    virtual void OnMouseExit();

    virtual void Draw(bool checktrans = false);
    virtual bool Select();
};
