// MIT License
// Copyright (C) August 2016 Hotride

#include "GUIPolygonal.h"
#include "../Managers/MouseManager.h"

CGUIPolygonal::CGUIPolygonal(
    GUMP_OBJECT_TYPE type, int x, int y, int width, int height, bool callOnMouseUp)
    : CBaseGUI(type, 0, 0, 0, x, y)
    , Width(width)
    , Height(height)
    , CallOnMouseUp(callOnMouseUp)
{
}

CGUIPolygonal::~CGUIPolygonal()
{
}

bool CGUIPolygonal::Select()
{
    Core::TMousePos pos = g_MouseManager.GetPosition();
    int x = pos.x - m_X;
    int y = pos.y - m_Y;
    return (x >= 0 && y >= 0 && x < Width && y < Height);
}
