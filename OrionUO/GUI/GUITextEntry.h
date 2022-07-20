#pragma once

#include "BaseGUI.h"
#include "TextEngine/EntryText.h"

class CGUITextEntry : public CBaseGUI
{
public:
    u16 ColorSelected = 0;
    u16 ColorFocused = 0;
    bool Unicode = false;
    u8 Font = 0;
    TEXT_ALIGN_TYPE Align = TS_LEFT;
    u16 TextFlags = 0;
    bool CheckOnSerial = false;
    bool ReadOnly = false;
    bool Focused = false;
    bool UseGlobalColor = false;
    u8 GlobalColorR = 0;
    u8 GlobalColorG = 0;
    u8 GlobalColorB = 0;
    u8 GlobalColorA = 0;
    u8 GlobalColorSelectedR = 0;
    u8 GlobalColorSelectedG = 0;
    u8 GlobalColorSelectedB = 0;
    u8 GlobalColorSelectedA = 0;
    u8 GlobalColorFocusedR = 0;
    u8 GlobalColorFocusedG = 0;
    u8 GlobalColorFocusedB = 0;
    u8 GlobalColorFocusedA = 0;
    char FocusedOffsetY = 0;

    CGUITextEntry(
        int serial,
        u16 color,
        u16 colorSelected,
        u16 colorFocused,
        int x,
        int y,
        int maxWidth = 0,
        bool unicode = true,
        u8 font = 0,
        TEXT_ALIGN_TYPE align = TS_LEFT,
        u16 textFlags = 0,
        int maxLength = 0);
    virtual ~CGUITextEntry();

    class CEntryText m_Entry;
    virtual void PrepareTextures();
    virtual bool EntryPointerHere();
    virtual Core::Vec2<i32> GetSize();
    void SetGlobalColor(bool use, int color, int selected, int focused);
    void OnClick(CGump *gump, int x, int y);

    virtual void OnMouseEnter();
    virtual void OnMouseExit();

    virtual void Draw(bool checktrans = false);
    virtual bool Select();
};
