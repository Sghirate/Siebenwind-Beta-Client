#pragma once

#include "Core/Minimal.h"
#include "Gump.h"
#include "Platform.h"
#include <string>

class CGumpTextEntryDialog : public CGump
{
public:
    u8 Variant  = 0;
    u8 ParentID = 0;
    u8 ButtonID = 0;

private:
    static constexpr int ID_GTED_BUTTON_OKAY   = 1;
    static constexpr int ID_GTED_BUTTON_CANCEL = 2;
    static constexpr int ID_GTED_TEXT_FIELD    = 3;

    std::string Text          = std::string("");
    std::string m_Description = std::string("");
    int m_MaxLength{ 0 };

    CGUIGumppic* m_TextField{ nullptr };
    CGUITextEntry* m_Entry{ nullptr };

public:
    CGumpTextEntryDialog(
        u32 serial,
        short x,
        short y,
        u8 variant,
        int maxLength,
        std::string text,
        std::string description);
    virtual ~CGumpTextEntryDialog();

    void SendTextEntryDialogResponse(bool mode);

    virtual void PrepareContent();
    virtual void UpdateContent();

    GUMP_BUTTON_EVENT_H;

     void OnTextInput(const Core::TextEvent& ev)  override;
     void OnKeyDown(const Core::KeyEvent& ev)  override;
};
