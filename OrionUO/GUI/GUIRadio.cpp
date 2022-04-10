// MIT License
// Copyright (C) August 2016 Hotride

#include "GUIRadio.h"

CGUIRadio::CGUIRadio(
    int serial, u16 graphic, u16 graphicChecked, u16 graphicDisabled, int x, int y)
    : CGUICheckbox(serial, graphic, graphicChecked, graphicDisabled, x, y)
{
    Type = GOT_RADIO;
}

CGUIRadio::~CGUIRadio()
{
}
