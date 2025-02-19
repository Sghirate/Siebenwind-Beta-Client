#include "GUIComboBox.h"
#include "GUIText.h"
#include "OrionUO.h"
#include "PressedObject.h"
#include "SelectedObject.h"
#include "Managers/MouseManager.h"

CGUIComboBox::CGUIComboBox(
    int serial,
    u16 graphic,
    bool compositeBackground,
    u16 openGraphic,
    int x,
    int y,
    int width,
    int showItemsCount,
    bool showMaximizedCenter)
    : CBaseGUI(GOT_COMBOBOX, serial, graphic, 0, x, y)
    , OpenGraphic(openGraphic)
    , CompositeBackground(compositeBackground)
    , m_ShowItemsCount(showItemsCount)
    , Width(width)
    , ShowMaximizedCenter(showMaximizedCenter)
    , OpenedWidth(width)
{
    MoveOnDrag        = false;
    m_ArrowX          = 0;
    m_OffsetY         = 0;
    m_StepY           = 0;
    m_MinimizedArrowX = Width - 16;
    m_WorkWidth       = Width - 6;
    m_WorkHeight      = m_ShowItemsCount * 15;

    if (CompositeBackground)
    {
        CGLTexture* th = g_Orion.ExecuteGump(OpenGraphic);

        if (th != nullptr)
        {
            m_ArrowX  = th->Width - 24;
            m_OffsetY = th->Height;
        }

        th = g_Orion.ExecuteGump(OpenGraphic + 1);

        if (th != nullptr)
        {
            m_StepY     = th->Height;
            m_WorkWidth = th->Width - 12;
        }

        th = g_Orion.ExecuteGump(Graphic);

        if (th != nullptr && (Width == 0))
        {
            m_MinimizedArrowX = th->Width - 16;
        }
    }
}

CGUIComboBox::~CGUIComboBox()
{
    if (Text != nullptr)
    {
        delete Text;
        Text = nullptr;
    }
}

void CGUIComboBox::RecalculateWidth()
{
    if (!CompositeBackground)
    {
        OpenedWidth = 0;

        QFOR(item, m_Items, CBaseGUI*)
        {
            if (item->Type == GOT_TEXT)
            {
                CGUIText* text = (CGUIText*)item;

                if (OpenedWidth < text->m_Texture.Width)
                {
                    OpenedWidth = text->m_Texture.Width;
                }
            }
        }

        OpenedWidth += 8;

        if (OpenedWidth < Width)
        {
            OpenedWidth = Width;
        }

        m_WorkWidth = OpenedWidth - 6;
    }
}

void CGUIComboBox::SetShowItemsCount(int val)
{
    m_WorkHeight     = val * 15;
    m_ShowItemsCount = val;
}

Core::Vec2<i32> CGUIComboBox::GetSize()
{
    return Core::Vec2<i32>(m_WorkWidth, m_WorkHeight);
}

void CGUIComboBox::PrepareTextures()
{
    if (CompositeBackground)
    {
        g_Orion.ExecuteGump(Graphic);
        g_Orion.ExecuteGump(0x0985);
        g_Orion.ExecuteGump(0x0983);
        g_Orion.ExecuteGumpPart(OpenGraphic, 5);
    }
    else
    {
        g_Orion.ExecuteResizepic(Graphic);
        g_Orion.ExecuteResizepic(OpenGraphic);
        g_Orion.ExecuteGump(0x00FC);
    }
}

CBaseGUI* CGUIComboBox::SkipToStart()
{
    CBaseGUI* start = (CBaseGUI*)m_Items;

    int index = 0;

    QFOR(item, m_Items, CBaseGUI*)
    {
        if (item->Type == GOT_TEXT)
        {
            if (index == StartIndex)
            {
                start = (CBaseGUI*)item;
                break;
            }

            index++;
        }
    }

    return start;
}

