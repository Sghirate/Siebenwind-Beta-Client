#pragma once

#include "BaseGUI.h"

class CGUILine : public CBaseGUI
{
    //!Целевая координата по оси X
    int TargetX = 0;

    //!Целевая координата по оси Y
    int TargetY = 0;

    //!R-компонента цвета
    u8 ColorR = 0;

    //!G-компонента цвета
    u8 ColorG = 0;

    //!B-компонента цвета
    u8 ColorB = 0;

    //!Значение альфа-канала
    u8 ColorA = 0;

public:
    CGUILine(int x, int y, int targetX, int targetY, int polygoneColor);
    virtual ~CGUILine();

    virtual void Draw(bool checktrans = false);
};
