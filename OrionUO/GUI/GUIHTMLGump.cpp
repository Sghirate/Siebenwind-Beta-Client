#include "GUIHTMLGump.h"
#include "GUIHTMLResizepic.h"
#include "GUIHTMLHitBox.h"
#include "GUIHTMLSlider.h"
#include "GUIHTMLButton.h"
#include "GUIScissor.h"
#include "OrionUO.h"
#include "Managers/MouseManager.h"

CGUIHTMLGump::CGUIHTMLGump(
    int serial,
    u16 graphic,
    int x,
    int y,
    int width,
    int height,
    bool haveBackground,
    bool haveScrollbar)
    : CGUIPolygonal(GOT_HTMLGUMP, x, y, width, height)
    , HaveBackground(haveBackground)
    , HaveScrollbar(haveScrollbar)

{
    Serial = serial;
    Graphic = graphic;

    Initalize();
}

CGUIHTMLGump::~CGUIHTMLGump()
{
}

void CGUIHTMLGump::PrepareTextures()
{
    QFOR(item, m_Items, CBaseGUI *)
    item->PrepareTextures();
}

void CGUIHTMLGump::Initalize(bool menu)
{
    Clear();

    if (menu)
    {
        m_Background = new CGUIHTMLResizepic(this, 0, 0, 0, 0, 1, 1);
        m_Background->Visible = false;

        m_HitBoxLeft = new CGUIHTMLHitBox(this, Serial, -15, -2, 12, 52);

        m_HitBoxRight = new CGUIHTMLHitBox(this, Serial, Width + 3, -2, 12, 52);
        m_HitBoxRight->Color = 1;

        m_Slider = new CGUIHTMLSlider(
            this,
            Serial,
            0x00D8,
            0x00D8,
            0x00D8,
            0x00D5,
            true,
            false,
            -10,
            Height + 1,
            Width + 20,
            0,
            0,
            0);
        m_Slider->ScrollStep = GUMP_MENU_PIXEL_STEP;
        m_Slider->Vertical = false;

        m_Scissor = new CGUIScissor(true, m_X, m_Y, 0, 0, Width, Height);

        Add(m_Background);
        Add(m_Slider);
        Add(m_HitBoxLeft);
        Add(m_HitBoxRight);
        Add(m_Scissor);
    }
    else
    {
        int offsetWidth = Width;

        if (HaveScrollbar)
        {
            CGLTexture *th = g_Orion.ExecuteGump(0x00FE);

            if (th != nullptr)
            {
                offsetWidth -= th->Width;
            }
        }

        m_Background = new CGUIHTMLResizepic(this, 0, Graphic, 0, 0, offsetWidth, Height);
        m_Background->Visible = HaveBackground;

        m_ButtonUp = new CGUIHTMLButton(this, Serial, 0x00FA, 0x00FA, 0x00FA, offsetWidth, 0);
        m_ButtonUp->Visible = HaveScrollbar;

        CGLTexture *thDown = g_Orion.ExecuteGump(0x00FC);

        int sliderHeight = Height;

        if (thDown != nullptr)
        {
            sliderHeight -= thDown->Height;
        }

        m_ButtonDown =
            new CGUIHTMLButton(this, Serial, 0x00FC, 0x00FC, 0x00FC, offsetWidth, sliderHeight);
        m_ButtonDown->Color = 1;
        m_ButtonDown->Visible = HaveScrollbar;

        CGLTexture *thUp = g_Orion.ExecuteGump(0x00FA);

        int sliderStartY = 0;

        if (thUp != nullptr)
        {
            sliderStartY = thUp->Height;
            sliderHeight -= sliderStartY;
        }

        m_Slider = new CGUIHTMLSlider(
            this,
            Serial,
            0x00FE,
            0x00FE,
            0x00FE,
            0x0100,
            false,
            true,
            offsetWidth,
            sliderStartY,
            sliderHeight,
            0,
            0,
            0);
        //m_Slider->SetTextParameters(true, STP_RIGHT_CENTER, 3, 0x0021, false);
        m_Slider->Visible = HaveScrollbar;

        m_Scissor = new CGUIScissor(true, m_X, m_Y, 0, 0, offsetWidth, Height);

        Add(m_Background);
        Add(m_Slider);
        Add(m_ButtonUp);
        Add(m_ButtonDown);
        Add(m_Scissor);
    }
}

