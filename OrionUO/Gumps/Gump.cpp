#include "Gump.h"
#include "GameWindow.h"
#include "Globals.h"
#include "OrionUO.h"
#include "Profiler.h"
#include "PressedObject.h"
#include "SelectedObject.h"
#include "ClickObject.h"
#include "Managers/FontsManager.h"
#include "Managers/MouseManager.h"
#include "Managers/ConfigManager.h"
#include "GameObjects/ObjectOnCursor.h"
#include "ScreenStages/BaseScreen.h"
#include "ScreenStages/GameScreen.h"

CGump* g_ResizedGump      = nullptr;
CGump* g_CurrentCheckGump = nullptr;

CGump::CGump()
    : CGump(GT_NONE, 0, 0, 0)
{
}

CGump::CGump(GUMP_TYPE type, u32 serial, int x, int y)
    : CRenderObject(serial, 0, 0, x, y)
    , GumpType(type)
{
}

CGump::~CGump()
{
    if (Blocked)
    {
        g_GrayMenuCount--;
        if (g_GrayMenuCount <= 0)
        {
            g_GrayMenuCount = 0;
            g_GameState     = GS_GAME;
            g_CurrentScreen = &g_GameScreen;
        }
    }

    if (g_ClickObject.Gump == this)
    {
        g_ClickObject.Clear();
    }

    if (g_SelectedObject.Gump == this)
    {
        g_SelectedObject.Clear();
    }

    if (g_LastSelectedObject.Gump == this)
    {
        g_LastSelectedObject.Clear();
    }

    if (g_PressedObject.LeftGump == this)
    {
        g_PressedObject.ClearLeft();
    }
    if (g_PressedObject.RightGump == this)
    {
        g_PressedObject.ClearRight();
    }
    if (g_PressedObject.MidGump == this)
    {
        g_PressedObject.ClearMid();
    }
}

void CGump::GUMP_DIRECT_HTML_LINK_EVENT_C
{
    g_FontManager.GoToWebLink(link);
}

void CGump::FixCoordinates()
{
    const int gumpOffsetX = Core::Min(40, GumpRect.size.x);
    const int gumpOffsetY = Core::Min(40, GumpRect.size.y);
    int maxX              = g_gameWindow.GetSize().x - gumpOffsetX;
    int maxY              = g_gameWindow.GetSize().y - gumpOffsetY;

    if (Minimized && GumpType != GT_MINIMAP)
    {
        if (MinimizedX + GumpRect.pos.x > maxX)
        {
            WantRedraw = true;
            MinimizedX = maxX;
        }

        if (MinimizedX < GumpRect.pos.x &&
            MinimizedX + GumpRect.pos.x + GumpRect.size.x - gumpOffsetX < 0)
        {
            WantRedraw = true;
            MinimizedX = gumpOffsetX - (GumpRect.pos.x + GumpRect.size.x);
        }

        if (MinimizedY + GumpRect.pos.y > maxY)
        {
            WantRedraw = true;
            MinimizedY = maxY;
        }

        if (MinimizedY < GumpRect.pos.y &&
            MinimizedY + GumpRect.pos.y + GumpRect.size.y - gumpOffsetY < 0)
        {
            WantRedraw = true;
            MinimizedY = gumpOffsetY - (GumpRect.pos.y + GumpRect.size.y);
        }
    }
    else
    {
        if (m_X + GumpRect.pos.x > maxX)
        {
            WantRedraw = true;
            m_X        = maxX;
        }

        if (m_X < GumpRect.pos.x && m_X + GumpRect.pos.x + GumpRect.size.x - gumpOffsetX < 0)
        {
            WantRedraw = true;
            m_X        = gumpOffsetX - (GumpRect.pos.x + GumpRect.size.x);
        }

        if (m_Y + GumpRect.pos.y > maxY)
        {
            WantRedraw = true;
            m_Y        = maxY;
        }

        if (m_Y < GumpRect.pos.y && m_Y + GumpRect.pos.y + GumpRect.size.y - gumpOffsetY < 0)
        {
            WantRedraw = true;
            m_Y        = gumpOffsetY - (GumpRect.pos.y + GumpRect.size.y);
        }
    }
}

bool CGump::CanBeMoved()
{
    bool result = true;

    if (NoMove)
    {
        result = false;
    }
    else if (g_ConfigManager.LockGumpsMoving)
    {
        result = !LockMoving;
    }

    return result;
}

void CGump::DrawLocker()
{
    if ((m_Locker.Serial != 0u) && g_ShowGumpLocker)
    {
        g_TextureGumpState[LockMoving].Draw(m_Locker.GetX(), m_Locker.GetY());
    }
}

bool CGump::SelectLocker()
{
    return (
        (m_Locker.Serial != 0u) && g_ShowGumpLocker &&
        g_Orion.PolygonePixelsInXY(m_Locker.GetX(), m_Locker.GetY(), 10, 14));
}

bool CGump::TestLockerClick()
{
    bool result =
        ((m_Locker.Serial != 0u) && g_ShowGumpLocker && g_PressedObject.LeftObject == &m_Locker);

    if (result)
    {
        OnButton(m_Locker.Serial);
    }

    return result;
}

void CGump::CalculateGumpState()
{
    PROFILER_EVENT();
    g_GumpPressed =
        (!g_ObjectInHand.Enabled &&
         g_PressedObject.LeftGump == this /*&& g_SelectedObject.Gump() == this*/);
    g_GumpSelectedElement = ((g_SelectedObject.Gump == this) ? g_SelectedObject.Object : nullptr);
    g_GumpPressedElement  = nullptr;

    CRenderObject* leftObj = g_PressedObject.LeftObject;

    if (g_GumpPressed && leftObj != nullptr)
    {
        if (leftObj == g_SelectedObject.Object)
        {
            g_GumpPressedElement = leftObj;
        }
        else if (leftObj->IsGUI() && ((CBaseGUI*)leftObj)->IsPressedOuthit())
        {
            g_GumpPressedElement = leftObj;
        }
    }

    if (CanBeMoved() && g_GumpPressed && !g_ObjectInHand.Enabled &&
        ((g_PressedObject.LeftSerial == 0u) || g_GumpPressedElement == nullptr ||
         g_PressedObject.TestMoveOnDrag()))
    {
        g_GumpMovingOffset = g_MouseManager.GetLeftDroppedOffset();
    }
    else
    {
        g_GumpMovingOffset.set(0, 0);
    }

    if (Minimized)
    {
        g_GumpTranslate.x = (float)(MinimizedX + g_GumpMovingOffset.x);
        g_GumpTranslate.y = (float)(MinimizedY + g_GumpMovingOffset.y);
    }
    else
    {
        g_GumpTranslate.x = (float)(m_X + g_GumpMovingOffset.x);
        g_GumpTranslate.y = (float)(m_Y + g_GumpMovingOffset.y);
    }
}

