#pragma once

#include "GLHeaders.h"
#include "Core/BitSet.h"
#include <GL/glew.h>

class CGLTexture
{
public:
    short Width                 = 0;
    short Height                = 0;
    short ImageOffsetX          = 0;
    short ImageOffsetY          = 0;
    short ImageWidth            = 0;
    short ImageHeight           = 0;
    GLuint VertexBuffer         = 0;
    GLuint MirroredVertexBuffer = 0;

    CGLTexture();
    virtual ~CGLTexture();

    GLuint Texture = 0;
    Core::DynamicBitSet m_hitMap;

    virtual void Draw(int x, int y, bool checktrans = false);
    virtual void Draw(int x, int y, int width, int height, bool checktrans = false);
    virtual void DrawRotated(int x, int y, float angle);
    virtual void DrawTransparent(int x, int y, bool stencil = true);
    virtual bool Select(int x, int y, bool pixelCheck = true);
    virtual void Clear();
};
