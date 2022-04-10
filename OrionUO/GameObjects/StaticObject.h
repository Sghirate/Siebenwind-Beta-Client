// MIT License
// Copyright (C) August 2016 Hotride

#pragma once

#include "RenderStaticObject.h"

class CStaticObject : public CRenderStaticObject
{
    //Оригинальный индекс картинки
    u16 OriginalGraphic = 0;

public:
    CStaticObject(int serial, u16 graphic, u16 color, short x, short y, char z);
    virtual ~CStaticObject() {}

    virtual void UpdateGraphicBySeason();

    //Отрисовать объект
    virtual void Draw(int x, int y);

    //Выбрать объект
    virtual void Select(int x, int y);

    //Это объект статики
    bool IsStaticObject() { return true; }
};