void CGump::ProcessListing()
{
    PROFILER_EVENT();
    if (g_PressedObject.LeftGump != nullptr && !g_PressedObject.LeftGump->NoProcess &&
        g_PressedObject.LeftObject != nullptr && g_PressedObject.LeftObject->IsGUI())
    {
        CBaseGUI* item = (CBaseGUI*)g_PressedObject.LeftObject;

        if (item->IsControlHTML())
        {
            if (item->Type == GOT_BUTTON)
            {
                ((CGUIHTMLButton*)item)->Scroll(item->Color != 0, SCROLL_LISTING_DELAY / 7);
                g_PressedObject.LeftGump->WantRedraw = true;
                g_PressedObject.LeftGump->OnScrollButton();
            }
            else if (item->Type == GOT_HITBOX)
            {
                ((CGUIHTMLHitBox*)item)->Scroll(item->Color != 0, SCROLL_LISTING_DELAY / 7);
                g_PressedObject.LeftGump->WantRedraw = true;
                g_PressedObject.LeftGump->OnScrollButton();
            }
        }
        else if (item->Type == GOT_BUTTON && ((CGUIButton*)item)->ProcessPressedState)
        {
            g_PressedObject.LeftGump->WantRedraw = true;
            g_PressedObject.LeftGump->OnButton(item->Serial);
        }
        else if (item->Type == GOT_MINMAXBUTTONS)
        {
            ((CGUIMinMaxButtons*)item)->Scroll(SCROLL_LISTING_DELAY / 2);
            g_PressedObject.LeftGump->OnScrollButton();
            g_PressedObject.LeftGump->WantRedraw = true;
        }
        else if (item->Type == GOT_COMBOBOX)
        {
            CGUIComboBox* combo = (CGUIComboBox*)item;

            if (combo->ListingTimer < g_Ticks)
            {
                int index     = combo->StartIndex;
                int direction = combo->ListingDirection;

                if (direction == 1 && index > 0)
                {
                    index--;
                }
                else if (direction == 2 && index + 1 < combo->GetItemsCount())
                {
                    index++;
                }

                if (index != combo->StartIndex)
                {
                    if (index < 0)
                    {
                        index = 0;
                    }

                    combo->StartIndex                    = index;
                    g_PressedObject.LeftGump->WantRedraw = true;
                }

                combo->ListingTimer = g_Ticks + 50;
            }
        }
    }
}

bool CGump::ApplyTransparent(CBaseGUI* item, int page, int currentPage, const int draw2Page)
{
    bool transparent = false;

    glClear(GL_STENCIL_BUFFER_BIT);
    glEnable(GL_STENCIL_TEST);

    bool canDraw =
        ((page == -1) || ((page >= currentPage && page <= currentPage + draw2Page) ||
                          ((page == 0) && (draw2Page == 0))));

    for (; item != nullptr; item = (CBaseGUI*)item->m_Next)
    {
        if (item->Type == GOT_PAGE)
        {
            page = ((CGUIPage*)item)->Index;

            //if (page >= 2 && page > currentPage + draw2Page)
            //	break;

            canDraw =
                ((page == -1) || ((page >= currentPage && page <= currentPage + draw2Page) ||
                                  ((page == 0) && (draw2Page == 0))));
        }
        else if (canDraw && item->Visible && item->Type == GOT_CHECKTRANS)
        {
            item->Draw(transparent);

            transparent = true;
        }
    }

    glDisable(GL_STENCIL_TEST);

    return transparent;
}

void CGump::DrawItems(CBaseGUI* start, int currentPage, int draw2Page)
{
    PROFILER_EVENT();
    float alpha[2]      = { 1.0f, 0.7f };
    CGUIComboBox* combo = nullptr;

    bool transparent = ApplyTransparent(start, 0, currentPage, draw2Page);
    glColor4f(1.0f, 1.0f, 1.0f, alpha[transparent]);

    int page     = 0;
    bool canDraw = ((draw2Page == 0) || (page >= currentPage && page <= currentPage + draw2Page));

    QFOR(item, start, CBaseGUI*)
    {
        if (item->Type == GOT_PAGE)
        {
            page = ((CGUIPage*)item)->Index;

            //if (page >= 2 && page > currentPage + draw2Page)
            //	break;

            canDraw =
                ((page == -1) || ((page >= currentPage && page <= currentPage + draw2Page) ||
                                  ((page == 0) && (draw2Page == 0))));
        }
        else if (canDraw && item->Visible && !item->SelectOnly)
        {
            switch (item->Type)
            {
                case GOT_DATABOX:
                {
                    CGump::DrawItems((CBaseGUI*)item->m_Items, currentPage, draw2Page);
                    break;
                }
                case GOT_HTMLGUMP:
                case GOT_XFMHTMLGUMP:
                case GOT_XFMHTMLTOKEN:
                {
                    CGUIHTMLGump* htmlGump = (CGUIHTMLGump*)item;

                    GLfloat x = (GLfloat)htmlGump->GetX();
                    GLfloat y = (GLfloat)htmlGump->GetY();

                    glTranslatef(x, y, 0.0f);

                    CBaseGUI* item = (CBaseGUI*)htmlGump->m_Items;

                    for (int j = 0; j < 5; j++)
                    {
                        if (item->Visible && !item->SelectOnly)
                        {
                            item->Draw(false);
                        }

                        item = (CBaseGUI*)item->m_Next;
                    }

                    GLfloat offsetX = (GLfloat)(htmlGump->DataOffset.x - htmlGump->CurrentOffset.x);
                    GLfloat offsetY = (GLfloat)(htmlGump->DataOffset.y - htmlGump->CurrentOffset.y);

                    glTranslatef(offsetX, offsetY, 0.0f);

                    CGump::DrawItems(item, currentPage, draw2Page);
                    g_GL.PopScissor();

                    glTranslatef(-(x + offsetX), -(y + offsetY), 0.0f);

                    break;
                }
                case GOT_CHECKTRANS:
                {
                    transparent = ApplyTransparent(
                        (CBaseGUI*)item->m_Next, page /*Page*/, currentPage, draw2Page);

                    glColor4f(1.0f, 1.0f, 1.0f, alpha[transparent]);

                    break;
                }
                case GOT_COMBOBOX:
                {
                    if (g_PressedObject.LeftObject == item)
                    {
                        combo = (CGUIComboBox*)item;
                        break;
                    }
                }
                default:
                {
                    item->Draw(transparent);

                    break;
                }
            }
        }
    }

    if (combo != nullptr)
    {
        combo->Draw(false);
    }

    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}

