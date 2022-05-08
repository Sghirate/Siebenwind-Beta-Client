#include "GUISlider.h"
#include "Globals.h"
#include "OrionUO.h"
#include "SelectedObject.h"
#include "Managers/FontsManager.h"
#include "Managers/MouseManager.h"
#include "Gumps/Gump.h"

CGUISlider::CGUISlider(
    int serial,
    u16 graphic,
    u16 graphicSelected,
    u16 graphicPressed,
    u16 backgroundGraphic,
    bool compositeBackground,
    bool vertical,
    int x,
    int y,
    int length,
    int minValue,
    int maxValue,
    int value)
    : CBaseGUI(GOT_SLIDER, serial, graphic, 0, x, y)
    , GraphicSelected(graphicSelected)
    , GraphicPressed(graphicPressed)
    , BackgroundGraphic(backgroundGraphic)
    , CompositeBackground(compositeBackground)
    , Vertical(vertical)
    , Length(length)
    , MinValue(minValue)
    , MaxValue(maxValue)
    , Value(value)
{
    CalculateOffset();
}

CGUISlider::~CGUISlider()
{
    Text.Clear();
}

void CGUISlider::UpdateText()
{
    if (HaveText)
    {
        if (Unicode)
        {
            g_FontManager.GenerateW(
                Font, Text, std::to_wstring(Value), TextColor, 30, TextWidth, Align, TextFlags);
        }
        else
        {
            g_FontManager.GenerateA(
                Font, Text, std::to_string(Value), TextColor, TextWidth, Align, TextFlags);
        }

        CGLTexture* th = g_Orion.ExecuteGump(Graphic);

        if (th != nullptr)
        {
            int textX = m_X;
            int textY = m_Y;

            switch (TextPosition)
            {
                case STP_TOP:
                case STP_TOP_CENTER:
                {
                    textY -= Text.Height - DefaultTextOffset;

                    break;
                }
                case STP_BOTTOM:
                case STP_BOTTOM_CENTER:
                {
                    if (Vertical)
                    {
                        textY += Length + DefaultTextOffset;
                    }
                    else
                    {
                        textY += th->Height + DefaultTextOffset;
                    }

                    break;
                }
                case STP_LEFT:
                    //textY -= 4;
                case STP_LEFT_CENTER:
                {
                    textX -= Text.Width - DefaultTextOffset;

                    break;
                }
                case STP_RIGHT:
                    //textY -= 4;
                case STP_RIGHT_CENTER:
                {
                    if (Vertical)
                    {
                        textX += th->Width + DefaultTextOffset;
                    }
                    else
                    {
                        textX += Length + DefaultTextOffset;
                    }

                    break;
                }
                default: break;
            }

            switch (TextPosition)
            {
                case STP_TOP_CENTER:
                case STP_BOTTOM_CENTER:
                {
                    int textWidth   = Text.Width;
                    int sliderWidth = Length;

                    if (Vertical)
                    {
                        sliderWidth = th->Width;
                    }

                    int deltaX = abs(sliderWidth - textWidth) / 2;

                    if (sliderWidth > textWidth)
                    {
                        textX += deltaX;
                    }
                    else
                    {
                        textX -= deltaX;
                    }

                    break;
                }
                case STP_LEFT_CENTER:
                case STP_RIGHT_CENTER:
                {
                    int textHeight   = Text.Height;
                    int sliderHeight = Length;

                    if (!Vertical)
                    {
                        sliderHeight = th->Height;
                    }

                    int deltaY = abs(sliderHeight - textHeight) / 2;

                    if (sliderHeight > textHeight)
                    {
                        textY += deltaY;
                    }
                    else
                    {
                        textY -= deltaY;
                    }

                    break;
                }
                default: break;
            }

            TextX = textX;
            TextY = textY;
        }
    }
}

Core::Vec2<i32> CGUISlider::GetSize()
{
    Core::Vec2<i32> size;
    CGLTexture* th = g_Orion.ExecuteGump(Graphic);
    if (th != nullptr)
    {
        if (Vertical)
        {
            size.x = th->Width;
            size.y = Length;
        }
        else
        {
            size.x = Length;
            size.y = th->Height;
        }
        if (HaveText)
        {
            //Text.Draw(TextX, TextY, checktrans);
        }
    }
    return size;
}

void CGUISlider::OnScroll(bool up, int delay)
{
    if (LastScrollTime < g_Ticks)
    {
        if (up)
            Value += ScrollStep;
        else
            Value -= ScrollStep;
        if (Value < MinValue)
            Value = MinValue;
        else if (Value > MaxValue)
            Value = MaxValue;
        LastScrollTime = g_Ticks + delay;
        CalculateOffset();
        UpdateText();
    }
}

void CGUISlider::OnClick(int x, int y)
{
    int length     = Length;
    int maxValue   = MaxValue - MinValue;
    CGLTexture* th = g_Orion.ExecuteGump(Graphic);
    if (th != nullptr)
        length -= (Vertical ? (th->Height / 2) : th->Width);
    float percents = ((Vertical ? y : x) / (float)length) * 100.0f;
    Value          = (int)((maxValue * percents) / 100.0f) + MinValue;
    CalculateOffset();
    UpdateText();
}

