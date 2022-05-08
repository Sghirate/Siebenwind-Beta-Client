#include "GUIButton.h"
#include "Globals.h"
#include "OrionUO.h"
#include "SelectedObject.h"
#include "Gumps/Gump.h"

CGUIButton::CGUIButton(
    int serial, u16 graphic, u16 graphicSelected, u16 graphicPressed, int x, int y)
    : CGUIDrawObject(GOT_BUTTON, serial, graphic, 0, x, y)
    , GraphicSelected(graphicSelected)
    , GraphicPressed(graphicPressed)
{
}

CGUIButton::~CGUIButton()
{
}

void CGUIButton::PrepareTextures()
{
    g_Orion.ExecuteGump(Graphic);
    g_Orion.ExecuteGump(GraphicSelected);
    g_Orion.ExecuteGump(GraphicPressed);
}

u16 CGUIButton::GetDrawGraphic()
{
    u16 graphic = Graphic;

    if (g_GumpPressedElement == this)
    {
        graphic = GraphicPressed;
    }
    else if (g_GumpSelectedElement == this)
    {
        graphic = GraphicSelected;
    }

    return graphic;
}

void CGUIButton::OnMouseEnter()
{
    if (g_SelectedObject.Gump != nullptr)
    {
        g_SelectedObject.Gump->WantRedraw = true;
    }
}

void CGUIButton::OnMouseExit()
{
    if (g_LastSelectedObject.Gump != nullptr)
    {
        g_LastSelectedObject.Gump->WantRedraw = true;
    }
}