CRenderObject* CGump::SelectItems(CBaseGUI* start, int currentPage, int draw2Page)
{
    PROFILER_EVENT();
    CRenderObject* selected = nullptr;

    int page     = 0;
    bool canDraw = ((draw2Page == 0) || (page >= currentPage && page <= currentPage + draw2Page));
    std::vector<bool> scissorList;
    bool currentScissorState = true;
    CGUIComboBox* combo      = nullptr;

    Core::TMousePos oldPos = g_MouseManager.GetPosition();

    QFOR(item, start, CBaseGUI*)
    {
        if (item->Type == GOT_PAGE)
        {
            page = ((CGUIPage*)item)->Index;

            //if (page >= 2 && page > currentPage + draw2Page)
            //	break;

            canDraw =
                ((page == -1) || ((page >= currentPage && page <= currentPage + draw2Page) ||
                                  ((page == 0) && (draw2Page == 0))));
        }
        else if (canDraw && item->Visible)
        {
            if (item->Type == GOT_SCISSOR)
            {
                if (item->Enabled)
                {
                    currentScissorState = item->Select();
                    scissorList.push_back(currentScissorState);
                }
                else
                {
                    scissorList.pop_back();

                    if (static_cast<unsigned int>(!scissorList.empty()) != 0u)
                    {
                        currentScissorState = scissorList.back();
                    }
                    else
                    {
                        currentScissorState = true;
                    }
                }

                continue;
            }
            if (!currentScissorState || !item->Enabled || (item->DrawOnly && selected != nullptr) ||
                !item->Select())
            {
                continue;
            }

            switch (item->Type)
            {
                case GOT_HTMLGUMP:
                case GOT_XFMHTMLGUMP:
                case GOT_XFMHTMLTOKEN:
                {
                    CGUIHTMLGump* htmlGump = (CGUIHTMLGump*)item;

                    g_MouseManager.SetPosition(
                        Core::Vec2<i32>(oldPos.x - htmlGump->GetX(), oldPos.y - htmlGump->GetY()));

                    CBaseGUI* item = (CBaseGUI*)htmlGump->m_Items;

                    CRenderObject* selectedHTML = nullptr;

                    for (int j = 0; j < 4; j++)
                    {
                        if (item->Select())
                        {
                            selectedHTML = item;
                        }

                        item = (CBaseGUI*)item->m_Next;
                    }

                    //Scissor
                    if (item->Select())
                    {
                        int offsetX = htmlGump->DataOffset.x - htmlGump->CurrentOffset.x;
                        int offsetY = htmlGump->DataOffset.y - htmlGump->CurrentOffset.y;

                        Core::TMousePos pos = g_MouseManager.GetPosition();
                        g_MouseManager.SetPosition(
                            Core::Vec2<i32>(pos.x - offsetX, pos.y - offsetY));

                        selected =
                            CGump::SelectItems((CBaseGUI*)item->m_Next, currentPage, draw2Page);
                    }
                    else
                    {
                        selected = nullptr;
                    }

                    if (selected == nullptr)
                    {
                        selected = selectedHTML;

                        if (selected == nullptr)
                        {
                            selected = item;
                        }
                    }

                    g_MouseManager.SetPosition(oldPos);

                    break;
                }
                case GOT_DATABOX:
                {
                    CRenderObject* selectedBox =
                        CGump::SelectItems((CBaseGUI*)item->m_Items, currentPage, draw2Page);

                    if (selectedBox != nullptr)
                    {
                        selected = selectedBox;
                    }

                    break;
                }
                case GOT_COMBOBOX:
                {
                    //selected = ((CGUIComboBox*)item)->SelectedItem();

                    if (g_PressedObject.LeftObject == item)
                    {
                        combo = (CGUIComboBox*)item;
                    }
                    else
                    {
                        selected = item;
                    }

                    break;
                }
                case GOT_SHOPRESULT:
                {
                    selected = ((CGUIShopResult*)item)->SelectedItem();

                    break;
                }
                case GOT_SKILLITEM:
                {
                    selected = ((CGUISkillItem*)item)->SelectedItem();

                    break;
                }
                case GOT_SKILLGROUP:
                {
                    selected = ((CGUISkillGroup*)item)->SelectedItem();

                    break;
                }
                default:
                {
                    selected = item;

                    break;
                }
            }
        }
    }

    if (combo != nullptr)
    {
        selected = combo->SelectedItem();
    }

    return selected;
}

