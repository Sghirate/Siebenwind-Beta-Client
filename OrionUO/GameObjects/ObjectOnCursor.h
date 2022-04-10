// MIT License
// Copyright (C) August 2016 Hotride

#pragma once

struct STATIC_TILES;

class CObjectOnCursor
{
public:
    //!Активен
    bool Enabled = false;
    //!Сброшен
    bool Dropped = false;
    //!Серийник объекта
    u32 Serial = 0;
    //!Индекс картинки
    u16 Graphic = 0;
    //!Цвет
    u16 Color = 0;
    //!Количество для перемещения
    u16 Count = 0;
    //!Количество ранее в стеке
    u16 TotalCount = 0;
    //!Слой
    u8 Layer = 0;
    //!Флаги
    u8 Flags = 0;
    //!Контейнер
    u32 Container = 0;
    //!Координата X
    u16 X = 0;
    //!Координата Y
    u16 Y = 0;
    //!Координата Z
    char Z = 0;
    //!Это игровая фигура
    bool IsGameFigure = false;
    //!Указатель на тайлдату для объекта
    STATIC_TILES *TiledataPtr = nullptr; //!Обновлен ли предмет в мире
    bool UpdatedInWorld = false;

    CObjectOnCursor() {}
    virtual ~CObjectOnCursor() {}

    void Clear();

    u16 GetDrawGraphic(bool &doubleDraw);
};

extern CObjectOnCursor g_ObjectInHand;
