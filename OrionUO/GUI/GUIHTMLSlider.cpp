// MIT License
// Copyright (C) August 2016 Hotride

#include "GUIHTMLSlider.h"
#include "GUIHTMLGump.h"

CGUIHTMLSlider::CGUIHTMLSlider(
    CGUIHTMLGump *htmlGump,
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
    : CGUISlider(
          serial,
          graphic,
          graphicSelected,
          graphicPressed,
          backgroundGraphic,
          compositeBackground,
          vertical,
          x,
          y,
          length,
          minValue,
          maxValue,
          value)
    , m_HTMLGump(htmlGump)
{
}

CGUIHTMLSlider::~CGUIHTMLSlider()
{
}

void CGUIHTMLSlider::CalculateOffset()
{
    CGUISlider::CalculateOffset();

    if (m_HTMLGump != nullptr)
    {
        Core::Vec2<i32> currentOffset = m_HTMLGump->CurrentOffset;
        Core::Vec2<i32> availableOffset = m_HTMLGump->AvailableOffset;

        if (Vertical)
        {
            currentOffset.y = (int)((availableOffset.y * Percents) / 100.0f);
        }
        else
        {
            currentOffset.x = (int)((availableOffset.x * Percents) / 100.0f);
        }

        m_HTMLGump->CurrentOffset = currentOffset;
    }
}
