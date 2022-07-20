#pragma once

#include "GumpBaseScroll.h"
#include "Platform.h"

class CGumpProfile : public CGumpBaseScroll
{
private:
    static const int ID_GP_APPLY = 1;
    static const int ID_GP_TEXT_FIELD = 2;

    CGUITextEntry *m_Entry{ nullptr };
    CGUIHitBox *m_HitBox{ nullptr };
    CBaseGUI *m_BottomData[4];

    void RecalculateHeight();

public:
    CGumpProfile(
        u32 serial,
        short x,
        short y,
        const std::wstring &topText,
        const std::wstring &bottomText,
        const std::wstring &dataText);
    virtual ~CGumpProfile();

    GUMP_BUTTON_EVENT_H;

    virtual bool OnLeftMouseButtonDoubleClick();
     void OnTextInput(const Core::TextEvent &ev)  override;
     void OnKeyDown(const Core::KeyEvent &ev)  override;
};
