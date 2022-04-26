#include "GumpManager.h"
#include "Core/File.h"
#include "Core/MappedFile.h"
#include "Core/StringUtils.h"
#include "ConfigManager.h"
#include "GameWindow.h"
#include "Globals.h"
#include "SkillsManager.h"
#include "MacroManager.h"
#include "OptionsMacroManager.h"
#include "MouseManager.h"
#include "Platform.h"
#include "../OrionUO.h"
#include "../Party.h"
#include "../PressedObject.h"
#include "../SelectedObject.h"
#include "../UseItemsList.h"
#include "../ContainerStack.h"
#include "../Container.h"
#include "../TextEngine/GameConsole.h"
#include "../GameObjects/ObjectOnCursor.h"
#include "../GameObjects/GamePlayer.h"
#include "../Gumps/Gump.h"
#include "../Gumps/GumpMenubar.h"
#include "../Gumps/GumpMinimap.h"
#include "../Gumps/GumpSkills.h"
#include "../Gumps/GumpSkill.h"
#include "../Gumps/GumpJournal.h"
#include "../Gumps/GumpBook.h"
#include "../Gumps/GumpSpell.h"
#include "../Gumps/GumpConsoleType.h"
#include "../Gumps/GumpBuff.h"
#include "../Gumps/GumpStatusbar.h"
#include "../Gumps/GumpPaperdoll.h"
#include "../Gumps/GumpWorldMap.h"
#include "../Gumps/GumpAbility.h"
#include "../Gumps/GumpRacialAbility.h"
#include "../Gumps/GumpRacialAbilitiesBook.h"
#include "../Gumps/GumpPropertyIcon.h"
#include "../Gumps/GumpCombatBook.h"
#include "../Gumps/GumpSecureTrading.h"
#include "../Gumps/GumpCustomHouse.h"
#include "../Gumps/GumpPopupMenu.h"
#include "../Gumps/GumpOptions.h"
#include "../Gumps/GumpMenu.h"
#include "../Gumps/GumpGeneric.h"
#include "../Network/Packets.h"
#include <assert.h>

CGumpManager g_GumpManager;

int CGumpManager::GetNonpartyStatusbarsCount()
{
    int count = 0;

    QFOR(gump, m_Items, CGump *)
    {
        if (gump->GumpType == GT_STATUSBAR && gump->Serial != g_PlayerSerial &&
            !g_Party.Contains(gump->Serial))
        {
            count++;
        }
    }

    return count;
}

// FIXME: This API taking ownership of obj and deleting "sometimes" is not safe
void CGumpManager::AddGump(CGump *obj)
{
    if (m_Items == nullptr)
    {
        m_Items = obj;
        obj->m_Next = nullptr;
        obj->m_Prev = nullptr;
    }
    else
    {
        bool canCheck =
            (obj->GumpType != GT_GENERIC && obj->GumpType != GT_MENU && obj->GumpType != GT_TRADE &&
             obj->GumpType != GT_BULLETIN_BOARD_ITEM);

        QFOR(gump, m_Items, CGump *)
        {
            GUMP_TYPE gumpType = gump->GumpType;
            if (gumpType == obj->GumpType && canCheck)
            {
                if (gump->Serial == obj->Serial)
                {
                    if (gump->CanBeMoved())
                    {
                        if (gumpType == GT_DRAG || gumpType == GT_ABILITY ||
                            gumpType == GT_RACIAL_ABILITY)
                        {
                            gump->SetX(obj->GetX());
                            gump->SetY(obj->GetY());
                        }
                        else if (gumpType == GT_STATUSBAR)
                        {
                            CGumpStatusbar *sb = (CGumpStatusbar *)gump;
                            int gx = obj->GetX();
                            int gy = obj->GetY();
                            if (obj->Minimized)
                            {
                                gx = obj->MinimizedX;
                                gy = obj->MinimizedY;
                            }

                            if (sb->InGroup())
                            {
                                sb->UpdateGroup(-(gump->MinimizedX - gx), -(gump->MinimizedY - gy));
                            }

                            if (gump->Minimized)
                            {
                                gump->MinimizedX = gx;
                                gump->MinimizedY = gy;
                            }
                            else
                            {
                                gump->SetX(gx);
                                gump->SetY(gy);
                            }
                        }
                        else if (gumpType == GT_SPELL)
                        {
                            CGumpSpell *spell = (CGumpSpell *)gump;
                            int gx = obj->GetX();
                            int gy = obj->GetY();
                            if (spell->InGroup())
                            {
                                spell->UpdateGroup(-(gump->GetX() - gx), -(gump->GetY() - gy));
                            }

                            gump->SetX(gx);
                            gump->SetY(gy);
                        }
                    }

                    obj->m_Next = nullptr;
                    delete obj;
                    obj = nullptr;

                    if (gumpType == GT_WORLD_MAP && !((CGumpWorldMap *)gump)->Called)
                    {
                        ((CGumpWorldMap *)gump)->Called = true;
                    }
                    else if (gump->GumpType == GT_POPUP_MENU)
                    {
                        g_PopupMenu = (CGumpPopupMenu *)gump;
                    }
                    else if (
                        gumpType == GT_CONTAINER || gumpType == GT_JOURNAL || gumpType == GT_SKILLS)
                    {
                        gump->Minimized = false;
                        gump->Page = 2;
                    }
                    else if (gumpType == GT_MINIMAP)
                    {
                        gump->Minimized = !gump->Minimized;
                        ((CGumpMinimap *)gump)->LastX = 0;
                    }
                    else if (gumpType == GT_SPELLBOOK && gump->Minimized)
                    {
                        gump->Minimized = false;
                        gump->WantUpdateContent = true;
                    }

                    MoveToBack(gump);
                    gump->FrameCreated = false;
                    return;
                }
            }

            if (gump->m_Next == nullptr)
            {
                gump->m_Next = obj;
                obj->m_Prev = gump;
                obj->m_Next = nullptr;
                break;
            }
        }
    }

    if (obj->GumpType == GT_STATUSBAR && !g_Party.Contains(obj->Serial) &&
        GetNonpartyStatusbarsCount() > 10)
    {
        //RemoveGump(obj);
        QFOR(gump, m_Items, CGump *)
        {
            if (gump->GumpType == GT_STATUSBAR && gump->Serial != g_PlayerSerial &&
                !g_Party.Contains(gump->Serial))
            {
                RemoveGump(gump);
                g_Orion.CreateTextMessage(
                    TT_SYSTEM, 0xFFFFFFFF, 3, 0, "You have too many non-party status bars");

                break;
            }
        }
        obj->PrepareTextures();
    }
    else
    {
        switch (obj->GumpType)
        {
            case GT_CONTAINER:
            {
                u16 sound = g_ContainerOffset[obj->Graphic].OpenSound;
                if (sound != 0u)
                {
                    g_Orion.PlaySoundEffect(sound);
                }
                break;
            }
            case GT_SPELLBOOK:
            {
                //UO->PlaySoundEffect(0x????);
                break;
            }
            case GT_SKILLS:
            {
                ((CGumpSkills *)obj)->Init();
                break;
            }
            case GT_OPTIONS:
            {
                ((CGumpOptions *)obj)->Init();
                break;
            }
            case GT_WORLD_MAP:
            {
                if (g_ConfigManager.GetUseGlobalMapLayer())
                {
                    MoveToFront(obj);
                }
                break;
            }
            default:
                break;
        }
        obj->PrepareTextures();
    }
}

