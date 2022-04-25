#pragma once

#include "GLTexture.h"

struct WEB_LINK_RECT
{
    //Индекс ссылки
    u16 LinkID;

    //Начало ссылки относительно начальных координат текста
    int StartX;
    int StartY;

    //Конец ссылки относительно начальных координат текста
    int EndX;
    int EndY;
};

class CGLTextTexture : public CGLTexture
{
public:
    int LinesCount = 0;

    CGLTextTexture();
    virtual ~CGLTextTexture();

    bool Empty() { return (Texture == 0); }

    virtual void Clear();

    //Отрисовать текстуру
    virtual void Draw(int x, int y, bool checktrans = false);

    //Очистка веб-ссылки
    virtual void ClearWebLink() {}

    //Добавиление веб-ссылки
    virtual void AddWebLink(WEB_LINK_RECT &wl) {}

    //Проверка веб-ссылки под мышкой
    virtual u16 WebLinkUnderMouse(int x, int y) { return 0; }
};
