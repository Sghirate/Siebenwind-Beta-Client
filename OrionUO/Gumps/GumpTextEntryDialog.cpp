#include "GumpTextEntryDialog.h"
#include "Globals.h"
#include "OrionUO.h"
#include "SelectedObject.h"
#include "Managers/GumpManager.h"
#include "ScreenStages/GameBlockedScreen.h"
#include "Network/Packets.h"
#include <utility>

CGumpTextEntryDialog::CGumpTextEntryDialog(
    u32 serial,
    short x,
    short y,
    u8 variant,
    int maxLength,
    std::string text,
    std::string description)
    : CGump(GT_TEXT_ENTRY_DIALOG, serial, x, y)
    , Text(std::move(text))
    , m_Description(std::move(description))
    , Variant(variant)
    , m_MaxLength(maxLength)
{
    NoMove = true;
    Blocked = true;

    if (g_GrayMenuCount == 0)
    {
        g_Orion.InitScreen(GS_GAME_BLOCKED);
        g_GameBlockedScreen.Code = 2;
    }

    g_GrayMenuCount++;
}

CGumpTextEntryDialog::~CGumpTextEntryDialog()
{
}

void CGumpTextEntryDialog::PrepareContent()
{
    if (m_TextField != nullptr && m_Entry != nullptr)
    {
        u16 newGraphic = 0x0475; //Text field

        if (g_EntryPointer == &m_Entry->m_Entry)
        {
            newGraphic = 0x0477; //Text field (active)
        }
        else if (g_SelectedObject.Object == m_Entry)
        {
            newGraphic = 0x0476; //Text field (lighted)
        }

        if (m_TextField->Graphic != newGraphic)
        {
            m_TextField->Graphic = newGraphic;
            WantRedraw = true;
        }
    }
}

void CGumpTextEntryDialog::UpdateContent()
{
    Clear();

    Add(new CGUIGumppic(0x0474, 0, 0));

    CGUIText *text = (CGUIText *)Add(new CGUIText(0x0386, 60, 50));
    text->CreateTextureA(2, Text);

    text = (CGUIText *)Add(new CGUIText(0x0386, 60, 108));
    text->CreateTextureA(2, m_Description);

    CGump *gumpEntry = g_GumpManager.GetTextEntryOwner();

    if (gumpEntry != nullptr)
    {
        gumpEntry->WantRedraw = true;
    }

    m_TextField = (CGUIGumppic *)Add(new CGUIGumppic(0x0477, 60, 130));
    m_TextField->Serial = ID_GTED_TEXT_FIELD;

    m_Entry = (CGUITextEntry *)Add(
        new CGUITextEntry(ID_GTED_TEXT_FIELD, 0x0386, 0x0386, 0x0386, 71, 137, 0, false, 1));
    m_Entry->m_Entry.MaxLength = m_MaxLength;
    m_Entry->m_Entry.NumberOnly = (Variant == 2);
    m_Entry->CheckOnSerial = true;
    g_EntryPointer = &m_Entry->m_Entry;

    Add(new CGUIButton(ID_GTED_BUTTON_OKAY, 0x047B, 0x047D, 0x047C, 117, 190));

    if (!NoClose)
    {
        Add(new CGUIButton(ID_GTED_BUTTON_CANCEL, 0x0478, 0x047A, 0x0479, 204, 190));
    }
}

void CGumpTextEntryDialog::GUMP_BUTTON_EVENT_C
{
    if (serial == ID_GTED_BUTTON_OKAY)
    { //Button okay
        SendTextEntryDialogResponse(true);
    }
    else if (serial == ID_GTED_BUTTON_CANCEL)
    { //Button cancel
        SendTextEntryDialogResponse(false);
    }
}

void CGumpTextEntryDialog::OnTextInput(const Core::TextEvent &ev)
{
    const auto ch = ev.text[0];
    if (Variant == 2) // Only numbers
    {
        if (ch >= '0' && ch <= '9')
        {
            g_EntryPointer->Insert(ch);
            int val = atoi(g_EntryPointer->c_str());
            if (val > m_MaxLength)
            {
                g_EntryPointer->Remove(true);
            }
            else
            {
                WantRedraw = true;
            }
        }
    }
    else if (Variant == 1) // Anything, but limited in length
    {
        if ((int)g_EntryPointer->Length() < m_MaxLength)
        {
            g_EntryPointer->Insert(ev.wtext[0]);
            WantRedraw = true;
        }
    }
}

void CGumpTextEntryDialog::OnKeyDown(const Core::KeyEvent &ev)
{
    switch (ev.key)
    {
        case Core::EKey::Key_Return:
        case Core::EKey::Key_Return2:
        case Core::EKey::Key_Escape:
        {
            SendTextEntryDialogResponse(ev.key == Core::EKey::Key_Return);
            break;
        }
        case Core::EKey::Key_Home:
        case Core::EKey::Key_End:
        case Core::EKey::Key_Left:
        case Core::EKey::Key_Right:
        case Core::EKey::Key_Backspace:
        case Core::EKey::Key_Delete:
        {
            g_EntryPointer->OnKey(this, ev.key);
            WantRedraw = true;
            break;
        }
        default:
            break;
    }
}

void CGumpTextEntryDialog::SendTextEntryDialogResponse(bool mode)
{
    if (!RemoveMark &&
        m_Entry != nullptr) //Непредвиденная ошибка при отсутствии поля ввода текста в гампе
    {
        //Отправляем ответ на ентри диалог
        CPacketTextEntryDialogResponse(this, &m_Entry->m_Entry, mode).Send();
    }

    //Удаляем использованный гамп
    RemoveMark = true;
}
