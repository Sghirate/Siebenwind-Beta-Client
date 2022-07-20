#pragma once

#include "GUIButton.h"

class CGUIButtonTileart : public CGUIButton
{
    //!ИД картинки статики
    u16 TileGraphic = 0;

    //!Цвет картинки статики
    u16 TileColor = 0;

    //!Координата по оси X для отображения статики
    int TileX = 0;

    //!Координата по оси Y для отображения статики
    int TileY = 0;

public:
    CGUIButtonTileart(
        int serial,
        u16 graphic,
        u16 graphicSelected,
        u16 graphicPressed,
        int x,
        int y,
        u16 tileGraphic,
        u16 tileColor,
        int tileX,
        int tileY);
    virtual ~CGUIButtonTileart();

    virtual Core::Vec2<i32> GetSize();

    virtual void PrepareTextures();

    virtual void Draw(bool checktrans = false);
    virtual bool Select();
};
