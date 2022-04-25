#include "GUIResizeButton.h"

CGUIResizeButton::CGUIResizeButton(
    int serial, u16 graphic, u16 graphicSelected, u16 graphicPressed, int x, int y)
    : CGUIButton(serial, graphic, graphicSelected, graphicPressed, x, y)
{
    Type = GOT_RESIZEBUTTON;
}

CGUIResizeButton::~CGUIResizeButton()
{
}
