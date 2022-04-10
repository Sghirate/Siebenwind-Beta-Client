// MIT License
// Copyright (C) August 2016 Hotride

#pragma once

#include "../plugin/mulstruct.h"

typedef struct MULTILINES_FONT_DATA
{
    wchar_t item;
    u16 flags;
    u8 font;
    u16 linkID;
    u32 color;

    MULTILINES_FONT_DATA *Next;
} * PMULTILINES_FONT_DATA;

typedef struct MULTILINES_FONT_INFO
{
    int Width;
    int IndentionOffset;
    int MaxHeight;
    int CharStart;
    int CharCount;
    TEXT_ALIGN_TYPE Align;
    std::vector<MULTILINES_FONT_DATA> Data;

    MULTILINES_FONT_INFO *m_Next;

    void Reset()
    {
        Width = 0;
        IndentionOffset = 0;
        MaxHeight = 0;
        CharStart = 0;
        CharCount = 0;
        Align = TS_LEFT;
        m_Next = nullptr;
    }
} * PMULTILINES_FONT_INFO;

struct WEB_LINK
{
    bool Visited;
    string WebLink;
};

struct HTML_char
{
    wchar_t Char;
    u8 Font;
    TEXT_ALIGN_TYPE Align;
    u16 Flags;
    u32 Color;
    u16 LinkID;
};

struct HTML_DATA_INFO
{
    HTML_TAG_TYPE Tag;
    TEXT_ALIGN_TYPE Align;
    u16 Flags;
    u8 Font;
    u32 Color;
    u16 Link;
};

typedef std::map<u16, WEB_LINK> WEBLINK_MAP;
typedef std::vector<HTML_char> HTMLCHAR_LIST;
typedef std::vector<HTML_DATA_INFO> HTMLINFO_LIST;

class CFontsManager
{
public:
    bool RecalculateWidthByInfo = false;
    bool UnusePartialHue = false;
    int FontCount = 0;

private:
    FONT_DATA *Font = nullptr;
    WEBLINK_MAP m_WebLink;

    static u8 m_FontIndex[256];

    size_t m_UnicodeFontAddress[20];
    u32 m_UnicodeFontSize[20];

    bool m_UseHTML = false;
    u32 m_HTMLColor = 0xFFFFFFFF;
    bool m_HTMLBackgroundCanBeColored = false;

    u32 m_BackgroundColor = 0;
    u32 m_WebLinkColor = 0;
    u32 m_VisitedWebLinkColor = 0;

    int m_LeftMargin = 0;
    int m_TopMargin = 0;
    int m_RightMargin = 0;
    int m_BottomMargin = 0;

    u16 GetWebLinkID(const std::string &link, u32 &color);
    u16 GetWebLinkID(const std::wstring &link, u32 &color);

    HTMLCHAR_LIST
    GetHTMLData(u8 font, const wchar_t *str, int &len, TEXT_ALIGN_TYPE align, u16 flags);

    HTML_DATA_INFO GetHTMLInfoFromTag(const HTML_TAG_TYPE &tag);
    HTML_DATA_INFO GetCurrentHTMLInfo(const HTMLINFO_LIST &list);

    void GetHTMLInfoFromContent(HTML_DATA_INFO &info, const std::string &content);
    void TrimHTMLString(string &str);
    u32 GetHTMLColorFromText(string &str);

    HTML_TAG_TYPE
    ParseHTMLTag(const wchar_t *str, int len, int &i, bool &endTag, HTML_DATA_INFO &info);

    HTMLCHAR_LIST
    GetHTMLDataOld(
        u8 font, const wchar_t *str, int &len, TEXT_ALIGN_TYPE align, u16 flags);

    PMULTILINES_FONT_INFO GetInfoHTML(
        u8 font,
        const wchar_t *str,
        int len,
        TEXT_ALIGN_TYPE align,
        u16 flags,
        int width);

    bool GenerateABase(
        u8 font,
        CGLTextTexture &th,
        const std::string &str,
        u16 color,
        int width,
        TEXT_ALIGN_TYPE align,
        u16 flags);

    bool GenerateWBase(
        u8 font,
        CGLTextTexture &th,
        const std::wstring &str,
        u16 color,
        u8 cell,
        int width,
        TEXT_ALIGN_TYPE align,
        u16 flags);

public:
    CFontsManager();
    ~CFontsManager();

