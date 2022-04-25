#include "GumpSkill.h"
#include "Core/StringUtils.h"
#include "../OrionUO.h"
#include "../SelectedObject.h"
#include "../Managers/MouseManager.h"
#include "../Managers/SkillsManager.h"

CGumpSkill::CGumpSkill(int serial, int x, int y)
    : CGump(GT_SKILL, serial, x, y)
{
    m_Locker.Serial = ID_GS_LOCK_MOVING;

    CGUIResizepic *resizepic = (CGUIResizepic *)Add(new CGUIResizepic(0, 0x24EA, 0, 0, 140, 20));
    resizepic->DrawOnly = true;

    if (Serial < (u32)g_SkillsManager.Count)
    {
        CGUIText *text = (CGUIText *)Add(new CGUIText(0x0481, 10, 5));

        CSkill *skill = g_SkillsManager.Get(Serial);

        if (skill != nullptr)
        {
            text->CreateTextureW(1, Core::ToWString(skill->Name), 30, 120, TS_CENTER);
        }

        resizepic->Height = 20 + text->m_Texture.Height;
    }
}

CGumpSkill::~CGumpSkill()
{
}

void CGumpSkill::GUMP_BUTTON_EVENT_C
{
    if (serial == ID_GS_LOCK_MOVING)
    {
        LockMoving = !LockMoving;
    }
}

void CGumpSkill::OnLeftMouseButtonUp()
{
    CGump::OnLeftMouseButtonUp();

    if (g_SelectedObject.Serial != ID_GS_LOCK_MOVING && Serial < (u32)g_SkillsManager.Count)
    {
        CSkill *skill = g_SkillsManager.Get(Serial);

        if (skill != nullptr && skill->Button)
        {
            Core::Vec2<i32> offset = g_MouseManager.GetLeftDroppedOffset();

            if ((offset.x == 0) && (offset.y == 0))
            {
                g_Orion.UseSkill(Serial);
            }
        }
    }
}
