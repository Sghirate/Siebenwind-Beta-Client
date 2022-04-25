#pragma once

#include "GUIText.h"

class CGUIComboboxText : public CGUIText
{
public:
    CGUIComboboxText(
        u16 color,
        u8 font,
        const std::string &str,
        int width = 0,
        TEXT_ALIGN_TYPE align = TS_LEFT,
        u16 flags = 0);
    CGUIComboboxText(
        u16 color,
        u8 font,
        const std::wstring &str,
        int width = 0,
        TEXT_ALIGN_TYPE align = TS_LEFT,
        u16 flags = 0);
    virtual ~CGUIComboboxText();

    virtual void OnMouseEnter();
    virtual void OnMouseExit();
};
