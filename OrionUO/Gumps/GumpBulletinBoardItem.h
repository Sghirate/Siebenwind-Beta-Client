#pragma once

#include "GumpBaseScroll.h"
#include "Platform.h"
#include <string>

class CGumpBulletinBoardItem : public CGumpBaseScroll
{
protected:
    u8 m_Variant{ 0 };
    CGUIButton* m_ButtonPost{ nullptr };
    CGUIButton* m_ButtonRemove{ nullptr };
    CGUIButton* m_ButtonReply{ nullptr };
    CGUIHitBox* m_HitBox{ nullptr };

    void RecalculateHeight();
    virtual void UpdateHeight();

public:
    CGumpBulletinBoardItem(
        int serial,
        int x,
        int y,
        u8 variant,
        int id,
        const std::wstring& poster,
        const std::wstring& subject,
        const std::wstring& dataTime,
        const std::wstring& data);
    virtual ~CGumpBulletinBoardItem();

    CGUITextEntry* m_EntrySubject{ nullptr };
    CGUITextEntry* m_Entry{ nullptr };

    GUMP_BUTTON_EVENT_H;

     void OnTextInput(const Core::TextEvent& ev)  override;
     void OnKeyDown(const Core::KeyEvent& ev)  override;
};
