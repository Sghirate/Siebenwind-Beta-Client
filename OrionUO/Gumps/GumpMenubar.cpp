#include "GumpMenubar.h"
#include "Globals.h"
#include "OrionUO.h"
#include "ToolTip.h"
#include "SelectedObject.h"
#include "Managers/ClilocManager.h"
#include "Network/Packets.h"

enum
{
    ID_GMB_NONE,

    ID_GMB_MINIMIZE,
    ID_GMB_MAP,
    ID_GMB_PAPERDOLL,
    ID_GMB_INVENTORY,
    ID_GMB_JOURNAL,
    ID_GMB_CHAT,
    ID_GMB_HELP,
    ID_GMB_INFO,

    ID_GMB_COUNT,
};

CGumpMenubar::CGumpMenubar(short x, short y)
    : CGump(GT_MENUBAR, 0, x, y)
{
    Page = 2;

    Add(new CGUIPage(1));
    Add(new CGUIResizepic(0, 0x13BE, 0, 0, 30, 27));
    Add(new CGUIButton(ID_GMB_MINIMIZE, 0x15A1, 0x15A1, 0x15A1, 5, 3));

    Add(new CGUIPage(2));
    CGLTexture* th1 = g_Orion.ExecuteGump(0x098B);
    CGLTexture* th2 = g_Orion.ExecuteGump(0x098D);

    int smallWidth = 50;
    if (th1 != nullptr)
    {
        smallWidth = th1->Width;
    }

    int largeWidth = 100;
    if (th2 != nullptr)
    {
        largeWidth = th2->Width;
    }

    static const int textPosTable[7][5] = { { 0x098B, 30, 32, smallWidth, ID_GMB_MAP },
                                            { 0x098D, 93, 96, largeWidth, ID_GMB_PAPERDOLL },
                                            { 0x098D, 201, 204, largeWidth, ID_GMB_INVENTORY },
                                            { 0x098D, 309, 312, largeWidth, ID_GMB_JOURNAL },
                                            { 0x098B, 417, 422, smallWidth, ID_GMB_CHAT },
                                            { 0x098B, 480, 482, smallWidth, ID_GMB_HELP },
                                            { 0x098B, 543, 546, smallWidth, ID_GMB_INFO } };

    static const std::wstring text[7] = {
        g_ClilocManager.GetCliloc(g_Language)->GetW(3000430, false, "Map"),
        g_ClilocManager.GetCliloc(g_Language)->GetW(3002133, false, "Paperdoll"),
        g_ClilocManager.GetCliloc(g_Language)->GetW(3000431, false, "Inventory"),
        g_ClilocManager.GetCliloc(g_Language)->GetW(3002135, false, "Journal"),
        L"Chat",
        g_ClilocManager.GetCliloc(g_Language)->GetW(3000134, false, "Help"),
        L"< ? >"
    };

    Add(new CGUIResizepic(0, 0x13BE, 0, 0, 601, 27));
    Add(new CGUIButton(ID_GMB_MINIMIZE, 0x15A4, 0x15A4, 0x15A4, 5, 3));

    for (int i = 0; i < 7; i++)
    {
        Add(new CGUIButton(
            textPosTable[i][4],
            textPosTable[i][0],
            textPosTable[i][0],
            textPosTable[i][0],
            textPosTable[i][1],
            1));

        CGUITextEntry* entry = (CGUITextEntry*)Add(new CGUITextEntry(
            textPosTable[i][4],
            0,
            0x0036,
            0x0036,
            textPosTable[i][2],
            2,
            textPosTable[i][3],
            true,
            1,
            TS_CENTER));
        entry->m_Entry.SetTextW(text[i]);
        entry->CheckOnSerial  = true;
        entry->ReadOnly       = true;
        entry->FocusedOffsetY = 2;
    }
}

CGumpMenubar::~CGumpMenubar()
{
}

void CGumpMenubar::SetOpened(bool val)
{
    m_Opened = val;

    if (val)
    {
        Page = 2;
    }
    else
    {
        Page = 1;
    }

    WantRedraw = true;
}

void CGumpMenubar::InitToolTip()
{
    u32 id = g_SelectedObject.Serial;

    if (!Minimized)
    {
        switch (id)
        {
            case ID_GMB_MINIMIZE:
            {
                g_ToolTip.Set(L"Minimize the menubar gump");
                break;
            }
            case ID_GMB_MAP:
            {
                g_ToolTip.Set(L"Open the minimap gump");
                break;
            }
            case ID_GMB_PAPERDOLL:
            {
                g_ToolTip.Set(L"Open the paperdoll gump");
                break;
            }
            case ID_GMB_INVENTORY:
            {
                g_ToolTip.Set(L"Open backpack");
                break;
            }
            case ID_GMB_JOURNAL:
            {
                g_ToolTip.Set(L"Open the journal gump");
                break;
            }
            case ID_GMB_CHAT:
            {
                g_ToolTip.Set(L"Open the chat gump");
                break;
            }
            case ID_GMB_HELP:
            {
                g_ToolTip.Set(L"Open server's help menu gump");
                break;
            }
            case ID_GMB_INFO:
            {
                g_ToolTip.Set(L"This option is not work now");
                break;
            }
            default: break;
        }
    }
    else
    {
        g_ToolTip.Set(L"Maximize the menubar gump");
    }
}

void CGumpMenubar::GUMP_BUTTON_EVENT_C
{
    switch (serial)
    {
        case ID_GMB_MINIMIZE:
        {
            m_Opened = !m_Opened;

            Page              = 1 + (int)m_Opened;
            WantUpdateContent = true;

            break;
        }
        case ID_GMB_MAP:
        {
            g_Orion.OpenMinimap();

            break;
        }
        case ID_GMB_PAPERDOLL:
        {
            g_Orion.PaperdollReq(g_PlayerSerial);

            break;
        }
        case ID_GMB_INVENTORY:
        {
            g_Orion.OpenBackpack();

            break;
        }
        case ID_GMB_JOURNAL:
        {
            g_Orion.OpenJournal();

            break;
        }
        case ID_GMB_CHAT:
        {
            g_Orion.OpenChat();

            break;
        }
        case ID_GMB_HELP:
        {
            CPacketHelpRequest().Send();

            break;
        }
        case ID_GMB_INFO:
        {
            break;
        }
        default: break;
    }
}

void CGumpMenubar::GUMP_TEXT_ENTRY_EVENT_C
{
    QFOR(item, m_Items, CBaseGUI*)
    {
        if (item->Type == GOT_TEXTENTRY)
        {
            CGUITextEntry* entry = (CGUITextEntry*)item;
            entry->Focused       = (entry->Serial == serial);
        }
    }

    OnButton(serial);
}

void CGumpMenubar::OnLeftMouseButtonUp()
{
    CGump::OnLeftMouseButtonUp();

    QFOR(item, m_Items, CBaseGUI*)
    {
        if (item->Type == GOT_TEXTENTRY)
        {
            CGUITextEntry* entry = (CGUITextEntry*)item;
            entry->Focused       = false;
        }
    }

    WantRedraw = true;
}