void CGUIComboBox::Draw(bool checktrans)
{
    if (Text != nullptr)
    {
        Text->m_Texture.Draw(m_X + Text->GetX(), m_Y + Text->GetY() + TextOffsetY, checktrans);
    }

    if (g_PressedObject.LeftObject == this) //maximized
    {
        int currentX = m_X + 3;
        int currentY = m_Y + 3;

        if (ShowMaximizedCenter)
        {
            currentY -= m_WorkHeight / 2;
        }

        if (CompositeBackground)
        {
            int bodyY    = m_Y + m_OffsetY;
            int bodyStep = m_StepY;

            currentX = m_X + 12;
            currentY = bodyY + 1;
            int posY = m_Y;

            if (ShowMaximizedCenter)
            {
                currentY -= m_WorkHeight / 2;
                posY -= m_WorkHeight / 2;
                bodyY -= m_WorkHeight / 2;
            }

            g_Orion.DrawGump(OpenGraphic, 0, m_X, posY);
            g_Orion.DrawGump(0x0983, 0, m_X + m_ArrowX, posY + 2);

            int graphicOffset = 0;

            for (int i = 0; i < m_ShowItemsCount; i++)
            {
                g_Orion.DrawGump(OpenGraphic + 1 + graphicOffset, 0, m_X + 5, bodyY);
                graphicOffset = (graphicOffset + 1) % 3;
                bodyY += bodyStep;
            }

            g_Orion.DrawGump(OpenGraphic + 4, 0, m_X, bodyY);
            g_Orion.DrawGump(0x0985, 0, m_X + m_ArrowX, bodyY);
        }
        else
        {
            g_Orion.DrawResizepicGump(OpenGraphic, m_X, m_Y, OpenedWidth, m_WorkHeight + 6);
        }

        g_GL.PushScissor(currentX, currentY, m_WorkWidth, m_WorkHeight);

        CBaseGUI* start = SkipToStart();
        int count       = 0;

        QFOR(item, start, CBaseGUI*)
        {
            if (item->Type == GOT_TEXT)
            {
                if (g_SelectedObject.Object == item)
                {
                    glColor4f(0.7f, 0.7f, 0.7f, 1.0f);
                    g_GL.DrawPolygone(currentX, currentY, m_WorkWidth, 14);
                    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
                }

                CGUIText* text = (CGUIText*)item;

                text->m_Texture.Draw(currentX, currentY + TextOffsetY);
                currentY += 15;

                count++;

                if (count > m_ShowItemsCount)
                {
                    break;
                }
            }
        }

        g_GL.PopScissor();
    }
    else
    {
        CGUIText* selected = nullptr;
        int index          = 0;

        QFOR(item, m_Items, CBaseGUI*)
        {
            if (item->Type == GOT_TEXT)
            {
                if (index == SelectedIndex)
                {
                    selected = (CGUIText*)item;
                    break;
                }

                index++;
            }
        }

        if (CompositeBackground)
        {
            if (Width != 0)
            {
                g_Orion.DrawGump(Graphic, 0, m_X, m_Y, Width, 0);
            }
            else
            {
                g_Orion.DrawGump(Graphic, 0, m_X, m_Y);
            }

            if (selected != nullptr)
            {
                g_GL.PushScissor(m_X + 6, m_Y, m_MinimizedArrowX, 20);
                selected->m_Texture.Draw(m_X + 6, m_Y + 6 + TextOffsetY);
                g_GL.PopScissor();
            }

            g_Orion.DrawGump(0x0985, 0, m_X + m_MinimizedArrowX, m_Y + 6);
        }
        else
        {
            g_Orion.DrawResizepicGump(Graphic, m_X, m_Y, Width, 20);

            if (selected != nullptr)
            {
                g_GL.PushScissor(m_X + 3, m_Y, Width - 6, 20);
                selected->m_Texture.Draw(m_X + 3, m_Y + 4 + TextOffsetY);
                g_GL.PopScissor();
            }

            g_Orion.DrawGump(0x00FC, 0, m_X + m_MinimizedArrowX, m_Y - 1);
        }
    }
}

bool CGUIComboBox::Select()
{
    ListingDirection = 0;
    bool select      = false;

    if (g_PressedObject.LeftObject == this) //maximized
    {
        int currentX = m_X + 3;
        int currentY = m_Y + 3;

        if (CompositeBackground)
        {
            currentY = m_Y + m_OffsetY + 1;
            currentX = m_X + 12;
        }

        if (ShowMaximizedCenter)
        {
            currentY -= m_WorkHeight / 2;
        }

        select = g_Orion.PolygonePixelsInXY(currentX, currentY, m_WorkWidth, m_WorkHeight);
        if (!select)
        {
            Core::TMousePos pos = g_MouseManager.GetPosition();
            if (pos.y < currentY)
                ListingDirection = 1;
            else if (pos.y > currentY + m_WorkHeight)
                ListingDirection = 2;
        }
    }
    else
    {
        if (CompositeBackground)
        {
            if (Width != 0)
            {
                select = g_Orion.GumpPixelsInXY(Graphic, m_X, m_Y, Width, 0);
            }
            else
            {
                select = g_Orion.GumpPixelsInXY(Graphic, m_X, m_Y);
            }
        }
        else
        {
            select = g_Orion.ResizepicPixelsInXY(Graphic, m_X, m_Y, Width, 20);
        }
    }

    return select;
}

CBaseGUI* CGUIComboBox::SelectedItem()
{
    CBaseGUI* select = this;

    if (g_PressedObject.LeftObject == this) //maximized
    {
        int currentX = m_X + 3;
        int currentY = m_Y + 3;

        if (CompositeBackground)
        {
            currentY = m_Y + m_OffsetY + 1;
            currentX = m_X + 12;
        }

        if (ShowMaximizedCenter)
        {
            currentY -= m_WorkHeight / 2;
        }

        CBaseGUI* start = SkipToStart();
        int count       = 0;

        QFOR(item, start, CBaseGUI*)
        {
            if (item->Type == GOT_TEXT)
            {
                if (g_Orion.PolygonePixelsInXY(currentX, currentY, m_WorkWidth, 14))
                {
                    select = item;
                    break;
                }

                currentY += 15;

                count++;

                if (count > m_ShowItemsCount)
                {
                    break;
                }
            }
        }
    }

    return select;
}

int CGUIComboBox::IsSelectedItem()
{
    int select = -1;

    if (g_PressedObject.LeftObject == this) //maximized
    {
        CBaseGUI* start = SkipToStart();
        int count       = 0;

        QFOR(item, start, CBaseGUI*)
        {
            if (item->Type == GOT_TEXT)
            {
                if (g_SelectedObject.Object == item)
                {
                    select = StartIndex + count;
                    break;
                }

                count++;

                if (count > m_ShowItemsCount)
                {
                    break;
                }
            }
        }
    }

    return select;
}