CGump *CGumpManager::GetTextEntryOwner()
{
    QFOR(gump, m_Items, CGump *)
    {
        if (gump->EntryPointerHere())
        {
            return gump;
        }
    }

    return nullptr;
}

CGump *CGumpManager::GumpExists(uintptr_t gumpID)
{
    QFOR(gump, m_Items, CGump *)
    {
        if (gumpID == (uintptr_t)gump)
        {
            return gump;
        }
    }

    return nullptr;
}

CGump *CGumpManager::UpdateContent(int serial, int id, const GUMP_TYPE &type)
{
    CGump *gump = GetGump(serial, id, type);

    if (gump != nullptr)
    {
        gump->WantUpdateContent = true;
    }

    return gump;
}

CGump *CGumpManager::UpdateGump(int serial, int id, const GUMP_TYPE &type)
{
    CGump *gump = GetGump(serial, id, type);

    if (gump != nullptr)
    {
        gump->WantRedraw = true;
    }

    return gump;
}

CGump *CGumpManager::GetGump(int serial, int id, const GUMP_TYPE &type)
{
    CGump *gump = (CGump *)m_Items;

    while (gump != nullptr)
    {
        if (gump->GumpType == type)
        {
            if (type == GT_SHOP || type == GT_TARGET_SYSTEM || type == GT_CUSTOM_HOUSE)
            {
                break;
            }
            if (type == GT_TRADE)
            {
                if (gump->Serial == serial)
                {
                    break;
                }
                if ((serial == 0) && (gump->ID == id || ((CGumpSecureTrading *)gump)->ID2 == id))
                {
                    break;
                }
            }
            else if (gump->Serial == serial)
            {
                if (type == GT_CONTAINER || type == GT_SPELLBOOK || type == GT_BULLETIN_BOARD_ITEM)
                {
                    break;
                }
                if (gump->ID == id)
                {
                    break;
                }
            }
        }

        gump = (CGump *)gump->m_Next;
    }

    return gump;
}

void CGumpManager::CloseGump(u32 serial, u32 id, GUMP_TYPE type)
{
    for (CGump *gump = (CGump *)m_Items; gump != nullptr;)
    {
        CGump *next = (CGump *)gump->m_Next;

        if (gump->GumpType == type)
        {
            if (type == GT_TARGET_SYSTEM)
            {
                RemoveGump(gump);
            }
            else if (type == GT_GENERIC && gump->Serial == serial && gump->ID == id)
            {
                RemoveGump(gump);
            }
            else if (type == GT_TRADE && gump->ID == serial)
            {
                RemoveGump(gump);
            }
            else if (type == GT_BULLETIN_BOARD_ITEM && gump->ID == id)
            {
                if (serial == 0xFFFFFFFF || gump->Serial == serial)
                {
                    RemoveGump(gump);
                }
            }
            else if (type == GT_SHOP || type == GT_SPELLBOOK || type == GT_DRAG)
            {
                if (serial != 0u)
                {
                    if (serial == gump->Serial)
                    {
                        RemoveGump(gump);
                    }
                }
                else
                {
                    RemoveGump(gump);
                }
            }
            else if (gump->Serial == serial)
            {
                RemoveGump(gump);
            }
        }

        gump = next;
    }
}

void CGumpManager::RemoveGump(CGump *obj)
{
    Unlink(obj);

    if (g_World != nullptr)
    {
        CGameItem *selobj = g_World->FindWorldItem(obj->Serial);

        if (selobj != nullptr)
        {
            selobj->Dragged = false;
            selobj->Opened = false;
        }
    }

    if (obj->GumpType == GT_CONTAINER && obj->Graphic > 0 &&
        obj->Graphic < g_ContainerOffset.size())
    {
        u16 sound = g_ContainerOffset[obj->Graphic].CloseSound;

        if (sound != 0u)
        {
            g_Orion.PlaySoundEffect(sound);
        }
    }

    obj->m_Next = nullptr;
    obj->m_Prev = nullptr;
    delete obj;
}

void CGumpManager::OnDelete()
{
    CGump *gump = (CGump *)m_Items;

    while (gump != nullptr)
    {
        CGump *tmp = (CGump *)gump->m_Next;

        if (gump->GumpType == GT_STATUSBAR)
        {
            RemoveGump(gump);
        }

        gump = tmp;
    }
}