void CGUIHTMLGump::UpdateHeight(int height)
{
    Height = height;

    m_Background->Height = height;

    CGLTexture *thDown = g_Orion.ExecuteGump(m_ButtonDown->Graphic);

    int sliderHeight = height;

    if (thDown != nullptr)
    {
        sliderHeight -= thDown->Height;
    }

    m_ButtonDown->SetY(sliderHeight);

    CGLTexture *thUp = g_Orion.ExecuteGump(m_ButtonUp->Graphic);

    int sliderStartY = 0;

    if (thUp != nullptr)
    {
        sliderStartY = thUp->Height;
        sliderHeight -= sliderStartY;
    }

    m_Slider->Length = sliderHeight;

    m_Scissor->Height = height;

    CalculateDataSize();
}

void CGUIHTMLGump::ResetDataOffset()
{
    m_Slider->Value = 0;
    CurrentOffset.set(0, 0);
}

void CGUIHTMLGump::CalculateDataSize(CBaseGUI *item, int &startX, int &startY, int &endX, int &endY)
{
    for (; item != nullptr; item = (CBaseGUI *)item->m_Next)
    {
        if (item->Type == GOT_HITBOX || !item->Visible)
        {
            continue;
        }
        if (item->Type == GOT_DATABOX)
        {
            CalculateDataSize((CBaseGUI *)item->m_Items, startX, startY, endX, endY);
            continue;
        }

        if (item->GetX() < startX)
            startX = item->GetX();
        if (item->GetY() < startY)
            startY = item->GetY();

        Core::Vec2<i32> size = item->GetSize();

        int curX = item->GetX() + size.x;
        int curY = item->GetY() + size.y;

        if (curX > endX)
            endX = curX;
        if (curY > endY)
            endY = curY;
    }
}

void CGUIHTMLGump::CalculateDataSize()
{
    CBaseGUI *item = (CBaseGUI *)m_Items;

    for (int i = 0; i < 5; i++)
    {
        item = (CBaseGUI *)item->m_Next;
    }

    int startX = 0;
    int startY = 0;
    int endX = 0;
    int endY = 0;

    CalculateDataSize(item, startX, startY, endX, endY);

    DataSize.x = abs(startX) + abs(endX);
    DataSize.y = abs(startY) + abs(endY);

    DataOffset.x = startX;
    DataOffset.y = startY;

    AvailableOffset.x = DataSize.x - m_Scissor->Width;

    if (AvailableOffset.x < 0)
    {
        AvailableOffset.x = 0;
    }

    AvailableOffset.y = DataSize.y - m_Scissor->Height;

    if (AvailableOffset.y < 0)
    {
        AvailableOffset.y = 0;
    }

    m_Slider->MinValue = 0;

    if (m_Slider->Vertical)
    {
        m_Slider->MaxValue = AvailableOffset.y;
    }
    else
    {
        m_Slider->MaxValue = AvailableOffset.x;
    }

    m_Slider->CalculateOffset();
}

bool CGUIHTMLGump::EntryPointerHere()
{
    QFOR(item, m_Items, CBaseGUI *)
    {
        if (item->Visible && item->EntryPointerHere())
            return true;
    }
    return false;
}

bool CGUIHTMLGump::Select()
{
    Core::TMousePos oldPos = g_MouseManager.GetPosition();
    g_MouseManager.SetPosition(oldPos - Core::TMousePos(m_X, m_Y));
    bool selected = false;
    CBaseGUI* item = (CBaseGUI*)m_Items;
    for (int i = 0; i < 5 && !selected; i++)
    {
        selected = item->Select();
        item = (CBaseGUI*)item->m_Next;
    }
    g_MouseManager.SetPosition(oldPos);
    return selected;
}

void CGUIHTMLGump::Scroll(bool up, int delay)
{
    if (m_Slider != nullptr)
        m_Slider->OnScroll(up, delay);
}
