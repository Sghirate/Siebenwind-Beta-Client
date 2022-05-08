#pragma once

#include "EntryText.h"
#include "Globals.h"
#include <string>

class CGameConsole : public CEntryText
{
private:
    GAME_CONSOLE_TEXT_TYPE m_Type = GCTT_NORMAL;
    std::wstring m_ConsoleStack[MAX_CONSOLE_STACK_SIZE];
    int m_ConsoleStackCount    = 0;
    int m_ConsoleSelectedIndex = 0;
    bool m_PositionChanged     = false;

public:
    CGameConsole();
    virtual ~CGameConsole();

    static void Send(std::wstring text, u16 defaultColor = 0);
    static std::wstring
    IsSystemCommand(const wchar_t* text, size_t& len, int& member, GAME_CONSOLE_TEXT_TYPE& type);

    void DrawW(u8 font, u16 color, int x, int y, TEXT_ALIGN_TYPE align = TS_LEFT, u16 flags = 0);

    bool InChat() const;
    void Send();
    void SaveConsoleMessage();
    void ChangeConsoleMessage(bool next);
    void ClearStack();
};

static const std::wstring g_ConsolePrefix[] = {
    L"",    //Normal
    L"! ",  //Yell
    L"; ",  //Whisper
    L": ",  //Emote
    L".",   //Command
    L"? ",  //Broadcast
    L"/ ",  //Party
    L"\\ ", //Guild
    L"| "   //Alliance
};

extern CGameConsole g_GameConsole;