void CGumpManager::RemoveRangedGumps()
{
    if (g_World != nullptr)
    {
        CGump *gump = (CGump *)m_Items;

        while (gump != nullptr)
        {
            CGump *tmp = (CGump *)gump->m_Next;

            switch (gump->GumpType)
            {
                //case GT_STATUSBAR:
                case GT_PAPERDOLL:
                case GT_MAP:
                case GT_SPELLBOOK:
                case GT_DRAG:
                {
                    if (g_World->FindWorldObject(gump->Serial) == nullptr)
                    {
                        RemoveGump(gump); //Или CloseGump() ?
                    }
                    break;
                }
                case GT_TRADE:
                case GT_SHOP:
                {
                    if (GetTopObjDistance(g_Player, g_World->FindWorldObject(gump->Serial)) >
                        MAX_VIEW_RANGE_OLD)
                    {
                        RemoveGump(gump);
                    }
                    break;
                }
                case GT_CONTAINER:
                {
                    if (GetTopObjDistance(g_Player, g_World->FindWorldObject(gump->Serial)) >
                        REMOVE_CONTAINER_GUMP_RANGE)
                    {
                        RemoveGump(gump);
                    }
                    break;
                }
                default:
                    break;
            }

            gump = tmp;
        }
    }
}

void CGumpManager::PrepareContent()
{
    for (CGump *gump = (CGump *)m_Items; gump != nullptr;)
    {
        CGump *next = (CGump *)gump->m_Next;

        if (!gump->RemoveMark)
        {
            gump->PrepareContent();
        }

        if (gump->RemoveMark)
        {
            RemoveGump(gump);
        }

        gump = next;
    }
}

void CGumpManager::RemoveMarked()
{
    for (CGump *gump = (CGump *)m_Items; gump != nullptr;)
    {
        CGump *next = (CGump *)gump->m_Next;

        if (gump->RemoveMark)
        {
            RemoveGump(gump);
        }

        gump = next;
    }
}

void CGumpManager::PrepareTextures()
{
    QFOR(gump, m_Items, CGump *)
    {
        g_CurrentCheckGump = gump;
        gump->PrepareTextures();
    }

    g_CurrentCheckGump = nullptr;
}

void CGumpManager::Draw(bool blocked)
{
    CGump *gump = (CGump *)m_Items;
    CGump *menuBarGump = nullptr;

    while (gump != nullptr)
    {
        CGump *next = (CGump *)gump->m_Next;

        if (blocked == gump->Blocked)
        {
            if (gump->CanBeDisplayed())
            {
                if (g_CustomHouseGump == nullptr || gump == g_CustomHouseGump ||
                    gump->GumpType == GT_GENERIC)
                {
                    gump->Draw();
                }
            }

            if (gump->GumpType == GT_MENUBAR)
            {
                menuBarGump = gump;
            }
        }

        gump = next;
    }

    if (menuBarGump != nullptr &&
        (g_CustomHouseGump == nullptr || menuBarGump == g_CustomHouseGump))
    {
        menuBarGump->Draw();
    }
}

void CGumpManager::Select(bool blocked)
{
    CGump *gump = (CGump *)m_Items;
    CGump *menuBarGump = nullptr;

    while (gump != nullptr)
    {
        CGump *next = (CGump *)gump->m_Next;

        if (blocked == gump->Blocked)
        {
            if (gump->CanBeDisplayed())
            {
                if (g_CustomHouseGump == nullptr || gump == g_CustomHouseGump ||
                    gump->GumpType == GT_GENERIC)
                {
                    gump->Select();
                }
            }

            if (gump->GumpType == GT_MENUBAR)
            {
                menuBarGump = gump;
            }
        }

        gump = next;
    }

    if (menuBarGump != nullptr && blocked == menuBarGump->Blocked &&
        (g_CustomHouseGump == nullptr || menuBarGump == g_CustomHouseGump))
    {
        menuBarGump->Select();
    }
}

void CGumpManager::InitToolTip()
{
    CGump *gump = g_SelectedObject.Gump;

    if (gump != nullptr)
    {
        if (!g_ConfigManager.UseToolTips &&
            (!g_TooltipsEnabled || (gump->GumpType != GT_CONTAINER &&
                                    gump->GumpType != GT_PAPERDOLL && gump->GumpType != GT_TRADE)))
        {
            return;
        }

        gump->InitToolTip();
    }
}

void CGumpManager::RedrawAll()
{
    QFOR(gump, m_Items, CGump *)
    gump->WantRedraw = true;
}

void CGumpManager::OnLeftMouseButtonDown(bool blocked)
{
    if (g_SelectedObject.Object != nullptr && g_SelectedObject.Object->IsText())
    {
        return;
    }

    QFOR(gump, m_Items, CGump *)
    {
        if (g_SelectedObject.Gump == gump && !gump->NoProcess)
        {
            if (gump->GumpType == GT_STATUSBAR && ((CGumpStatusbar *)gump)->InGroup())
            {
                ((CGumpStatusbar *)gump)->UpdateGroup(0, 0);
            }
            else if (gump->GumpType == GT_SPELL && ((CGumpSpell *)gump)->InGroup())
            {
                ((CGumpSpell *)gump)->UpdateGroup(0, 0);
            }
            else
            {
                MoveToBack(gump);
            }

            gump->OnLeftMouseButtonDown();

            break;
        }
    }

    RemoveMarked();
}

