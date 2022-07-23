#include "GumpSkills.h"
#include "Globals.h"
#include "GumpSkill.h"
#include "Platform.h"
#include "OrionUO.h"
#include "ToolTip.h"
#include "SkillGroup.h"
#include "PressedObject.h"
#include "SelectedObject.h"
#include "TextEngine/GameConsole.h"
#include "Managers/ConfigManager.h"
#include "Managers/FontsManager.h"
#include "Managers/GumpManager.h"
#include "Managers/MouseManager.h"
#include "Managers/SkillsManager.h"
#include "Managers/SkillGroupManager.h"
#include "Network/Packets.h"
#include "GameObjects/GamePlayer.h"

enum
{
    ID_GS_SHOW_REAL = 1,
    ID_GS_SHOW_CAP = 2,
    ID_GS_BUTTON_NEW_GROUP = 3,
    ID_GS_LOCK_MOVING = 4,
    ID_GS_GROUP_MINIMIZE = 10000,
    ID_GS_GROUP = 11000,
    ID_GS_SKILL_BUTTON = 12000,
    ID_GS_SKILL = 13000,
    ID_GS_SKILL_STATE = 14000,
};

CGumpSkills::CGumpSkills(short x, short y, bool minimized, int height)
    : CGumpBaseScroll(GT_SKILLS, 0, 0x1F40, height, x, y, true, 0, true, 15)
{
    m_Locker.Serial = ID_GS_LOCK_MOVING;

    if (minimized)
    {
        Page = 1;
        Minimized = minimized;
        MinimizedX = x;
        MinimizedY = y;
    }
    else
    {
        Page = 2;
    }

    Add(new CGUIPage(1));
    Add(new CGUIGumppic(0x0839, 0, 0));

    Add(new CGUIPage(2));
    Add(new CGUIGumppic(0x0834, 82, 34)); //Skills text gump

    CGUIText *text = (CGUIText *)Add(new CGUIText(0x0386, 180, 33));
    text->CreateTextureA(1, "Show:   Real    Cap");

    m_CheckboxShowReal =
        (CGUICheckbox *)Add(new CGUICheckbox(ID_GS_SHOW_REAL, 0x0938, 0x0939, 0x0938, 226, 34));
    m_CheckboxShowReal->Checked = m_ShowReal;
    m_CheckboxShowCap =
        (CGUICheckbox *)Add(new CGUICheckbox(ID_GS_SHOW_CAP, 0x0938, 0x0939, 0x0938, 280, 34));
    m_CheckboxShowCap->Checked = m_ShowCap;

    Add(new CGUIGumppic(0x082B, 30, 60));                                        //Top line
    m_BottomLine = (CGUIGumppic *)Add(new CGUIGumppic(0x082B, 31, Height - 48)); //Bottom line
    m_Comment = (CGUIGumppic *)Add(new CGUIGumppic(0x0836, 30, Height - 35)); //Skills comment gump
    m_CreateGroup = (CGUIButton *)Add(new CGUIButton(
        ID_GS_BUTTON_NEW_GROUP, 0x083A, 0x083A, 0x083A, 60, Height - 3)); //New Group gump

    m_SkillSum = (CGUIText *)Add(new CGUIText(0x0065, 235, Height - 6));
    UpdateSkillsSum();

    //Если игрок присутствует
    if (g_Player != nullptr)
    {
        int currentIndex = 0;
        int currentY = 0;

        QFOR(group, g_SkillGroupManager.m_Groups, CSkillGroupObject *)
        {
            CGUISkillGroup *skillGroup = (CGUISkillGroup *)m_HTMLGump->Add(new CGUISkillGroup(
                ID_GS_GROUP + currentIndex,
                ID_GS_GROUP_MINIMIZE + currentIndex,
                group,
                0,
                currentY));
            skillGroup->SetMinimized(true);

            int count = group->Count;

            for (int i = 0; i < count; i++)
            {
                u8 index = group->GetItem(i); //Получаем индекс скилла по порядковому номеру

                if (index < g_SkillsManager.Count)
                { //Он валиден
                    skillGroup->Add(new CGUISkillItem(
                        ID_GS_SKILL + index,
                        ID_GS_SKILL_BUTTON + index,
                        ID_GS_SKILL_STATE + index,
                        index,
                        0,
                        (int)i * 17));
                }
            }

            currentY += 19;

            if (!skillGroup->GetMinimized())
            {
                currentY += count * 17;
            }

            currentIndex++;
        }
    }

    m_HTMLGump->CalculateDataSize();
}