void CGump::TestItemsLeftMouseDown(
    CGump* gump, CBaseGUI* start, int currentPage, int draw2Page, int count)
{
    int group    = 0;
    int page     = 0;
    bool canDraw = ((draw2Page == 0) || (page >= currentPage && page <= currentPage + draw2Page));

    static bool htmlTextBackgroundCanBeColored = false;

    if (!(start != nullptr && start->m_Next == nullptr && start->Type == GOT_HTMLTEXT))
    {
        htmlTextBackgroundCanBeColored = false;
    }

    QFOR(item, start, CBaseGUI*)
    {
        if (count == 0)
        {
            break;
        }

        count--;

        if (item->Type == GOT_PAGE)
        {
            page = ((CGUIPage*)item)->Index;

            //if (page >= 2 && page > currentPage + draw2Page)
            //	break;

            canDraw =
                ((page == -1) || ((page >= currentPage && page <= currentPage + draw2Page) ||
                                  ((page == 0) && (draw2Page == 0))));
        }
        else if (canDraw && item->Enabled && item->Visible)
        {
            if (item->Type == GOT_GROUP)
            {
                group = ((CGUIGroup*)item)->Index;
                continue;
            }
            if (g_SelectedObject.Object != item && !item->IsHTMLGump())
            {
                if (item->Type == GOT_SHOPRESULT)
                {
                    if (g_SelectedObject.Object == ((CGUIShopResult*)item)->m_MinMaxButtons)
                    {
                        Core::TMousePos clickPos = g_MouseManager.GetPosition() -
                                                   Core::TMousePos(item->GetX(), item->GetY());
                        ((CGUIShopResult*)item)->m_MinMaxButtons->OnClick(clickPos);
                    }

                    continue;
                }
                if (item->Type != GOT_SKILLGROUP &&
                    item->Type != GOT_DATABOX /*&& item->Type != GOT_TEXTENTRY*/)
                {
                    continue;
                }
            }

            switch (item->Type)
            {
                case GOT_HITBOX:
                {
                    if (((CGUIPolygonal*)item)->CallOnMouseUp)
                    {
                        break;
                    }

                    CGUIHitBox* box = (CGUIHitBox*)item;

                    if (box->ToPage != -1)
                    {
                        gump->Page = box->ToPage;

                        //if (gump->Page < 1)
                        //	gump->Page = 1;
                    }
                }
                case GOT_COLOREDPOLYGONE:
                {
                    if (((CGUIPolygonal*)item)->CallOnMouseUp)
                    {
                        break;
                    }
                }
                case GOT_RESIZEPIC:
                {
                    u32 serial = item->Serial;

                    if (serial == 0u)
                    {
                        break;
                    }

                    int tempPage     = -1;
                    bool tempCanDraw = true;

                    QFOR(testItem, start, CBaseGUI*)
                    {
                        if (testItem->Type == GOT_PAGE)
                        {
                            tempPage = ((CGUIPage*)testItem)->Index;

                            tempCanDraw =
                                ((tempPage == -1) ||
                                 ((tempPage >= page && tempPage <= page + draw2Page) ||
                                  ((tempPage == 0) && (draw2Page == 0))));
                        }
                        else if (
                            tempCanDraw && testItem->Type == GOT_TEXTENTRY &&
                            testItem->Serial == serial && testItem->Enabled && testItem->Visible)
                        {
                            CGUITextEntry* entry = (CGUITextEntry*)testItem;

                            if (!entry->ReadOnly)
                            {
                                Core::TMousePos pos = g_MouseManager.GetPosition();
                                int x               = pos.x - item->GetX();
                                int y               = pos.y - item->GetY();

                                entry->OnClick(gump, x, y);
                            }

                            break;
                        }
                    }

                    gump->OnTextEntry(serial);
                    gump->WantRedraw = true;

                    return;
                }
                case GOT_SKILLGROUP:
                {
                    CGUISkillGroup* skillGroup = (CGUISkillGroup*)item;

                    if (g_SelectedObject.Object == skillGroup->m_Name)
                    {
                        gump->OnTextEntry(g_SelectedObject.Object->Serial);
                        gump->WantRedraw = true;

                        return;
                    }

                    break;
                }
                case GOT_RESIZEBUTTON:
                {
                    gump->OnResizeStart(item->Serial);
                    gump->WantRedraw = true;
                    break;
                }
                case GOT_SHOPITEM:
                {
                    ((CGUIShopItem*)item)->OnClick();
                    gump->WantRedraw = true;
                    break;
                }
                case GOT_HTMLTEXT:
                {
                    CGUIHTMLText* htmlText = (CGUIHTMLText*)item;

                    u16 link = htmlText->m_Texture.WebLinkUnderMouse(item->GetX(), item->GetY());

                    if ((link != 0u) && link != 0xFFFF)
                    {
                        gump->OnDirectHTMLLink(link);
                        gump->WantRedraw = true;

                        htmlText->m_Texture.ClearWebLink();
                        htmlText->CreateTexture(htmlTextBackgroundCanBeColored);
                    }

                    break;
                }
                case GOT_DATABOX:
                {
                    CGump::TestItemsLeftMouseDown(
                        gump, (CBaseGUI*)item->m_Items, currentPage, draw2Page);
                    break;
                }
                case GOT_BUTTON:
                case GOT_BUTTONTILEART:
                {
                    gump->WantRedraw = true;
                    break;
                }
                case GOT_TEXTENTRY:
                {
                    CGUITextEntry* entry = (CGUITextEntry*)item;

                    if (!entry->ReadOnly)
                    {
                        Core::TMousePos mousePos = g_MouseManager.GetPosition();
                        int x                    = mousePos.x - item->GetX();
                        int y                    = mousePos.y - item->GetY();

                        entry->OnClick(gump, x, y);
                    }

                    gump->OnTextEntry(item->Serial);
                    gump->WantRedraw = true;

                    return;
                }
                case GOT_SLIDER:
                {
                    Core::TMousePos mousePos = g_MouseManager.GetPosition();
                    int x                    = mousePos.x - item->GetX();
                    int y                    = mousePos.y - item->GetY();

                    ((CGUISlider*)item)->OnClick(x, y);

                    gump->OnSliderClick(item->Serial);
                    gump->WantRedraw = true;

                    return;
                }
                case GOT_MINMAXBUTTONS:
                {
                    ((CGUIMinMaxButtons*)item)->OnClick();
                    gump->WantRedraw = true;

                    return;
                }
                case GOT_COMBOBOX:
                {
                    gump->WantRedraw = true;

                    return;
                }
                case GOT_HTMLGUMP:
                case GOT_XFMHTMLGUMP:
                case GOT_XFMHTMLTOKEN:
                {
                    CGUIHTMLGump* htmlGump = (CGUIHTMLGump*)item;

                    htmlTextBackgroundCanBeColored = !htmlGump->HaveBackground;

                    Core::TMousePos oldPos = g_MouseManager.GetPosition();
                    g_MouseManager.SetPosition(
                        oldPos - Core::TMousePos(htmlGump->GetX(), htmlGump->GetY()));
                    CBaseGUI* item = (CBaseGUI*)htmlGump->m_Items;

                    TestItemsLeftMouseDown(gump, item, currentPage, draw2Page, 5);

                    for (int j = 0; j < 5; j++)
                    {
                        item = (CBaseGUI*)item->m_Next;
                    }

                    int offsetX = htmlGump->DataOffset.x - htmlGump->CurrentOffset.x;
                    int offsetY = htmlGump->DataOffset.y - htmlGump->CurrentOffset.y;

                    Core::TMousePos mousePos = g_MouseManager.GetPosition();
                    g_MouseManager.SetPosition(mousePos - Core::TMousePos(offsetX, offsetY));
                    TestItemsLeftMouseDown(gump, item, currentPage, draw2Page);

                    g_MouseManager.SetPosition(oldPos);

                    break;
                }
                default: break;
            }
        }
    }
}