bool CGumpManager::OnLeftMouseButtonUp(bool blocked)
{
    if (g_SelectedObject.Object != nullptr && g_SelectedObject.Object->IsText())
    {
        return false;
    }

    QFOR(gump, m_Items, CGump *)
    {
        if (g_PressedObject.LeftGump == gump && !gump->NoProcess)
        {
            bool canMove = false;

            if (g_PressedObject.LeftObject != nullptr)
            {
                if (!g_PressedObject.LeftObject->IsText())
                {
                    if (g_PressedObject.LeftObject->Serial == 0u)
                    {
                        canMove = true;
                    }
                    else if (
                        g_PressedObject.LeftObject->IsGUI() &&
                        ((CBaseGUI *)g_PressedObject.LeftObject)->MoveOnDrag)
                    {
                        canMove = true;
                    }
                }
            }
            else
            {
                canMove = true;
            }

            if (canMove && gump->CanBeMoved() && !gump->NoMove && !g_ObjectInHand.Enabled)
            {
                Core::Vec2<i32> offset = g_MouseManager.GetLeftDroppedOffset();

                if (gump->GumpType == GT_STATUSBAR)
                {
                    CGumpStatusbar *sb = (CGumpStatusbar *)gump;

                    if (!gump->Minimized)
                    {
                        sb->RemoveFromGroup();
                        gump->SetX(gump->GetX() + offset.x);
                        gump->SetY(gump->GetY() + offset.y);

                        gump->FixCoordinates();
                    }
                    else
                    {
                        gump->MinimizedX = gump->MinimizedX + offset.x;
                        gump->MinimizedY = gump->MinimizedY + offset.y;

                        gump->FixCoordinates();

                        if (sb->InGroup())
                        {
                            sb->UpdateGroup(offset.x, offset.y);
                        }
                        else
                        {
                            int testX = g_MouseManager.GetPosition().x;
                            int testY = g_MouseManager.GetPosition().y;

                            CGumpStatusbar *nearBar = sb->GetNearStatusbar(testX, testY);

                            if (nearBar != nullptr)
                            {
                                gump->MinimizedX = testX;
                                gump->MinimizedY = testY;

                                nearBar->AddStatusbar(sb);
                            }
                        }
                    }
                }
                else if (gump->GumpType == GT_SPELL)
                {
                    CGumpSpell *spell = (CGumpSpell *)gump;

                    gump->SetX(gump->GetX() + offset.x);
                    gump->SetY(gump->GetY() + offset.y);

                    gump->FixCoordinates();

                    if (spell->InGroup())
                    {
                        spell->UpdateGroup(offset.x, offset.y);
                    }
                    else
                    {
                        int testX = g_MouseManager.GetPosition().x;
                        int testY = g_MouseManager.GetPosition().y;

                        CGumpSpell *nearSpell = spell->GetNearSpell(testX, testY);

                        if (nearSpell != nullptr)
                        {
                            gump->SetX(testX);
                            gump->SetY(testY);

                            nearSpell->AddSpell(spell);
                        }
                    }
                }
                else if (gump->Minimized && gump->GumpType != GT_MINIMAP)
                {
                    gump->MinimizedX = gump->MinimizedX + offset.x;
                    gump->MinimizedY = gump->MinimizedY + offset.y;

                    gump->FixCoordinates();
                }
                else
                {
                    gump->SetX(gump->GetX() + offset.x);
                    gump->SetY(gump->GetY() + offset.y);

                    gump->FixCoordinates();
                }

                g_MouseManager.CancelDoubleClick = true;

                //gump->FrameCreated = false;
            }

            if (g_ObjectInHand.Enabled)
            {
                if (g_SelectedObject.Gump == gump)
                {
                    gump->OnLeftMouseButtonUp();
                }
            }
            else
            {
                gump->OnLeftMouseButtonUp();
            }

            if (gump->GumpType == GT_WORLD_MAP && g_ConfigManager.GetUseGlobalMapLayer())
            {
                MoveToFront(gump);
            }

            RemoveMarked();
            //MoveGumpToTop(gump);
            return true;
        }
        if (g_SelectedObject.Gump == gump && g_ObjectInHand.Enabled &&
            (gump->GumpType == GT_PAPERDOLL || gump->GumpType == GT_CONTAINER ||
             gump->GumpType == GT_TRADE))
        {
            gump->OnLeftMouseButtonUp();

            RemoveMarked();
            return true;
        }
    }

    return false;
}

bool CGumpManager::OnLeftMouseButtonDoubleClick(bool blocked)
{
    bool result = false;

    if (g_SelectedObject.Object != nullptr && g_SelectedObject.Object->IsText())
    {
        return result;
    }

    QFOR(gump, m_Items, CGump *)
    {
        if (g_SelectedObject.Gump == gump && !gump->NoProcess)
        {
            if (gump->OnLeftMouseButtonDoubleClick())
            {
                result = true;
            }

            //MoveGumpToTop(gump);
            break;
        }
    }

    RemoveMarked();

    return result;
}

void CGumpManager::OnRightMouseButtonDown(bool blocked)
{
    if (g_SelectedObject.Object != nullptr && g_SelectedObject.Object->IsText())
    {
        return;
    }

    QFOR(gump, m_Items, CGump *)
    {
        if (g_SelectedObject.Gump == gump && !gump->NoProcess)
        {
            if (gump->GumpType == GT_STATUSBAR && ((CGumpStatusbar *)gump)->InGroup())
            {
                ((CGumpStatusbar *)gump)->UpdateGroup(0, 0);
            }
            else if (gump->GumpType == GT_SPELL && ((CGumpSpell *)gump)->InGroup())
            {
                ((CGumpSpell *)gump)->UpdateGroup(0, 0);
            }
            else
            {
                MoveToBack(gump);
            }

            g_MouseManager.CancelDoubleClick = true;

            break;
        }
    }

    RemoveMarked();
}

