#include "GumpSecureTrading.h"
#include "GameVars.h"
#include "Globals.h"
#include "../Config.h"
#include "../OrionUO.h"
#include "../Target.h"
#include "../PressedObject.h"
#include "../SelectedObject.h"
#include "../ClickObject.h"
#include "../Managers/FontsManager.h"
#include "../Managers/MouseManager.h"
#include "../GameObjects/GameWorld.h"
#include "../GameObjects/ObjectOnCursor.h"
#include "../GameObjects/GamePlayer.h"
#include "../Network/Packets.h"

CGumpSecureTrading::CGumpSecureTrading(u32 serial, short x, short y, u32 id, u32 id2)
    : CGump(GT_TRADE, serial, x, y)
    , ID2(id2)
{
    ID = id;
}

CGumpSecureTrading::~CGumpSecureTrading()
{
}

void CGumpSecureTrading::CalculateGumpState()
{
    CGump::CalculateGumpState();

    if (g_GumpPressed && g_PressedObject.LeftObject != nullptr &&
        g_PressedObject.LeftObject->IsText())
    {
        g_GumpMovingOffset.set(0, 0);

        g_GumpTranslate.x = (float)m_X;
        g_GumpTranslate.y = (float)m_Y;
    }

    if ((g_GumpTranslate.x != 0.0f) || (g_GumpTranslate.y != 0.0f))
    {
        WantRedraw = true;
    }
}

void CGumpSecureTrading::PrepareContent()
{
    if (m_MyCheck != nullptr)
    {
        if (StateMy)
        {
            if (m_MyCheck->Graphic != 0x0869)
            {
                m_MyCheck->Graphic = 0x0869;
                m_MyCheck->GraphicSelected = 0x086A;
                m_MyCheck->GraphicPressed = 0x086A;
                WantRedraw = true;
            }
        }
        else if (m_MyCheck->Graphic != 0x0867)
        {
            m_MyCheck->Graphic = 0x0867;
            m_MyCheck->GraphicSelected = 0x0868;
            m_MyCheck->GraphicPressed = 0x0868;
            WantRedraw = true;
        }
    }

    if (m_OpponentCheck != nullptr)
    {
        if (StateOpponent)
        {
            if (m_OpponentCheck->Graphic != 0x0869)
            {
                m_OpponentCheck->Graphic = 0x0869;
                WantRedraw = true;
            }
        }
        else if (m_OpponentCheck->Graphic != 0x0867)
        {
            m_OpponentCheck->Graphic = 0x0867;
            WantRedraw = true;
        }
    }

    if (m_TextRenderer.CalculatePositions(false))
    {
        WantRedraw = true;
    }
}

void CGumpSecureTrading::UpdateContent()
{
    CGameObject *selobj = g_World->FindWorldObject(Serial);

    if (selobj == nullptr)
    {
        return; //Объект, к которому привязан гамп - исчез
    }

    if (m_Items == nullptr)
    {
        Add(new CGUIGumppic(0x0866, 0, 0)); //Trade Gump

        if (GameVars::GetClientVersion() < CV_500A)
        {
            Add(new CGUIColoredPolygone(0, 0, 45, 90, 110, 60, 0xFF000001));

            Add(new CGUIColoredPolygone(0, 0, 192, 70, 110, 60, 0xFF000001));
        }

        if (StateMy)
        {
            m_MyCheck =
                (CGUIButton *)Add(new CGUIButton(ID_GST_CHECKBOX, 0x0869, 0x086A, 0x086A, 52, 29));
        }
        else
        {
            m_MyCheck =
                (CGUIButton *)Add(new CGUIButton(ID_GST_CHECKBOX, 0x0867, 0x0868, 0x0868, 52, 29));
        }

        CGUIText *text = (CGUIText *)Add(new CGUIText(0x0386, 84, 40));
        text->CreateTextureA(1, g_Player->GetName());

        if (StateOpponent)
        {
            m_OpponentCheck = (CGUIGumppic *)Add(new CGUIGumppic(0x0869, 266, 160));
        }
        else
        {
            m_OpponentCheck = (CGUIGumppic *)Add(new CGUIGumppic(0x0867, 266, 160));
        }

        int fontWidth = 260 - g_FontManager.GetWidthA(1, Text);

        text = (CGUIText *)Add(new CGUIText(0x0386, fontWidth, 170));
        text->CreateTextureA(1, Text);

        Add(new CGUIShader(&g_ColorizerShader, true));

        Add(new CGUIScissor(true, 0, 0, 45, 70, 110, 80));
        m_MyDataBox = (CGUIDataBox *)Add(new CGUIDataBox());
        Add(new CGUIScissor(false));

        Add(new CGUIScissor(true, 0, 0, 192, 70, 110, 80));
        m_OpponentDataBox = (CGUIDataBox *)Add(new CGUIDataBox());
        Add(new CGUIScissor(false));

        Add(new CGUIShader(&g_ColorizerShader, false));
    }
    else
    {
        m_MyDataBox->Clear();
        m_OpponentDataBox->Clear();
    }

    //Отрисовка нашего товара (при наличии товара)
    CGameObject *container = g_World->FindWorldObject(ID);

    if (container != nullptr && container->m_Items != nullptr)
    {
        QFOR(item, container->m_Items, CGameItem *)
        {
            bool doubleDraw = false;
            u16 graphic = item->GetDrawGraphic(doubleDraw);

            CGUITilepicHightlighted *dataObject =
                (CGUITilepicHightlighted *)m_MyDataBox->Add(new CGUITilepicHightlighted(
                    item->Serial,
                    graphic,
                    item->Color & 0x3FFF,
                    0x0035,
                    45 + item->GetX(),
                    70 + item->GetY(),
                    doubleDraw));
            dataObject->PartialHue = IsPartialHue(g_Orion.GetStaticFlags(graphic));

            if (dataObject->GetY() >= 150)
            {
                dataObject->SetY(120);
            }

            if (dataObject->GetX() >= 155)
            {
                dataObject->SetX(125);
            }
        }
    }

    //Отрисовка нашего опонента (при наличии товара)
    container = g_World->FindWorldObject(ID2);
    if (container != nullptr && container->m_Items != nullptr)
    {
        QFOR(item, container->m_Items, CGameItem *)
        {
            bool doubleDraw = false;
            u16 graphic = item->GetDrawGraphic(doubleDraw);

            CGUITilepicHightlighted *dataObject =
                (CGUITilepicHightlighted *)m_OpponentDataBox->Add(new CGUITilepicHightlighted(
                    item->Serial,
                    graphic,
                    item->Color & 0x3FFF,
                    0x0035,
                    192 + item->GetX(),
                    70 + item->GetY(),
                    doubleDraw));
            dataObject->PartialHue = IsPartialHue(g_Orion.GetStaticFlags(graphic));

            if (dataObject->GetY() >= 150)
            {
                dataObject->SetY(120);
            }

            if (dataObject->GetX() >= 302)
            {
                dataObject->SetX(272);
            }
        }
    }
}

