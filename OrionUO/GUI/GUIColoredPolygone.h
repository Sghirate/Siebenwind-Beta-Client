#pragma once

#include "Core/Minimal.h"
#include "GUIPolygonal.h"

class CGUIColoredPolygone : public CGUIPolygonal
{
public:
    u8 ColorR    = 0;
    u8 ColorG    = 0;
    u8 ColorB    = 0;
    u8 ColorA    = 0;
    bool DrawDot = false;
    bool Focused = false;

    CGUIColoredPolygone(
        int serial, u16 color, int x, int y, int width, int height, int polygoneColor);
    virtual ~CGUIColoredPolygone();

    void UpdateColor(u16 color, int polygoneColor);
    virtual void Draw(bool checktrans = false);
    virtual void OnMouseEnter();
    virtual void OnMouseExit();
};