void CGump::TestItemsLeftMouseUp(CGump* gump, CBaseGUI* start, int currentPage, int draw2Page)
{
    int group    = 0;
    int page     = 0;
    bool canDraw = ((draw2Page == 0) || (page >= currentPage && page <= currentPage + draw2Page));

    QFOR(item, start, CBaseGUI*)
    {
        if (item->Type == GOT_PAGE)
        {
            page = ((CGUIPage*)item)->Index;

            //if (page >= 2 && page > currentPage + draw2Page)
            //	break;

            canDraw =
                ((page == -1) || ((page >= currentPage && page <= currentPage + draw2Page) ||
                                  ((page == 0) && (draw2Page == 0))));
        }
        else if (canDraw && item->Enabled && item->Visible)
        {
            if (item->Type == GOT_GROUP)
            {
                group = ((CGUIGroup*)item)->Index;
                continue;
            }
            if (!item->IsHTMLGump())
            {
                if (item->Type != GOT_DATABOX && item->Type != GOT_COMBOBOX &&
                    item->Type != GOT_SKILLITEM && item->Type != GOT_SKILLGROUP)
                {
                    if (g_PressedObject.LeftObject == item)
                    {
                        if (g_SelectedObject.Object != g_PressedObject.LeftObject &&
                            !item->IsPressedOuthit())
                        {
                            continue;
                        }
                    }
                    else
                    {
                        continue;
                    }
                }
            }

            switch (item->Type)
            {
                case GOT_HITBOX:
                {
                    if (!((CGUIPolygonal*)item)->CallOnMouseUp)
                    {
                        break;
                    }

                    CGUIHitBox* box = (CGUIHitBox*)item;

                    if (box->ToPage != -1)
                    {
                        gump->Page = box->ToPage;

                        //if (gump->Page < 1)
                        //	gump->Page = 1;
                    }

                    gump->OnButton(item->Serial);
                    gump->WantRedraw = true;

                    return;
                }
                case GOT_COLOREDPOLYGONE:
                {
                    if (!((CGUIPolygonal*)item)->CallOnMouseUp)
                    {
                        break;
                    }

                    gump->OnButton(item->Serial);
                    gump->WantRedraw = true;

                    return;
                }
                case GOT_RESIZEBUTTON:
                {
                    gump->OnResizeEnd(item->Serial);
                    gump->WantRedraw = true;
                    break;
                }
                case GOT_SLIDER:
                {
                    gump->WantRedraw = true;
                    break;
                }
                case GOT_DATABOX:
                {
                    CGump::TestItemsLeftMouseUp(gump, (CBaseGUI*)item->m_Items, 0);
                    break;
                }
                case GOT_BUTTON:
                case GOT_BUTTONTILEART:
                {
                    if (item->IsControlHTML())
                    {
                        break;
                    }

                    CGUIButton* button = (CGUIButton*)item;

                    if (button->ToPage != -1)
                    {
                        gump->Page = button->ToPage;

                        if (gump->GumpType == GT_GENERIC)
                        {
                            gump->WantUpdateContent = true;
                        }

                        //if (gump->Page < 1)
                        //	gump->Page = 1;
                    }
                    else
                    {
                        gump->OnButton(item->Serial);
                    }

                    gump->WantRedraw = true;

                    return;
                }
                case GOT_TILEPICHIGHTLIGHTED:
                case GOT_GUMPPICHIGHTLIGHTED:
                {
                    gump->OnButton(item->Serial);
                    gump->WantRedraw = true;

                    return;
                }
                case GOT_CHECKBOX:
                {
                    CGUICheckbox* checkbox = (CGUICheckbox*)item;
                    checkbox->Checked      = !checkbox->Checked;

                    gump->OnCheckbox(item->Serial, checkbox->Checked);
                    gump->WantRedraw = true;

                    return;
                }
                case GOT_RADIO:
                {
                    CGUIRadio* radio = (CGUIRadio*)item;

                    int radioPage  = 0;
                    int radioGroup = 0;

                    QFOR(testRadio, start, CBaseGUI*)
                    {
                        if (testRadio->Type == GOT_PAGE)
                        {
                            radioPage = ((CGUIPage*)testRadio)->Index;
                        }
                        else if (testRadio->Type == GOT_GROUP)
                        {
                            radioGroup = ((CGUIGroup*)testRadio)->Index;
                        }
                        else if (testRadio->Type == GOT_RADIO)
                        {
                            if (page <= 1 && radioPage <= 1)
                            {
                                if (group == radioGroup)
                                {
                                    if (((CGUIRadio*)testRadio)->Checked && testRadio != radio)
                                    {
                                        gump->OnRadio(testRadio->Serial, false);
                                    }

                                    ((CGUIRadio*)testRadio)->Checked = false;
                                }
                            }
                            else if (page == radioPage)
                            {
                                if (group == radioGroup)
                                {
                                    if (((CGUIRadio*)testRadio)->Checked && testRadio != radio)
                                    {
                                        gump->OnRadio(testRadio->Serial, false);
                                    }

                                    ((CGUIRadio*)testRadio)->Checked = false;
                                }
                            }
                        }
                    }

                    radio->Checked = true;

                    gump->OnRadio(item->Serial, true);
                    gump->WantRedraw = true;

                    return;
                }
                case GOT_COMBOBOX:
                {
                    CGUIComboBox* combo = (CGUIComboBox*)item;

                    int selectedCombo = combo->IsSelectedItem();

                    if (selectedCombo != -1)
                    {
                        combo->SelectedIndex = selectedCombo;
                        gump->OnComboboxSelection(item->Serial + selectedCombo);
                        gump->WantRedraw = true;
                    }

                    break;
                }
                case GOT_SKILLITEM:
                {
                    CGUISkillItem* skillItem = (CGUISkillItem*)item;

                    if ((g_PressedObject.LeftObject == skillItem->m_ButtonUse &&
                         skillItem->m_ButtonUse != nullptr) ||
                        g_PressedObject.LeftObject == skillItem->m_ButtonStatus)
                    {
                        gump->OnButton(g_PressedObject.LeftSerial);
                        gump->WantRedraw = true;
                    }

                    break;
                }
                case GOT_SKILLGROUP:
                {
                    CGUISkillGroup* skillGroup = (CGUISkillGroup*)item;

                    if (g_PressedObject.LeftObject == skillGroup->m_Minimizer)
                    {
                        gump->OnButton(g_PressedObject.LeftSerial);
                        gump->WantRedraw = true;
                    }
                    else
                    {
                        TestItemsLeftMouseUp(
                            gump, (CBaseGUI*)skillGroup->m_Items, currentPage, draw2Page);
                    }

                    break;
                }
                case GOT_HTMLGUMP:
                case GOT_XFMHTMLGUMP:
                case GOT_XFMHTMLTOKEN:
                {
                    TestItemsLeftMouseUp(gump, (CBaseGUI*)item->m_Items, currentPage, draw2Page);

                    break;
                }
                default: break;
            }
        }
    }
}

