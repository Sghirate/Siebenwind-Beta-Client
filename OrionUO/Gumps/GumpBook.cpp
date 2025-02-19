#include "GumpBook.h"
#include "Core/Enums.h"
#include "OrionUO.h"
#include "PressedObject.h"
#include "ClickObject.h"
#include "TextEngine/GameConsole.h"
#include "Managers/FontsManager.h"
#include "Managers/ConfigManager.h"
#include "Managers/MouseManager.h"
#include "Network/Packets.h"

enum
{
    ID_GB_NONE,

    ID_GB_TEXT_AREA_AUTHOR,
    ID_GB_TEXT_AREA_TITLE,
    ID_GB_TEXT_AREA_PAGE_LEFT,
    ID_GB_TEXT_AREA_PAGE_RIGHT,
    ID_GB_BUTTON_PREV,
    ID_GB_BUTTON_NEXT,

    ID_GB_COUNT,
};

CGumpBook::CGumpBook(u32 serial, i16 x, i16 y, u16 pageCount, bool writable, bool unicode)
    : CGump(GT_BOOK, serial, x, y)
    , PageCount(pageCount)
    , Writable(writable)
    , Unicode(true) // The Siebenwind Server will send and receive all book data in Unicode.
{
    m_ChangedPage      = new bool[pageCount + 1];
    m_PageDataReceived = new bool[pageCount + 1];
    Page               = 0;
    Draw2Page          = 1;

    Add(new CGUIPage(-1));
    Add(new CGUIGumppic(0x01FE, 0, 0)); //Body
    m_PrevPage = (CGUIButton*)Add(new CGUIButton(ID_GB_BUTTON_PREV, 0x01FF, 0x01FF, 0x01FF, 0, 0));
    m_PrevPage->Visible = (Page != 0);
    m_NextPage =
        (CGUIButton*)Add(new CGUIButton(ID_GB_BUTTON_NEXT, 0x0200, 0x0200, 0x0200, 356, 0));
    m_NextPage->Visible = (Page + 2 <= PageCount);

    Add(new CGUIPage(0));
    CGUIText* text = (CGUIText*)Add(new CGUIText(0x0386, 78, 32));

    u8 entryFont = 1;

    Add(new CGUIHitBox(ID_GB_TEXT_AREA_TITLE, 41, 65, 150, 22));

    m_EntryTitle = (CGUITextEntry*)Add(
        new CGUITextEntry(ID_GB_TEXT_AREA_TITLE, 0, 0, 0, 41, 65, 140, Unicode, entryFont));
    m_EntryTitle->ReadOnly      = !Writable;
    m_EntryTitle->CheckOnSerial = true;

    text                          = (CGUIText*)Add(new CGUIText(0x0386, 41, 134));
    g_FontManager.UnusePartialHue = true;
    text->CreateTextureA(9, "von");
    g_FontManager.UnusePartialHue = false;

    Add(new CGUIHitBox(ID_GB_TEXT_AREA_AUTHOR, 41, 160, 150, 22));

    m_EntryAuthor = (CGUITextEntry*)Add(
        new CGUITextEntry(ID_GB_TEXT_AREA_AUTHOR, 0, 0, 0, 41, 160, 140, Unicode, entryFont));
    m_EntryAuthor->ReadOnly      = !Writable;
    m_EntryAuthor->CheckOnSerial = true;

    u16 textColor = 0x2681;

    for (int i = 0; i <= PageCount; i++)
    {
        m_ChangedPage[i]      = false;
        m_PageDataReceived[i] = false;

        if (i != 0)
        {
            Add(new CGUIPage(i));
            CGUIHitBox* box =
                (CGUIHitBox*)Add(new CGUIHitBox(ID_GB_TEXT_AREA_PAGE_LEFT, 38, 42, 160, 166));
            box->MoveOnDrag = true;

            CGUITextEntry* entry = (CGUITextEntry*)Add(new CGUITextEntry(
                ID_GB_TEXT_AREA_PAGE_LEFT,
                textColor,
                textColor,
                textColor,
                38,
                42,
                0,
                Unicode,
                entryFont));
            entry->m_Entry.Width = 155;
            entry->ReadOnly      = !Writable;
            entry->CheckOnSerial = true;
            entry->MoveOnDrag    = true;

            text = (CGUIText*)Add(new CGUIText(0x0386, 112, 190));
            text->CreateTextureA(9, std::to_string(i));
        }

        i++;

        if (i <= PageCount)
        {
            m_ChangedPage[i]      = false;
            m_PageDataReceived[i] = false;

            Add(new CGUIPage(i));
            CGUIHitBox* box =
                (CGUIHitBox*)Add(new CGUIHitBox(ID_GB_TEXT_AREA_PAGE_RIGHT, 224, 42, 160, 166));
            box->MoveOnDrag = true;

            CGUITextEntry* entry = (CGUITextEntry*)Add(new CGUITextEntry(
                ID_GB_TEXT_AREA_PAGE_RIGHT,
                textColor,
                textColor,
                textColor,
                224,
                42,
                0,
                Unicode,
                entryFont));
            entry->m_Entry.Width = 155;
            entry->ReadOnly      = !Writable;
            entry->CheckOnSerial = true;
            entry->MoveOnDrag    = true;

            text = (CGUIText*)Add(new CGUIText(0x0386, 299, 190));
            text->CreateTextureA(9, std::to_string(i));
        }
    }
}

