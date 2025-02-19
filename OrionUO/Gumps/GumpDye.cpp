#include "GumpDye.h"
#include "Network/Packets.h"

CGumpDye::CGumpDye(u32 serial, i16 x, i16 y, u16 graphic)
    : CGumpSelectColor(serial, x, y, SCGS_OPT_TOOLTIP_TEXT)
{
    NoClose = true;
    GumpType = GT_DYE;
    Graphic = graphic;
}

CGumpDye::~CGumpDye()
{
}

u16 CGumpDye::GetCurrentColor()
{
    u16 startColor = m_ColorRef + 2;
    u16 color = 0;

    for (int y = 0; y < 10 && (color == 0u); y++)
    {
        for (int x = 0; x < 20; x++)
        {
            if (m_SelectedIndex == ID_GSC_COLORS + (x * 30 + y))
            {
                color = startColor;
                break;
            }

            startColor += 5;
        }
    }

    return color;
}

void CGumpDye::UpdateContent()
{
    CGumpSelectColor::UpdateContent();

    if (m_Tube == nullptr)
    {
        Add(new CGUIShader(&g_ColorizerShader, true));

        m_Tube = (CGUITilepic *)Add(new CGUITilepic(0x0FAB, GetCurrentColor(), 200, 58));
        m_Tube->PartialHue = true;

        Add(new CGUIShader(&g_ColorizerShader, false));
    }
    else
    {
        m_Tube->Color = GetCurrentColor();
    }
}

void CGumpDye::OnSelectColor(u16 color)
{
    CPacketDyeDataResponse(Serial, Graphic, color + 1).Send();
    RemoveMark = true;
}

void CGumpDye::GUMP_BUTTON_EVENT_C
{
    CGumpSelectColor::OnButton(serial);

    if (serial >= ID_GSC_COLORS && m_Tube != nullptr)
    {
        m_Tube->Color = GetCurrentColor();
    }
}

void CGumpDye::GUMP_SLIDER_CLICK_EVENT_C
{
    OnSliderMove(serial);
}

void CGumpDye::GUMP_SLIDER_MOVE_EVENT_C
{
    CGumpSelectColor::OnSliderMove(serial);

    if (m_Tube != nullptr)
    {
        m_Tube->Color = GetCurrentColor();
    }
}
