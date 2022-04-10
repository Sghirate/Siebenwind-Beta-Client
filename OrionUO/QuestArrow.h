#pragma once

class CQuestArrow
{
public:
    u32 Timer = 0;
    u16 X = 0;
    u16 Y = 0;
    bool Enabled = false;

    CQuestArrow();
    ~CQuestArrow();

    static const u16 m_Gump = 0x1194;
    void Draw();
};

extern CQuestArrow g_QuestArrow;