CGumpBook::~CGumpBook()
{
    if (m_ChangedPage)
    {
        delete m_ChangedPage;
        m_ChangedPage = nullptr;
    }
    if (m_PageDataReceived)
    {
        delete m_PageDataReceived;
        m_PageDataReceived = nullptr;
    }
}

void CGumpBook::PrepareContent()
{
    /*if (!m_PageDataReceived[Page])
	{
		CPacketBookPageDataRequest(Serial, Page).Send();
		m_PageDataReceived[Page] = true;
	}

	if (Page + 1 <= PageCount && !m_PageDataReceived[Page + 1])
	{
		CPacketBookPageDataRequest(Serial, Page + 1).Send();
		m_PageDataReceived[Page + 1] = true;
	}*/
}

CGUITextEntry* CGumpBook::GetEntry(int page)
{
    int currentPage = -1;

    QFOR(item, m_Items, CBaseGUI*)
    {
        if (item->Type == GOT_PAGE)
        {
            currentPage = ((CGUIPage*)item)->Index;
        }
        else if (item->Type == GOT_TEXTENTRY && currentPage == page)
        {
            return (CGUITextEntry*)item;
        }
    }

    return nullptr;
}

void CGumpBook::SetPageData(int page, const std::wstring& data)
{
    CGUITextEntry* entry     = GetEntry(page);
    m_PageDataReceived[page] = true;

    if (entry != nullptr)
    {
        entry->m_Entry.SetTextW(data);
    }
}

void CGumpBook::ChangePage(int newPage, bool playSound)
{
    for (int i = 0; i < 2; i++)
    {
        if (Page + i > PageCount)
        {
            break;
        }

        if (m_ChangedPage[Page + i])
        {
            m_ChangedPage[Page + i] = false;
            if ((Page + i) < 1)
            {
                CPacketBookHeaderChange(this).Send();
            }
            else
            {
                CPacketBookPageData(this, Page + (int)i).Send();
            }
        }
    }

    if (playSound && Page != newPage)
    {
        g_Orion.PlaySoundEffect(0x0055);
    }

    Page = newPage;

    m_PrevPage->Visible = (Page != 0);
    m_NextPage->Visible = (Page + 2 <= PageCount);

    if (EntryPointerHere())
    {
        if (g_ConfigManager.GetConsoleNeedEnter())
        {
            g_EntryPointer = nullptr;
        }
        else
        {
            g_EntryPointer = &g_GameConsole;
        }
    }
}

void CGumpBook::DelayedClick(CRenderObject* obj)
{
    if (obj != nullptr)
    {
        ChangePage(g_ClickObject.Page);
        WantRedraw = true;
    }
}

void CGumpBook::GUMP_BUTTON_EVENT_C
{
    if (!g_ClickObject.Enabled)
    {
        int newPage = -1;

        if (serial == ID_GB_BUTTON_PREV) //Prev
        {
            if (Page > 0)
            {
                newPage = Page - 2;

                if (newPage < 0)
                {
                    newPage = 0;
                }
            }
        }
        else if (serial == ID_GB_BUTTON_NEXT) //Next
        {
            if (Page < PageCount)
            {
                newPage = Page + 2;

                if (newPage > PageCount)
                {
                    newPage = PageCount - 1;
                }
            }
        }

        if (newPage > -1)
        {
            g_ClickObject.Init(g_PressedObject.LeftObject, this);
            g_ClickObject.Timer = g_Ticks + g_MouseManager.DoubleClickDelay;
            g_ClickObject.Page  = newPage;
        }
    }
}

