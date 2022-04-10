#include "GUISkillGroup.h"
#include "GUIButton.h"
#include "GUITextEntry.h"
#include "GUISkillItem.h"
#include "../OrionUO.h"
#include "../SkillGroup.h"
#include "../Managers/MouseManager.h"

CGUISkillGroup::CGUISkillGroup(
    int serial, int minimizeSerial, CSkillGroupObject *group, int x, int y)
    : CBaseGUI(GOT_SKILLGROUP, serial, 0, 0, x, y)
{
    const bool isMinimized = !group->Maximized;
    const u16 graphic = (isMinimized ? 0x0827 : 0x0826);
    m_Minimizer = new CGUIButton(minimizeSerial, graphic, graphic, graphic, 0, 0);
    SetMinimized(isMinimized);

    m_Name = new CGUITextEntry(serial, 0, 0, 0, 16, -5, 0, false, 6);
    m_Name->m_Entry.SetTextA(group->Name);
}

CGUISkillGroup::~CGUISkillGroup()
{
    RELEASE_POINTER(m_Minimizer);
    RELEASE_POINTER(m_Name);
}

void CGUISkillGroup::SetMinimized(bool val)
{
    assert(m_Minimizer);
    m_Minimized = val;
    const u16 graphic = (val ? 0x0827 : 0x0826);
    m_Minimizer->Graphic = graphic;
    m_Minimizer->GraphicSelected = graphic;
    m_Minimizer->GraphicPressed = graphic;
}

void CGUISkillGroup::UpdateDataPositions()
{
    int y = 0;

    QFOR(item, m_Items, CBaseGUI *)
    {
        item->SetY(y);
        y += 17;
    }
}

void CGUISkillGroup::PrepareTextures()
{
    m_Minimizer->PrepareTextures();
    g_Orion.ExecuteGump(0x0835);
    m_Name->PrepareTextures();

    QFOR(item, m_Items, CBaseGUI *)
    item->PrepareTextures();
}

bool CGUISkillGroup::EntryPointerHere()
{
    return (g_EntryPointer == &m_Name->m_Entry);
}

CBaseGUI *CGUISkillGroup::SelectedItem()
{
    CBaseGUI *selected = m_Name;

    if (g_Orion.PolygonePixelsInXY(m_X + m_Minimizer->GetX(), m_Y + m_Minimizer->GetY(), 14, 14))
    {
        selected = m_Minimizer;
    }
    else if (!GetMinimized())
    {
        Core::TMousePos oldMouse = g_MouseManager.GetPosition();
        g_MouseManager.SetPosition(Core::Vec2<i32>(oldMouse.x - m_X, oldMouse.y - (m_Y + 19)));
        QFOR(item, m_Items, CBaseGUI *)
        {
            if (item->Select())
            {
                selected = item;
                if (item->Type == GOT_SKILLITEM)
                    selected = ((CGUISkillItem *)item)->SelectedItem();
                break;
            }
        }
        g_MouseManager.SetPosition(oldMouse);
    }
    return selected;
}

Core::Vec2<i32> CGUISkillGroup::GetSize()
{
    Core::Vec2<i32> size(220, 19);
    if (!GetMinimized() && m_Items != nullptr)
        size.y += GetItemsCount() * 17;
    return size;
}

void CGUISkillGroup::Draw(bool checktrans)
{
    glTranslatef((GLfloat)m_X, (GLfloat)m_Y, 0.0f);
    m_Minimizer->Draw(checktrans);
    bool drawOrnament = true;
    if (m_Name->Focused && g_EntryPointer == &m_Name->m_Entry)
    {
        drawOrnament = false;
        g_GL.DrawPolygone(16, 0, 200, 14);
    }
    else if (m_Name->Focused)
    {
        g_GL.DrawPolygone(16, 0, m_Name->m_Entry.m_Texture.Width, 14);
    }
    m_Name->Draw(checktrans);
    if (drawOrnament)
    {
        int x = 11 + m_Name->m_Entry.m_Texture.Width;
        int width = 215 - x;
        if (x > 0)
            g_Orion.DrawGump(0x0835, 0, x, 5, width, 0);
    }
    if (!GetMinimized() && m_Items != nullptr)
    {
        glTranslatef(0.0f, 19.0f, 0.0f);
        QFOR(item, m_Items, CBaseGUI *)
        item->Draw(checktrans);
        glTranslatef(0.0f, -19.0f, 0.0f);
    }
    glTranslatef((GLfloat)-m_X, (GLfloat)-m_Y, 0.0f);
}

bool CGUISkillGroup::Select()
{
    Core::TMousePos pos = g_MouseManager.GetPosition();
    int y = pos.y - m_Y;
    int x = pos.x - m_X;
    bool result = (x >= 0 && y >= 0 && x < 220 && y < 19);
    if (!GetMinimized() && !result)
    {
        Core::TMousePos oldMouse = g_MouseManager.GetPosition();
        g_MouseManager.SetPosition(Core::Vec2<i32>(oldMouse.x - m_X, oldMouse.y - (m_Y + 19)));
        QFOR(item, m_Items, CBaseGUI *)
        {
            if (item->Select())
            {
                result = true;
                break;
            }
        }
        g_MouseManager.SetPosition(oldMouse);
    }
    return result;
}
