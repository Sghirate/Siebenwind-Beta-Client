#include "GUITextEntry.h"
#include "../SelectedObject.h"
#include "../Gumps/Gump.h"
#include "../Managers/MouseManager.h"

CGUITextEntry::CGUITextEntry(
    int serial,
    u16 color,
    u16 colorSelected,
    u16 colorFocused,
    int x,
    int y,
    int maxWidth,
    bool unicode,
    u8 font,
    TEXT_ALIGN_TYPE align,
    u16 textFlags,
    int maxLength)
    : CBaseGUI(GOT_TEXTENTRY, serial, 0, color, x, y)
    , ColorSelected(colorSelected)
    , ColorFocused(colorFocused)
    , Unicode(unicode)
    , Font(font)
    , Align(align)
    , TextFlags(textFlags)
    , m_Entry(maxLength, maxWidth, maxWidth)
{
}

CGUITextEntry::~CGUITextEntry()
{
    m_Entry.Clear();
}

bool CGUITextEntry::EntryPointerHere()
{
    return (g_EntryPointer == &m_Entry);
}

Core::Vec2<i32> CGUITextEntry::GetSize()
{
    return Core::Vec2<i32>(m_Entry.m_Texture.Width, m_Entry.m_Texture.Height);
}

void CGUITextEntry::SetGlobalColor(bool use, int color, int selected, int focused)
{
    UseGlobalColor = use;

    if (use)
    {
        GlobalColorR = ToColorR(color);
        GlobalColorG = ToColorG(color);
        GlobalColorB = ToColorB(color);
        GlobalColorA = ToColorA(color);

        if (GlobalColorA == 0u)
        {
            GlobalColorA = 0xFF;
        }

        GlobalColorSelectedR = ToColorR(selected);
        GlobalColorSelectedG = ToColorG(selected);
        GlobalColorSelectedB = ToColorB(selected);
        GlobalColorSelectedA = ToColorA(selected);

        if (GlobalColorSelectedA == 0u)
        {
            GlobalColorSelectedA = 0xFF;
        }

        GlobalColorFocusedR = ToColorR(focused);
        GlobalColorFocusedG = ToColorG(focused);
        GlobalColorFocusedB = ToColorB(focused);
        GlobalColorFocusedA = ToColorA(focused);

        if (GlobalColorFocusedA == 0u)
        {
            GlobalColorFocusedA = 0xFF;
        }
    }
}

void CGUITextEntry::OnClick(CGump *gump, int x, int y)
{
    m_Entry.OnClick(gump, Font, Unicode, x, y, Align, TextFlags);
}

void CGUITextEntry::OnMouseEnter()
{
    if (g_SelectedObject.Gump != nullptr)
    {
        g_SelectedObject.Gump->WantRedraw = true;
    }
}

void CGUITextEntry::OnMouseExit()
{
    if (g_LastSelectedObject.Gump != nullptr)
    {
        g_LastSelectedObject.Gump->WantRedraw = true;
    }
}

void CGUITextEntry::PrepareTextures()
{
    u16 color = Color;

    if (!UseGlobalColor)
    {
        if (Focused || &m_Entry == g_EntryPointer)
        {
            color = ColorFocused;
        }
        else if (
            g_GumpSelectedElement == this ||
            (CheckOnSerial && g_CurrentCheckGump != nullptr &&
             g_CurrentCheckGump == g_SelectedObject.Gump && Serial == g_SelectedObject.Serial &&
             g_SelectedObject.Object != nullptr && g_SelectedObject.Object->IsGUI()))
        {
            color = ColorSelected;
        }
    }

    if ((color != 0u) && Unicode)
    {
        color++;
    }

    if (Unicode)
    {
        m_Entry.PrepareToDrawW(Font, color, Align, TextFlags);
    }
    else
    {
        m_Entry.PrepareToDrawA(Font, color, Align, TextFlags);
    }
}

void CGUITextEntry::Draw(bool checktrans)
{
    int y = m_Y;
    u16 color = Color;

    if (Focused || &m_Entry == g_EntryPointer)
    {
        if (UseGlobalColor)
        {
            glColor4ub(
                GlobalColorFocusedR, GlobalColorFocusedG, GlobalColorFocusedB, GlobalColorFocusedA);
        }
        else
        {
            color = ColorFocused;
        }

        y += FocusedOffsetY;
    }
    else if (
        g_GumpSelectedElement == this ||
        (CheckOnSerial && g_CurrentCheckGump != nullptr &&
         g_CurrentCheckGump == g_SelectedObject.Gump && Serial == g_SelectedObject.Serial &&
         g_SelectedObject.Object != nullptr && g_SelectedObject.Object->IsGUI()))
    {
        if (UseGlobalColor)
        {
            glColor4ub(
                GlobalColorSelectedR,
                GlobalColorSelectedG,
                GlobalColorSelectedB,
                GlobalColorSelectedA);
        }
        else
        {
            color = ColorSelected;
        }
    }
    else if (UseGlobalColor)
    {
        glColor4ub(GlobalColorR, GlobalColorG, GlobalColorB, GlobalColorA);
    }

    if ((color != 0u) && Unicode)
    {
        color++;
    }

    if (checktrans)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        if (Unicode)
        {
            m_Entry.DrawW(Font, color, m_X, y, Align, TextFlags);
        }
        else
        {
            m_Entry.DrawA(Font, color, m_X, y, Align, TextFlags);
        }

        glDisable(GL_BLEND);

        glEnable(GL_STENCIL_TEST);

        if (Unicode)
        {
            m_Entry.DrawW(Font, color, m_X, y, Align, TextFlags);
        }
        else
        {
            m_Entry.DrawA(Font, color, m_X, y, Align, TextFlags);
        }

        glDisable(GL_STENCIL_TEST);
    }
    else
    {
        if (Unicode)
        {
            m_Entry.DrawW(Font, color, m_X, y, Align, TextFlags);
        }
        else
        {
            m_Entry.DrawA(Font, color, m_X, y, Align, TextFlags);
        }
    }

    if (UseGlobalColor)
    {
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    }
}

bool CGUITextEntry::Select()
{
    Core::TMousePos pos = g_MouseManager.GetPosition();
    int x = pos.x - m_X;
    int y = pos.y - m_Y;
    return (x >= 0 && y >= 0 && x < m_Entry.m_Texture.Width && y < m_Entry.m_Texture.Height);
}