bool CGumpBook::OnLeftMouseButtonDoubleClick()
{
    if (g_PressedObject.LeftSerial == ID_GB_BUTTON_PREV) //Prev
    {
        ChangePage(0);

        WantRedraw = true;

        return true;
    }
    if (g_PressedObject.LeftSerial == ID_GB_BUTTON_NEXT) //Next
    {
        int page = PageCount;

        if ((PageCount % 2) != 0)
        {
            page--;
        }

        ChangePage(page);

        WantRedraw = true;

        return true;
    }

    return false;
}

void CGumpBook::InsertInContent(int key, bool isCharPress)
{
    int page = Page;

    if (page >= 0 && page <= PageCount)
    {
        bool isSecondEntry   = false;
        CGUITextEntry* entry = GetEntry(page);

        if (entry == nullptr)
        {
            return;
        }

        if (g_EntryPointer != &entry->m_Entry)
        {
            entry = GetEntry(page + 1);

            if (entry != nullptr && page < PageCount - 1 && g_EntryPointer == &entry->m_Entry)
            {
                isSecondEntry = true;
                page++;
            }
            else
            {
                return;
            }
        }

        if (isCharPress)
        {
            if (!Unicode && (int)key >= 0x0100)
            {
                return;
            }

            if (g_EntryPointer->Insert((wchar_t)key))
            {
                int linesCount    = 0;
                int maxLinesCount = 8;

                if (!Unicode)
                {
                    linesCount = g_EntryPointer->GetLinesCountA(4);
                }
                else
                {
                    linesCount = g_EntryPointer->GetLinesCountW(1);
                }

                if (linesCount > maxLinesCount)
                {
                    int newPage = page + 1;

                    bool pageLimitExceeded = false;
                    if (newPage > PageCount)
                    {
                        pageLimitExceeded = true;
                    }

                    int current = g_EntryPointer->Pos();

                    //if we have to paste last line from text entry on the next page and flip back
                    bool goBack = true;

                    //get info with last line of text on current page
                    PMULTILINES_FONT_INFO info = Unicode ? g_FontManager.GetInfoW(
                                                               1,
                                                               g_EntryPointer->GetTextW().c_str(),
                                                               int(g_EntryPointer->Length()),
                                                               TS_LEFT,
                                                               0,
                                                               155) :
                                                           g_FontManager.GetInfoA(
                                                               4,
                                                               g_EntryPointer->GetTextA().c_str(),
                                                               int(g_EntryPointer->Length()),
                                                               TS_LEFT,
                                                               0,
                                                               155);

                    bool addNewLine = false;
                    while (info != nullptr)
                    {
                        PMULTILINES_FONT_INFO next = info->m_Next;
                        if (next != nullptr)
                        {
                            if (next->m_Next == nullptr && !next->Data.empty() &&
                                info->Data.empty())
                            {
                                addNewLine = true;
                            }
                            info->Data.clear();
                            delete info;
                            info = next;
                        }
                        else
                        {
                            break;
                        }
                    }
                    assert(info != nullptr);

                    m_ChangedPage[page] = true;
                    //determine if we're staying on a new page or going back to the current
                    if (g_EntryPointer->Pos() >= info->CharStart)
                    {
                        goBack = false;
                    }

                    //remove characters which do not fit on current page
                    if (info->CharCount == 0)
                    {
                        g_EntryPointer->RemoveSequence(
                            checked_cast<int>(g_EntryPointer->Length() - 1), 1);
                    }
                    else
                    {
                        int start = info->CharStart;
                        int count = info->CharCount;
                        if (addNewLine)
                        {
                            start -= 1;
                            count += 1;
                        }
                        g_EntryPointer->RemoveSequence(start, count);
                    }

                    if (pageLimitExceeded)
                    {
                        return;
                    }

                    //go to the next page and set position for text entry there
                    if (newPage % 2 == 0)
                    {
                        ChangePage(newPage, !goBack);
                    }
                    SetPagePos(0, newPage);

                    //insert data on the next page
                    if (info->Data.empty() || addNewLine)
                    {
                        InsertInContent('\n');
                    }
                    for (int i = 0; i < (int)info->Data.size(); i++)
                    {
                        InsertInContent(info->Data[i].item);
                    }

                    if (goBack)
                    {
                        //go back to initial position on your current page
                        m_ChangedPage[page + 1] = true;
                        ChangePage(page % 2 == 0 ? page : page - 1, false);
                        SetPagePos(current, page);
                    }
                    else
                    {
                        g_EntryPointer->SetPos(info->CharCount, this);
                    }
                }
                else
                {
                    m_ChangedPage[page] = true;
                }

                WantRedraw = true;
            }
        }
        else
        {
            Core::EKey ekey = (Core::EKey)key;
            if (page > 0)
            {
                if (!WasAtEnd)
                {
                    int pos = g_EntryPointer->Pos();
                    if (pos == g_EntryPointer->Length() &&
                        (ekey == Core::EKey::Key_Right || ekey == Core::EKey::Key_End))
                    {
                        int nextpage = page + 1;
                        if (nextpage <= PageCount)
                        {
                            entry = GetEntry(nextpage);
                            if (entry->m_Entry.Length() > 0)
                            {
                                if ((page + 1) % 2 == 0)
                                {
                                    ChangePage(nextpage);
                                }
                                SetPagePos(0, nextpage);
                            }
                        }
                    }
                    else if (
                        pos == 0 &&
                        (ekey == Core::EKey::Key_Left || ekey == Core::EKey::Key_Backspace ||
                         ekey == Core::EKey::Key_Home))
                    {
                        int previousPage = page - 2;
                        if (previousPage >= 0)
                        {
                            if (page % 2 == 0)
                            {
                                ChangePage(previousPage);
                            }
                            SetPagePos(-1, page - 1);
                        }
                    }
                }
                else
                {
                    WasAtEnd = false;
                }
                if (ekey == Core::EKey::Key_Backspace || ekey == Core::EKey::Key_Delete)
                {
                    m_ChangedPage[page] = true;
                }
                WantRedraw = true;
            }
        }
    }
}

