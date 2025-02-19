#pragma once

#include "Core/Minimal.h"
#include "MapObject.h"
#include <GL/glew.h>

class CLandObject : public CMapObject
{
public:
    char MinZ     = 0;
    char AverageZ = 0;
    // true - picture from texmaps, false - from art.mul
    bool IsStretched      = false;
    u16 OriginalGraphic   = 0;
    GLuint PositionBuffer = 0;
    GLuint VertexBuffer   = 0;
    GLuint NormalBuffer   = 0;

private:
    int GetDirectionZ(int direction);

public:
    CLandObject(int serial, u16 graphic, u16 color, short x, short y, char z);
    virtual ~CLandObject();

    Core::Rect<int> m_rect;
    Core::Vec3<double> m_Normals[4];

    virtual void UpdateGraphicBySeason();
    int CalculateCurrentAverageZ(int direction);
    virtual void Draw(int x, int y);
    virtual void Select(int x, int y);
    bool IsLandObject() { return true; }
    void UpdateZ(int zTop, int zRight, int zBottom);
    bool Ignored()
    {
        return (Graphic == 2 || Graphic == 0x1DB || (Graphic >= 0x1AE && Graphic <= 0x1B5));
    }
    virtual CLandObject* LandObjectPtr() { return this; }
};
