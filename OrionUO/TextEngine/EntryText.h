#pragma once

#include "Backend.h"
#include "Core/Minimal.h"
#include "Core/Input.h"
#include "plugin/enumlist.h"
#include <string>

class CGump;

class CEntryText
{
public:
    int MaxLength            = 0;
    int Width                = 0;
    int MaxWidth             = 0;
    bool Changed             = true;
    u16 Color                = 0;
    int DrawOffset           = 0;
    Core::Vec2<i32> CaretPos = {};
    bool NumberOnly          = false;

protected:
    int m_Position = 0;
    std::string m_CText;
    std::wstring Text;

public:
    CEntryText(int maxLength = 0, int width = 0, int maxWidth = 0, bool numberOnly = false);
    virtual ~CEntryText();

    CGLTextTexture m_Texture;
    CGLTextTexture m_CaretTexture;

    void Clear();
    void Paste();

    int GetLinesCountA(u8 font, TEXT_ALIGN_TYPE align = TS_LEFT, u16 flags = 0, int width = 0);

    int GetLinesCountW(u8 font, TEXT_ALIGN_TYPE align = TS_LEFT, u16 flags = 0, int width = 0);

    bool Insert(wchar_t ch, CGump* gump = nullptr);
    void Remove(bool left, CGump* gump = nullptr);
    void RemoveSequence(int startPos, int length);
    void AddPos(int val, CGump* gump = nullptr);
    void SetPos(int val, CGump* gump = nullptr);
    const wchar_t* Data() const { return Text.c_str(); }
    const char* c_str();
    void SetTextA(const std::string& text);
    void SetTextW(const std::wstring& text);
    std::string GetTextA() const;
    std::wstring GetTextW() const;

    void OnClick(
        CGump* gump,
        u8 font,
        bool unicode,
        int x,
        int y,
        TEXT_ALIGN_TYPE align = TS_LEFT,
        u16 flags             = 0);

    void OnKey(CGump* gump, Core::EKey key);
    int Pos() const { return m_Position; }
    size_t Length() const { return Text.length(); }
    std::string CheckMaxWidthA(u8 font, std::string str);
    std::wstring CheckMaxWidthW(u8 font, std::wstring str);
    void FixMaxWidthA(u8 font);
    void FixMaxWidthW(u8 font);

    void CreateTextureA(
        u8 font, const std::string& str, u16 color, int width, TEXT_ALIGN_TYPE align, u16 flags);

    void CreateTextureW(
        u8 font, const std::wstring& str, u16 color, int width, TEXT_ALIGN_TYPE align, u16 flags);

    virtual void PrepareToDrawA(u8 font, u16 color, TEXT_ALIGN_TYPE align = TS_LEFT, u16 flags = 0);

    virtual void PrepareToDrawW(u8 font, u16 color, TEXT_ALIGN_TYPE align = TS_LEFT, u16 flags = 0);

    virtual void
    DrawA(u8 font, u16 color, int x, int y, TEXT_ALIGN_TYPE align = TS_LEFT, u16 flags = 0);

    virtual void
    DrawW(u8 font, u16 color, int x, int y, TEXT_ALIGN_TYPE align = TS_LEFT, u16 flags = 0);

    void
    DrawMaskA(u8 font, u16 color, int X, int Y, TEXT_ALIGN_TYPE align = TS_LEFT, u16 flags = 0);

    void
    DrawMaskW(u8 font, u16 color, int X, int Y, TEXT_ALIGN_TYPE align = TS_LEFT, u16 flags = 0);
};

extern CEntryText* g_EntryPointer;
