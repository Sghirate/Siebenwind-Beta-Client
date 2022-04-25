#pragma once

#include "RenderTextObject.h"

class CRenderWorldObject;

class CTextData : public CRenderTextObject
{
public:
    bool Unicode = false;
    TEXT_TYPE Type = TT_CLIENT;
    u8 Font = 0;
    u32 Timer = 0;
    u32 MoveTimer = 0;
    string Text = "";
    std::wstring UnicodeText = {};
    u8 Alpha = 0xFF;
    CRenderWorldObject *Owner = nullptr;
    CGLTextTexture m_Texture;

    CTextData();
    CTextData(CTextData *obj);
    virtual ~CTextData();

    virtual bool IsText() { return true; }
    bool CanBeDrawedInJournalGump();
    void GenerateTexture(
        int maxWidth,
        u16 flags = 0,
        TEXT_ALIGN_TYPE align = TS_LEFT,
        u8 cell = 30,
        int font = -1);
};
