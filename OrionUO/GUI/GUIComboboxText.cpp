#include "GUIComboboxText.h"
#include "SelectedObject.h"
#include "Gumps/Gump.h"

CGUIComboboxText::CGUIComboboxText(
    u16 color,
    u8 font,
    const std::string &str,
    int width,
    TEXT_ALIGN_TYPE align,
    u16 flags)
    : CGUIText(color, 0, 0)
{
    Serial = 0xFFFFFFFE;
    CreateTextureA(font, str, width, align, flags);
}

CGUIComboboxText::CGUIComboboxText(
    u16 color,
    u8 font,
    const std::wstring &str,
    int width,
    TEXT_ALIGN_TYPE align,
    u16 flags)
    : CGUIText(color, 0, 0)
{
    Serial = 0xFFFFFFFE;
    CreateTextureW(font, str, 30, width, align, flags);
}

CGUIComboboxText::~CGUIComboboxText()
{
}

void CGUIComboboxText::OnMouseEnter()
{
    if (g_SelectedObject.Gump != nullptr)
    {
        g_SelectedObject.Gump->WantRedraw = true;
    }
}

void CGUIComboboxText::OnMouseExit()
{
    if (g_LastSelectedObject.Gump != nullptr)
    {
        g_LastSelectedObject.Gump->WantRedraw = true;
    }
}
