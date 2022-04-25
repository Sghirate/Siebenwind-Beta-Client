#pragma once

#include "BaseGUI.h"

class CGUIScrollBackground : public CBaseGUI
{
public:
    //!Высота
    int Height = 0;

    //!Смещение по оси X
    int OffsetX = 0;

    //!Смещение нижней части по оси X
    int BottomOffsetX = 0;

    //!Ширина
    int Width = 0;

    //!Рабочая область
    Core::Rect<i32> WorkSpace = Core::Rect<i32>();

    CGUIScrollBackground(int serial, u16 graphic, int x, int y, int height);
    virtual ~CGUIScrollBackground();

    //!Обновить высоту
    void UpdateHeight(int height);

    virtual Core::Vec2<i32> GetSize() { return Core::Vec2<i32>(Width, Height); }

    virtual void PrepareTextures();

    virtual void Draw(bool checktrans = false);
    virtual bool Select();
};