CGumpSkills::~CGumpSkills()
{
}

void CGumpSkills::InitToolTip()
{
    u32 id = g_SelectedObject.Serial;

    if (!Minimized)
    {
        switch (id)
        {
            case ID_GS_SHOW_REAL:
            {
                g_ToolTip.Set(L"Show/hide real skill values");
                break;
            }
            case ID_GS_SHOW_CAP:
            {
                g_ToolTip.Set(L"Show/hide cap skill values");
                break;
            }
            case ID_GS_BUTTON_NEW_GROUP:
            {
                g_ToolTip.Set(L"Create the new skills group");
                break;
            }
            case ID_GBS_BUTTON_MINIMIZE:
            {
                g_ToolTip.Set(L"Minimize the skills gump");
                break;
            }
            case ID_GBS_BUTTON_RESIZE:
            {
                g_ToolTip.Set(L"Start resizing for skills gump");
                break;
            }
            case ID_GS_LOCK_MOVING:
            {
                g_ToolTip.Set(L"Lock moving/closing the skills gump");
                break;
            }
            default:
            {
                if (id >= ID_GS_GROUP_MINIMIZE && id < ID_GS_GROUP)
                {
                    g_ToolTip.Set(L"Show/hide skills in this group");
                }
                else if (id >= ID_GS_SKILL_BUTTON && id < ID_GS_SKILL)
                {
                    g_ToolTip.Set(L"Use the skill");
                }
                else if (id >= ID_GS_SKILL_STATE)
                {
                    g_ToolTip.Set(L"Change skill state");
                }
                break;
            }
        }
    }
    else
    {
        g_ToolTip.Set(L"Double click to maximize skills gump");
    }
}

void CGumpSkills::UpdateHeight()
{
    CGumpBaseScroll::UpdateHeight();

    m_BottomLine->SetY(Height - 48); //Bottom line
    m_Comment->SetY(Height - 35);    //Skills comment gump
    m_CreateGroup->SetY(Height - 3); //New Group gump
    m_SkillSum->SetY(Height - 6);
}

void CGumpSkills::UpdateGroupPositions()
{
    int index = 0;
    int currentY = 0;

    QFOR(group, m_HTMLGump->m_Items, CBaseGUI *)
    {
        if (group->Type == GOT_SKILLGROUP)
        {
            CGUISkillGroup *skillGroup = (CGUISkillGroup *)group;
            skillGroup->SetY(currentY);
            skillGroup->Serial = ID_GS_GROUP + index;
            skillGroup->m_Minimizer->Serial = ID_GS_GROUP_MINIMIZE + index;
            skillGroup->m_Name->Serial = ID_GS_GROUP + index;

            currentY += 19;

            if (!skillGroup->GetMinimized())
            {
                currentY += group->GetItemsCount() * 17;
            }

            index++;
        }
    }
}

CGUISkillGroup *CGumpSkills::GetSkillGroup(int index)
{
    int currentIndex = 0;

    QFOR(group, m_HTMLGump->m_Items, CBaseGUI *)
    {
        if (group->Type == GOT_SKILLGROUP)
        {
            if (currentIndex == index)
            {
                return (CGUISkillGroup *)group;
            }

            currentIndex++;
        }
    }

    return nullptr;
}

CGUISkillItem *CGumpSkills::GetSkill(int index)
{
    QFOR(group, m_HTMLGump->m_Items, CBaseGUI *)
    {
        if (group->Type == GOT_SKILLGROUP)
        {
            QFOR(item, group->m_Items, CBaseGUI *)
            {
                if (item->Type == GOT_SKILLITEM && ((CGUISkillItem *)item)->Index == index)
                {
                    return (CGUISkillItem *)item;
                }
            }
        }
    }

    return nullptr;
}

