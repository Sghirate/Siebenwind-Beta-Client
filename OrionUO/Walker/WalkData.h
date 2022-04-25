#pragma once

class CWalkData
{
public:
    short X = 0;
    short Y = 0;
    char Z = 0;
    u8 Direction = 0;
    u16 Graphic = 0;
    u8 Flags = 0;

    CWalkData() {}
    CWalkData(short x, short y, char z, u8 direction, u16 graphic, u8 flags)
        : X(x)
        , Y(y)
        , Z(z)
        , Direction(direction)
        , Graphic(graphic)
        , Flags(flags)
    {
    }
    ~CWalkData() {}

    bool Run() { return (Direction & 0x80); }
    void GetOffset(float &x, float &y, float &steps);
};