void CGump::TestItemsScrolling(
    CGump* gump, CBaseGUI* start, bool up, int currentPage, int draw2Page)
{
    const int delay = SCROLL_LISTING_DELAY / 7;

    int group    = 0;
    int page     = 0;
    bool canDraw = ((draw2Page == 0) || (page >= currentPage && page <= currentPage + draw2Page));

    QFOR(item, start, CBaseGUI*)
    {
        if (item->Type == GOT_PAGE)
        {
            page = ((CGUIPage*)item)->Index;

            //if (page >= 2 && page > currentPage + draw2Page)
            //	break;

            canDraw =
                ((page == -1) || ((page >= currentPage && page <= currentPage + draw2Page) ||
                                  ((page == 0) && (draw2Page == 0))));
        }
        else if (canDraw && item->Enabled && item->Visible)
        {
            if (item->Type == GOT_GROUP)
            {
                group = ((CGUIGroup*)item)->Index;
                continue;
            }
            if (g_SelectedObject.Object != item && !item->IsHTMLGump())
            {
                continue;
            }

            switch (item->Type)
            {
                case GOT_RESIZEPIC:
                {
                    if (!item->IsControlHTML())
                    {
                        break;
                    }

                    CGUIHTMLResizepic* resizepic = (CGUIHTMLResizepic*)item;
                    resizepic->Scroll(up, delay);

                    gump->OnSliderMove(item->Serial);
                    gump->WantRedraw = true;

                    break;
                }
                case GOT_SLIDER:
                {
                    ((CGUISlider*)item)->OnScroll(up, delay);

                    gump->OnSliderMove(item->Serial);
                    gump->WantRedraw = true;

                    return;
                }
                case GOT_DATABOX:
                {
                    CGump::TestItemsScrolling(
                        gump, (CBaseGUI*)item->m_Items, up, currentPage, draw2Page);
                    break;
                }
                case GOT_HTMLGUMP:
                case GOT_XFMHTMLGUMP:
                case GOT_XFMHTMLTOKEN:
                {
                    if (item->Select())
                    {
                        CGUIHTMLGump* htmlGump = (CGUIHTMLGump*)item;

                        Core::TMousePos oldPos = g_MouseManager.GetPosition();
                        g_MouseManager.SetPosition(
                            oldPos - Core::TMousePos(htmlGump->GetX(), htmlGump->GetY()));

                        CBaseGUI* item = (CBaseGUI*)htmlGump->m_Items;

                        for (int j = 0; j < 5; j++)
                        {
                            if (item->Type == GOT_SLIDER)
                            {
                                ((CGUISlider*)item)->OnScroll(up, delay);

                                gump->OnSliderMove(item->Serial);
                            }

                            item = (CBaseGUI*)item->m_Next;
                        }

                        int offsetX = htmlGump->DataOffset.x - htmlGump->CurrentOffset.x;
                        int offsetY = htmlGump->DataOffset.y - htmlGump->CurrentOffset.y;

                        Core::TMousePos mousePos = g_MouseManager.GetPosition();
                        g_MouseManager.SetPosition(mousePos - Core::TMousePos(offsetX, offsetY));
                        TestItemsScrolling(gump, (CBaseGUI*)item, up, currentPage, draw2Page);

                        g_MouseManager.SetPosition(oldPos);

                        gump->WantRedraw = true;
                    }

                    break;
                }
                default: break;
            }
        }
    }
}