void CGumpManager::OnRightMouseButtonUp(bool blocked)
{
    if (g_SelectedObject.Object != nullptr && g_SelectedObject.Object->IsText())
    {
        return;
    }

    QFOR(gump, m_Items, CGump *)
    {
        if (g_PressedObject.RightGump == gump && !gump->NoProcess && !gump->NoClose &&
            (gump->CanBeMoved() || gump->GumpType == GT_GENERIC))
        {
            //gump->OnClose();
            switch (gump->GumpType)
            {
                case GT_CONTAINER:
                {
                    CGameItem *obj = g_World->FindWorldItem(gump->Serial);

                    if (obj != nullptr)
                    {
                        if (obj->IsCorpse())
                        {
                            for (CGameItem *gi = (CGameItem *)obj->m_Items; gi != nullptr;)
                            {
                                CGameItem *next = (CGameItem *)gi->m_Next;

                                if (gi->Layer == OL_NONE)
                                {
                                    g_World->RemoveObject(gi);
                                }

                                gi = next;
                            }
                        }
                        else
                        {
                            obj->Clear();
                        }

                        obj->Opened = false;
                        CloseGump(gump->Serial, 0, GT_CONTAINER);
                    }

                    break;
                }
                case GT_MENUBAR:
                {
                    gump->SetX(0);
                    gump->SetY(0);
                    gump->FrameCreated = false;

                    break;
                }
                case GT_BUFF:
                {
                    //gump->SetX(g_GameWindowPosX);
                    //gump->SetY(g_GameWindowPosY + g_GameWindowHeight);
                    gump->FrameCreated = false;
                    g_ConfigManager.ToggleBufficonWindow = false;

                    break;
                }
                case GT_MENU:
                {
                    ((CGumpMenu *)gump)->SendMenuResponse(0);

                    break;
                }
                case GT_PROPERTY_ICON:
                {
                    if (g_ConfigManager.GetItemPropertiesMode() == OPM_ALWAYS_UP)
                    {
                        g_ConfigManager.SetItemPropertiesMode(OPM_FOLLOW_MOUSE);
                    }

                    break;
                }
                case GT_TRADE:
                {
                    ((CGumpSecureTrading *)gump)->SendTradingResponse(1);

                    break;
                }
                case GT_GENERIC:
                {
                    ((CGumpGeneric *)gump)->SendGumpResponse(0);

                    break;
                }
                case GT_OPTIONS:
                {
                    g_OptionsMacroManager.Clear();

                    CloseGump(gump->Serial, gump->ID, gump->GumpType);

                    break;
                }
                case GT_STATUSBAR:
                {
                    CGumpStatusbar *sb = ((CGumpStatusbar *)gump)->GetTopStatusbar();

                    if (sb != nullptr)
                    {
                        while (sb != nullptr)
                        {
                            CGumpStatusbar *next = sb->m_GroupNext;

                            sb->RemoveFromGroup();
                            RemoveGump(sb);

                            sb = next;
                        }
                    }
                    else
                    {
                        RemoveGump(gump);
                    }

                    break;
                }
                /*case GT_SPELL:
				{
					CGumpSpell *spell = ((CGumpSpell*)gump)->GetTopSpell();

					if (spell != nullptr)
					{
						while (spell != nullptr)
						{
							CGumpSpell *next = spell->m_GroupNext;

							spell->RemoveFromGroup();
							RemoveGump(spell);

							spell = next;
						}
					}
					else
						RemoveGump(gump);

					break;
				}*/
                case GT_SPELLBOOK:
                {
                    g_Orion.PlaySoundEffect(0x0055);
                    CloseGump(gump->Serial, gump->ID, gump->GumpType);

                    break;
                }
                case GT_BOOK:
                {
                    ((CGumpBook *)gump)->ChangePage(0);
                    CloseGump(gump->Serial, gump->ID, gump->GumpType);

                    break;
                }
                default:
                {
                    CloseGump(gump->Serial, gump->ID, gump->GumpType);

                    break;
                }
            }

            break;
        }
    }

    RemoveMarked();
}

void CGumpManager::OnMidMouseButtonScroll(bool up, bool blocked)
{
    QFOR(gump, m_Items, CGump *)
    {
        if (g_SelectedObject.Gump == gump && !gump->NoProcess)
        {
            gump->OnMidMouseButtonScroll(up);

            break;
        }
    }
}

void CGumpManager::OnDragging(bool blocked)
{
    QFOR(gump, m_Items, CGump *)
    {
        if (g_PressedObject.LeftGump == gump && !gump->NoProcess)
        {
            gump->OnDragging();

            break;
        }
    }
}

bool CGumpManager::OnTextInput(const TextEvent &ev, bool blocked)
{

    CGump *gump = GetTextEntryOwner();
    bool result = false;
    if (gump != nullptr && !gump->NoProcess)
    {
        if (g_GameState == GS_GAME_BLOCKED)
        {
            if (gump->GumpType == GT_TEXT_ENTRY_DIALOG)
            {
                gump->OnTextInput(ev);
                result = true;
            }
        }
        else
        {
            gump->OnTextInput(ev);
            result = true;
        }
    }

    RemoveMarked();
    return result;
}

bool CGumpManager::OnKeyDown(const KeyEvent &ev, bool blocked)
{

    bool result = false;
    const auto key = EvKey(ev);
    if (g_EntryPointer != nullptr && g_EntryPointer != &g_GameConsole)
    {
        CGump *gump = GetTextEntryOwner();
        if (gump != nullptr && !gump->NoProcess)
        {
            switch (gump->GumpType)
            {
                case GT_STATUSBAR:
                case GT_DRAG:
                case GT_OPTIONS:
                case GT_SKILLS:
                case GT_PROFILE:
                case GT_BULLETIN_BOARD_ITEM:
                case GT_GENERIC:
                case GT_BOOK:
                {
                    gump->OnKeyDown(ev);
                    result = true;
                    break;
                }
                default:
                    break;
            }
        }
        else if (key == KEY_DELETE)
        {
            gump = GetGump(0, 0, GT_SKILLS);
            if (gump != nullptr && !gump->NoProcess)
            {
                gump->OnKeyDown(ev);
            }
        }
    }
    else if (key == KEY_DELETE)
    {
        CGump *gump = GetGump(0, 0, GT_SKILLS);
        if (gump != nullptr && !gump->NoProcess)
        {
            gump->OnKeyDown(ev);
        }
    }

    RemoveMarked();
    return result;
}

