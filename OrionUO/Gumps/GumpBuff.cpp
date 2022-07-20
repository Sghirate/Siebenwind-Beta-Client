#include "GumpBuff.h"
#include "Globals.h"
#include "OrionUO.h"
#include "ToolTip.h"
#include "SelectedObject.h"
#include "Managers/FontsManager.h"
#include "Managers/ConfigManager.h"

CGumpBuff::CGumpBuff(short x, short y)
    : CGump(GT_BUFF, 0, x, y)
{
    Graphic         = 0x7580;
    m_Locker.Serial = ID_GB_LOCK_MOVING;

    Add(new CGUIGumppic(Graphic, 0, 0));
    Add(new CGUIHitBox(0, 0, 0, 0, 0));
    Add(new CGUIButton(ID_GB_NEXT_WINDOW_DIRECTION, 0x7585, 0x7589, 0x7589, 0, 0));
    Add(new CGUIAlphaBlending(true, 0.1f));
    Add(new CGUIAlphaBlending(false, 0.1f));
}

CGumpBuff::~CGumpBuff()
{
}

bool CGumpBuff::CanBeDisplayed()
{
    return g_ConfigManager.ToggleBufficonWindow;
}

void CGumpBuff::UpdateBuffIcons()
{
    for (CBaseGUI* item = (CBaseGUI*)m_Items; item != nullptr;)
    {
        CBaseGUI* next = (CBaseGUI*)item->m_Next;

        if (item->Type == GOT_BUFF)
        {
            CGUIBuff* buff = (CGUIBuff*)item;

            int delta = (int)(buff->Timer - g_Ticks);

            if (buff->Timer != 0xFFFFFFFF && delta < USE_ALPHA_BLENDING_WHEN_TIMER_LESS)
            {
                WantRedraw = true;

                if (delta <= 0)
                {
                    Delete(buff);
                    WantUpdateContent = true;
                }
                else
                {
                    int alpha  = buff->Alpha;
                    int addVal = (USE_ALPHA_BLENDING_WHEN_TIMER_LESS - delta) / ALPHA_CHANGE_KOEFF;

                    if (buff->DecAlpha)
                    {
                        alpha -= addVal;

                        if (alpha <= MUNIMUM_ICON_ALPHA)
                        {
                            buff->DecAlpha = false;
                            alpha          = MUNIMUM_ICON_ALPHA;
                        }
                    }
                    else
                    {
                        alpha += addVal;

                        if (alpha >= 255)
                        {
                            buff->DecAlpha = true;
                            alpha          = 255;
                        }
                    }

                    buff->Alpha = alpha;
                }
            }
        }

        item = next;
    }
}

void CGumpBuff::AddBuff(u16 id, u16 timer, const std::wstring& text)
{
    u32 ticks = 0xFFFFFFFF;
    if (timer != 0u)
    {
        ticks = g_Ticks + (timer * 1000);
    }

    QFOR(item, m_Items, CBaseGUI*)
    {
        if (item->Type == GOT_BUFF && item->Graphic == id)
        {
            CGUIBuff* buff = (CGUIBuff*)item;

            buff->Timer        = ticks;
            buff->Text         = text;
            buff->DecAlpha     = true;
            buff->Alpha        = 0xFF;
            buff->TooltipTimer = 0;

            MoveToBack(buff);
            WantUpdateContent = true;

            return;
        }
    }

    Add(new CGUIBuff(id, ticks, text));
    WantUpdateContent = true;
}

void CGumpBuff::DeleteBuff(u16 id)
{
    QFOR(item, m_Items, CBaseGUI*)
    {
        if (item->Type == GOT_BUFF && item->Graphic == id)
        {
            Delete(item);
            WantUpdateContent = true;

            break;
        }
    }
}

void CGumpBuff::InitToolTip()
{
    if (g_SelectedObject.Serial == ID_GB_NEXT_WINDOW_DIRECTION)
    {
        g_ToolTip.Set(L"Change buff window gump");
    }
    else if (g_SelectedObject.Object != nullptr)
    {
        QFOR(item, m_Items, CBaseGUI*)
        {
            if (item != g_SelectedObject.Object)
            {
                continue;
            }

            if (item->Type == GOT_BUFF)
            {
                CGUIBuff* buff = (CGUIBuff*)item;

                g_FontManager.SetUseHTML(true);

                if (buff->Timer != 0xFFFFFFFF && buff->TooltipTimer < g_Ticks)
                {
                    buff->TooltipTimer = g_Ticks + ((buff->Timer - g_Ticks) % 1000);
                    g_ToolTip.Reset();

                    wchar_t buf[512] = { 0 };
                    swprintf(
                        buf,
                        sizeof(buf),
                        L"%s\nTimeLeft: %i seconds.",
                        buff->Text.c_str(),
                        (buff->Timer - g_Ticks) / 1000); // FIXME: check windows

                    g_ToolTip.Set(buf);

                    g_ToolTip.Timer = 0;
                    g_ToolTip.Use   = true;
                }
                else
                {
                    g_ToolTip.Set(buff->Text);
                }

                g_FontManager.SetUseHTML(false);
            }
            else
            {
                g_ToolTip.Set(L"Buffs and Debuffs will appear here.", 120);
            }

            break;
        }
    }
}