void CGump::TestItemsDragging(
    CGump* gump, CBaseGUI* start, int currentPage, int draw2Page, int count)
{
    int group = 0;
    int page  = 0;
    bool canDraw =
        ((page == -1) || ((page == 0) && (draw2Page == 0)) ||
         (page >= currentPage && page <= currentPage + draw2Page));

    QFOR(item, start, CBaseGUI*)
    {
        if (count == 0)
        {
            break;
        }

        count--;

        if (item->Type == GOT_PAGE)
        {
            page = ((CGUIPage*)item)->Index;

            //if (page >= 2 && page > currentPage + draw2Page)
            //	break;

            canDraw =
                ((page == -1) || ((page >= currentPage && page <= currentPage + draw2Page) ||
                                  ((page == 0) && (draw2Page == 0))));
        }
        else if (canDraw && item->Enabled && item->Visible)
        {
            if (item->Type == GOT_GROUP)
            {
                group = ((CGUIGroup*)item)->Index;
                continue;
            }
            if (g_PressedObject.LeftObject != item && !item->IsHTMLGump())
            {
                continue;
            }

            switch (item->Type)
            {
                case GOT_SLIDER:
                {
                    Core::TMousePos mousePos = g_MouseManager.GetPosition();
                    int x                    = mousePos.x - item->GetX();
                    int y                    = mousePos.y - item->GetY();
                    ((CGUISlider*)item)->OnClick(x, y);

                    gump->OnSliderMove(item->Serial);
                    gump->WantRedraw = true;

                    return;
                }
                case GOT_DATABOX:
                {
                    CGump::TestItemsDragging(
                        gump, (CBaseGUI*)item->m_Items, currentPage, draw2Page);
                    break;
                }
                case GOT_RESIZEBUTTON:
                {
                    gump->OnResize(item->Serial);
                    gump->WantRedraw = true;

                    break;
                }
                case GOT_HTMLGUMP:
                case GOT_XFMHTMLGUMP:
                case GOT_XFMHTMLTOKEN:
                {
                    CGUIHTMLGump* htmlGump = (CGUIHTMLGump*)item;

                    Core::TMousePos oldPos = g_MouseManager.GetPosition();
                    g_MouseManager.SetPosition(
                        oldPos - Core::TMousePos(htmlGump->GetX(), htmlGump->GetY()));

                    CBaseGUI* item = (CBaseGUI*)htmlGump->m_Items;

                    TestItemsDragging(gump, item, currentPage, draw2Page, 5);

                    for (int j = 0; j < 5; j++)
                    {
                        item = (CBaseGUI*)item->m_Next;
                    }

                    int offsetX = htmlGump->DataOffset.x - htmlGump->CurrentOffset.x;
                    int offsetY = htmlGump->DataOffset.y - htmlGump->CurrentOffset.y;

                    Core::TMousePos mousePos = g_MouseManager.GetPosition();
                    g_MouseManager.SetPosition(mousePos - Core::TMousePos(offsetX, offsetY));
                    TestItemsDragging(gump, item, currentPage, draw2Page);

                    g_MouseManager.SetPosition(oldPos);

                    gump->WantRedraw = true;
                    break;
                }
                default: break;
            }
        }
    }
}

void CGump::PrepareTextures()
{
    QFOR(item, m_Items, CBaseGUI*)
    item->PrepareTextures();
}

bool CGump::EntryPointerHere()
{
    QFOR(item, m_Items, CBaseGUI*)
    {
        if (item->Visible && item->EntryPointerHere())
        {
            return true;
        }
    }

    return false;
}

void CGump::GenerateFrame(bool stop)
{
    if (!g_GL.Drawing)
    {
        FrameCreated = false;
        WantRedraw   = true;

        return;
    }

    CalculateGumpState();

    PrepareTextures();

    DrawItems((CBaseGUI*)m_Items, Page, Draw2Page);

    WantRedraw   = true;
    FrameCreated = true;
}

void CGump::Draw()
{
    PROFILER_EVENT();
    CalculateGumpState();

    if (WantUpdateContent)
    {
        UpdateContent();
        RecalculateSize();
        WantUpdateContent = false;
        FrameCreated      = false;
    }

    if (!FrameCreated)
    {
    loc_create_frame:

        if (!m_FrameBuffer.Ready(GumpRect.size))
        {
            m_FrameBuffer.Init(GumpRect.size);
        }

        if (m_FrameBuffer.Use())
        {
            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

            glTranslatef(-(GLfloat)GumpRect.pos.x, -(GLfloat)GumpRect.pos.y, 0.0f);

            GenerateFrame(true);

            if (g_DeveloperMode == DM_DEBUGGING)
            {
                if (g_SelectedObject.Gump == this)
                {
                    glColor4f(0.0f, 1.0f, 0.0f, 0.2f);
                }
                else
                {
                    glColor4f(1.0f, 1.0f, 1.0f, 0.2f);
                }

                g_GL.DrawLine(
                    GumpRect.pos.x + 1,
                    GumpRect.pos.y + 1,
                    GumpRect.pos.x + GumpRect.size.x,
                    GumpRect.pos.y + 1);
                g_GL.DrawLine(
                    GumpRect.pos.x + GumpRect.size.x,
                    GumpRect.pos.y + 1,
                    GumpRect.pos.x + GumpRect.size.x,
                    GumpRect.pos.y + GumpRect.size.y);
                g_GL.DrawLine(
                    GumpRect.pos.x + GumpRect.size.x,
                    GumpRect.pos.y + GumpRect.size.y,
                    GumpRect.pos.x + 1,
                    GumpRect.pos.y + GumpRect.size.y);
                g_GL.DrawLine(
                    GumpRect.pos.x + 1,
                    GumpRect.pos.y + GumpRect.size.y,
                    GumpRect.pos.x + 1,
                    GumpRect.pos.y + 1);

                glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
            }

            glTranslatef((GLfloat)GumpRect.pos.x, (GLfloat)GumpRect.pos.y, 0.0f);

            m_FrameBuffer.Release();
        }
    }
    else if (WantRedraw)
    {
        WantRedraw = false;
        goto loc_create_frame;
    }

    GLfloat posX = g_GumpTranslate.x;
    GLfloat posY = g_GumpTranslate.y;

    posX += (GLfloat)GumpRect.pos.x;
    posY += (GLfloat)GumpRect.pos.y;

    glTranslatef(posX, posY, 0.0f);

    glEnable(GL_BLEND);
    //glBlendFunc(GL_DST_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

    m_FrameBuffer.Draw(0, 0);

    glDisable(GL_BLEND);

    DrawLocker();

    glTranslatef(-posX, -posY, 0.0f);
}

