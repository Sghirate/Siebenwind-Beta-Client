#pragma once

#include "Core/Minimal.h"
#include "GLTexture.h"
#include <vector>

class CGLTextureCircleOfTransparency : public CGLTexture
{
public:
    int Radius = 0;
    int X = 0;
    int Y = 0;

    CGLTextureCircleOfTransparency();
    virtual ~CGLTextureCircleOfTransparency();

    //Создать текстуру
    static void CreatePixels(int radius, short &width, short &height, std::vector<u32> &pixels);

    //Создать текстуру
    bool Create(int radius);

    //Отрисовать текстуру
    virtual void Draw(int x, int y, bool checktrans = false);

    //Отрисовать текстуру
    void Redraw();
};

extern CGLTextureCircleOfTransparency g_CircleOfTransparency;