void CGumpSkills::UpdateSkillValue(int index)
{
    QFOR(group, m_HTMLGump->m_Items, CBaseGUI *)
    {
        if (group->Type == GOT_SKILLGROUP)
        {
            QFOR(item, group->m_Items, CBaseGUI *)
            {
                if (item->Type == GOT_SKILLITEM && ((CGUISkillItem *)item)->Index == index)
                {
                    ((CGUISkillItem *)item)->CreateValueText(m_ShowReal, m_ShowCap);

                    CSkill *skill = g_SkillsManager.Get(index);

                    if (skill != nullptr)
                    {
                        ((CGUISkillItem *)item)->SetStatus(skill->Status);
                    }

                    return;
                }
            }
        }
    }
}

void CGumpSkills::UpdateSkillValues()
{
    QFOR(group, m_HTMLGump->m_Items, CBaseGUI *)
    {
        if (group->Type == GOT_SKILLGROUP)
        {
            QFOR(item, group->m_Items, CBaseGUI *)
            {
                if (item->Type == GOT_SKILLITEM)
                {
                    ((CGUISkillItem *)item)->CreateValueText(m_ShowReal, m_ShowCap);
                }
            }
        }
    }
}

void CGumpSkills::UpdateSkillsSum()
{
    char str[20] = { 0 };
    sprintf_s(str, "%.1f", g_SkillsManager.SkillsTotal);
    m_SkillSum->CreateTextureA(3, str);
}

void CGumpSkills::Init()
{
    //Свернем все доступные группы
    QFOR(group, g_SkillGroupManager.m_Groups, CSkillGroupObject *)
    group->Maximized = false;
}

void CGumpSkills::CalculateGumpState()
{
    CGump::CalculateGumpState();

    if (g_PressedObject.LeftGump == this && g_PressedObject.LeftSerial >= ID_GS_SKILL &&
        g_PressedObject.LeftSerial < ID_GS_SKILL_STATE)
    {
        g_GumpMovingOffset.set(0, 0);

        if (Minimized)
        {
            g_GumpTranslate.x = (float)MinimizedX;
            g_GumpTranslate.y = (float)MinimizedY;
        }
        else
        {
            g_GumpTranslate.x = (float)m_X;
            g_GumpTranslate.y = (float)m_Y;
        }
    }
}

void CGumpSkills::PrepareContent()
{
    u32 serial = g_PressedObject.LeftSerial;

    if (g_PressedObject.LeftGump == this && serial >= ID_GS_SKILL && serial < ID_GS_SKILL_STATE)
    {
        int y = g_MouseManager.GetPosition().y;
        int testY = m_Y + m_HTMLGump->GetY();

        if (y < testY)
        {
            m_HTMLGump->Scroll(false, (SCROLL_LISTING_DELAY / 3));
            WantRedraw = true;
        }
        else if (y > testY + m_HTMLGump->Height)
        {
            m_HTMLGump->Scroll(true, (SCROLL_LISTING_DELAY / 3));
            WantRedraw = true;
        }
        else if (
            g_PressedObject.LeftObject != nullptr &&
            ((CBaseGUI *)g_PressedObject.LeftObject)->Type == GOT_SKILLITEM)
        {
            int index = 0;
            CSkillGroupObject *groupObject = GetGroupUnderCursor(index);

            if (groupObject != nullptr)
            {
                CSkillGroupObject *currentGroupObject = nullptr;
                CSkillGroupObject *groupPtr = g_SkillGroupManager.m_Groups;
                CGUISkillGroup *groupUnderCursor = nullptr;
                CGUISkillGroup *currentGroup = nullptr;
                int currentIndex = 0;

                QFOR(group, m_HTMLGump->m_Items, CBaseGUI *)
                {
                    if (group->Type == GOT_SKILLGROUP)
                    {
                        if (currentGroup == nullptr)
                        {
                            QFOR(item, group->m_Items, CBaseGUI *)
                            {
                                if (item->Serial == serial)
                                {
                                    currentGroup = (CGUISkillGroup *)group;
                                    currentGroupObject = groupPtr;
                                    break;
                                }
                            }
                        }

                        if (index == currentIndex)
                        {
                            groupUnderCursor = (CGUISkillGroup *)group;
                        }

                        if (groupPtr != nullptr)
                        {
                            groupPtr = groupPtr->m_Next;
                        }

                        currentIndex++;
                    }
                }

                int skillIndex = serial - ID_GS_SKILL;

                if (groupUnderCursor != nullptr && currentGroup != nullptr &&
                    currentGroupObject != nullptr && currentGroup != groupUnderCursor &&
                    currentGroupObject != groupObject && !groupObject->Contains(skillIndex))
                {
                    currentGroupObject->Remove(skillIndex);
                    groupObject->AddSorted(skillIndex);

                    CGUISkillItem *skillItem = (CGUISkillItem *)g_PressedObject.LeftObject;

                    currentGroup->Unlink(skillItem);

                    if (groupObject->GetItem(0) == skillIndex)
                    {
                        groupUnderCursor->Insert(nullptr, skillItem);
                    }
                    else
                    {
                        CGUISkillGroup *skillInsertElement =
                            (CGUISkillGroup *)groupUnderCursor->m_Items;
                        int itemsCount = groupObject->Count;

                        for (int i = 1; i < itemsCount; i++)
                        {
                            if (groupObject->GetItem(i) == skillIndex)
                            {
                                groupUnderCursor->Insert(skillInsertElement, skillItem);
                                break;
                            }

                            if (skillInsertElement != nullptr)
                            {
                                skillInsertElement = (CGUISkillGroup *)skillInsertElement->m_Next;
                            }
                        }
                    }

                    currentGroup->UpdateDataPositions();
                    groupUnderCursor->UpdateDataPositions();

                    UpdateGroupPositions();

                    WantRedraw = true;
                }
            }
        }
    }
}

