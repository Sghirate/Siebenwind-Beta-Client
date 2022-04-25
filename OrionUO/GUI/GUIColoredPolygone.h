#pragma once

#include "GUIPolygonal.h"

class CGUIColoredPolygone : public CGUIPolygonal
{
public:
    //!R-компонента цвета
    u8 ColorR = 0;

    //!G-компонента цвета
    u8 ColorG = 0;

    //!B-компонента цвета
    u8 ColorB = 0;

    //!Значение альфа-канала
    u8 ColorA = 0;

    //!Отрисовывать ли белую точку
    bool DrawDot = false;

    //!Флаг выбора текущей компоненты
    bool Focused = false;

    CGUIColoredPolygone(
        int serial, u16 color, int x, int y, int width, int height, int polygoneColor);
    virtual ~CGUIColoredPolygone();

    //!Обновить цвет
    void UpdateColor(u16 color, int polygoneColor);

    virtual void Draw(bool checktrans = false);

    virtual void OnMouseEnter();
    virtual void OnMouseExit();
};
