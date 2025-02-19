#include "GumpSelectColor.h"
#include "GumpOptions.h"
#include "Managers/ColorManager.h"
#include "Managers/GumpManager.h"

CGumpSelectColor::CGumpSelectColor(u32 serial, short x, short y, SELECT_COLOR_GUMP_STATE state)
    : CGump(GT_SELECT_COLOR, serial, x, y)
    , m_State(state)
{
}

CGumpSelectColor::~CGumpSelectColor()
{
}

void CGumpSelectColor::UpdateContent()
{
    if (m_Items == nullptr)
    {
        Add(new CGUIGumppic(0x0906, 0, 0));

        Add(new CGUIButton(ID_GSC_BUTTON_OKAY, 0x0907, 0x0909, 0x0908, 208, 138));

        m_Slider = (CGUISlider *)Add(new CGUISlider(
            ID_GSC_SLIDER,
            0x0845,
            0x0846,
            0x0846,
            0,
            false,
            false,
            39,
            142,
            145,
            0,
            4,
            m_ColorRef));

        m_DataBox = (CGUIDataBox *)Add(new CGUIDataBox());
    }
    else
    {
        m_DataBox->Clear();
    }

    const int cellWidthX = 8;
    const int cellWidthY = 8;

    u16 startColor = m_ColorRef + 1;

    u8 *huesData = (u8 *)g_ColorManager.GetHuesRangePointer() + 32 + 4;
    const int colorOffsetDivider = sizeof(HUES_GROUP) - 4;

    for (int y = 0; y < 10; y++)
    {
        for (int x = 0; x < 20; x++)
        {
            int colorIndex = (startColor + ((startColor + (startColor << 2)) << 1)) << 3;

            colorIndex += (colorIndex / colorOffsetDivider) << 2;
            u16 color = *(u16 *)(huesData + colorIndex);

            u32 clr = g_ColorManager.Color16To32(color);

            u32 serial = ID_GSC_COLORS + ((int)x * 30 + (int)y);
            CGUIColoredPolygone *polygone =
                (CGUIColoredPolygone *)m_DataBox->Add(new CGUIColoredPolygone(
                    serial,
                    startColor,
                    34 + ((int)x * cellWidthX),
                    34 + ((int)y * cellWidthY),
                    cellWidthX,
                    cellWidthY,
                    clr));
            polygone->CallOnMouseUp = true;
            polygone->Focused = (m_SelectedIndex == serial);

            startColor += 5;
        }
    }
}

void CGumpSelectColor::GUMP_BUTTON_EVENT_C
{
    if (serial == ID_GSC_BUTTON_OKAY && m_DataBox != nullptr)
    {
        u16 color = 0;

        QFOR(item, m_DataBox->m_Items, CBaseGUI *)
        {
            if (item->Serial == m_SelectedIndex)
            {
                OnSelectColor(item->Color + 1);

                break;
            }
        }
    }
    else if (serial >= ID_GSC_COLORS)
    {
        m_SelectedIndex = serial;
        WantRedraw = true;

        QFOR(item, m_DataBox->m_Items, CBaseGUI *)
        ((CGUIColoredPolygone *)item)->Focused = (item->Serial == m_SelectedIndex);
    }
}

void CGumpSelectColor::GUMP_SLIDER_CLICK_EVENT_C
{
    OnSliderMove(serial);
}

void CGumpSelectColor::GUMP_SLIDER_MOVE_EVENT_C
{
    if (m_Slider != nullptr && m_ColorRef != m_Slider->Value)
    {
        m_ColorRef = m_Slider->Value;
        WantUpdateContent = true;
    }
}

void CGumpSelectColor::OnSelectColor(u16 color)
{
    CGumpOptions *gump = (CGumpOptions *)g_GumpManager.UpdateGump(0, 0, GT_OPTIONS);

    if (gump != nullptr)
    {
        gump->UpdateColor(m_State, color);
    }

    RemoveMark = true;
}
