#include <SDL_clipboard.h>

#include "EntryText.h"
#include "Core/StringUtils.h"
#include "../DefinitionMacro.h"
#include "../TextEngine/GameConsole.h"
#include "../Managers/ConfigManager.h"
#include "../Managers/FontsManager.h"
#include "../Managers/GumpManager.h"
#include "../Gumps/Gump.h"

CEntryText* g_EntryPointer = nullptr;

CEntryText::CEntryText(int maxLength, int width, int maxWidth, bool numberOnly)
    : MaxLength(maxLength)
    , Width(width)
    , MaxWidth(maxWidth)
    , NumberOnly(numberOnly)
{
}

CEntryText::~CEntryText()
{
    if (g_EntryPointer == this)
    {
        if (g_ConfigManager.GetConsoleNeedEnter())
        {
            g_EntryPointer = nullptr;
        }
        else
        {
            if (g_GameState >= GS_GAME)
            {
                g_EntryPointer = &g_GameConsole;
            }
            else
            {
                g_EntryPointer = nullptr;
            }
        }
    }

    Clear();
}

const char* CEntryText::c_str()
{
    m_CText = Core::ToString(Text);
    return m_CText.c_str();
}

void CEntryText::OnClick(
    CGump* gump, u8 font, bool unicode, int x, int y, TEXT_ALIGN_TYPE align, u16 flags)
{
    if (g_EntryPointer != this)
    {
        CGump* gumpEntry = g_GumpManager.GetTextEntryOwner();
        if (gumpEntry != nullptr)
        {
            gumpEntry->FrameCreated = false;
        }

        g_EntryPointer = this;
        Changed        = true;
    }
    if (g_EntryPointer == this)
    {
        int oldPos = m_Position;
        if (unicode)
        {
            m_Position = g_FontManager.CalculateCaretPosW(font, Text, x, y, Width, align, flags);
        }
        else
        {
            m_Position = g_FontManager.CalculateCaretPosA(font, c_str(), x, y, Width, align, flags);
        }

        if (oldPos != m_Position)
        {
            Changed = true;
        }
    }

    if (gump != nullptr)
    {
        gump->FrameCreated = false;
    }
}

void CEntryText::OnKey(CGump* gump, Keycode key)
{
    switch (key)
    {
        case KEY_HOME:
        {
            SetPos(0, gump);
            break;
        }
        case KEY_END:
        {
            SetPos((int)Length(), gump);
            break;
        }
        case KEY_LEFT:
        {
            AddPos(-1, gump);
            break;
        }
        case KEY_RIGHT:
        {
            AddPos(1, gump);
            break;
        }
        case KEY_BACK:
        {
            Remove(true, gump);
            break;
        }
        case KEY_DELETE:
        {
            Remove(false, gump);
            break;
        }
        default: break;
    }
}

int CEntryText::GetLinesCountA(u8 font, TEXT_ALIGN_TYPE align, u16 flags, int width)
{
    if (width == 0)
    {
        width = Width;
    }

    MULTILINES_FONT_INFO* info =
        g_FontManager.GetInfoA(font, c_str(), (int)Length(), align, flags, width);

    int count = 0;
    while (info != nullptr)
    {
        MULTILINES_FONT_INFO* next = info->m_Next;
        delete info;
        info = next;
        count++;
    }

    return count;
}

int CEntryText::GetLinesCountW(u8 font, TEXT_ALIGN_TYPE align, u16 flags, int width)
{
    if (width == 0)
    {
        width = Width;
    }

    MULTILINES_FONT_INFO* info =
        g_FontManager.GetInfoW(font, Data(), (int)Length(), align, flags, width);

    int count = 0;
    while (info != nullptr)
    {
        MULTILINES_FONT_INFO* next = info->m_Next;
        delete info;
        info = next;
        count++;
    }

    return count;
}

bool CEntryText::Insert(wchar_t ch, CGump* gump)
{
    if (m_Position < 0)
    {
        m_Position = 0;
    }

    if (m_Position > (int)Text.length())
    {
        m_Position = (int)Text.length();
    }

    if (MaxLength > 0)
    {
        if (NumberOnly)
        {
            std::wstring wstr = Text;
            wstr.insert(wstr.begin() + m_Position, ch);
            if (std::stoi(wstr) >= MaxLength)
            {
                return false;
            }
        }
        else if ((int)Text.length() >= MaxLength)
        {
            return false;
        }
    }

    Text.insert(Text.begin() + m_Position, ch);
    m_Position++;
    Changed = true;
    if (gump != nullptr)
    {
        gump->FrameCreated = false;
    }

    return true;
}

void CEntryText::Remove(bool left, CGump* gump)
{
    if (left)
    {
        if (m_Position < 1)
        {
            return;
        }
        m_Position--;
    }
    else
    {
        if (m_Position >= (int)Text.length())
        {
            return;
        }
    }

    if (m_Position < (int)Text.length())
    {
        Text.erase(Text.begin() + m_Position);
    }
    else
    {
        Text.erase(Text.length() - 1);
    }

    Changed = true;
    if (gump != nullptr)
    {
        gump->FrameCreated = false;
    }
}