void CGumpSecureTrading::Draw()
{
    CGameObject *selobj = g_World->FindWorldObject(Serial);

    if (selobj == nullptr)
    {
        return; //Объект, к которому привязан гамп - исчез
    }

    if (g_GumpPressed)
    {
        WantRedraw = true;
    }

    CGump::Draw();

    glTranslatef(g_GumpTranslate.x, g_GumpTranslate.y, 0.0f);

    g_FontColorizerShader.Use();

    m_TextRenderer.Draw();

    UnuseShader();

    glTranslatef(-g_GumpTranslate.x, -g_GumpTranslate.y, 0.0f);
}

CRenderObject *CGumpSecureTrading::Select()
{
    CGameObject *selobj = g_World->FindWorldObject(Serial);

    if (selobj == nullptr)
    {
        return nullptr; //Объект, к которому привязан гамп - исчез
    }

    CRenderObject *selected = CGump::Select();

    Core::TMousePos oldPos = g_MouseManager.GetPosition();
    g_MouseManager.SetPosition(oldPos - Core::TMousePos(g_GumpTranslate.x, g_GumpTranslate.y));
    m_TextRenderer.Select(this);
    g_MouseManager.SetPosition(oldPos);

    return selected;
}

void CGumpSecureTrading::GUMP_BUTTON_EVENT_C
{
    if (serial == ID_GST_CHECKBOX) //Изменение состояния чекбокса
    {
        StateMy = !StateMy;

        SendTradingResponse(2);
    }
    else
    {
        if (!g_ClickObject.Enabled)
        {
            CGameObject *clickTarget = g_World->FindWorldObject(serial);

            if (clickTarget == nullptr)
            {
                return;
            }

            g_ClickObject.Init(clickTarget);
            g_ClickObject.Timer = g_Ticks + DCLICK_DELAY;
            g_ClickObject.X = g_MouseManager.GetPosition().x - m_X;
            g_ClickObject.Y = g_MouseManager.GetPosition().y - m_Y;
        }
    }
}

void CGumpSecureTrading::OnLeftMouseButtonUp()
{
    CGump::OnLeftMouseButtonUp();

    if (g_ObjectInHand.Enabled)
    {
        int x = m_X;
        int y = m_Y;

        if (g_Orion.PolygonePixelsInXY(x + 45, y + 70, 110, 80))
        {
            //if (GetTopObjDistance(g_Player, g_World->FindWorldObject(ID2)) <= DRAG_ITEMS_DISTANCE)
            {
                x = g_MouseManager.GetPosition().x - x - 45;
                y = g_MouseManager.GetPosition().y - y - 70;

                bool doubleDraw = false;
                u16 graphic = g_ObjectInHand.GetDrawGraphic(doubleDraw);

                CGLTexture *th = g_Orion.ExecuteStaticArt(graphic);

                if (th != nullptr)
                {
                    x -= (th->Width / 2);
                    y -= (th->Height / 2);

                    if (x + th->Width > 110)
                    {
                        x = 110 - th->Width;
                    }

                    if (y + th->Height > 80)
                    {
                        y = 80 - th->Height;
                    }
                }

                if (x < 0)
                {
                    x = 0;
                }

                if (y < 0)
                {
                    y = 0;
                }

                g_Orion.DropItem(ID, x, y, 0);
                g_MouseManager.CancelDoubleClick = true;
            }
            //else
            //	g_Orion.PlaySoundEffect(0x0051);
        }
    }
    else if (
        g_Target.IsTargeting() && g_SelectedObject.Serial >= 0x40000000 &&
        g_World->FindWorldObject(g_SelectedObject.Serial) != nullptr)
    {
        g_Target.SendTargetObject(g_SelectedObject.Serial);
        g_MouseManager.CancelDoubleClick = true;
    }
}

void CGumpSecureTrading::SendTradingResponse(int code)
{
    //Ответ на трэйд окно
    CPacketTradeResponse(this, code).Send();

    if (code == 1)
    { //Закрываем окно
        RemoveMark = true;
    }
}
