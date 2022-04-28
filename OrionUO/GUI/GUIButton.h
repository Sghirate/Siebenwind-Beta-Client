#pragma once

#include "GUIDrawObject.h"

class CGUIButton : public CGUIDrawObject
{
public:
    //!ИД картинки для состояния, когда мышка находится над кнопкой
    u16 GraphicSelected = 0;

    //!ИД картинки для зажатого состояния
    u16 GraphicPressed = 0;

    //!Индекс страницы, если указан -1 то используется событие OnButton для гампа с идентификатором кнопки, если отличное значение - переход на страницу без вызова события
    int ToPage = -1;

    //!Флаг обработки зажатого состояния кнопки каждую итерацию рендера
    bool ProcessPressedState = false;

    CGUIButton(
        int serial,
        u16 graphic,
        u16 graphicSelected,
        u16 graphicPressed,
        int x,
        int y);
    virtual ~CGUIButton();

    virtual void PrepareTextures();
    virtual u16 GetDrawGraphic();

    virtual void OnMouseEnter();
    virtual void OnMouseExit();
};