void CGUISlider::CalculateOffset()
{
    if (Value < MinValue)
        Value = MinValue;
    else if (Value > MaxValue)
        Value = MaxValue;

    int value      = Value - MinValue;
    int maxValue   = MaxValue - MinValue;
    int length     = Length;
    CGLTexture* th = g_Orion.ExecuteGump(Graphic);

    if (th != nullptr)
        length -= (Vertical ? th->Height : th->Width);

    if (maxValue > 0)
        Percents = ((value / (float)maxValue) * 100.0f);
    else
        Percents = 0.0f;

    Offset = (int)((length * Percents) / 100.0f);
    if (Offset < 0)
        Offset = 0;
}

void CGUISlider::SetTextParameters(
    bool haveText,
    SLIDER_TEXT_POSITION textPosition,
    u8 font,
    u16 color,
    bool unicode,
    int textWidth,
    TEXT_ALIGN_TYPE align,
    u16 textFlags)
{
    HaveText     = haveText;
    TextPosition = textPosition;
    Font         = font;
    TextColor    = color;
    Unicode      = unicode;
    TextWidth    = textWidth;
    Align        = align;
    TextFlags    = textFlags;

    UpdateText();
}

void CGUISlider::PrepareTextures()
{
    g_Orion.ExecuteGump(Graphic);
    g_Orion.ExecuteGump(GraphicSelected);
    g_Orion.ExecuteGump(GraphicPressed);
    if (BackgroundGraphic != 0u)
    {
        if (CompositeBackground)
            g_Orion.ExecuteGumpPart(BackgroundGraphic, 3);
        else
            g_Orion.ExecuteGump(BackgroundGraphic);
    }
}

u16 CGUISlider::GetDrawGraphic()
{
    u16 graphic = Graphic;
    if (g_GumpPressedElement == this)
        graphic = GraphicPressed;
    else if (g_GumpSelectedElement == this)
        graphic = GraphicSelected;
    return graphic;
}

void CGUISlider::Draw(bool checktrans)
{
    /*Value++;
	if (Value > MaxValue)
	{
		Value = MinValue;

		TextPosition = (SLIDER_TEXT_POSITION)(TextPosition + 1);

		if (TextPosition > STP_RIGHT)
			TextPosition = STP_TOP;
	}
	UpdateText();
	CalculateOffset();*/

    glUniform1iARB(g_ShaderDrawMode, SDM_NO_COLOR);

    if (BackgroundGraphic != 0u)
    {
        if (CompositeBackground)
        {
            CGLTexture* th[3] = { nullptr };

            for (int i = 0; i < 3; i++)
            {
                th[i] = g_Orion.ExecuteGump(BackgroundGraphic + (int)i);

                if (th[i] == nullptr)
                {
                    return;
                }
            }

            if (Vertical)
            {
                th[0]->Draw(m_X, m_Y, checktrans);
                th[2]->Draw(m_X, m_Y + (Length - th[2]->Height), checktrans);
                th[1]->Draw(
                    m_X,
                    m_Y + th[0]->Height,
                    0,
                    (Length - (th[0]->Height + th[2]->Height)),
                    checktrans);
            }
            else
            {
                th[0]->Draw(m_X, m_Y, checktrans);
                th[2]->Draw(m_X + (Length - th[2]->Width), m_Y, checktrans);
                th[1]->Draw(
                    m_X + th[0]->Width,
                    m_Y,
                    (Length - (th[0]->Width + th[2]->Width)),
                    0,
                    checktrans);
            }
        }
        else
        {
            CGLTexture* th = g_Orion.ExecuteGump(BackgroundGraphic);

            if (th != nullptr)
            {
                if (Vertical)
                {
                    th->Draw(m_X, m_Y, 0, Length, checktrans);
                }
                else
                {
                    th->Draw(m_X, m_Y, Length, 0, checktrans);
                }
            }
        }
    }

    CGLTexture* th = g_Orion.ExecuteGump(GetDrawGraphic());

    if (th != nullptr)
    {
        if (Vertical)
        {
            th->Draw(m_X, m_Y + Offset, checktrans);
        }
        else
        {
            th->Draw(m_X + Offset, m_Y, checktrans);
        }

        if (HaveText)
        {
            Text.Draw(TextX, TextY, checktrans);
        }
    }
}

bool CGUISlider::Select()
{
    CGLTexture* th = g_Orion.ExecuteGump(Graphic);
    if (th != nullptr)
    {
        int buttonX = m_X;
        int buttonY = m_Y;

        if (Vertical)
        {
            buttonY += Offset;
        }
        else
        {
            buttonX += Offset;
        }

        if (th->Select(buttonX, buttonY, !CheckPolygone))
        {
            return true;
        }

        if (BackgroundGraphic != 0u)
        {
            Core::TMousePos pos = g_MouseManager.GetPosition();
            int x               = pos.x - m_X;
            int y               = pos.y - m_Y;
            if (x >= 0 && y >= 0)
            {
                if (Vertical)
                {
                    return (x < th->Width && y < Length);
                }
                {
                    return (x < Length && y < th->Height);
                }
            }
        }
    }

    return false;
}

void CGUISlider::OnMouseEnter()
{
    if (g_SelectedObject.Gump != nullptr)
    {
        g_SelectedObject.Gump->WantRedraw = true;
    }
}

void CGUISlider::OnMouseExit()
{
    if (g_LastSelectedObject.Gump != nullptr)
    {
        g_LastSelectedObject.Gump->WantRedraw = true;
    }
}
