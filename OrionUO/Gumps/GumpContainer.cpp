#include "GumpContainer.h"
#include "Globals.h"
#include "GumpDrag.h"
#include "OrionUO.h"
#include "ToolTip.h"
#include "Target.h"
#include "PressedObject.h"
#include "SelectedObject.h"
#include "ClickObject.h"
#include "Container.h"
#include "Managers/MouseManager.h"
#include "Managers/ConfigManager.h"
#include "Managers/GumpManager.h"
#include "GameObjects/GameItem.h"
#include "GameObjects/GameWorld.h"
#include "GameObjects/ObjectOnCursor.h"
#include "GameObjects/GamePlayer.h"

const u32 CGumpContainer::ID_GC_LOCK_MOVING = 0xFFFFFFFE;
const u32 CGumpContainer::ID_GC_MINIMIZE = 0xFFFFFFFF;

CGumpContainer::CGumpContainer(u32 serial, u32 id, short x, short y)
    : CGump(GT_CONTAINER, serial, x, y)
    , IsGameBoard(id == 0x091A || id == 0x092E)
{
    Page = 1;
    m_Locker.Serial = ID_GC_LOCK_MOVING;
    ID = id;

    Add(new CGUIPage(1));
    Add(new CGUIGumppic(0x0050, 0, 0));

    Add(new CGUIPage(2));

    m_BodyGump = (CGUIGumppic *)Add(new CGUIGumppic((u16)ID, 0, 0));

    if (ID == 0x0009)
    {
        if (m_CorpseEyesTicks < g_Ticks)
        {
            m_CorpseEyesOffset = static_cast<u8>((u8)m_CorpseEyesOffset == 0u);
            m_CorpseEyesTicks = g_Ticks + 750;
        }

        m_CorpseEyes = (CGUIGumppic *)Add(new CGUIGumppic(0x0045, 45, 30));
    }

    if (ID == 0x003C)
    {
        CGUIHitBox *box = (CGUIHitBox *)Add(new CGUIHitBox(ID_GC_MINIMIZE, 106, 162, 16, 16, true));
        box->ToPage = 1;
    }

    Add(new CGUIShader(&g_ColorizerShader, true));

    m_DataBox = (CGUIDataBox *)Add(new CGUIDataBox());

    Add(new CGUIShader(&g_ColorizerShader, false));
}

CGumpContainer::~CGumpContainer()
{
}

void CGumpContainer::UpdateItemCoordinates(CGameObject *item)
{
    if (Graphic < g_ContainerOffset.size())
    {
        const CContainerOffsetRect &rect = g_ContainerOffset[Graphic].Rect;

        if (item->GetX() < rect.MinX)
        {
            item->SetX(rect.MinX);
        }

        if (item->GetY() < rect.MinY)
        {
            item->SetY(rect.MinY);
        }

        if (item->GetX() > rect.MinX + rect.MaxX)
        {
            item->SetX(rect.MinX + rect.MaxX);
        }

        if (item->GetY() > rect.MinY + rect.MaxY)
        {
            item->SetY(rect.MinY + rect.MaxY);
        }
    }
}

