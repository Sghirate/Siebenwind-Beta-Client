#pragma once
/*
graphic = 0x2B02;
minimizedGraphic = 0x2B05;
iconStartGraphic = 0x5200;
*/
#include "Core/Minimal.h"
#include "Gump.h"
#include <string>

class CGumpRacialAbilitiesBook : public CGump
{
    int DictionaryPagesCount = 2;
    int AbilityCount         = 4;
    int PagesCount           = 6;
    int TooltipOffset        = 1112198;

private:
    const int ID_GRAB_BUTTON_PREV     = 1;
    const int ID_GRAB_BUTTON_NEXT     = 2;
    const int ID_GRAB_BUTTON_MINIMIZE = 3;
    const int ID_GRAB_LOCK_MOVING     = 4;
    const int ID_GRAB_DICTIONARY_ICON = 100;
    const int ID_GRAB_ICON            = 1000;

    CGUIButton* m_PrevPage{ nullptr };
    CGUIButton* m_NextPage{ nullptr };

    void GetSummaryBookInfo(int& abilityOnPage, u16& iconStartGraphic);
    std::string GetAbilityName(int offset, bool& passive);

public:
    CGumpRacialAbilitiesBook(int x, int y);
    virtual ~CGumpRacialAbilitiesBook();

    virtual void DelayedClick(CRenderObject* obj);
    virtual void PrepareContent();
    void ChangePage(int newPage);
    virtual void InitToolTip();
    virtual void UpdateContent();
    GUMP_BUTTON_EVENT_H;
    virtual bool OnLeftMouseButtonDoubleClick();
};