CRenderObject* CGump::Select()
{
    PROFILER_EVENT();
    g_CurrentCheckGump = this;
    CalculateGumpState();

    if (WantUpdateContent)
    {
        UpdateContent();
        RecalculateSize();
        WantUpdateContent = false;
        FrameCreated      = false;
    }

    Core::TMousePos oldPos = g_MouseManager.GetPosition();
    g_MouseManager.SetPosition(
        oldPos - Core::TMousePos((i32)g_GumpTranslate.x, (i32)g_GumpTranslate.y));
    CRenderObject* selected = nullptr;

    if (SelectLocker())
    {
        selected = &m_Locker;
    }
    else if (
        g_MouseManager.GetPosition().x >= GumpRect.pos.x &&
        g_MouseManager.GetPosition().x < GumpRect.pos.x + GumpRect.size.x &&
        g_MouseManager.GetPosition().y >= GumpRect.pos.y &&
        g_MouseManager.GetPosition().y < GumpRect.pos.y + GumpRect.size.y)
    {
        selected = SelectItems((CBaseGUI*)m_Items, Page, Draw2Page);
    }

    if (selected != nullptr)
    {
        CBaseGUI* sel = (CBaseGUI*)selected;
        g_SelectedObject.Init(selected, this);
    }

    g_MouseManager.SetPosition(oldPos);
    g_CurrentCheckGump = nullptr;

    return selected;
}

void CGump::RecalculateSize()
{
    PROFILER_EVENT();

    Core::Vec2<i32> minPosition(999, 999);
    Core::Vec2<i32> maxPosition;
    Core::Vec2<i32> offset;

    GetItemsSize(this, (CBaseGUI*)m_Items, minPosition, maxPosition, offset, -1, Page, Draw2Page);

    Core::Vec2<i32> size(maxPosition.x - minPosition.x, maxPosition.y - minPosition.y);

    GumpRect = Core::Rect<i32>(minPosition, size);
}

void CGump::GetItemsSize(
    CGump* gump,
    CBaseGUI* start,
    Core::Vec2<i32>& minPosition,
    Core::Vec2<i32>& maxPosition,
    Core::Vec2<i32>& offset,
    int count,
    int currentPage,
    int draw2Page)
{
    int page     = 0;
    bool canDraw = ((draw2Page == 0) || (page >= currentPage && page <= currentPage + draw2Page));

    QFOR(item, start, CBaseGUI*)
    {
        if (count == 0)
        {
            break;
        }

        count--;

        if (item->Type == GOT_PAGE)
        {
            page = ((CGUIPage*)item)->Index;

            //if (page >= 2 && page > currentPage + draw2Page)
            //	break;

            canDraw =
                ((page == -1) || ((page >= currentPage && page <= currentPage + draw2Page) ||
                                  ((page == 0) && (draw2Page == 0))));

            continue;
        }

        if (!canDraw || !item->Visible)
        {
            continue;
        }

        switch (item->Type)
        {
            case GOT_PAGE:
            case GOT_GROUP:
            case GOT_NONE:
            case GOT_MASTERGUMP:
            case GOT_CHECKTRANS:
            case GOT_SHADER:
            case GOT_BLENDING:
            case GOT_GLOBAL_COLOR:
            case GOT_TOOLTIP: break;
            case GOT_DATABOX:
            {
                CGump::GetItemsSize(
                    gump,
                    (CBaseGUI*)item->m_Items,
                    minPosition,
                    maxPosition,
                    offset,
                    count,
                    currentPage,
                    draw2Page);
                break;
            }
            case GOT_HTMLGUMP:
            case GOT_XFMHTMLGUMP:
            case GOT_XFMHTMLTOKEN:
            {
                Core::Vec2<i32> htmlOffset(offset.x + item->GetX(), offset.x + item->GetY());
                CGump::GetItemsSize(
                    gump,
                    (CBaseGUI*)item->m_Items,
                    minPosition,
                    maxPosition,
                    htmlOffset,
                    5,
                    currentPage,
                    draw2Page);
                break;
            }
            case GOT_SCISSOR: ((CGUIScissor*)item)->GumpParent = gump;
            default:
            {
                int x = item->GetX() + offset.x;
                int y = item->GetY() + offset.y;

                if (x < minPosition.x)
                {
                    minPosition.x = x;
                }

                if (y < minPosition.y)
                {
                    minPosition.y = y;
                }

                Core::Vec2<i32> itemSize = item->GetSize();

                x += itemSize.x;
                y += itemSize.y;

                if (x > maxPosition.x)
                {
                    maxPosition.x = x;
                }

                if (y > maxPosition.y)
                {
                    maxPosition.y = y;
                }

                break;
            }
        }
    }
}

void CGump::OnLeftMouseButtonDown()
{
    g_CurrentCheckGump     = this;
    Core::TMousePos oldPos = g_MouseManager.GetPosition();
    g_MouseManager.SetPosition(oldPos - Core::TMousePos(m_X, m_Y));
    TestItemsLeftMouseDown(this, (CBaseGUI*)m_Items, Page, Draw2Page);
    g_MouseManager.SetPosition(oldPos);
    g_CurrentCheckGump = nullptr;
}

void CGump::OnLeftMouseButtonUp()
{
    g_CurrentCheckGump = this;
    TestItemsLeftMouseUp(this, (CBaseGUI*)m_Items, Page, Draw2Page);
    TestLockerClick();
    g_CurrentCheckGump = nullptr;
}

void CGump::OnMidMouseButtonScroll(bool up)
{
    g_CurrentCheckGump      = this;
    Core::TMousePos oldPos = g_MouseManager.GetPosition();
    g_MouseManager.SetPosition(oldPos - Core::TMousePos(m_X, m_Y));
    TestItemsScrolling(this, (CBaseGUI*)m_Items, up, Page, Draw2Page);
    g_MouseManager.SetPosition(oldPos);
    g_CurrentCheckGump      = nullptr;
}

void CGump::OnDragging()
{
    g_CurrentCheckGump      = this;
    Core::TMousePos oldPos = g_MouseManager.GetPosition();
    g_MouseManager.SetPosition(oldPos - Core::TMousePos(m_X, m_Y));
    TestItemsDragging(this, (CBaseGUI*)m_Items, Page, Draw2Page);
    g_MouseManager.SetPosition(oldPos);
    g_CurrentCheckGump      = nullptr;
}

void CGump::PasteClipboardData(std::wstring& data)
{
}