void CGumpManager::Load(const std::filesystem::path& a_path)
{
    bool paperdollRequested = false;
    bool menubarFound = false;
    bool bufficonWindowFound = false;
    bool minimizedConsoleType = false;
    bool showFullTextConsoleType = false;

    Core::MappedFile file;
    if (file.Load(a_path) && (file.GetSize() != 0u))
    {
        u8 version = file.ReadLE<u8>();
        u8 *oldPtr = file.GetPtr();

        short count = 0;
        short spellGroupsCount = 0;

        if (version != 0u)
        {
            file.SetPtr((u8 *)file.GetBuffer() + (file.GetSize() - 8));
            spellGroupsCount = file.ReadLE<i16>();
            count = file.ReadLE<i16>();
        }
        else
        {
            file.SetPtr((u8 *)file.GetBuffer() + (file.GetSize() - 6));
            count = file.ReadLE<i16>();
        }

        file.SetPtr(oldPtr);
        bool menubarLoaded = false;

        for (int i = 0; i < count; i++)
        {
            CGump *gump = nullptr;
            u8 *next = file.GetPtr();
            u8 size = file.ReadLE<u8>();
            next += size;

            GUMP_TYPE gumpType = (GUMP_TYPE)file.ReadLE<u8>();
            u16 gumpX = file.ReadLE<u16>();
            u16 gumpY = file.ReadLE<u16>();
            u8 gumpMinimized = file.ReadLE<u8>();
            u16 gumpMinimizedX = file.ReadLE<u16>();
            u16 gumpMinimizedY = file.ReadLE<u16>();
            u8 gumpLockMoving = file.ReadLE<u8>();

            switch ((GUMP_TYPE)gumpType)
            {
                case GT_PAPERDOLL:
                {
                    gump = new CGumpPaperdoll(g_PlayerSerial, gumpX, gumpY, gumpMinimized != 0u);
                    g_Orion.PaperdollReq(g_PlayerSerial);
                    paperdollRequested = true;
                    break;
                }
                case GT_STATUSBAR:
                {
                    gump = new CGumpStatusbar(g_PlayerSerial, gumpX, gumpY, gumpMinimized != 0u);
                    break;
                }
                case GT_MINIMAP:
                {
                    gump = new CGumpMinimap(gumpX, gumpY, gumpMinimized != 0u);
                    break;
                }
                case GT_SKILLS:
                {
                    gump = new CGumpSkills(gumpX, gumpY, gumpMinimized != 0u, file.ReadLE<i16>());
                    gump->Visible = false;
                    g_SkillsManager.SkillsRequested = true;
                    CPacketSkillsRequest(g_PlayerSerial).Send();
                    break;
                }
                case GT_JOURNAL:
                {
                    gump = new CGumpJournal(gumpX, gumpY, gumpMinimized != 0u, file.ReadLE<i16>());
                    break;
                }
                case GT_WORLD_MAP:
                {
                    /*CGumpWorldMap *wmg = new CGumpWorldMap(gumpX, gumpY);
					gump = wmg;
					wmg->Called = g_ConfigManager.AutoDisplayWorldMap;

					wmg->Minimized = gumpMinimized;

					if (gumpMinimized)
						wmg->Page = 1;
					else
						wmg->Page = 2;

					wmg->Map = file.ReadBE<u8>();
					wmg->Scale = file.ReadBE<u8>();
					wmg->LinkWithPlayer = file.ReadBE<u8>();

					wmg->Width = file.ReadLE<i16>();
					wmg->Height = file.ReadLE<i16>();

					wmg->OffsetX = file.ReadLE<i16>();
					wmg->OffsetY = file.ReadLE<i16>();

					wmg->UpdateSize();*/

                    break;
                }
                case GT_MENUBAR:
                {
                    if (!g_ConfigManager.DisableMenubar)
                    {
                        menubarFound = true;
                        gump = new CGumpMenubar(gumpX, gumpY);
                        ((CGumpMenubar *)gump)->SetOpened(gumpMinimized != 0u);
                    }

                    break;
                }
                case GT_BUFF:
                {
                    bufficonWindowFound = true;
                    gump = new CGumpBuff(gumpX, gumpY);
                    gump->Graphic = file.ReadLE<u16>();
                    break;
                }
                case GT_ABILITY:
                {
                    gump = new CGumpAbility(file.ReadLE<u32>(), gumpX, gumpY);
                    break;
                }
                case GT_PROPERTY_ICON:
                {
                    gump = new CGumpPropertyIcon(gumpX, gumpY);
                    break;
                }
                case GT_COMBAT_BOOK:
                {
                    gump = new CGumpCombatBook(gumpX, gumpY);
                    gump->Minimized = (gumpMinimized != 0u);
                    break;
                }
                case GT_CONTAINER:
                case GT_SPELLBOOK:
                case GT_SPELL:
                {
                    u32 serial = file.ReadLE<u32>();
                    if (gumpType == GT_SPELL)
                    {
                        u16 graphic = file.ReadLE<u16>();
                        SPELLBOOK_TYPE spellType = ST_MAGE;
                        if (size > 18)
                        {
                            spellType = (SPELLBOOK_TYPE)file.ReadLE<u8>();
                        }
                        gump = new CGumpSpell(serial, gumpX, gumpY, graphic, spellType);
                    }
                    else
                    {
                        g_ContainerStack.push_back(CContainerStackItem(
                            serial,
                            gumpX,
                            gumpY,
                            gumpMinimizedX,
                            gumpMinimizedY,
                            gumpMinimized != 0u,
                            gumpLockMoving != 0u));

                        g_UseItemActions.Add(serial);
                        //g_Orion.DoubleClick(serial);
                    }

                    break;
                }
                case GT_CONSOLE_TYPE:
                {
                    minimizedConsoleType = (gumpMinimized != 0u);
                    showFullTextConsoleType = (file.ReadLE<u8>() != 0u);
                    break;
                }
                case GT_SKILL:
                {
                    u32 serial = file.ReadLE<u32>();
                    gump = new CGumpSkill(serial, gumpX, gumpY);
                    break;
                }
                case GT_RACIAL_ABILITIES_BOOK:
                {
                    gump = new CGumpRacialAbilitiesBook(gumpX, gumpY);
                    gump->Minimized = (gumpMinimized != 0u);
                    break;
                }
                case GT_RACIAL_ABILITY:
                {
                    u32 serial = file.ReadLE<u32>();
                    gump = new CGumpRacialAbility(serial, gumpX, gumpY);
                    break;
                }
                default:
                    break;
            }

            if (gump != nullptr)
            {
                if (gumpType == GT_MENUBAR)
                {
                    if (menubarLoaded)
                    {
                        delete gump;
                        continue;
                    }

                    menubarLoaded = true;
                }

                gump->MinimizedX = gumpMinimizedX;
                gump->MinimizedY = gumpMinimizedY;
                gump->LockMoving = (gumpLockMoving != 0u);
                //gump->FixCoordinates();
                AddGump(gump);
                assert(gump != nullptr && "AddGump should not delete obj here");
                gump->WantUpdateContent = true;
            }
            file.SetPtr(next);
        }

        for (int i = 0; i < spellGroupsCount; i++)
        {
            CGumpSpell *topSpell = nullptr;
            u16 spellsCount = file.ReadLE<u16>();
            for (int j = 0; j < spellsCount; j++)
            {
                u8 *next = file.GetPtr();
                u8 size = file.ReadLE<u8>();
                next += size;

                GUMP_TYPE gumpType = (GUMP_TYPE)file.ReadLE<u8>();
                u16 gumpX = file.ReadLE<u16>();
                u16 gumpY = file.ReadLE<u16>();
                file.Move(5); //Minimized state, x, y
                u8 gumpLockMoving = file.ReadLE<u8>();

                u32 serial = file.ReadLE<u32>();
                u16 graphic = file.ReadLE<u16>();

                SPELLBOOK_TYPE spellType = ST_MAGE;

                if (size > 18)
                {
                    spellType = (SPELLBOOK_TYPE)file.ReadLE<u8>();
                }

                CGumpSpell *spell = new CGumpSpell(serial, gumpX, gumpY, graphic, spellType);
                spell->LockMoving = (gumpLockMoving != 0u);

                AddGump(spell);
                assert(spell != nullptr && "AddGump should not delete obj here");
                if (topSpell == nullptr)
                {
                    topSpell = spell;
                }
                else
                {
                    topSpell->AddSpell(spell);
                }

                spell->WantUpdateContent = true;
                file.SetPtr(next);
            }
        }

        file.Unload();
    }
    else
    {
        if (!g_ConfigManager.DisableMenubar && (g_ConfigManager.GameWindowX == 0) &&
            (g_ConfigManager.GameWindowY == 0))
        {
            g_ConfigManager.GameWindowY = 40;
        }

        Core::Vec2<i32> windowSize = g_gameWindow.GetSize();

        int x = g_ConfigManager.GameWindowX + g_ConfigManager.GameWindowWidth;
        int y = g_ConfigManager.GameWindowY;

        if (x + 260 >= windowSize.x)
            x = windowSize.x - 260;

        if (y + 320 >= windowSize.y)
            y = windowSize.y - 320;

        AddGump(new CGumpPaperdoll(g_PlayerSerial, x, y, false));
        x = g_ConfigManager.GameWindowX + g_ConfigManager.GameWindowWidth;
        y = g_ConfigManager.GameWindowY + g_ConfigManager.GameWindowHeight - 50;

        if (x + 150 >= windowSize.x)
            x = windowSize.x - 150;

        if (y + 60 >= windowSize.y)
            y = windowSize.y - 60;

        AddGump(new CGumpStatusbar(g_PlayerSerial, x, y, false));
        AddGump(new CGumpMinimap(g_ConfigManager.GameWindowX, g_ConfigManager.GameWindowY, true));
        if (g_Player != nullptr)
        {
            CGameItem *backpack = g_Player->FindLayer(OL_BACKPACK);
            if (backpack != nullptr)
            {
                g_ContainerStack.push_back(CContainerStackItem(
                    backpack->Serial,
                    g_ConfigManager.GameWindowX,
                    g_ConfigManager.GameWindowY,
                    g_ConfigManager.GameWindowX,
                    g_ConfigManager.GameWindowY,
                    false,
                    false));

                g_UseItemActions.Add(backpack->Serial);
                //g_Orion.DoubleClick(backpack->Serial);
            }
        }
    }

    if (!g_ConfigManager.DisableMenubar && !menubarFound)
    {
        CGumpMenubar *mbg = new CGumpMenubar(0, 0);
        mbg->SetOpened(true);
        AddGump(mbg);
    }

    if (!bufficonWindowFound)
    {
        Core::Vec2<i32> windowSize = g_gameWindow.GetSize();

        int x = g_ConfigManager.GameWindowX + (int)(g_ConfigManager.GameWindowWidth * 0.7f);
        int y = g_ConfigManager.GameWindowY + g_ConfigManager.GameWindowHeight;
        if (x + 100 >= windowSize.x)
            x = windowSize.x - 100;

        if (y + 60 >= windowSize.y)
            y = windowSize.y - 60;

        AddGump(new CGumpBuff(x, y));
    }

    if (!paperdollRequested)
        g_Orion.PaperdollReq(g_PlayerSerial);

    AddGump(new CGumpConsoleType(minimizedConsoleType, showFullTextConsoleType));
}

