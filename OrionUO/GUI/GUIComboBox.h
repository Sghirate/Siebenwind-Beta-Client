#pragma once

#include "Core/Minimal.h"
#include "BaseGUI.h"

class CGUIComboBox : public CBaseGUI
{
    u16 OpenGraphic          = 0;
    bool CompositeBackground = false;

protected:
    int m_ShowItemsCount = 0;

public:
    int GetShowItemsCount() { return m_ShowItemsCount; };
    void SetShowItemsCount(int val);

    int Width                = 0;
    int OpenedWidth          = 0;
    int StartIndex           = 0;
    int SelectedIndex        = -1;
    int ListingDirection     = 0;
    u32 ListingTimer         = 0;
    bool ShowMaximizedCenter = false;
    int TextOffsetY          = 0;

private:
    CBaseGUI* SkipToStart();

    int m_MinimizedArrowX{ 0 };
    int m_ArrowX{ 0 };
    int m_OffsetY{ 0 };
    int m_StepY{ 0 };
    int m_WorkWidth{ 0 };
    int m_WorkHeight{ 0 };
    class CGUIText* Text{ nullptr };

public:
    CGUIComboBox(
        int serial,
        u16 graphic,
        bool compositeBackground,
        u16 openGraphic,
        int x,
        int y,
        int width,
        int showItemsCount,
        bool showMaximizedCenter);
    virtual ~CGUIComboBox();

    void SetText(class CGUIText* text) { Text = text; }
    void RecalculateWidth();

    virtual Core::Vec2<i32> GetSize();
    virtual void PrepareTextures();
    virtual void Draw(bool checktrans = false);
    virtual bool Select();

    CBaseGUI* SelectedItem();
    int IsSelectedItem();
};