void CEntryText::Clear()
{
    Text       = {};
    m_CText    = "";
    m_Position = 0;
    Changed    = true;
    DrawOffset = 0;
    m_Texture.Clear();
    CaretPos.set(0, 0);
}

void CEntryText::Paste()
{
    auto chBuffer = SDL_GetClipboardText();
    if (chBuffer != nullptr && (strlen(chBuffer) != 0u))
    {
        std::wstring str = g_EntryPointer->Data() + Core::ToWString(chBuffer);
        g_EntryPointer->SetTextW(str);
    }
}

void CEntryText::AddPos(int val, CGump* gump)
{
    m_Position += val;
    if (m_Position < 0)
    {
        m_Position = 0;
    }

    if (m_Position > (int)Text.length())
    {
        m_Position = (int)Text.length();
    }

    Changed = true;
    if (gump != nullptr)
    {
        gump->FrameCreated = false;
    }
}

void CEntryText::SetPos(int val, CGump* gump)
{
    m_Position = val;
    if (m_Position < 0)
    {
        m_Position = 0;
    }

    if (m_Position > (int)Text.length())
    {
        m_Position = (int)Text.length();
    }

    Changed = true;
    if (gump != nullptr)
    {
        gump->FrameCreated = false;
    }
}

void CEntryText::SetTextA(const std::string& text)
{
    std::wstring wtext = Core::ToWString(text);
    SetTextW(wtext);
    m_CText = text;
}

void CEntryText::SetTextW(const std::wstring& text)
{
    Clear();

    Text       = text;
    m_Position = (int)Text.length();
    if (m_Position < 0)
    {
        m_Position = 0;
    }

    if (MaxLength > 0)
    {
        if (NumberOnly)
        {
            std::string str = Core::ToString(Text);
            while (true)
            {
                size_t len = str.length();

                if (std::atoi(str.c_str()) >= MaxLength && len > 0)
                {
                    str.resize(len - 1);
                }
                else
                {
                    break;
                }
            }
        }
        else if ((int)Text.length() >= MaxLength)
        {
            Text.resize(MaxLength);
        }
    }

    CGump* gump = g_GumpManager.GetTextEntryOwner();
    if (gump != nullptr)
    {
        gump->FrameCreated = false;
    }
}

std::string CEntryText::CheckMaxWidthA(u8 font, std::string str)
{
    if (MaxWidth > 0)
    {
        int width  = g_FontManager.GetWidthA(font, str);
        size_t len = str.length();
        while (MaxWidth < width && len > 0)
        {
            str.erase(str.begin() + len);
            len--;
            width = g_FontManager.GetWidthA(font, str);
        }
    }

    return str;
}

std::wstring CEntryText::CheckMaxWidthW(u8 font, std::wstring str)
{
    if (MaxWidth > 0)
    {
        int width  = g_FontManager.GetWidthW(font, str);
        size_t len = str.length();
        while (MaxWidth < width && len > 0)
        {
            str.erase(str.begin() + len);
            len--;
            width = g_FontManager.GetWidthW(font, str);
        }
    }

    return str;
}

void CEntryText::FixMaxWidthA(u8 font)
{
    c_str();
    if (MaxWidth <= 0)
    {
        return;
    }

    int width  = g_FontManager.GetWidthA(font, m_CText);
    size_t len = m_CText.length();
    while (MaxWidth < width && len > 0)
    {
        Remove((m_Position > 0), nullptr);
        len--;
        width = g_FontManager.GetWidthA(font, c_str());
    }
}

void CEntryText::FixMaxWidthW(u8 font)
{
    if (MaxWidth <= 0)
    {
        return;
    }

    int width  = g_FontManager.GetWidthW(font, Text);
    size_t len = Text.length();
    while (MaxWidth < width && len > 0)
    {
        Remove((m_Position > 0), nullptr);
        len--;
        width = g_FontManager.GetWidthW(font, Text);
    }
}

void CEntryText::CreateTextureA(
    u8 font, const std::string& str, u16 color, int width, TEXT_ALIGN_TYPE align, u16 flags)
{
    if (str.length() == 0u)
    {
        Clear();
        return;
    }
    m_Texture.Clear();
    if (m_Position != 0)
    {
        CaretPos = g_FontManager.GetCaretPosA(font, str, m_Position, width, align, flags);

        if ((flags & UOFONT_FIXED) != 0)
        {
            if (DrawOffset != 0)
            {
                if (CaretPos.x + DrawOffset < 0)
                {
                    DrawOffset = -CaretPos.x;
                }
                else if (Width + -DrawOffset < CaretPos.x)
                {
                    DrawOffset = Width - CaretPos.x;
                }
            }
            else if (Width + DrawOffset < CaretPos.x)
            {
                DrawOffset = Width - CaretPos.x;
            }
            else
            {
                DrawOffset = 0;
            }

            /*if (Width + DrawOffset < CaretPos.x)
                DrawOffset = Width - CaretPos.x;
            else
                DrawOffset = 0;*/
        }
    }
    else
    {
        CaretPos.set(0, 0);
        DrawOffset = 0;
    }
    g_FontManager.GenerateA(font, m_Texture, str, color, Width + abs(DrawOffset), align, flags);
}