CSkillGroupObject *CGumpSkills::GetGroupUnderCursor(int &index)
{
    index = 0;

    //Получить группу под курсором
    int mouseY = g_MouseManager.GetPosition().y;

    //mouse.X -= m_X + m_HTMLGump->GetX();
    mouseY -= m_Y + m_HTMLGump->GetY();

    //Если вышли за пределы гампа по оси X
    //if (mouse.X < 0 || mouse.X > m_HTMLGump->Width)
    //	return nullptr; //Exit from bounds on X

    //Если назодимся в пределах гампа по оси Y
    if (mouseY >= 0 && mouseY < m_HTMLGump->Height) //Bounds of Y
    {
        int drawY = m_HTMLGump->DataOffset.y - m_HTMLGump->CurrentOffset.y;
        CSkillGroupObject *group = g_SkillGroupManager.m_Groups;

        QFOR(item, m_HTMLGump->m_Items, CBaseGUI *)
        {
            if (item->Type == GOT_SKILLGROUP)
            {
                int height = ((CGUISkillGroup *)item)->GetSize().y;

                if (mouseY >= drawY && mouseY < drawY + height)
                {
                    return group;
                }

                drawY += height;

                index++;

                if (group != nullptr)
                {
                    group = group->m_Next;
                }
            }
        }
    }

    //Ничего не нашлось
    return nullptr;
}

void CGumpSkills::UpdateGroupText()
{
    QFOR(item, m_HTMLGump->m_Items, CBaseGUI *)
    {
        if (item->Type == GOT_SKILLGROUP)
        {
            CGUISkillGroup *group = (CGUISkillGroup *)item;

            if (group->m_Name != g_SelectedObject.Object)
            {
                if (g_EntryPointer == &group->m_Name->m_Entry)
                {
                    SetGroupTextFromEntry();

                    if (g_ConfigManager.GetConsoleNeedEnter())
                    {
                        g_EntryPointer = nullptr;
                    }
                    else
                    {
                        g_EntryPointer = &g_GameConsole;
                    }
                }

                group->m_Name->Focused = false;

                WantRedraw = true;
            }
        }
    }
}

