#pragma once

#include "Gump.h"
#include "Core/Minimal.h"
#include <string>

struct SPELL_REQURIES
{
    u8 ManaCost;
    u8 MinSkill;
};

struct PALADIN_SPELL_REQURIES
{
    u8 ManaCost;
    u8 MinSkill;
    u8 TithingPoints;
};

class CGumpSpellbook : public CGump
{
    SPELLBOOK_TYPE BookType = ST_MAGE;

public:
    static const int MAX_SPELLS_COUNT = 64;
    static const int SPELLBOOK_1_SPELLS_COUNT = 64;
    static const int SPELLBOOK_2_SPELLS_COUNT = 17;
    static const int SPELLBOOK_3_SPELLS_COUNT = 10;
    static const int SPELLBOOK_4_SPELLS_COUNT = 6;
    static const int SPELLBOOK_5_SPELLS_COUNT = 8;
    static const int SPELLBOOK_6_SPELLS_COUNT = 16;
    static const int SPELLBOOK_7_SPELLS_COUNT = 30;

    static std::string m_SpellName1[SPELLBOOK_1_SPELLS_COUNT][2];
    static const std::string m_SpellName2[SPELLBOOK_2_SPELLS_COUNT][2];
    static const std::string m_SpellName3[SPELLBOOK_3_SPELLS_COUNT][2];
    static const std::string m_SpellName4[SPELLBOOK_4_SPELLS_COUNT];
    static const std::string m_SpellName5[SPELLBOOK_5_SPELLS_COUNT];
    static const std::string m_SpellName6[SPELLBOOK_6_SPELLS_COUNT][2];
    static const std::string m_SpellName7[SPELLBOOK_7_SPELLS_COUNT][2];

    static std::string m_SpellReagents1[SPELLBOOK_1_SPELLS_COUNT];

private:
    static const std::string m_SpellReagents2[SPELLBOOK_2_SPELLS_COUNT];
    static const SPELL_REQURIES m_SpellRequries2[SPELLBOOK_2_SPELLS_COUNT];
    static const PALADIN_SPELL_REQURIES m_SpellRequries3[SPELLBOOK_3_SPELLS_COUNT];
    static const SPELL_REQURIES m_SpellRequries4[SPELLBOOK_4_SPELLS_COUNT];
    static const SPELL_REQURIES m_SpellRequries5[SPELLBOOK_5_SPELLS_COUNT];
    static const SPELL_REQURIES m_SpellRequries6[SPELLBOOK_6_SPELLS_COUNT];
    //static const SPELL_REQURIES m_SpellRequries7[SPELLBOOK_7_SPELLS_COUNT];

    int m_SpellCount{ 0 };
    u8 m_Spells[MAX_SPELLS_COUNT];
    int PageCount{ 8 };

    CGUIGumppic *m_Body{ nullptr };
    CGUIText *m_TithingPointsText{ nullptr };

    CGUIButton *m_PrevPage{ nullptr };
    CGUIButton *m_NextPage{ nullptr };

    CGUIGumppic *m_LastSpellPointer{ nullptr };
    CGUIGumppic *m_LastSpellBookmark{ nullptr };

    void GetTooltipBookInfo(int &dictionaryPagesCount, int &tooltipOffset);
    void GetSummaryBookInfo(
        int &maxSpellsCount,
        int &dictionaryPagesCount,
        int &spellsOnPage,
        int &spellIndexOffset,
        u16 &graphic,
        u16 &minimizedGraphic,
        u16 &iconStartGraphic);

    std::string GetSpellName(int offset,std::string&abbreviature,std::string&reagents);
    std::string GetSpellRequries(int offset, int &y);

public:
    CGumpSpellbook(u32 serial, int x, int y);
    virtual ~CGumpSpellbook();

    static void InitStaticData();
    void UpdateGraphic(u16 parentGraphic);
    void ChangePage(int newPage);

     void InitToolTip()  override;
     void PrepareContent()  override;
     void UpdateContent()  override;
     void DelayedClick(CRenderObject *obj)  override;

    GUMP_BUTTON_EVENT_H;
    GUMP_TEXT_ENTRY_EVENT_H;

     bool OnLeftMouseButtonDoubleClick()  override;
};