void CEntryText::CreateTextureW(
    u8 font, const std::wstring& str, u16 color, int width, TEXT_ALIGN_TYPE align, u16 flags)
{
    if (str.length() == 0u)
    {
        Clear();
        return;
    }

    m_Texture.Clear();
    if (m_Position != 0)
    {
        CaretPos = g_FontManager.GetCaretPosW(font, str, m_Position, width, align, flags);
        if ((flags & UOFONT_FIXED) != 0)
        {
            if (DrawOffset != 0)
            {
                if (CaretPos.x + DrawOffset < 0)
                {
                    DrawOffset = -CaretPos.x;
                }
                else if (Width + -DrawOffset < CaretPos.x)
                {
                    DrawOffset = Width - CaretPos.x;
                }
            }
            else if (Width + DrawOffset < CaretPos.x)
            {
                DrawOffset = Width - CaretPos.x;
            }
            else
            {
                DrawOffset = 0;
            }
        }
    }
    else
    {
        CaretPos.set(0, 0);
        DrawOffset = 0;
    }
    g_FontManager.GenerateW(font, m_Texture, str, color, 30, Width, align, flags);
}

void CEntryText::PrepareToDrawA(u8 font, u16 color, TEXT_ALIGN_TYPE align, u16 flags)
{
    if (Changed || Color != color)
    {
        FixMaxWidthA(font);
        CreateTextureA(font, m_CText, color, /*MaxWidth*/ Width, align, flags);
        Changed = false;
        Color   = color;
        if (this == g_EntryPointer)
        {
            g_FontManager.GenerateA(font, m_CaretTexture, "_", color);
        }
    }
}

void CEntryText::PrepareToDrawW(u8 font, u16 color, TEXT_ALIGN_TYPE align, u16 flags)
{
    if (Changed || Color != color)
    {
        FixMaxWidthW(font);
        CreateTextureW(font, Text, color, Width, align, flags);
        Changed = false;
        Color   = color;
        if (this == g_EntryPointer)
        {
            g_FontManager.GenerateW(font, m_CaretTexture, L"_", color, 30);
        }
    }
}

void CEntryText::DrawA(u8 font, u16 color, int x, int y, TEXT_ALIGN_TYPE align, u16 flags)
{
    PrepareToDrawA(font, color, align, flags);
    m_Texture.Draw(x + DrawOffset, y);
    if (this == g_EntryPointer)
    {
        const int offsetTable[] = { 1, 2, 1, 1, 1, 2, 1, 1, 2, 2 };
        const int offsY         = offsetTable[font % 10];
        m_CaretTexture.Draw(x + DrawOffset + CaretPos.x, y + offsY + CaretPos.y);
    }
}

void CEntryText::DrawW(u8 font, u16 color, int x, int y, TEXT_ALIGN_TYPE align, u16 flags)
{
    PrepareToDrawW(font, color, align, flags);
    m_Texture.Draw(x + DrawOffset, y);
    if (this == g_EntryPointer)
    {
        m_CaretTexture.Draw(x + DrawOffset + CaretPos.x, y + CaretPos.y);
    }
}

void CEntryText::DrawMaskA(u8 font, u16 color, int x, int y, TEXT_ALIGN_TYPE align, u16 flags)
{
    const auto len = (int)Length();
    std::string str{};
    for (int i = 0; i < len; i++)
    {
        str += "*";
    }

    if (len != 0)
    {
        g_FontManager.DrawA(font, str, color, x + DrawOffset, y);
    }

    if (this == g_EntryPointer)
    {
        const int offsetTable[] = { 1, 2, 1, 1, 1, 2, 1, 1, 2, 2 };
        const int offsY         = offsetTable[font % 10];
        if (m_Position != 0)
        {
            str.resize(m_Position);
            x += g_FontManager.GetWidthA(font, str);
        }
        g_FontManager.DrawA(font, "_", color, x + DrawOffset, y + offsY);
    }
}

void CEntryText::DrawMaskW(u8 font, u16 color, int x, int y, TEXT_ALIGN_TYPE align, u16 flags)
{
    const auto len   = (int)Length();
    std::wstring str = {};
    for (int i = 0; i < len; i++)
    {
        str += L"*";
    }

    if (len != 0)
    {
        g_FontManager.DrawW(font, str, color, x + DrawOffset, y, 30, 0, TS_LEFT, flags);
    }

    if (this == g_EntryPointer)
    {
        if (m_Position != 0)
        {
            str.resize(m_Position);
            x += g_FontManager.GetWidthW(font, str);
        }
        g_FontManager.DrawW(font, L"_", color, x + DrawOffset, y, 30, 0, TS_LEFT, flags);
    }
}

void CEntryText::RemoveSequence(int startPos, int length)
{
    Text.erase(startPos, length);
}

std::string CEntryText::GetTextA() const
{
    return m_CText;
}

std::wstring CEntryText::GetTextW() const
{
    return Text;
}
