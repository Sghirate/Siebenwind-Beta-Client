#include "GUIMenuObject.h"
#include "../OrionUO.h"
#include "../SelectedObject.h"
#include "../Managers/MouseManager.h"
#include "../Gumps/Gump.h"
#include "../Gumps/GumpMenu.h"

CGUIMenuObject::CGUIMenuObject(
    int serial, u16 graphic, u16 color, int x, int y, const std::string &text)
    : CGUITilepic(graphic, color, x, y)
    , Text(text)
{
    Serial = serial;
    MoveOnDrag = true;
}

CGUIMenuObject::~CGUIMenuObject()
{
}

bool CGUIMenuObject::Select()
{
    Core::TMousePos pos = g_MouseManager.GetPosition();
    int x = pos.x - m_X;
    int y = pos.y - m_Y;
    Core::Vec2<i32> size = g_Orion.GetStaticArtDimension(Graphic);
    return (x >= 0 && y >= 0 && x < size.x && y < size.y);
}

void CGUIMenuObject::OnMouseEnter()
{
    if (g_SelectedObject.Gump != nullptr && g_SelectedObject.Gump->GumpType == GT_MENU)
    {
        CGumpMenu *menu = (CGumpMenu *)g_SelectedObject.Gump;
        if (menu->Text != Text)
        {
            menu->Text = Text;
            menu->TextChanged = true;
        }
    }
}

void CGUIMenuObject::OnMouseExit()
{
    if (g_LastSelectedObject.Gump != nullptr && g_LastSelectedObject.Gump->GumpType == GT_MENU)
    {
        CGumpMenu *menu = (CGumpMenu *)g_LastSelectedObject.Gump;
        if (menu->Text == Text)
        {
            menu->Text = "";
            menu->TextChanged = true;
        }
    }
}