    void
    SetUseHTML(bool val, int htmlStartColor = 0xFFFFFFFF, const bool backgroundCanBeColored = false)
    {
        m_UseHTML = val;
        m_HTMLColor = htmlStartColor;
        m_HTMLBackgroundCanBeColored = backgroundCanBeColored;
    }
    bool GetUseHTML() const { return m_UseHTML; }

    bool LoadFonts();
    bool UnicodeFontExists(u8 font);
    void GoToWebLink(u16 link);

    inline bool IsPrintASCII(u8 index) { return (m_FontIndex[index] != 0xFF); }
    int GetFontOffsetY(u8 font, u8 index);
    Core::Vec2<i32> GetCaretPosA(
        u8 font, const std::string &str, int pos, int width, TEXT_ALIGN_TYPE align, u16 flags);

    int CalculateCaretPosA(
        u8 font,
        const std::string &str,
        int x,
        int y,
        int width,
        TEXT_ALIGN_TYPE align,
        u16 flags);

    int GetWidthA(u8 font, const std::string &str);

    int GetWidthExA(
        u8 font, const std::string &str, int maxWidth, TEXT_ALIGN_TYPE align, u16 flags);

    int GetHeightA(
        u8 font,
        const std::string &str,
        int width = 0,
        TEXT_ALIGN_TYPE align = TS_LEFT,
        u16 flags = 0);

    int GetHeightA(PMULTILINES_FONT_INFO info);

    string GetTextByWidthA(u8 font, const std::string &str, int width, bool isCropped);

    PMULTILINES_FONT_INFO
    GetInfoA(
        u8 font, const char *str, int len, TEXT_ALIGN_TYPE align, u16 flags, int width);

    std::vector<u32> GeneratePixelsA(
        u8 font,
        CGLTextTexture &th,
        const char *str,
        u16 color,
        int width,
        TEXT_ALIGN_TYPE align,
        u16 flags);

    bool GenerateA(
        u8 font,
        CGLTextTexture &th,
        const std::string &str,
        u16 color = 0,
        int width = 0,
        TEXT_ALIGN_TYPE align = TS_LEFT,
        u16 flags = 0);

    void DrawA(
        u8 font,
        const std::string &str,
        u16 color,
        int x,
        int y,
        int width = 0,
        TEXT_ALIGN_TYPE align = TS_LEFT,
        u16 flags = 0);

    Core::Vec2<i32> GetCaretPosW(
        u8 font,
        const std::wstring &str,
        int pos,
        int width,
        TEXT_ALIGN_TYPE align,
        u16 flags);

    int CalculateCaretPosW(
        u8 font,
        const std::wstring &str,
        int x,
        int y,
        int width,
        TEXT_ALIGN_TYPE align,
        u16 flags);

    int GetWidthW(u8 font, const std::wstring &str);

    int GetWidthExW(
        u8 font, const std::wstring &str, int maxWidth, TEXT_ALIGN_TYPE align, u16 flags);

    int GetHeightW(
        u8 font,
        const std::wstring &str,
        int width = 0,
        TEXT_ALIGN_TYPE align = TS_LEFT,
        u16 flags = 0);

    int GetHeightW(PMULTILINES_FONT_INFO info);

    std::wstring GetTextByWidthW(u8 font, const std::wstring &str, int width, bool isCropped);

    PMULTILINES_FONT_INFO GetInfoW(
        u8 font,
        const wchar_t *str,
        int len,
        TEXT_ALIGN_TYPE align,
        u16 flags,
        int width);

    std::vector<u32> GeneratePixelsW(
        u8 font,
        CGLTextTexture &th,
        const wchar_t *str,
        u16 color,
        u8 cell,
        int width,
        TEXT_ALIGN_TYPE align,
        u16 flags);

    bool GenerateW(
        u8 font,
        CGLTextTexture &th,
        const std::wstring &str,
        u16 color = 0,
        u8 cell = 30,
        int width = 0,
        TEXT_ALIGN_TYPE align = TS_LEFT,
        u16 flags = 0);

    void DrawW(
        u8 font,
        const std::wstring &str,
        u16 color,
        int x,
        int y,
        u8 cell = 30,
        int width = 0,
        TEXT_ALIGN_TYPE align = TS_LEFT,
        u16 flags = 0);
};

extern CFontsManager g_FontManager;