void CGumpContainer::CalculateGumpState()
{
    CGump::CalculateGumpState();

    if (g_GumpPressed && g_PressedObject.LeftObject != nullptr &&
        g_PressedObject.LeftObject->IsText())
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

void CGumpContainer::PrepareTextures()
{
    CGump::PrepareTextures();
    g_Orion.ExecuteGumpPart(0x0045, 2); //Corpse eyes
}

void CGumpContainer::InitToolTip()
{
    if (!Minimized)
    {
        if (g_SelectedObject.Serial == ID_GC_MINIMIZE)
        {
            g_ToolTip.Set(L"Minimize the container gump");
        }
        else if (g_SelectedObject.Serial == ID_GC_LOCK_MOVING)
        {
            g_ToolTip.Set(L"Lock moving/closing the container gump");
        }
    }
    else
    {
        g_ToolTip.Set(L"Double click to maximize container gump");
    }
}

void CGumpContainer::PrepareContent()
{
    if (!g_Player->Dead() &&
        GetTopObjDistance(g_Player, g_World->FindWorldObject(Serial)) <= DRAG_ITEMS_DISTANCE &&
        g_PressedObject.LeftGump == this && !g_ObjectInHand.Enabled &&
        g_PressedObject.LeftSerial != ID_GC_MINIMIZE &&
        g_MouseManager.LastLeftButtonClickTimer < g_Ticks)
    {
        Core::Vec2<i32> offset = g_MouseManager.GetLeftDroppedOffset();

        if (CanBeDraggedByOffset(offset) ||
            (g_MouseManager.LastLeftButtonClickTimer + g_MouseManager.DoubleClickDelay < g_Ticks))
        {
            CGameItem *selobj = g_World->FindWorldItem(g_PressedObject.LeftSerial);

            if (selobj != nullptr && selobj->IsStackable() && selobj->Count > 1 && !g_ShiftPressed)
            {
                CGumpDrag *newgump = new CGumpDrag(
                    g_PressedObject.LeftSerial,
                    g_MouseManager.GetPosition().x - 80,
                    g_MouseManager.GetPosition().y - 38);

                g_GumpManager.AddGump(newgump);
                g_MouseManager.EmulateOnLeftMouseButtonDown();
                selobj->Dragged = true;
            }
            else if (selobj != nullptr)
            {
                //if (g_Target.IsTargeting())
                //    g_Target.SendCancelTarget();

                g_Orion.PickupItem(selobj, 0, IsGameBoard);

                g_PressedObject.ClearLeft();

                WantRedraw = true;
            }
        }
    }

    if (ID == 0x09 && m_CorpseEyes != nullptr)
    {
        if (m_CorpseEyesTicks < g_Ticks)
        {
            m_CorpseEyesOffset = static_cast<u8>((u8)m_CorpseEyesOffset == 0u);
            m_CorpseEyesTicks = g_Ticks + 750;

            m_CorpseEyes->Graphic = 0x0045 + m_CorpseEyesOffset;
            WantRedraw = true;
        }
    }

    if (Minimized)
    {
        if (Page != 1)
        {
            Page = 1;
            WantUpdateContent = true;
        }
    }
    else
    {
        if (Page != 2)
        {
            Page = 2;
            WantUpdateContent = true;
        }

        if (m_TextRenderer.CalculatePositions(false))
        {
            WantRedraw = true;
        }
    }
}

void CGumpContainer::UpdateContent()
{
    CGameItem *container = g_World->FindWorldItem(Serial);

    if (container == nullptr)
    {
        return;
    }

    if ((u16)ID == 0x003C)
    {
        u16 graphic = (u16)ID;

        CGameItem *backpack = g_Player->FindLayer(OL_BACKPACK);

        if (backpack != nullptr && backpack->Serial == Serial)
        {
            switch (g_ConfigManager.GetCharacterBackpackStyle())
            {
                case CBS_SUEDE:
                    graphic = 0x775E;
                    break;
                case CBS_POLAR_BEAR:
                    graphic = 0x7760;
                    break;
                case CBS_GHOUL_SKIN:
                    graphic = 0x7762;
                    break;
                default:
                    graphic = 0x003C;
                    break;
            }

            if (g_Orion.ExecuteGump(graphic) == nullptr)
            {
                graphic = 0x003C;
            }

            m_BodyGump->Graphic = graphic;
        }
    }

    m_DataBox->Clear();

    IsGameBoard = (ID == 0x091A || ID == 0x092E);

    QFOR(obj, container->m_Items, CGameItem *)
    {
        int count = obj->Count;

        if ((obj->Layer == OL_NONE || (container->IsCorpse() && LAYER_UNSAFE[obj->Layer])) &&
            count > 0)
        {
            bool doubleDraw = false;
            u16 graphic = obj->GetDrawGraphic(doubleDraw);
            CGUIGumppicHightlighted *item = nullptr;

            if (IsGameBoard)
            {
                item = (CGUIGumppicHightlighted *)m_DataBox->Add(new CGUIGumppicHightlighted(
                    obj->Serial,
                    graphic - GAME_FIGURE_GUMP_OFFSET,
                    obj->Color & 0x3FFF,
                    0x0035,
                    obj->GetX(),
                    obj->GetY() - 20));
                item->PartialHue = false;
            }
            else
            {
                item = (CGUIGumppicHightlighted *)m_DataBox->Add(new CGUITilepicHightlighted(
                    obj->Serial,
                    graphic,
                    obj->Color & 0x3FFF,
                    0x0035,
                    obj->GetX(),
                    obj->GetY(),
                    doubleDraw));
                item->PartialHue = IsPartialHue(g_Orion.GetStaticFlags(graphic));
            }
        }
    }
}

void CGumpContainer::Draw()
{
    CGump::Draw();

    if (!Minimized)
    {
        glTranslatef(g_GumpTranslate.x, g_GumpTranslate.y, 0.0f);

        g_FontColorizerShader.Use();

        m_TextRenderer.Draw();

        UnuseShader();

        glTranslatef(-g_GumpTranslate.x, -g_GumpTranslate.y, 0.0f);
    }
}

CRenderObject *CGumpContainer::Select()
{
    CRenderObject *selected = CGump::Select();

    if (!Minimized)
    {
        Core::TMousePos oldPos = g_MouseManager.GetPosition();
        g_MouseManager.SetPosition(oldPos - Core::TMousePos(g_GumpTranslate.x, g_GumpTranslate.y));
        m_TextRenderer.Select(this);
        g_MouseManager.SetPosition(oldPos);
    }

    return selected;
}

void CGumpContainer::GUMP_BUTTON_EVENT_C
{
    if (!Minimized && serial == ID_GC_MINIMIZE && ID == 0x003C)
    {
        Minimized = true;
    }
    else if (serial == ID_GC_LOCK_MOVING)
    {
        LockMoving = !LockMoving;
        g_MouseManager.CancelDoubleClick = true;
    }
}

void CGumpContainer::OnLeftMouseButtonUp()
{
    CGump::OnLeftMouseButtonUp();

    u32 dropContainer = Serial;
    u32 selectedSerial = g_SelectedObject.Serial;

    if (g_Target.IsTargeting() && !g_ObjectInHand.Enabled && (selectedSerial != 0u) &&
        selectedSerial != ID_GC_MINIMIZE && selectedSerial != ID_GC_LOCK_MOVING)
    {
        g_Target.SendTargetObject(selectedSerial);
        g_MouseManager.CancelDoubleClick = true;

        return;
    }

    bool canDrop =
        (GetTopObjDistance(g_Player, g_World->FindWorldObject(dropContainer)) <=
         DRAG_ITEMS_DISTANCE);

    if (canDrop && (selectedSerial != 0u) && selectedSerial != ID_GC_MINIMIZE &&
        selectedSerial != ID_GC_LOCK_MOVING)
    {
        canDrop = false;

        if (g_ObjectInHand.Enabled)
        {
            canDrop = true;

            CGameItem *target = g_World->FindWorldItem(selectedSerial);

            if (target != nullptr)
            {
                if (target->IsContainer())
                {
                    dropContainer = target->Serial;
                }
                else if (target->IsStackable() && target->Graphic == g_ObjectInHand.Graphic)
                {
                    dropContainer = target->Serial;
                }
                else
                {
                    switch (target->Graphic)
                    {
                        case 0x0EFA:
                        case 0x2253:
                        case 0x2252:
                        case 0x238C:
                        case 0x23A0:
                        case 0x2D50:
                        {
                            dropContainer = target->Serial;
                            break;
                        }
                        default:
                            break;
                    }
                }
            }
        }
    }

    if (!canDrop && g_ObjectInHand.Enabled)
    {
        g_Orion.PlaySoundEffect(0x0051);
    }

    int x = g_MouseManager.GetPosition().x - m_X;
    int y = g_MouseManager.GetPosition().y - m_Y;

    if (canDrop && g_ObjectInHand.Enabled)
    {
        const CContainerOffsetRect &r = g_ContainerOffset[Graphic].Rect;

        bool doubleDraw = false;
        u16 graphic = g_ObjectInHand.GetDrawGraphic(doubleDraw);

        CGLTexture *th = g_Orion.ExecuteStaticArt(graphic);

        if (IsGameBoard)
        {
            th = g_Orion.ExecuteGump(graphic - GAME_FIGURE_GUMP_OFFSET);
            y += 20;
        }

        if (th != nullptr)
        {
            x -= (th->Width / 2);
            y -= (th->Height / 2);

            if (x + th->Width > r.MaxX)
            {
                x = r.MaxX - th->Width;
            }

            if (y + th->Height > r.MaxY)
            {
                y = r.MaxY - th->Height;
            }
        }

        if (x < r.MinX)
        {
            x = r.MinX;
        }

        if (y < r.MinY)
        {
            y = r.MinY;
        }

        if (dropContainer != Serial)
        {
            CGameItem *target = g_World->FindWorldItem(selectedSerial);

            if (target->IsContainer())
            {
                x = -1;
                y = -1;
            }
        }

        g_Orion.DropItem(dropContainer, x, y, 0);
        g_MouseManager.CancelDoubleClick = true;
    }
    else if (!g_ObjectInHand.Enabled)
    {
        if (!g_ClickObject.Enabled)
        {
            CGameObject *clickTarget = g_World->FindWorldObject(selectedSerial);

            if (clickTarget != nullptr)
            {
                g_ClickObject.Init(clickTarget);
                g_ClickObject.Timer = g_Ticks + g_MouseManager.DoubleClickDelay;
                g_ClickObject.X = x;
                g_ClickObject.Y = y;
            }
        }
    }
}

bool CGumpContainer::OnLeftMouseButtonDoubleClick()
{
    bool result = false;

    if ((g_PressedObject.LeftSerial == 0u) && Minimized && ID == 0x003C)
    {
        Minimized = false;
        Page = 2;
        WantUpdateContent = true;

        result = true;
    }
    else if ((g_PressedObject.LeftSerial != 0u) && g_PressedObject.LeftSerial != ID_GC_MINIMIZE)
    {
        g_Orion.DoubleClick(g_PressedObject.LeftSerial);
        FrameCreated = false;

        result = true;
    }

    return result;
}
