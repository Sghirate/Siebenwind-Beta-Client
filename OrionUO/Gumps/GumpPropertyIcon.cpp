#include "GumpPropertyIcon.h"
#include "ToolTip.h"
#include "SelectedObject.h"
#include "Managers/ConfigManager.h"
#include "Managers/ObjectPropertiesManager.h"

CGumpPropertyIcon::CGumpPropertyIcon(int x, int y)
    : CGump(GT_PROPERTY_ICON, 0, x, y)
{
    m_Locker.Serial = ID_GPI_LOCK_MOVING;

    int width = 0;
    g_ToolTip.CreateTextTexture(m_Texture, m_Text, width, 300);
}

CGumpPropertyIcon::~CGumpPropertyIcon()
{
}

void CGumpPropertyIcon::SetTextW(const std::wstring &val)
{
    m_Text = val;
    int width = 0;
    g_ToolTip.CreateTextTexture(
        m_Texture,
        val,
        width,
        (g_ConfigManager.GetItemPropertiesMode() == OPM_ALWAYS_UP ? 300 : 0));
    Object = nullptr;
    WantUpdateContent = true;
}

void CGumpPropertyIcon::PrepareContent()
{
    if (g_ConfigManager.GetItemPropertiesMode() == OPM_AT_ICON && Object != nullptr &&
        Object != g_SelectedObject.Object)
    {
        Object = nullptr;
        g_ObjectPropertiesManager.Reset();
        WantUpdateContent = true;
    }
}

void CGumpPropertyIcon::UpdateContent()
{
    Clear();

    u8 mode = g_ConfigManager.GetItemPropertiesMode();

    if (mode == OPM_AT_ICON)
    {
        if (Object == nullptr)
        {
            Add(new CGUIGumppic(0x00E3, 0, 0));
        }
        else
        {
            Add(new CGUIColoredPolygone(
                0, 0, 0, 0, m_Texture.Width + 12, m_Texture.Height + 8, 0xC07F7F7F));
            Add(new CGUIColoredPolygone(
                0, 0, 6, 20, m_Texture.Width, m_Texture.Height - 16, 0xC0000000));

            Add(new CGUIExternalTexture(&m_Texture, false, 6, 2));
        }
    }
    else if (mode == OPM_ALWAYS_UP)
    {
        int height = m_Texture.Height;

        if (height < 150)
        {
            height = 150;
        }

        Add(new CGUIColoredPolygone(0, 0, 0, 0, m_Texture.Width + 12, height + 8, 0xC07F7F7F));
        Add(new CGUIColoredPolygone(0, 0, 6, 20, m_Texture.Width, height - 16, 0xC0000000));

        Add(new CGUIExternalTexture(&m_Texture, false, 6, 2));

        Add(new CGUIButton(ID_GPI_MINIMIZE, 0x0A96, 0x0A97, 0x0A97, m_Texture.Width - 10, 0));
    }
    else if (mode == OPM_FOLLOW_MOUSE)
    {
        Add(new CGUIGumppic(0x00E1, 0, 0));
    }
    else if (mode == OPM_SINGLE_CLICK)
    {
        Add(new CGUIGumppic(0x00EC, 0, 0));
    }
}

void CGumpPropertyIcon::GUMP_BUTTON_EVENT_C
{
    if (serial == ID_GPI_LOCK_MOVING)
    {
        LockMoving = !LockMoving;
    }
    else if (serial == ID_GPI_MINIMIZE)
    {
        g_ConfigManager.SetItemPropertiesMode(OPM_FOLLOW_MOUSE);
    }
}

bool CGumpPropertyIcon::OnLeftMouseButtonDoubleClick()
{

    u8 mode = g_ConfigManager.GetItemPropertiesMode() + 1;

    if (mode > OPM_SINGLE_CLICK)
    {
        mode = OPM_AT_ICON;
    }

    g_ConfigManager.SetItemPropertiesMode(mode);

    return true;
}