void CGumpBook::OnTextInput(const Core::TextEvent& ev)
{
    if (!Writable)
    {
        return;
    }
    if (g_EntryPointer == &m_EntryTitle->m_Entry || g_EntryPointer == &m_EntryAuthor->m_Entry)
    {
        if (g_EntryPointer->Insert(ev.wtext[0]))
        {
            if (Unicode)
            {
                if (g_EntryPointer->GetLinesCountW(1) > 1)
                {
                    g_EntryPointer->Remove(true);
                }
                else
                {
                    m_ChangedPage[0] = true;
                }
            }
            else
            {
                int count = 1;
                if (g_EntryPointer == &m_EntryTitle->m_Entry)
                {
                    count++;
                }

                if (g_EntryPointer->GetLinesCountA(4) > count)
                {
                    g_EntryPointer->Remove(true);
                }
                else
                {
                    m_ChangedPage[0] = true;
                }
            }

            WantRedraw = true;
        }
    }
    else
    {
        InsertInContent(ev.wtext[0]);
    }
}

void CGumpBook::OnKeyDown(const Core::KeyEvent& ev)
{
    if (!Writable)
    {
        return;
    }

    auto key = ev.key;
    switch (key)
    {
        case Core::EKey::Key_Return:
        case Core::EKey::Key_Return2:
        {
            if (g_EntryPointer != &m_EntryTitle->m_Entry &&
                g_EntryPointer != &m_EntryAuthor->m_Entry)
            {
                InsertInContent('\n');
                WantRedraw = true;
            }

            break;
        }
        case Core::EKey::Key_Home:
        case Core::EKey::Key_End:
        case Core::EKey::Key_Left:
        case Core::EKey::Key_Right:
        case Core::EKey::Key_Backspace:
        case Core::EKey::Key_Delete:
        {
            if (g_EntryPointer->Pos() == 1 || g_EntryPointer->Pos() == g_EntryPointer->Length() - 1)
            {
                WasAtEnd = true;
            }
            g_EntryPointer->OnKey(this, key);
            InsertInContent((int)key, false);
            break;
        }
        default: break;
    }
}

void CGumpBook::SetPagePos(int val, int page)
{
    if (page < 1)
    {
        page = 1;
    }
    if (page > PageCount)
    {
        page = PageCount;
    }

    CGUITextEntry* newEntry = GetEntry(page);
    g_EntryPointer          = &newEntry->m_Entry;

    if (val == -1)
    {
        val = checked_cast<int>(g_EntryPointer->Length());
    }
    g_EntryPointer->SetPos(val, this);
}

void CGumpBook::PasteClipboardData(std::wstring& data)
{
    for (int i = 0; i < (int)data.length(); i++)
    {
        InsertInContent(data[i]);
    }
}