void CGumpManager::SaveDefaultGumpProperties(Core::StreamWriter& a_writer, CGump* a_gump, int a_size)
{
    a_writer.WriteLE<i8>(a_size);
    a_writer.WriteLE<i8>(a_gump->GumpType);
    a_writer.WriteLE<u16>(a_gump->GetX());
    a_writer.WriteLE<u16>(a_gump->GetY());

    if (a_gump->GumpType == GT_MENUBAR)
        a_writer.WriteLE<i8>(static_cast<char>(((CGumpMenubar*)a_gump)->GetOpened()));
    else
        a_writer.WriteLE<i8>(static_cast<char>(a_gump->Minimized));

    a_writer.WriteLE<u16>(a_gump->MinimizedX);
    a_writer.WriteLE<u16>(a_gump->MinimizedY);
    a_writer.WriteLE<i8>(static_cast<char>(a_gump->LockMoving));
};

void CGumpManager::Save(const std::filesystem::path& a_path)
{
    Core::StreamWriter writer;
    writer.WriteLE<i8>(1); //version
    writer.WriteBuffer();

    short count = 0;

    std::vector<CGump *> containerList;
    std::vector<CGump *> spellInGroupList;

    QFOR(gump, m_Items, CGump *)
    {
        switch (gump->GumpType)
        {
            case GT_PAPERDOLL:
            case GT_STATUSBAR:
                if (gump->Serial != g_PlayerSerial)
                {
                    break;
                }
            case GT_MINIMAP:
            case GT_SKILLS:
            case GT_JOURNAL:
            case GT_WORLD_MAP:
            case GT_PROPERTY_ICON:
            {
                u8 size = 12;

                if (gump->GumpType == GT_JOURNAL)
                {
                    size += 2;
                }
                else if (gump->GumpType == GT_SKILLS)
                {
                    size += 2;
                }
                else if (gump->GumpType == GT_WORLD_MAP)
                {
                    size += 11;
                }

                SaveDefaultGumpProperties(writer, gump, size);

                if (gump->GumpType == GT_JOURNAL)
                {
                    writer.WriteLE<u16>(((CGumpJournal *)gump)->Height);
                }
                else if (gump->GumpType == GT_SKILLS)
                {
                    writer.WriteLE<u16>(((CGumpSkills *)gump)->Height);
                }
                else if (gump->GumpType == GT_WORLD_MAP)
                {
                    CGumpWorldMap *wmg = (CGumpWorldMap *)gump;

                    writer.WriteLE<u8>(wmg->GetMap());
                    writer.WriteLE<u8>(wmg->GetScale());
                    writer.WriteLE<u8>(static_cast<u8>(wmg->GetLinkWithPlayer()));

                    writer.WriteLE<i16>(wmg->Width);
                    writer.WriteLE<i16>(wmg->Height);

                    writer.WriteLE<i16>(wmg->OffsetX);
                    writer.WriteLE<i16>(wmg->OffsetY);
                }

                writer.WriteBuffer();
                count++;

                break;
            }
            case GT_MENUBAR:
            case GT_COMBAT_BOOK:
            case GT_RACIAL_ABILITIES_BOOK:
            {
                SaveDefaultGumpProperties(writer, gump, 12);

                writer.WriteBuffer();
                count++;

                break;
            }
            case GT_BUFF:
            {
                SaveDefaultGumpProperties(writer, gump, 14);

                writer.WriteLE<u16>(gump->Graphic);

                writer.WriteBuffer();
                count++;

                break;
            }
            case GT_CONTAINER:
            case GT_SPELLBOOK:
            case GT_SPELL:
            {
                if (gump->GumpType != GT_SPELL)
                {
                    CGameObject *topobj = g_World->FindWorldObject(gump->Serial);

                    if (topobj == nullptr || ((CGameItem *)topobj)->Layer == OL_BANK)
                    {
                        break;
                    }

                    topobj = topobj->GetTopObject();

                    if (topobj->Serial != g_PlayerSerial)
                    {
                        break;
                    }

                    containerList.push_back(gump);
                    break;
                }
                if (((CGumpSpell *)gump)->InGroup())
                {
                    spellInGroupList.push_back(gump);
                    break;
                }

                SaveDefaultGumpProperties(writer, gump, 19);

                writer.WriteLE<u32>(gump->Serial);
                writer.WriteLE<u16>(gump->Graphic);
                writer.WriteLE<u8>(((CGumpSpell *)gump)->SpellType);
                writer.WriteBuffer();

                count++;

                break;
            }
            case GT_CONSOLE_TYPE:
            {
                SaveDefaultGumpProperties(writer, gump, 13);

                writer.WriteLE<u8>(
                    static_cast<u8>(((CGumpConsoleType *)gump)->GetShowFullText()));

                writer.WriteBuffer();
                count++;
            }
            case GT_SKILL:
            case GT_ABILITY:
            case GT_RACIAL_ABILITY:
            {
                SaveDefaultGumpProperties(writer, gump, 16);

                writer.WriteLE<u32>(gump->Serial);
                writer.WriteBuffer();

                count++;

                break;
            }
            default:
                break;
        }
    }

    std::vector<u32> playerContainers;
    playerContainers.push_back(g_PlayerSerial);

    while (!playerContainers.empty() && !containerList.empty())
    {
        u32 containerSerial = playerContainers.front();
        playerContainers.erase(playerContainers.begin());

        for (std::vector<CGump *>::iterator it = containerList.begin(); it != containerList.end();)
        {
            CGump *gump = *it;

            if (gump->Serial == containerSerial)
            {
                SaveDefaultGumpProperties(writer, gump, 16);

                writer.WriteLE<u32>(gump->Serial);
                writer.WriteBuffer();

                count++;

                it = containerList.erase(it);

                break;
            }
            {
                ++it;
            }
        }

        CGameObject *owner = g_World->FindWorldObject(containerSerial);

        if (owner != nullptr)
        {
            QFOR(item, owner->m_Items, CGameItem *)
            {
                if (item->Opened)
                {
                    playerContainers.push_back(item->Serial);
                }
            }
        }
    }

    int spellGroupsCount = 0;

    if (static_cast<unsigned int>(!spellInGroupList.empty()) != 0u)
    {
        std::vector<CGump *> spellGroups;

        while (static_cast<unsigned int>(!spellInGroupList.empty()) != 0u)
        {
            CGumpSpell *spell = (CGumpSpell *)spellInGroupList[0];
            CGumpSpell *topSpell = spell->GetTopSpell();
            spellGroups.push_back(topSpell);
            spellGroupsCount++;

            for (spell = topSpell; spell != nullptr; spell = spell->m_GroupNext)
            {
                for (std::vector<CGump *>::iterator it = spellInGroupList.begin();
                     it != spellInGroupList.end();
                     ++it)
                {
                    if (*it == spell)
                    {
                        spellInGroupList.erase(it);
                        break;
                    }
                }
            }
        }

        for (int i = 0; i < spellGroupsCount; i++)
        {
            CGumpSpell *spell = (CGumpSpell *)spellGroups[i];
            int spellsCount = 0;

            for (CGumpSpell *spell = (CGumpSpell *)spellGroups[i]; spell != nullptr;
                 spell = spell->m_GroupNext)
            {
                spellsCount++;
            }

            writer.WriteLE<i16>(spellsCount);

            for (CGumpSpell *spell = (CGumpSpell *)spellGroups[i]; spell != nullptr;
                 spell = spell->m_GroupNext)
            {
                SaveDefaultGumpProperties(writer, spell, 19);

                writer.WriteLE<u32>(spell->Serial);
                writer.WriteLE<u16>(spell->Graphic);
                writer.WriteLE<u8>(spell->SpellType);
                writer.WriteBuffer();
            }
        }
    }

    writer.WriteLE<i16>(spellGroupsCount);
    writer.WriteLE<i16>(count);
    writer.WriteLE<u32>(0); //EOF
    writer.WriteBuffer();

    Core::File file(a_path, "wb");
    file.Write(writer.GetBuffer(), writer.GetSize(), 1);
}