void CGumpBuff::GetGumpStatus(
    Core::Vec2<i32>& ball,
    Core::Vec2<i32>& items,
    bool& useX,
    bool& decX,
    bool& decY,
    Core::Vec2<i32>& startGump,
    Core::Vec2<i32>& endGump)
{
    startGump.x = 0;
    startGump.y = 0;

    endGump = g_Orion.GetGumpDimension(Graphic);

    switch (Graphic)
    {
        case 0x757F: //v
        {
            ball.x  = 0;
            ball.y  = 0;
            items.x = 25;
            items.y = 25;
            decY    = false;
            decX    = false;
            useX    = false;
            break;
        }
        case 0x7581: //^
        {
            ball.x  = 34;
            ball.y  = 78;
            items.x = 7;
            items.y = 52;
            decY    = true;
            decX    = false;
            useX    = false;
            break;
        }
        case 0x7582: //<
        {
            ball.x  = 76;
            ball.y  = 36;
            items.x = 52;
            items.y = 7;
            decY    = false;
            decX    = true;
            useX    = true;
            break;
        }
        case 0x7580: //>
        default:
        {
            ball.x  = -2;
            ball.y  = 36;
            items.x = 20;
            items.y = 7;
            decY    = false;
            decX    = false;
            useX    = true;
            break;
        }
    }

    Core::Vec2<i32> itemsOfs = items;

    QFOR(item, m_Items, CBaseGUI*)
    {
        if (item->Type != GOT_BUFF)
        {
            continue;
        }

        bool moved = false;

        Core::Vec2<i32> gumpDim = g_Orion.GetGumpDimension(item->Graphic);

        if (useX)
        {
            if (decX)
            {
                itemsOfs.x -= gumpDim.x + BUFF_ITEM_STEP_OFFSET_X;
            }
            else
            {
                itemsOfs.x += gumpDim.x + BUFF_ITEM_STEP_OFFSET_X;
            }
        }
        else
        {
            if (decY)
            {
                itemsOfs.y -= gumpDim.y + BUFF_ITEM_STEP_OFFSET_Y;
            }
            else
            {
                itemsOfs.y += gumpDim.y + BUFF_ITEM_STEP_OFFSET_Y;
            }
        }
    }

    if (useX)
    {
        if (decX)
        {
            itemsOfs.x -= 20;
        }
        else
        {
            itemsOfs.x += 20;
        }
    }
    else
    {
        if (decY)
        {
            itemsOfs.y -= 20;
        }
        else
        {
            itemsOfs.y += 20;
        }
    }

    if (itemsOfs.x < startGump.x)
    {
        startGump.x = itemsOfs.x;
    }

    if (itemsOfs.y < startGump.y)
    {
        startGump.y = itemsOfs.y;
    }

    if (itemsOfs.x > endGump.x)
    {
        endGump.x = itemsOfs.x;
    }

    if (itemsOfs.y > endGump.y)
    {
        endGump.y = itemsOfs.y;
    }
}

void CGumpBuff::PrepareContent()
{
    if (Graphic < 0x757F || Graphic > 0x7582)
    {
        Graphic           = 0x7580;
        WantUpdateContent = true;
    }
}

void CGumpBuff::UpdateContent()
{
    bool decX = false;
    bool decY = false;
    bool useX = true;

    Core::Vec2<i32> ballCoordinates;
    Core::Vec2<i32> startCoordinates;
    Core::Vec2<i32> startGump;
    Core::Vec2<i32> endGump;

    GetGumpStatus(ballCoordinates, startCoordinates, useX, decX, decY, startGump, endGump);

    //Body
    CBaseGUI* gui = (CBaseGUI*)m_Items;
    gui->Graphic  = Graphic;

    //Selection zone
    gui = (CBaseGUI*)gui->m_Next;
    gui->SetX(startGump.x);
    gui->SetY(startGump.y);
    ((CGUIPolygonal*)gui)->Width  = endGump.x;
    ((CGUIPolygonal*)gui)->Height = endGump.y;

    //Crystall ball
    gui = (CBaseGUI*)gui->m_Next;
    gui->SetX(ballCoordinates.x);
    gui->SetY(ballCoordinates.y);

    gui = nullptr;

    QFOR(item, m_Items, CBaseGUI*)
    {
        if (item->Type != GOT_BUFF)
        {
            if (item->Type == GOT_BLENDING && !item->Enabled)
            {
                gui = item;
            }

            continue;
        }

        CGUIBuff* buff = (CGUIBuff*)item;

        Core::Vec2<i32> gumpDim = g_Orion.GetGumpDimension(buff->Graphic);
        buff->SetX(startCoordinates.x);
        buff->SetY(startCoordinates.y);

        if (useX)
        {
            if (decX)
            {
                startCoordinates.x -= gumpDim.x + BUFF_ITEM_STEP_OFFSET_X;
            }
            else
            {
                startCoordinates.x += gumpDim.x + BUFF_ITEM_STEP_OFFSET_X;
            }
        }
        else
        {
            if (decY)
            {
                startCoordinates.y -= gumpDim.y + BUFF_ITEM_STEP_OFFSET_Y;
            }
            else
            {
                startCoordinates.y += gumpDim.y + BUFF_ITEM_STEP_OFFSET_Y;
            }
        }
    }

    //Выключение прозрачности в задницу очереди
    if (gui != nullptr)
    {
        MoveToBack(gui);
    }
}

void CGumpBuff::GUMP_BUTTON_EVENT_C
{
    if (serial == ID_GB_NEXT_WINDOW_DIRECTION)
    {
        switch (Graphic)
        {
            case 0x7580:
            {
                Graphic = 0x7582;
                break;
            }
            case 0x7581:
            {
                Graphic = 0x757F;
                break;
            }
            case 0x7582:
            {
                Graphic = 0x7581;
                break;
            }
            case 0x757F:
            default:
            {
                Graphic = 0x7580;
                break;
            }
        }

        WantUpdateContent = true;
    }
    else if (serial == ID_GB_LOCK_MOVING)
    {
        LockMoving = !LockMoving;
    }
}