void CGumpSkills::SetGroupTextFromEntry()
{
    int index = 0;
    CSkillGroupObject *groupItem = g_SkillGroupManager.m_Groups;

    QFOR(item, m_HTMLGump->m_Items, CBaseGUI *)
    {
        if (item->Type == GOT_SKILLGROUP)
        {
            CGUISkillGroup *group = (CGUISkillGroup *)item;

            if (group->m_Name->Focused && g_EntryPointer == &group->m_Name->m_Entry &&
                groupItem != nullptr)
            {
                group->m_Name->Focused = false;

                if (g_EntryPointer->Length() == 0u)
                {
                    g_EntryPointer->SetTextA("NoNameGroup");
                }

                groupItem->Name = g_EntryPointer->c_str();

                break;
            }

            group->m_Name->Focused = false;

            index++;

            if (groupItem != nullptr)
            {
                groupItem = groupItem->m_Next;
            }
        }
    }
}

void CGumpSkills::OnLeftMouseButtonUp()
{
    CGump::OnLeftMouseButtonUp();

    if (g_PressedObject.LeftGump == this && g_PressedObject.LeftSerial >= ID_GS_SKILL &&
        g_PressedObject.LeftSerial < ID_GS_SKILL_STATE)
    {
        WantRedraw = true;

        if (g_SelectedObject.Gump != this)
        {
            Core::TMousePos pos = g_MouseManager.GetPosition();
            g_GumpManager.AddGump(
                new CGumpSkill(g_PressedObject.LeftSerial - ID_GS_SKILL, pos.x - 70, pos.y - 10));
        }
    }
}

void CGumpSkills::GUMP_BUTTON_EVENT_C
{
    if (serial == ID_GBS_BUTTON_MINIMIZE) //Сворачиваем гамп
    {
        Minimized = true;
        Page = 1;
        WantRedraw = true;
    }
    else if (serial == ID_GS_LOCK_MOVING)
    {
        LockMoving = !LockMoving;
    }
    else if (serial == ID_GS_BUTTON_NEW_GROUP) //Создание новой группы
    {
        CSkillGroupObject *group = new CSkillGroupObject();
        group->Name = "New Group";
        g_SkillGroupManager.Add(group);

        CGUISkillGroup *skillGroup = (CGUISkillGroup *)m_HTMLGump->Add(
            new CGUISkillGroup(ID_GS_GROUP, ID_GS_GROUP_MINIMIZE, group, 0, 0));
        skillGroup->SetMinimized(!group->Maximized);

        UpdateGroupPositions();
    }
    else if (serial >= ID_GS_GROUP_MINIMIZE) //Операции со скиллами
    {
        if (serial >= ID_GS_SKILL_STATE) //Изменение статуса
        {
            int index = serial - ID_GS_SKILL_STATE;

            if (index < 0 || index >= (int)g_SkillsManager.Count)
            {
                return;
            }

            if (g_Player == nullptr)
            {
                return;
            }

            CSkill *skill = g_SkillsManager.Get(index);

            if (skill != nullptr)
            {
                u8 status = skill->Status;

                if (status < 2)
                {
                    status++;
                }
                else
                {
                    status = 0;
                }

                CPacketSkillsStatusChangeRequest(index, status).Send();
                skill->Status = status;

                CGUISkillItem *skillItem = GetSkill(index);

                if (skillItem != nullptr)
                {
                    skillItem->SetStatus(status);
                }
            }
        }
        else if (serial >= ID_GS_SKILL_BUTTON) //Выбор кнопки для использования скилла
        {
            int index = serial - ID_GS_SKILL_BUTTON;

            if (index < 0 || index >= (int)g_SkillsManager.Count)
            {
                return;
            }

            g_Orion.UseSkill(index);
        }
        else if (serial >= ID_GS_GROUP_MINIMIZE) //Скрыть/раскрыть группу
        {
            int index = serial - ID_GS_GROUP_MINIMIZE;
            int currentIndex = 0;

            QFOR(group, g_SkillGroupManager.m_Groups, CSkillGroupObject *)
            {
                if (index == currentIndex)
                {
                    group->Maximized = !group->Maximized;

                    CGUISkillGroup *skillGroup = GetSkillGroup(index);

                    if (skillGroup != nullptr)
                    {
                        skillGroup->SetMinimized(!group->Maximized);
                        UpdateGroupPositions();

                        m_HTMLGump->CalculateDataSize();
                    }

                    break;
                }

                currentIndex++;
            }
        }
    }
}

