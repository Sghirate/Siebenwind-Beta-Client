// MIT License
// Copyright (C) August 2016 Hotride

#pragma once

class CToolTip
{
public:
    u32 Timer = 0;
    std::wstring Data = {};
    u32 ClilocID = 0;
    int MaxWidth = 0;
    Core::Vec2<i32> Position = Core::Vec2<i32>();
    bool Use = false;
    CGLTextTexture Texture;
    class CRenderObject *m_Object{ nullptr };

    CToolTip();
    ~CToolTip();

    void Reset();
    void CreateTextTexture(CGLTextTexture &texture, const std::wstring &str, int &width, int minWidth);
    void Set(const std::wstring &str, int maxWidth = 0);
    void Set(int clilocID, const std::string &str, int maxWidth = 0, bool toCamelCase = false);
    void Draw(int cursorWidth = 0, int cursorHeight = 0);
};

extern CToolTip g_ToolTip;
