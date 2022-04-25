#pragma once

#include "Gump.h"
#include "../Platform.h"

class CGumpTextEntryDialog : public CGump
{
public:
    u8 Variant = 0;
    u8 ParentID = 0;
    u8 ButtonID = 0;

private:
    static constexpr int ID_GTED_BUTTON_OKAY = 1;
    static constexpr int ID_GTED_BUTTON_CANCEL = 2;
    static constexpr int ID_GTED_TEXT_FIELD = 3;

    string Text = string("");
    string m_Description = string("");
    int m_MaxLength{ 0 };

    CGUIGumppic *m_TextField{ nullptr };
    CGUITextEntry *m_Entry{ nullptr };

public:
    CGumpTextEntryDialog(
        u32 serial,
        short x,
        short y,
        u8 variant,
        int maxLength,
        string text,
        string description);
    virtual ~CGumpTextEntryDialog();

    void SendTextEntryDialogResponse(bool mode);

    virtual void PrepareContent();
    virtual void UpdateContent();

    GUMP_BUTTON_EVENT_H;

    virtual void OnTextInput(const TextEvent &ev) override;
    virtual void OnKeyDown(const KeyEvent &ev) override;
};