void CGumpSkills::GUMP_CHECKBOX_EVENT_C
{
    if (serial == ID_GS_SHOW_REAL) //Показать реальное значение
    {
        m_ShowReal = state;
        m_ShowCap = false;
        m_CheckboxShowCap->Checked = false;
        UpdateSkillValues();
    }
    else if (serial == ID_GS_SHOW_CAP) //Показать доступный предел прокачки
    {
        m_ShowCap = state;
        m_ShowReal = false;
        m_CheckboxShowReal->Checked = false;
        UpdateSkillValues();
    }
}

void CGumpSkills::GUMP_TEXT_ENTRY_EVENT_C
{
    CGUISkillGroup *group = GetSkillGroup(serial - ID_GS_GROUP);

    if (group != nullptr)
    {
        if (group->m_Name->Focused)
        {
            int x = g_MouseManager.GetPosition().x - group->GetX();
            int y = g_MouseManager.GetPosition().y - group->GetY();

            group->m_Name->OnClick(this, x, y);
        }
        else
        {
            group->m_Name->Focused = true;
        }
    }
}

bool CGumpSkills::OnLeftMouseButtonDoubleClick()
{
    if (Minimized) //При даблклике по мини-гампу - раскрываем его
    {
        Minimized = false;
        Page = 2;
        WantRedraw = true;

        return true;
    }

    return false;
}

void CGumpSkills::OnTextInput(const Core::TextEvent &ev)
{
    g_EntryPointer->Insert(ev.wtext[0]);
    int val = g_FontManager.GetWidthA(6, g_EntryPointer->c_str());
    if (val > 170)
    {
        g_EntryPointer->Remove(true);
    }
    else
    {
        WantRedraw = true;
    }
}

void CGumpSkills::OnKeyDown(const Core::KeyEvent &ev)
{
    if (!EntryPointerHere())
    {
        if (ev.key == Core::EKey::Key_Delete)
        {
            int index = 0;
            CSkillGroupObject *groupItem = g_SkillGroupManager.m_Groups;
            CGUISkillGroup *first = nullptr;

            QFOR(item, m_HTMLGump->m_Items, CBaseGUI *)
            {
                if (item->Type == GOT_SKILLGROUP)
                {
                    CGUISkillGroup *group = (CGUISkillGroup *)item;
                    if (group->m_Name->Focused)
                    {
                        if (g_EntryPointer == &group->m_Name->m_Entry)
                        {
                            break;
                        }

                        if (g_SkillGroupManager.Remove(groupItem))
                        {
                            m_HTMLGump->Delete(item);

                            if (first != nullptr)
                            {
                                first->Clear();
                                groupItem = g_SkillGroupManager.m_Groups;
                                int count = groupItem->Count;

                                for (int i = 0; i < count; i++)
                                {
                                    u8 index = groupItem->GetItem(i);
                                    if (index < g_SkillsManager.Count)
                                    {
                                        first->Add(new CGUISkillItem(
                                            ID_GS_SKILL + index,
                                            ID_GS_SKILL_BUTTON + index,
                                            ID_GS_SKILL_STATE + index,
                                            index,
                                            0,
                                            (int)i * 17));
                                    }
                                }
                            }

                            UpdateGroupPositions();

                            m_HTMLGump->CalculateDataSize();
                        }
                        WantRedraw = true;
                        break;
                    }

                    if (index == 0)
                    {
                        first = group;
                    }

                    index++;
                    if (groupItem != nullptr)
                    {
                        groupItem = groupItem->m_Next;
                    }
                }
            }
        }
        return;
    }

    switch (ev.key)
    {
        case Core::EKey::Key_Return:
        case Core::EKey::Key_Return2:
        {
            SetGroupTextFromEntry();
            if (g_ConfigManager.GetConsoleNeedEnter())
            {
                g_EntryPointer = nullptr;
            }
            else
            {
                g_EntryPointer = &g_GameConsole;
            }
            WantRedraw = true;
            break;
        }
        case Core::EKey::Key_Home:
        case Core::EKey::Key_End:
        case Core::EKey::Key_Left:
        case Core::EKey::Key_Right:
        case Core::EKey::Key_Backspace:
        case Core::EKey::Key_Delete:
        {
            g_EntryPointer->OnKey(this, ev.key);
            break;
        }
        default:
            break;
    }
}
