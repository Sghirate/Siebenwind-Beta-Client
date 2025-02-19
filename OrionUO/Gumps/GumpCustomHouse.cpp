#include "GumpCustomHouse.h"
#include "Core/File.h"
#include "Globals.h"
#include "OrionUO.h"
#include "ToolTip.h"
#include "Target.h"
#include "SelectedObject.h"
#include "Multi.h"
#include "OrionApplication.h"
#include "Managers/MapManager.h"
#include "Managers/ClilocManager.h"
#include "GameObjects/GameItem.h"
#include "GameObjects/GameWorld.h"
#include "GameObjects/GamePlayer.h"
#include "GameObjects/CustomHouseMultiObject.h"
#include "Network/Packets.h"

CGumpCustomHouse* g_CustomHouseGump = nullptr;

template <class T, class A>
void ParseCustomHouseObjectFileWithCategory(std::vector<A>& list, const char* a_fileName)
{
    const std::filesystem::path filePath = g_App.GetGameDir() / a_fileName;
    Core::File file(filePath, "r");
    if (file)
    {
        int line = 0;

        while (!file.Eof())
        {
            char buf[256] = { 0 };
            file.Gets(buf);
            if (!strlen(buf))
                continue;

            line++;
            if (line <= 2)
                continue;

            T item;
            if (item.Parse(buf))
            {
                if (item.FeatureMask && !(g_LockedClientFeatures & item.FeatureMask))
                    continue;

                bool found = false;
                for (A& cat : list)
                {
                    if (cat.Index == item.Category)
                    {
                        cat.m_Items.push_back(item);
                        found = true;
                        break;
                    }
                }

                if (!found)
                {
                    A cat;
                    cat.Index = item.Category;
                    cat.m_Items.push_back(item);
                    list.push_back(cat);
                }
            }
        }
    }
}

template <class T>
void ParseCustomHouseObjectFile(std::vector<T>& list, const char* a_fileName)
{
    const std::filesystem::path filePath = g_App.GetGameDir() / a_fileName;
    LOG_INFO("GumpCustomHouse", "parse CH file: %s", filePath.string().c_str());
    Core::File file(filePath, "r");
    if (file)
    {
        int line = 0;

        while (!file.Eof())
        {
            char buf[256] = { 0 };
            file.Gets(buf);
            if (!strlen(buf))
                continue;

            line++;
            if (line <= 2)
                continue;

            T item;
            if (item.Parse(buf))
            {
                if (!item.FeatureMask || (g_LockedClientFeatures & item.FeatureMask))
                {
                    list.push_back(item);
                }
            }
        }
    }
}

CGumpCustomHouse::CGumpCustomHouse(int serial, int x, int y)
    : CGump(GT_CUSTOM_HOUSE, serial, x, y)
{
    m_FloorVisionState[0] = m_FloorVisionState[1] = m_FloorVisionState[2] = m_FloorVisionState[3] =
        CHGVS_NORMAL;

    g_CustomHouseGump = this;

    ParseCustomHouseObjectFileWithCategory<CCustomHouseObjectWall, CCustomHouseObjectWallCategory>(
        m_Walls, "walls.txt");
    ParseCustomHouseObjectFile<CCustomHouseObjectFloor>(m_Floors, "floors.txt");
    ParseCustomHouseObjectFile<CCustomHouseObjectDoor>(m_Doors, "doors.txt");
    ParseCustomHouseObjectFileWithCategory<CCustomHouseObjectMisc, CCustomHouseObjectMiscCategory>(
        m_Miscs, "misc.txt");
    ParseCustomHouseObjectFile<CCustomHouseObjectStair>(m_Stairs, "stairs.txt");
    ParseCustomHouseObjectFile<CCustomHouseObjectTeleport>(m_Teleports, "teleprts.txt");
    ParseCustomHouseObjectFileWithCategory<CCustomHouseObjectRoof, CCustomHouseObjectRoofCategory>(
        m_Roofs, "roof.txt");
    ParseCustomHouseObjectFile<CCustomHouseObjectPlaceInfo>(m_ObjectsInfo, "suppinfo.txt");

    CGameItem* foundationItem = g_World->GetWorldItem(serial);

    if (foundationItem != nullptr)
    {
        MinHouseZ = foundationItem->GetZ() + 7;

        CMulti* multi = foundationItem->GetMulti();

        if (multi != nullptr)
        {
            StartPos.x = foundationItem->GetX() + multi->MinX;
            StartPos.y = foundationItem->GetY() + multi->MinY;
            EndPos.x   = foundationItem->GetX() + multi->MaxX + 1;
            EndPos.y   = foundationItem->GetY() + multi->MaxY + 1;
        }

        int width  = abs(EndPos.x - StartPos.x);
        int height = abs(EndPos.y - StartPos.y);

        if (width > 13 || height > 13)
        {
            FloorCount = 4;
        }
        else
        {
            FloorCount = 3;
        }

        int componentsOnFloor = (width - 1) * (height - 1);

        MaxComponents = FloorCount * (componentsOnFloor + 2 * (width + height) - 4) -
                        (int)((double)(FloorCount * componentsOnFloor) * -0.25) + 2 * width +
                        3 * height - 5;
        MaxFixtures = MaxComponents / 20;
    }

    LOG_INFO(
        "GumpCustopmHouse",
        "multi Bounds: %i %i %i %i",
        StartPos.x,
        StartPos.y,
        EndPos.x,
        EndPos.y);
    LOG_INFO("GumpCustopmHouse", "MaxComponents=%i, MaxFixtures=%i", MaxComponents, MaxFixtures);

    Add(new CGUIPage(0));
    Add(new CGUIGumppicTiled(0x0E14, 121, 36, 397, 120));
    m_DataBox = (CGUIDataBox*)Add(new CGUIDataBox());

    Add(new CGUIGumppic(0x55F0, 0, 17));

    m_Gumppic = (CGUIGumppic*)Add(new CGUIGumppic((FloorCount == 4 ? 0x55F2 : 0x55F9), 486, 17));

    Add(new CGUIGumppicTiled(0x55F1, 153, 17, 333, 154));

    Add(new CGUIButton(ID_GCH_STATE_WALL, 0x5654, 0x5655, 0x5656, 9, 41));
    Add(new CGUIButton(ID_GCH_STATE_DOOR, 0x5657, 0x5658, 0x5659, 39, 40));
    Add(new CGUIButton(ID_GCH_STATE_FLOOR, 0x565A, 0x565B, 0x565C, 70, 40));
    Add(new CGUIButton(ID_GCH_STATE_STAIR, 0x565D, 0x565E, 0x565F, 9, 72));
    Add(new CGUIButton(ID_GCH_STATE_ROOF, 0x5788, 0x5789, 0x578A, 39, 72));
    Add(new CGUIButton(ID_GCH_STATE_MISC, 0x5663, 0x5664, 0x5665, 69, 72));
    Add(new CGUIButton(ID_GCH_STATE_MENU, 0x566C, 0x566D, 0x566E, 69, 100));

    m_TextComponents = (CGUIText*)Add(new CGUIText(0x0481, 82, 142));

    CGUIText* text = (CGUIText*)Add(new CGUIText(0x0481, 84, 142));
    text->CreateTextureA(9, ":");

    m_TextFixtures = (CGUIText*)Add(new CGUIText(0x0481, 94, 142));

    m_TextCost = (CGUIText*)Add(new CGUIText(0x0481, 524, 142));

    CGUIHitBox* box = (CGUIHitBox*)Add(new CGUIHitBox(ID_GCH_AREA_OBJECTS_INFO, 36, 137, 84, 23));
    box->MoveOnDrag = true;

    box             = (CGUIHitBox*)Add(new CGUIHitBox(ID_GCH_AREA_COST_INFO, 522, 137, 84, 23));
    box->MoveOnDrag = true;

    m_DataBoxGUI = (CGUIDataBox*)Add(new CGUIDataBox());

    UpdateMaxPage();
    WantUpdateContent = true;
}

CGumpCustomHouse::~CGumpCustomHouse()
{
    g_CustomHouseGump = nullptr;

    m_DataBox    = nullptr;
    m_DataBoxGUI = nullptr;

    m_Gumppic        = nullptr;
    m_TextComponents = nullptr;
    m_TextFixtures   = nullptr;
    m_TextCost       = nullptr;

    CPacketCustomHouseBuildingExit().Send();
    g_Target.SendCancelTarget();
}

void CGumpCustomHouse::CalculateGumpState()
{
    CGump::CalculateGumpState();

    if ((g_GumpTranslate.x != 0.0f) || (g_GumpTranslate.y != 0.0f))
    {
        WantRedraw = true;
    }
}

void CGumpCustomHouse::InitToolTip()
{
    u32 id = g_SelectedObject.Serial;

    if (id >= ID_GCH_ITEM_IN_LIST)
    {
        int index = id - ID_GCH_ITEM_IN_LIST;

        if (index < 0)
        {
            return;
        }

        int cliloc = 0;

        if (Category == -1 && (State == CHGS_WALL || State == CHGS_ROOF || State == CHGS_MISC))
        {
            if (State == CHGS_WALL && index < (int)m_Walls.size() &&
                (static_cast<unsigned int>(!m_Walls[index].m_Items.empty()) != 0u))
            {
                cliloc = m_Walls[index].m_Items[0].TID;
            }
            else if (
                State == CHGS_ROOF && index < (int)m_Roofs.size() &&
                (static_cast<unsigned int>(!m_Roofs[index].m_Items.empty()) != 0u))
            {
                cliloc = m_Roofs[index].m_Items[0].TID;
            }
            else if (
                State == CHGS_MISC && index < (int)m_Miscs.size() &&
                (static_cast<unsigned int>(!m_Roofs[index].m_Items.empty()) != 0u))
            {
                cliloc = m_Miscs[index].m_Items[0].TID;
            }
        }
        else if (State == CHGS_ROOF)
        {
            cliloc = 1070640 + index;
        }

        if (cliloc != 0)
        {
            g_ToolTip.Set(cliloc, "");
        }

        return;
    }

    switch (id)
    {
        case ID_GCH_STATE_WALL:
        {
            g_ToolTip.Set(1061016, "Walls");
            break;
        }
        case ID_GCH_STATE_DOOR:
        {
            g_ToolTip.Set(1061017, "Doors");
            break;
        }
        case ID_GCH_STATE_FLOOR:
        {
            g_ToolTip.Set(1061018, "Floors");
            break;
        }
        case ID_GCH_STATE_STAIR:
        {
            g_ToolTip.Set(1061019, "Stairs");
            break;
        }
        case ID_GCH_STATE_ROOF:
        {
            g_ToolTip.Set(1063364, "Roofs");
            break;
        }
        case ID_GCH_STATE_MISC:
        {
            g_ToolTip.Set(1061021, "Miscellaneous");
            break;
        }
        case ID_GCH_STATE_ERASE:
        {
            g_ToolTip.Set(1061022, "Erase");
            break;
        }
        case ID_GCH_STATE_EYEDROPPER:
        {
            g_ToolTip.Set(1061023, "Eyedropper Tool");
            break;
        }
        case ID_GCH_STATE_MENU:
        {
            g_ToolTip.Set(1061024, "System Menu");
            break;
        }
        case ID_GCH_VISIBILITY_STORY_1:
        {
            g_ToolTip.Set(1061029, "Store 1 Visibility");
            break;
        }
        case ID_GCH_VISIBILITY_STORY_2:
        {
            g_ToolTip.Set(1061030, "Store 2 Visibility");
            break;
        }
        case ID_GCH_VISIBILITY_STORY_3:
        {
            g_ToolTip.Set(1061031, "Store 3 Visibility");
            break;
        }
        case ID_GCH_VISIBILITY_STORY_4:
        {
            g_ToolTip.Set(1061032, "Store 4 Visibility");
            break;
        }
        case ID_GCH_GO_FLOOR_1:
        {
            g_ToolTip.Set(1061033, "Go To Story 1");
            break;
        }
        case ID_GCH_GO_FLOOR_2:
        {
            g_ToolTip.Set(1061034, "Go To Story 2");
            break;
        }
        case ID_GCH_GO_FLOOR_3:
        {
            g_ToolTip.Set(1061035, "Go To Story 3");
            break;
        }
        case ID_GCH_GO_FLOOR_4:
        {
            g_ToolTip.Set(1061036, "Go To Story 4");
            break;
        }
        case ID_GCH_LIST_LEFT:
        {
            g_ToolTip.Set(1061028, "Previous Page");
            break;
        }
        case ID_GCH_LIST_RIGHT:
        {
            g_ToolTip.Set(1061027, "Next Page");
            break;
        }
        case ID_GCH_MENU_BACKUP:
        {
            g_ToolTip.Set(
                1061041,
                "Store design in progress in a back up buffer, but do not finalize design.");
            break;
        }
        case ID_GCH_MENU_RESTORE:
        {
            g_ToolTip.Set(
                1061043,
                "Restore your design in progress to a design you have previously backed up.");
            break;
        }
        case ID_GCH_MENU_SYNCH:
        {
            g_ToolTip.Set(1061044, "Synchronize design state with server.");
            break;
        }
        case ID_GCH_MENU_CLEAR:
        {
            g_ToolTip.Set(
                1061045,
                "Clear all changes, returning your design in progress to a blank foundation.");
            break;
        }
        case ID_GCH_MENU_COMMIT:
        {
            g_ToolTip.Set(1061042, "Save existing changes and finalize design.");
            break;
        }
        case ID_GCH_MENU_REVERT:
        {
            g_ToolTip.Set(
                1061047,
                "Revert your design in progress to match your currently visible, finalized design.");
            break;
        }
        case ID_GCH_GO_CATEGORY:
        {
            g_ToolTip.Set(1061025, "To Category");
            break;
        }
        case ID_GCH_WALL_SHOW_WINDOW:
        {
            g_ToolTip.Set(1061026, "Window Toggle");
            break;
        }
        case ID_GCH_ROOF_Z_DOWN:
        {
            g_ToolTip.Set(1063392, "Lower Roof Placement Level");
            break;
        }
        case ID_GCH_ROOF_Z_UP:
        {
            g_ToolTip.Set(1063393, "Raise Roof Placement Level");
            break;
        }
        case ID_GCH_AREA_OBJECTS_INFO:
        {
            std::wstring str = g_ClilocManager.ParseArgumentsToClilocString(
                1061039,
                true,
                std::to_wstring(MaxComponents) + L"\t" + std::to_wstring(MaxFixtures));
            g_ToolTip.Set(str);
            break;
        }
        case ID_GCH_AREA_COST_INFO:
        {
            g_ToolTip.Set(1061038, "Cost");
            break;
        }
        case ID_GCH_AREA_ROOF_Z_INFO:
        {
            g_ToolTip.Set(1070759, "Current Roof Placement Level");
            break;
        }
        default: break;
    }
}

void CGumpCustomHouse::DrawWallSection()
{
    int x = 0;
    int y = 0;

    if (Category == -1)
    {
        int startCategory = Page * 16;
        int endCategory   = startCategory + 16;

        if (endCategory > (int)m_Walls.size())
        {
            endCategory = (int)m_Walls.size();
        }

        m_DataBox->Add(new CGUIScissor(true, 0, 0, 121, 36, 384, 60));

        for (int i = startCategory; i < endCategory; i++)
        {
            const std::vector<CCustomHouseObjectWall>& vec = m_Walls[i].m_Items;

            if (vec.empty())
            {
                continue;
            }

            Core::Vec2<i32> dims = g_Orion.GetStaticArtDimension(vec[0].East1);

            int offsetX = x + 121 + (48 - dims.x) / 2;
            int offsetY = y + 36;

            m_DataBox->Add(new CGUITilepic(vec[0].East1, 0, offsetX, offsetY));

            m_DataBox->Add(new CGUIHitBox(
                ID_GCH_ITEM_IN_LIST + (int)i, offsetX, offsetY, dims.x, dims.y, true));

            x += 48;

            if (x >= 384)
            {
                x = 0;
                y += 60;

                m_DataBox->Add(new CGUIScissor(false));
                m_DataBox->Add(new CGUIScissor(true, 0, 0, 121, 96, 384, 60));
            }
        }

        m_DataBox->Add(new CGUIScissor(false));
    }
    else if (Category >= 0 && Category < (int)m_Walls.size())
    {
        const std::vector<CCustomHouseObjectWall>& vec = m_Walls[Category].m_Items;

        if (Page >= 0 && Page < (int)vec.size())
        {
            const CCustomHouseObjectWall& item = vec[Page];

            m_DataBox->Add(new CGUIScissor(true, 0, 0, 130, 36, 384, 120));

            for (int i = 0; i < 8; i++)
            {
                u16 graphic = (ShowWindow ? item.m_WindowGraphics[i] : item.m_Graphics[i]);

                if (graphic != 0u)
                {
                    Core::Vec2<i32> dims = g_Orion.GetStaticArtDimension(graphic);

                    int offsetX = x + 130 + (48 - dims.x) / 2;
                    int offsetY = y + 36 + (120 - dims.y) / 2;

                    m_DataBox->Add(new CGUITilepic(graphic, 0, offsetX, offsetY));

                    m_DataBox->Add(new CGUIHitBox(
                        ID_GCH_ITEM_IN_LIST + (int)i, offsetX, offsetY, dims.x, dims.y, true));
                }

                x += 48;
            }

            m_DataBox->Add(new CGUIScissor(false));
        }

        m_DataBoxGUI->Add(new CGUIGumppic(0x55F3, 152, 0));
        m_DataBoxGUI->Add(new CGUIButton(ID_GCH_GO_CATEGORY, 0x5622, 0x5623, 0x5624, 167, 5));

        m_DataBoxGUI->Add(new CGUIGumppic(0x55F4, 218, 4));

        if (ShowWindow)
        {
            m_DataBoxGUI->Add(
                new CGUIButton(ID_GCH_WALL_SHOW_WINDOW, 0x562E, 0x562F, 0x5630, 228, 9));
        }
        else
        {
            m_DataBoxGUI->Add(
                new CGUIButton(ID_GCH_WALL_SHOW_WINDOW, 0x562B, 0x562C, 0x562D, 228, 9));
        }
    }
}

void CGumpCustomHouse::DrawDoorSection()
{
    if (Page >= 0 && Page < (int)m_Doors.size())
    {
        CCustomHouseObjectDoor& item = m_Doors[Page];

        int x = 0;
        int y = 0;

        m_DataBox->Add(new CGUIScissor(true, 0, 0, 138, 36, 384, 120));

        for (int i = 0; i < 8; i++)
        {
            u16 graphic = item.m_Graphics[i];

            if (graphic != 0u)
            {
                Core::Vec2<i32> dims = g_Orion.GetStaticArtDimension(graphic);

                int offsetX = x + 138 + (48 - dims.x) / 2;

                if (i > 3)
                {
                    offsetX -= 20;
                }

                int offsetY = y + 36 + (120 - dims.y) / 2;

                m_DataBox->Add(new CGUITilepic(graphic, 0, offsetX, offsetY));

                m_DataBox->Add(new CGUIHitBox(
                    ID_GCH_ITEM_IN_LIST + (int)i, offsetX, offsetY, dims.x, dims.y, true));
            }

            x += 48;
        }

        int direction = 0;

        switch (item.Category)
        {
            case 16:
            case 17:
            case 18: direction = 1; break;
            case 15: direction = 2; break;
            case 19:
            case 20:
            case 21:
            case 22:
            case 23:
            case 26:
            case 27:
            case 28:
            case 29:
            case 31:
            case 32:
            case 34: direction = 3; break;
            case 30:
            case 33: direction = 4; break;
            default: break;
        }

        switch (direction)
        {
            case 0:
            {
                m_DataBox->Add(new CGUIGumppic(0x5780, 151, 39));
                m_DataBox->Add(new CGUIGumppic(0x5781, 196, 39));
                m_DataBox->Add(new CGUIGumppic(0x5782, 219, 133));
                m_DataBox->Add(new CGUIGumppic(0x5783, 266, 136));
                m_DataBox->Add(new CGUIGumppic(0x5784, 357, 136));
                m_DataBox->Add(new CGUIGumppic(0x5785, 404, 133));
                m_DataBox->Add(new CGUIGumppic(0x5786, 431, 39));
                m_DataBox->Add(new CGUIGumppic(0x5787, 474, 39));
                break;
            }
            case 1:
            {
                m_DataBox->Add(new CGUIGumppic(0x5785, 245, 39));
                m_DataBox->Add(new CGUIGumppic(0x5787, 290, 39));
                m_DataBox->Add(new CGUIGumppic(0x5780, 337, 39));
                m_DataBox->Add(new CGUIGumppic(0x5782, 380, 39));
                break;
            }
            case 2:
            {
                m_DataBox->Add(new CGUIGumppic(0x5782, 219, 133));
                m_DataBox->Add(new CGUIGumppic(0x5785, 404, 133));
                break;
            }
            case 3:
            {
                m_DataBox->Add(new CGUIGumppic(0x5780, 245, 39));
                m_DataBox->Add(new CGUIGumppic(0x5781, 290, 39));
                m_DataBox->Add(new CGUIGumppic(0x5786, 337, 39));
                m_DataBox->Add(new CGUIGumppic(0x5787, 380, 39));
                break;
            }
            case 4:
            {
                m_DataBox->Add(new CGUIGumppic(0x5780, 151, 39));
                m_DataBox->Add(new CGUIGumppic(0x5781, 196, 39));
                m_DataBox->Add(new CGUIGumppic(0x5780, 245, 39));
                m_DataBox->Add(new CGUIGumppic(0x5781, 290, 39));
                m_DataBox->Add(new CGUIGumppic(0x5786, 337, 39));
                m_DataBox->Add(new CGUIGumppic(0x5787, 380, 39));
                m_DataBox->Add(new CGUIGumppic(0x5786, 431, 39));
                m_DataBox->Add(new CGUIGumppic(0x5787, 474, 39));
                break;
            }
            default: break;
        }

        m_DataBox->Add(new CGUIScissor(false));
    }
}

void CGumpCustomHouse::DrawFloorSection()
{
    if (Page >= 0 && Page < (int)m_Floors.size())
    {
        CCustomHouseObjectFloor& item = m_Floors[Page];

        int x = 0;
        int y = 0;

        m_DataBox->Add(new CGUIScissor(true, 0, 0, 123, 36, 384, 120));

        int index = 0;

        for (int j = 0; j < 2; j++)
        {
            for (int i = 0; i < 8; i++)
            {
                u16 graphic = item.m_Graphics[index];

                if (graphic != 0u)
                {
                    Core::Vec2<i32> dims = g_Orion.GetStaticArtDimension(graphic);

                    int offsetX = x + 123 + (48 - dims.x) / 2;
                    int offsetY = y + 36 + (60 - dims.y) / 2;

                    m_DataBox->Add(new CGUITilepic(graphic, 0, offsetX, offsetY));

                    m_DataBox->Add(new CGUIHitBox(
                        ID_GCH_ITEM_IN_LIST + index, offsetX, offsetY, dims.x, dims.y, true));
                }

                x += 48;
                index++;
            }

            x = 0;
            y += 60;
        }

        m_DataBox->Add(new CGUIScissor(false));
    }
}

void CGumpCustomHouse::DrawStairSection()
{
    if (Page >= 0 && Page < (int)m_Stairs.size())
    {
        CCustomHouseObjectStair& item = m_Stairs[Page];

        for (int j = 0; j < 2; j++)
        {
            int x = (j != 0 ? 96 : 192);
            int y = (j != 0 ? 60 : 0);

            m_DataBox->Add(new CGUIScissor(true, 0, 0, 123, 36 + y, 384, 60));

            CGUIText* text =
                (CGUIText*)m_DataBox->Add(new CGUIText(0xFFFF, 137, (j != 0 ? 111 : 51)));
            text->CreateTextureW(
                0, g_ClilocManager.GetCliloc(g_Language)->GetW(1062113 + (int)j), 30, 90);

            int start         = (j != 0 ? 0 : 5);
            int end           = (j != 0 ? 6 : 9);
            int combinedStair = (j != 0 ? 0 : 10);

            for (int i = start; i < end; i++)
            {
                u16 graphic = item.m_Graphics[i];

                if (graphic != 0u)
                {
                    Core::Vec2<i32> dims = g_Orion.GetStaticArtDimension(graphic);

                    int offsetX = x + 123 + (48 - dims.x) / 2;
                    int offsetY = y + 36 + (60 - dims.y) / 2;

                    m_DataBox->Add(new CGUITilepic(graphic, 0, offsetX, offsetY));

                    m_DataBox->Add(new CGUIHitBox(
                        ID_GCH_ITEM_IN_LIST + (int)i + combinedStair,
                        offsetX,
                        offsetY,
                        dims.x,
                        dims.y,
                        true));
                }

                x += 48;
            }

            m_DataBox->Add(new CGUIScissor(false));
        }

        m_DataBox->Add(new CGUIColoredPolygone(0, 0, 123, 96, 384, 2, 0xFF7F7F7F));
    }
}

void CGumpCustomHouse::DrawRoofSection()
{
    int x = 0;
    int y = 0;

    if (Category == -1)
    {
        int startCategory = Page * 16;
        int endCategory   = startCategory + 16;

        if (endCategory > (int)m_Roofs.size())
        {
            endCategory = (int)m_Roofs.size();
        }

        m_DataBox->Add(new CGUIScissor(true, 0, 0, 121, 36, 384, 60));

        for (int i = startCategory; i < endCategory; i++)
        {
            const std::vector<CCustomHouseObjectRoof>& vec = m_Roofs[i].m_Items;

            if (vec.empty())
            {
                continue;
            }

            Core::Vec2<i32> dims = g_Orion.GetStaticArtDimension(vec[0].NSCrosspiece);

            int offsetX = x + 121 + (48 - dims.x) / 2;
            int offsetY = y + 36;

            m_DataBox->Add(new CGUITilepic(vec[0].NSCrosspiece, 0, offsetX, offsetY));

            m_DataBox->Add(new CGUIHitBox(
                ID_GCH_ITEM_IN_LIST + (int)i, offsetX, offsetY, dims.x, dims.y, true));

            x += 48;

            if (x >= 384)
            {
                x = 0;
                y += 60;

                m_DataBox->Add(new CGUIScissor(false));
                m_DataBox->Add(new CGUIScissor(true, 0, 0, 121, 96, 384, 60));
            }
        }

        m_DataBox->Add(new CGUIScissor(false));
    }
    else if (Category >= 0 && Category < (int)m_Roofs.size())
    {
        const std::vector<CCustomHouseObjectRoof>& vec = m_Roofs[Category].m_Items;

        if (Page >= 0 && Page < (int)vec.size())
        {
            const CCustomHouseObjectRoof& item = vec[Page];

            m_DataBox->Add(new CGUIScissor(true, 0, 0, 130, 44, 384, 120));

            int index = 0;

            for (int j = 0; j < 2; j++)
            {
                for (int i = 0; i < 8; i++)
                {
                    u16 graphic = item.m_Graphics[index];

                    if (graphic != 0u)
                    {
                        Core::Vec2<i32> dims = g_Orion.GetStaticArtDimension(graphic);

                        int offsetX = x + 130 + (48 - dims.x) / 2;
                        int offsetY = y + 44 + (60 - dims.y) / 2;

                        m_DataBox->Add(new CGUITilepic(graphic, 0, offsetX, offsetY));

                        m_DataBox->Add(new CGUIHitBox(
                            ID_GCH_ITEM_IN_LIST + index, offsetX, offsetY, dims.x, dims.y, true));
                    }

                    x += 48;
                    index++;
                }

                x = 0;
                y += 60;
            }

            m_DataBox->Add(new CGUIScissor(false));
        }

        m_DataBoxGUI->Add(new CGUIGumppic(0x55F3, 152, 0));
        m_DataBoxGUI->Add(new CGUIButton(ID_GCH_GO_CATEGORY, 0x5622, 0x5623, 0x5624, 167, 5));

        m_DataBoxGUI->Add(new CGUIButton(ID_GCH_ROOF_Z_DOWN, 0x578B, 0x578C, 0x578D, 305, 0));
        m_DataBoxGUI->Add(new CGUIButton(ID_GCH_ROOF_Z_UP, 0x578E, 0x578F, 0x5790, 349, 0));

        CGUIGumppic* gumppic = (CGUIGumppic*)m_DataBoxGUI->Add(new CGUIGumppic(0x55F4, 383, 4));
        gumppic->Serial      = ID_GCH_AREA_ROOF_Z_INFO;
        gumppic->MoveOnDrag  = true;

        CGUIText* text = (CGUIText*)m_DataBoxGUI->Add(new CGUIText(0x04E9, 405, 15));
        text->CreateTextureA(3, std::to_string(RoofZ));
        text->Serial     = ID_GCH_AREA_ROOF_Z_INFO;
        text->MoveOnDrag = true;
    }
}

void CGumpCustomHouse::DrawMiscSection()
{
    int x = 0;
    int y = 0;

    if (Category == -1)
    {
        int startCategory = Page * 16;
        int endCategory   = startCategory + 16;

        if (endCategory > (int)m_Miscs.size())
        {
            endCategory = (int)m_Miscs.size();
        }

        m_DataBox->Add(new CGUIScissor(true, 0, 0, 121, 36, 384, 60));

        for (int i = startCategory; i < endCategory; i++)
        {
            const std::vector<CCustomHouseObjectMisc>& vec = m_Miscs[i].m_Items;

            if (vec.empty())
            {
                continue;
            }

            Core::Vec2<i32> dims = g_Orion.GetStaticArtDimension(vec[0].Piece5);

            int offsetX = x + 121 + (48 - dims.x) / 2;
            int offsetY = y + 36;

            m_DataBox->Add(new CGUITilepic(vec[0].Piece5, 0, offsetX, offsetY));

            m_DataBox->Add(new CGUIHitBox(
                ID_GCH_ITEM_IN_LIST + (int)i, offsetX, offsetY, dims.x, dims.y, true));

            x += 48;

            if (x >= 384)
            {
                x = 0;
                y += 60;

                m_DataBox->Add(new CGUIScissor(false));
                m_DataBox->Add(new CGUIScissor(true, 0, 0, 121, 96, 384, 60));
            }
        }

        m_DataBox->Add(new CGUIScissor(false));
    }
    else if (Category >= 0 && Category < (int)m_Miscs.size())
    {
        const std::vector<CCustomHouseObjectMisc>& vec = m_Miscs[Category].m_Items;

        if (Page >= 0 && Page < (int)vec.size())
        {
            const CCustomHouseObjectMisc& item = vec[Page];

            m_DataBox->Add(new CGUIScissor(true, 0, 0, 130, 44, 384, 120));

            for (int i = 0; i < 8; i++)
            {
                u16 graphic = item.m_Graphics[i];

                if (graphic != 0u)
                {
                    Core::Vec2<i32> dims = g_Orion.GetStaticArtDimension(graphic);

                    int offsetX = x + 130 + (48 - dims.x) / 2;
                    int offsetY = y + 44 + (120 - dims.y) / 2;

                    m_DataBox->Add(new CGUITilepic(graphic, 0, offsetX, offsetY));

                    m_DataBox->Add(new CGUIHitBox(
                        ID_GCH_ITEM_IN_LIST + (int)i, offsetX, offsetY, dims.x, dims.y, true));
                }

                x += 48;
            }

            m_DataBox->Add(new CGUIScissor(false));
        }

        m_DataBoxGUI->Add(new CGUIGumppic(0x55F3, 152, 0));
        m_DataBoxGUI->Add(new CGUIButton(ID_GCH_GO_CATEGORY, 0x5622, 0x5623, 0x5624, 167, 5));
    }
}

void CGumpCustomHouse::DrawMenuSection()
{
    const int textWidth = 108;

    m_DataBox->Add(new CGUIButton(ID_GCH_MENU_BACKUP, 0x098D, 0x098D, 0x098D, 150, 50));
    CGUITextEntry* entry = (CGUITextEntry*)m_DataBox->Add(new CGUITextEntry(
        ID_GCH_MENU_BACKUP, 0, 0x0036, 0x0036, 150, 50, textWidth, true, 0, TS_CENTER));
    entry->m_Entry.SetTextW(L"Backup");
    entry->CheckOnSerial  = true;
    entry->ReadOnly       = true;
    entry->FocusedOffsetY = 2;

    m_DataBox->Add(new CGUIButton(ID_GCH_MENU_RESTORE, 0x098D, 0x098D, 0x098D, 150, 90));
    entry = (CGUITextEntry*)m_DataBox->Add(new CGUITextEntry(
        ID_GCH_MENU_RESTORE, 0, 0x0036, 0x0036, 150, 90, textWidth, true, 0, TS_CENTER));
    entry->m_Entry.SetTextW(L"Restore");
    entry->CheckOnSerial  = true;
    entry->ReadOnly       = true;
    entry->FocusedOffsetY = 2;

    m_DataBox->Add(new CGUIButton(ID_GCH_MENU_SYNCH, 0x098D, 0x098D, 0x098D, 270, 50));
    entry = (CGUITextEntry*)m_DataBox->Add(new CGUITextEntry(
        ID_GCH_MENU_SYNCH, 0, 0x0036, 0x0036, 270, 50, textWidth, true, 0, TS_CENTER));
    entry->m_Entry.SetTextW(L"Synch");
    entry->CheckOnSerial  = true;
    entry->ReadOnly       = true;
    entry->FocusedOffsetY = 2;

    m_DataBox->Add(new CGUIButton(ID_GCH_MENU_CLEAR, 0x098D, 0x098D, 0x098D, 270, 90));
    entry = (CGUITextEntry*)m_DataBox->Add(new CGUITextEntry(
        ID_GCH_MENU_CLEAR, 0, 0x0036, 0x0036, 270, 90, textWidth, true, 0, TS_CENTER));
    entry->m_Entry.SetTextW(L"Clear");
    entry->CheckOnSerial  = true;
    entry->ReadOnly       = true;
    entry->FocusedOffsetY = 2;

    m_DataBox->Add(new CGUIButton(ID_GCH_MENU_COMMIT, 0x098D, 0x098D, 0x098D, 390, 50));
    entry = (CGUITextEntry*)m_DataBox->Add(new CGUITextEntry(
        ID_GCH_MENU_COMMIT, 0, 0x0036, 0x0036, 390, 50, textWidth, true, 0, TS_CENTER));
    entry->m_Entry.SetTextW(L"Commit");
    entry->CheckOnSerial  = true;
    entry->ReadOnly       = true;
    entry->FocusedOffsetY = 2;

    m_DataBox->Add(new CGUIButton(ID_GCH_MENU_REVERT, 0x098D, 0x098D, 0x098D, 390, 90));
    entry = (CGUITextEntry*)m_DataBox->Add(new CGUITextEntry(
        ID_GCH_MENU_REVERT, 0, 0x0036, 0x0036, 390, 90, textWidth, true, 0, TS_CENTER));
    entry->m_Entry.SetTextW(L"Revert");
    entry->CheckOnSerial  = true;
    entry->ReadOnly       = true;
    entry->FocusedOffsetY = 2;
}

template <class T, class A>
std::pair<int, int>
SeekGraphicInCustomHouseObjectListWithCategory(const std::vector<A>& list, u16 graphic)
{
    for (int i = 0; i < (int)list.size(); i++)
    {
        const A& cat = list[i];

        for (int j = 0; j < (int)cat.m_Items.size(); j++)
        {
            int contains = cat.m_Items[j].Contains(graphic);

            if (contains != -1)
            {
                return std::pair<int, int>((int)i, (int)j);
            }
        }
    }

    return std::pair<int, int>(-1, -1);
}

template <class T>
std::pair<int, int> SeekGraphicInCustomHouseObjectList(const std::vector<T>& list, u16 graphic)
{
    for (int i = 0; i < (int)list.size(); i++)
    {
        int contains = list[i].Contains(graphic);

        if (contains != -1)
        {
            return std::pair<int, int>((int)i, graphic);
        }
    }

    return std::pair<int, int>(-1, -1);
}

std::pair<int, int> CGumpCustomHouse::ExistsInList(CUSTOM_HOUSE_GUMP_STATE& state, u16 graphic)
{
    std::pair<int, int> result = SeekGraphicInCustomHouseObjectListWithCategory<
        CCustomHouseObjectWall,
        CCustomHouseObjectWallCategory>(m_Walls, graphic);

    if (result.first == -1 || result.second == -1)
    {
        result = SeekGraphicInCustomHouseObjectList<CCustomHouseObjectFloor>(m_Floors, graphic);

        if (result.first == -1 || result.second == -1)
        {
            result = SeekGraphicInCustomHouseObjectList<CCustomHouseObjectDoor>(m_Doors, graphic);

            if (result.first == -1 || result.second == -1)
            {
                result = SeekGraphicInCustomHouseObjectListWithCategory<
                    CCustomHouseObjectMisc,
                    CCustomHouseObjectMiscCategory>(m_Miscs, graphic);

                if (result.first == -1 || result.second == -1)
                {
                    result = SeekGraphicInCustomHouseObjectList<CCustomHouseObjectStair>(
                        m_Stairs, graphic);

                    if (result.first == -1 || result.second == -1)
                    {
                        result = SeekGraphicInCustomHouseObjectListWithCategory<
                            CCustomHouseObjectRoof,
                            CCustomHouseObjectRoofCategory>(m_Roofs, graphic);

                        if (result.first != -1 && result.second != -1)
                        {
                            state = CHGS_ROOF;
                        }
                    }
                    else
                    {
                        state = CHGS_STAIR;
                    }
                }
                else
                {
                    state = CHGS_MISC;
                }
            }
            else
            {
                state = CHGS_DOOR;
            }
        }
        else
        {
            state = CHGS_FLOOR;
        }
    }
    else
    {
        state = CHGS_WALL;
    }

    return result;
}

void CGumpCustomHouse::UpdateContent()
{
    m_DataBox->Clear();
    m_DataBoxGUI->Clear();

    m_Gumppic->Graphic = (FloorCount == 4 ? 0x55F2 : 0x55F9);

    m_DataBoxGUI->Add(
        new CGUIButton(ID_GCH_STATE_ERASE, 0x5666 + (int)Erasing, 0x5667, 0x5668, 9, 100));
    Add(new CGUIButton(ID_GCH_STATE_EYEDROPPER, 0x5669 + (int)SeekTile, 0x566A, 0x566B, 39, 100));

    u16 floorVisionGraphic1[3]   = { 0x572E, 0x5734, 0x5731 };
    u16 floorVisionGraphic2[3]   = { 0x5725, 0x5728, 0x572B };
    u16 floorVisionGraphic3[3]   = { 0x571C, 0x571F, 0x5722 };
    int associateGraphicTable[7] = { 0, 1, 2, 1, 2, 1, 2 };

    u16 floorVisionGraphic = floorVisionGraphic1[associateGraphicTable[m_FloorVisionState[0]]];
    int graphicOffset      = (CurrentFloor == 1 ? 3 : 0);
    int graphicOffset2     = (CurrentFloor == 1 ? 4 : 0);
    m_DataBoxGUI->Add(new CGUIButton(
        ID_GCH_VISIBILITY_STORY_1,
        floorVisionGraphic,
        floorVisionGraphic + 1,
        floorVisionGraphic + 2,
        533,
        108));
    m_DataBoxGUI->Add(new CGUIButton(
        ID_GCH_GO_FLOOR_1, 0x56CD + graphicOffset2, 0x56CD + graphicOffset2, 0x56D1, 583, 96));
    m_DataBoxGUI->Add(new CGUIButton(
        ID_GCH_GO_FLOOR_1,
        0x56F6 + graphicOffset,
        0x56F7 + graphicOffset,
        0x56F8 + graphicOffset,
        623,
        103));

    floorVisionGraphic = floorVisionGraphic2[associateGraphicTable[m_FloorVisionState[1]]];
    graphicOffset      = (CurrentFloor == 2 ? 3 : 0);
    graphicOffset2     = (CurrentFloor == 2 ? 4 : 0);
    m_DataBoxGUI->Add(new CGUIButton(
        ID_GCH_VISIBILITY_STORY_2,
        floorVisionGraphic,
        floorVisionGraphic + 1,
        floorVisionGraphic + 2,
        533,
        86));
    m_DataBoxGUI->Add(new CGUIButton(
        ID_GCH_GO_FLOOR_2, 0x56CE + graphicOffset2, 0x56CE + graphicOffset2, 0x56D2, 583, 73));
    m_DataBoxGUI->Add(new CGUIButton(
        ID_GCH_GO_FLOOR_2,
        0x56F0 + graphicOffset,
        0x56F1 + graphicOffset,
        0x56F2 + graphicOffset,
        623,
        86));

    graphicOffset  = (CurrentFloor == 3 ? 3 : 0);
    graphicOffset2 = (CurrentFloor == 3 ? 4 : 0);

    if (FloorCount == 4)
    {
        floorVisionGraphic = floorVisionGraphic2[associateGraphicTable[m_FloorVisionState[2]]];
        m_DataBoxGUI->Add(new CGUIButton(
            ID_GCH_VISIBILITY_STORY_3,
            floorVisionGraphic,
            floorVisionGraphic + 1,
            floorVisionGraphic + 2,
            533,
            64));
        m_DataBoxGUI->Add(new CGUIButton(
            ID_GCH_GO_FLOOR_3, 0x56CE + graphicOffset2, 0x56CE + graphicOffset2, 0x56D2, 582, 56));
        m_DataBoxGUI->Add(new CGUIButton(
            ID_GCH_GO_FLOOR_3,
            0x56F0 + graphicOffset,
            0x56F1 + graphicOffset,
            0x56F2 + graphicOffset,
            623,
            69));

        floorVisionGraphic = floorVisionGraphic3[associateGraphicTable[m_FloorVisionState[3]]];
        graphicOffset      = (CurrentFloor == 4 ? 3 : 0);
        graphicOffset2     = (CurrentFloor == 4 ? 4 : 0);
        m_DataBoxGUI->Add(new CGUIButton(
            ID_GCH_VISIBILITY_STORY_4,
            floorVisionGraphic,
            floorVisionGraphic + 1,
            floorVisionGraphic + 2,
            533,
            42));
        m_DataBoxGUI->Add(new CGUIButton(
            ID_GCH_GO_FLOOR_4, 0x56D0 + graphicOffset2, 0x56D0 + graphicOffset2, 0x56D4, 583, 42));
        m_DataBoxGUI->Add(new CGUIButton(
            ID_GCH_GO_FLOOR_4,
            0x56EA + graphicOffset,
            0x56EB + graphicOffset,
            0x56EC + graphicOffset,
            623,
            50));
    }
    else
    {
        floorVisionGraphic = floorVisionGraphic3[associateGraphicTable[m_FloorVisionState[2]]];
        m_DataBoxGUI->Add(new CGUIButton(
            ID_GCH_VISIBILITY_STORY_3,
            floorVisionGraphic,
            floorVisionGraphic + 1,
            floorVisionGraphic + 2,
            533,
            64));
        m_DataBoxGUI->Add(new CGUIButton(
            ID_GCH_GO_FLOOR_3, 0x56D0 + graphicOffset2, 0x56D0 + graphicOffset2, 0x56D4, 582, 56));
        m_DataBoxGUI->Add(new CGUIButton(
            ID_GCH_GO_FLOOR_3,
            0x56EA + graphicOffset,
            0x56EB + graphicOffset,
            0x56EC + graphicOffset,
            623,
            69));
    }

    switch (State)
    {
        case CHGS_WALL:
        {
            DrawWallSection();
            break;
        }
        case CHGS_DOOR:
        {
            DrawDoorSection();
            break;
        }
        case CHGS_FLOOR:
        {
            DrawFloorSection();
            break;
        }
        case CHGS_STAIR:
        {
            DrawStairSection();
            break;
        }
        case CHGS_ROOF:
        {
            DrawRoofSection();
            break;
        }
        case CHGS_MISC:
        {
            DrawMiscSection();
            break;
        }
        case CHGS_MENU:
        {
            DrawMenuSection();
            break;
        }
        default: break;
    }

    if (MaxPage > 1)
    {
        m_DataBoxGUI->Add(new CGUIButton(ID_GCH_LIST_LEFT, 0x5625, 0x5626, 0x5627, 110, 63));
        m_DataBoxGUI->Add(new CGUIButton(ID_GCH_LIST_RIGHT, 0x5628, 0x5629, 0x562A, 510, 63));
    }

    Components = 0;
    Fixtures   = 0;

    CGameItem* foundationItem = g_World->GetWorldItem(Serial);

    if (foundationItem != nullptr)
    {
        QFOR(multi, foundationItem->m_Items, CMulti*)
        {
            QFOR(item, multi->m_Items, CMultiObject*)
            {
                if (item->IsCustomHouseMulti() && ((item->State & CHMOF_GENERIC_INTERNAL) == 0))
                {
                    CUSTOM_HOUSE_GUMP_STATE state;
                    std::pair<int, int> result = ExistsInList(state, item->Graphic);

                    if (result.first != -1 && result.second != -1)
                    {
                        if (state == CHGS_DOOR)
                        {
                            Fixtures++;
                        }
                        else
                        {
                            Components++;
                        }
                    }
                }
            }
        }
    }

    m_TextComponents->Color = (Components >= MaxComponents ? 0x0026 : 0x0481);
    m_TextComponents->CreateTextureA(9, std::to_string(Components));
    m_TextComponents->SetX(82 - m_TextComponents->m_Texture.Width);

    m_TextFixtures->Color = (Fixtures >= MaxFixtures ? 0x0026 : 0x0481);
    m_TextFixtures->CreateTextureA(9, std::to_string(Fixtures));

    m_TextCost->CreateTextureA(9, std::to_string((Components + Fixtures) * 500));
}

void CGumpCustomHouse::UpdateMaxPage()
{
    MaxPage = 1;

    switch (State)
    {
        case CHGS_WALL:
        {
            if (Category == -1)
            {
                MaxPage = (int)ceil(m_Walls.size() / 16.0f);
            }
            else
            {
                for (const CCustomHouseObjectWallCategory& cat : m_Walls)
                {
                    if (cat.Index == Category)
                    {
                        MaxPage = (int)cat.m_Items.size();
                        break;
                    }
                }
            }

            break;
        }
        case CHGS_DOOR:
        {
            MaxPage = (int)m_Doors.size();

            break;
        }
        case CHGS_FLOOR:
        {
            MaxPage = (int)m_Floors.size();

            break;
        }
        case CHGS_STAIR:
        {
            MaxPage = (int)m_Stairs.size();

            break;
        }
        case CHGS_ROOF:
        {
            if (Category == -1)
            {
                MaxPage = (int)ceil(m_Roofs.size() / 16.0f);
            }
            else
            {
                for (const CCustomHouseObjectRoofCategory& cat : m_Roofs)
                {
                    if (cat.Index == Category)
                    {
                        MaxPage = (int)cat.m_Items.size();
                        break;
                    }
                }
            }

            break;
        }
        case CHGS_MISC:
        {
            if (Category == -1)
            {
                MaxPage = (int)ceil(m_Miscs.size() / 16.0f);
            }
            else
            {
                for (const CCustomHouseObjectMiscCategory& cat : m_Miscs)
                {
                    if (cat.Index == Category)
                    {
                        MaxPage = (int)cat.m_Items.size();
                        break;
                    }
                }
            }

            break;
        }
        default: break;
    }
}

void CGumpCustomHouse::SeekGraphic(u16 graphic)
{
    CUSTOM_HOUSE_GUMP_STATE state;
    std::pair<int, int> result = ExistsInList(state, graphic);

    if (result.first != -1 && result.second != -1)
    {
        State = state;

        if (State == CHGS_WALL || State == CHGS_ROOF || State == CHGS_MISC)
        {
            Category = result.first;
            Page     = result.second;
        }
        else
        {
            Category = -1;
            Page     = result.first;
        }

        UpdateMaxPage();
        g_Target.RequestFromCustomHouse();
        WantUpdateContent = true;
        SelectedGraphic   = graphic;
    }
}

bool CGumpCustomHouse::CanBuildHere(
    std::vector<CBuildObject>& list, CRenderWorldObject* place, CUSTOM_HOUSE_BUILD_TYPE& type)
{
    type = CHBT_NORMAL;

    if (SelectedGraphic == 0u)
    {
        return false;
    }

    if (CombinedStair)
    {
        if (Components + 10 > MaxComponents)
        {
            return false;
        }

        std::pair<int, int> result =
            SeekGraphicInCustomHouseObjectList<CCustomHouseObjectStair>(m_Stairs, SelectedGraphic);

        if (result.first == -1 || result.second == -1 || result.first >= (int)m_Stairs.size())
        {
            list.push_back(CBuildObject(SelectedGraphic, 0, 0, 0));
            return false;
        }

        const CCustomHouseObjectStair& item = m_Stairs[result.first];

        if (SelectedGraphic == item.North)
        {
            list.push_back(CBuildObject(item.Block, 0, -3, 0));
            list.push_back(CBuildObject(item.Block, 0, -2, 0));
            list.push_back(CBuildObject(item.Block, 0, -1, 0));
            list.push_back(CBuildObject(item.North, 0, 0, 0));
            list.push_back(CBuildObject(item.Block, 0, -3, 5));
            list.push_back(CBuildObject(item.Block, 0, -2, 5));
            list.push_back(CBuildObject(item.North, 0, -1, 5));
            list.push_back(CBuildObject(item.Block, 0, -3, 10));
            list.push_back(CBuildObject(item.North, 0, -2, 10));
            list.push_back(CBuildObject(item.North, 0, -3, 15));
        }
        else if (SelectedGraphic == item.East)
        {
            list.push_back(CBuildObject(item.East, 0, 0, 0));
            list.push_back(CBuildObject(item.Block, 1, 0, 0));
            list.push_back(CBuildObject(item.Block, 2, 0, 0));
            list.push_back(CBuildObject(item.Block, 3, 0, 0));
            list.push_back(CBuildObject(item.East, 1, 0, 5));
            list.push_back(CBuildObject(item.Block, 2, 0, 5));
            list.push_back(CBuildObject(item.Block, 3, 0, 5));
            list.push_back(CBuildObject(item.East, 2, 0, 10));
            list.push_back(CBuildObject(item.Block, 3, 0, 10));
            list.push_back(CBuildObject(item.East, 3, 0, 15));
        }
        else if (SelectedGraphic == item.South)
        {
            list.push_back(CBuildObject(item.South, 0, 0, 0));
            list.push_back(CBuildObject(item.Block, 0, 1, 0));
            list.push_back(CBuildObject(item.Block, 0, 2, 0));
            list.push_back(CBuildObject(item.Block, 0, 3, 0));
            list.push_back(CBuildObject(item.South, 0, 1, 5));
            list.push_back(CBuildObject(item.Block, 0, 2, 5));
            list.push_back(CBuildObject(item.Block, 0, 3, 5));
            list.push_back(CBuildObject(item.South, 0, 2, 10));
            list.push_back(CBuildObject(item.Block, 0, 3, 10));
            list.push_back(CBuildObject(item.South, 0, 3, 15));
        }
        else if (SelectedGraphic == item.West)
        {
            list.push_back(CBuildObject(item.Block, -3, 0, 0));
            list.push_back(CBuildObject(item.Block, -2, 0, 0));
            list.push_back(CBuildObject(item.Block, -1, 0, 0));
            list.push_back(CBuildObject(item.West, 0, 0, 0));
            list.push_back(CBuildObject(item.Block, -3, 0, 5));
            list.push_back(CBuildObject(item.Block, -2, 0, 5));
            list.push_back(CBuildObject(item.West, -1, 0, 5));
            list.push_back(CBuildObject(item.Block, -3, 0, 10));
            list.push_back(CBuildObject(item.West, -2, 0, 10));
            list.push_back(CBuildObject(item.West, -3, 0, 15));
        }
        else
        {
            list.push_back(CBuildObject(SelectedGraphic, 0, 0, 0));
        }

        type = CHBT_STAIR;
    }
    else
    {
        std::pair<int, int> fixtureCheck =
            SeekGraphicInCustomHouseObjectList<CCustomHouseObjectDoor>(m_Doors, SelectedGraphic);
        bool isFixture = false;

        if (fixtureCheck.first == -1 || fixtureCheck.second == -1)
        {
            fixtureCheck = SeekGraphicInCustomHouseObjectList<CCustomHouseObjectTeleport>(
                m_Teleports, SelectedGraphic);
            isFixture = (fixtureCheck.first != -1 && fixtureCheck.second != -1);
        }
        else
        {
            isFixture = true;
        }

        if (isFixture)
        {
            if (Fixtures + 1 > MaxFixtures)
            {
                return false;
            }
        }
        else if (Components + 1 > MaxComponents)
        {
            return false;
        }

        if (State == CHGS_ROOF)
        {
            list.push_back(CBuildObject(SelectedGraphic, 0, 0, (RoofZ - 2) * 3));
            type = CHBT_ROOF;
        }
        else
        {
            if (State == CHGS_STAIR)
            {
                type = CHBT_STAIR;
                list.push_back(CBuildObject(SelectedGraphic, 0, 1, 0));
            }
            else
            {
                if (State == CHGS_FLOOR)
                {
                    type = CHBT_FLOOR;
                }

                list.push_back(CBuildObject(SelectedGraphic, 0, 0, 0));
            }
        }
    }

    if (g_SelectedObject.Object != nullptr && g_SelectedObject.Object->IsWorldObject())
    {
        CRenderWorldObject* rwo = (CRenderWorldObject*)g_SelectedObject.Object;

        if ((type != CHBT_STAIR || CombinedStair) && rwo->GetZ() < MinHouseZ &&
            (rwo->GetX() == EndPos.x - 1 || rwo->GetY() == EndPos.y - 1))
        {
            return false;
        }

        CGameItem* foundationItem = g_World->GetWorldItem(Serial);

        int minZ =
            (foundationItem != nullptr ? foundationItem->GetZ() : 0) + 7 + (CurrentFloor - 1) * 20;
        int maxZ = minZ + 20;

        int boundsOffset = (int)(State != CHGS_WALL);
        Core::Rect<int> rect(
            StartPos.x + boundsOffset, StartPos.y + boundsOffset, EndPos.x, EndPos.y);

        for (const CBuildObject& item : list)
        {
            if (type == CHBT_STAIR)
            {
                if (CombinedStair)
                {
                    if (item.Z != 0)
                    {
                        continue;
                    }
                }
                else
                {
                    if (rwo->GetY() + item.Y < EndPos.y || rwo->GetX() + item.X == StartPos.x ||
                        rwo->GetZ() >= MinHouseZ)
                    {
                        return false;
                    }
                    if (rwo->GetY() + item.Y != EndPos.y)
                    {
                        list[0].Y = 0;
                    }

                    continue;
                }
            }

            if (!ValidateItemPlace(rect, item.Graphic, rwo->GetX() + item.X, rwo->GetY() + item.Y))
            {
                return false;
            }

            if (type != CHBT_FLOOR && foundationItem != nullptr)
            {
                CMulti* multi =
                    foundationItem->GetMultiAtXY(rwo->GetX() + item.X, rwo->GetY() + item.Y);

                if (multi != nullptr)
                {
                    QFOR(multiObject, multi->m_Items, CMultiObject*)
                    {
                        if (multiObject->IsCustomHouseMulti() &&
                            ((multiObject->State & CHMOF_GENERIC_INTERNAL) == 0) &&
                            multiObject->GetZ() >= minZ && multiObject->GetZ() < maxZ)
                        {
                            if (type == CHBT_STAIR)
                            {
                                if ((multiObject->State & CHMOF_FLOOR) == 0)
                                {
                                    return false;
                                }
                            }
                            else
                            {
                                if ((multiObject->State & CHMOF_STAIR) != 0)
                                {
                                    return false;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    else
    {
        return false;
    }

    return true;
}

bool CGumpCustomHouse::ValidateItemPlace(const Core::Rect<int>& rect, u16 graphic, int x, int y)
{
    if (!rect.contains(x, y))
        return false;

    std::pair<int, int> infoCheck =
        SeekGraphicInCustomHouseObjectList<CCustomHouseObjectPlaceInfo>(m_ObjectsInfo, graphic);

    if (infoCheck.first != -1 && infoCheck.second != -1)
    {
        const CCustomHouseObjectPlaceInfo& info = m_ObjectsInfo[infoCheck.first];

        if ((info.CanGoW == 0) && x == StartPos.x)
        {
            return false;
        }
        if ((info.CanGoN == 0) && y == StartPos.y)
        {
            return false;
        }
        if ((info.CanGoNWS == 0) && x == StartPos.x && y == StartPos.y)
        {
            return false;
        }
    }

    return true;
}

bool CGumpCustomHouse::ValidatePlaceStructure(
    CGameItem* foundationItem, CMulti* multi, int minZ, int maxZ, int flags)
{
    if (multi == nullptr)
    {
        return false;
    }

    QFOR(item, multi->m_Items, CMultiObject*)
    {
        std::vector<Core::Vec2<i32>> validatedFloors;

        if (item->IsCustomHouseMulti() &&
            ((item->State & (CHMOF_FLOOR | CHMOF_STAIR | CHMOF_ROOF | CHMOF_FIXTURE)) == 0) &&
            item->GetZ() >= minZ && item->GetZ() < maxZ)
        {
            std::pair<int, int> infoCheck =
                SeekGraphicInCustomHouseObjectList<CCustomHouseObjectPlaceInfo>(
                    m_ObjectsInfo, item->Graphic);

            if (infoCheck.first != -1 && infoCheck.second != -1)
            {
                const CCustomHouseObjectPlaceInfo& info = m_ObjectsInfo[infoCheck.first];

                if ((flags & CHVCF_DIRECT_SUPPORT) != 0)
                {
                    if (((item->State & CHMOF_INCORRECT_PLACE) != 0) || (info.DirectSupports == 0))
                    {
                        continue;
                    }

                    if ((flags & CHVCF_CANGO_W) != 0)
                    {
                        if (info.CanGoW != 0)
                        {
                            return true;
                        }
                    }
                    else if ((flags & CHVCF_CANGO_N) != 0)
                    {
                        if (info.CanGoN != 0)
                        {
                            return true;
                        }
                    }
                    else
                    {
                        return true;
                    }
                }
                else if (
                    (((flags & CHVCF_BOTTOM) != 0) && (info.Bottom != 0)) ||
                    (((flags & CHVCF_TOP) != 0) && (info.Top != 0)))
                {
                    if ((item->State & CHMOF_VALIDATED_PLACE) == 0)
                    {
                        if (!ValidateItemPlace(foundationItem, item, minZ, maxZ, validatedFloors))
                        {
                            item->State =
                                item->State | CHMOF_VALIDATED_PLACE | CHMOF_INCORRECT_PLACE;
                        }
                        else
                        {
                            item->State = item->State | CHMOF_VALIDATED_PLACE;
                        }
                    }

                    if ((item->State & CHMOF_INCORRECT_PLACE) == 0)
                    {
                        if ((flags & CHVCF_BOTTOM) != 0)
                        {
                            if (((flags & CHVCF_N) != 0) && (info.AdjUN != 0))
                            {
                                return true;
                            }
                            if (((flags & CHVCF_E) != 0) && (info.AdjUE != 0))
                            {
                                return true;
                            }
                            if (((flags & CHVCF_S) != 0) && (info.AdjUS != 0))
                            {
                                return true;
                            }
                            if (((flags & CHVCF_W) != 0) && (info.AdjUW != 0))
                            {
                                return true;
                            }
                        }
                        else
                        {
                            if (((flags & CHVCF_N) != 0) && (info.AdjLN != 0))
                            {
                                return true;
                            }
                            if (((flags & CHVCF_E) != 0) && (info.AdjLE != 0))
                            {
                                return true;
                            }
                            if (((flags & CHVCF_S) != 0) && (info.AdjLS != 0))
                            {
                                return true;
                            }
                            if (((flags & CHVCF_W) != 0) && (info.AdjLW != 0))
                            {
                                return true;
                            }
                        }
                    }
                }
            }
        }
    }

    return false;
}

bool CGumpCustomHouse::ValidateItemPlace(
    CGameItem* foundationItem,
    CMultiObject* item,
    int minZ,
    int maxZ,
    std::vector<Core::Vec2<i32>>& validatedFloors)
{
    if (item == nullptr || !item->IsCustomHouseMulti())
    {
        return true;
    }
    if ((item->State & CHMOF_FLOOR) != 0)
    {
        auto existsInList =
            [](std::vector<Core::Vec2<i32>>& validatedFloors, const Core::Vec2<i32>& testPoint)
        {
            for (const Core::Vec2<i32>& point : validatedFloors)
            {
                if (testPoint.x == point.x && testPoint.y == point.y)
                    return true;
            }

            return false;
        };

        //if (existsInList(validatedFloors, Core::Vec2<i32>(item->GetX(), item->GetY())))
        //	return false;

        if (ValidatePlaceStructure(
                foundationItem,
                foundationItem->GetMultiAtXY(item->GetX(), item->GetY()),
                minZ - 20,
                maxZ - 20,
                CHVCF_DIRECT_SUPPORT) ||
            ValidatePlaceStructure(
                foundationItem,
                foundationItem->GetMultiAtXY(item->GetX() - 1, item->GetY()),
                minZ - 20,
                maxZ - 20,
                CHVCF_DIRECT_SUPPORT | CHVCF_CANGO_W) ||
            ValidatePlaceStructure(
                foundationItem,
                foundationItem->GetMultiAtXY(item->GetX(), item->GetY() - 1),
                minZ - 20,
                maxZ - 20,
                CHVCF_DIRECT_SUPPORT | CHVCF_CANGO_N))
        {
            const Core::Vec2<i32> table[4] = { Core::Vec2<i32>(-1, 0),
                                               Core::Vec2<i32>(0, -1),
                                               Core::Vec2<i32>(1, 0),
                                               Core::Vec2<i32>(0, 1) };

            for (int i = 0; i < 4; i++)
            {
                Core::Vec2<i32> testPoint(item->GetX() + table[i].x, item->GetY() + table[i].y);

                if (!existsInList(validatedFloors, testPoint))
                {
                    validatedFloors.push_back(testPoint);
                }
            }

            return true;
        }

        return false;
    }
    if ((item->State & (CHMOF_STAIR | CHMOF_ROOF | CHMOF_FIXTURE)) != 0)
    {
        for (CMultiObject* temp = item; temp != nullptr; temp = (CMultiObject*)temp->m_Prev)
        {
            if (((temp->State & CHMOF_FLOOR) != 0) && temp->GetZ() >= minZ && temp->GetZ() < maxZ)
            {
                if (((temp->State & CHMOF_VALIDATED_PLACE) != 0) &&
                    ((temp->State & CHMOF_INCORRECT_PLACE) == 0))
                {
                    return true;
                }
            }
        }

        for (CMultiObject* temp = item; temp != nullptr; temp = (CMultiObject*)temp->m_Next)
        {
            if (((temp->State & CHMOF_FLOOR) != 0) && temp->GetZ() >= minZ && temp->GetZ() < maxZ)
            {
                if (((temp->State & CHMOF_VALIDATED_PLACE) != 0) &&
                    ((temp->State & CHMOF_INCORRECT_PLACE) == 0))
                {
                    return true;
                }
            }
        }

        return false;
    }

    std::pair<int, int> infoCheck = SeekGraphicInCustomHouseObjectList<CCustomHouseObjectPlaceInfo>(
        m_ObjectsInfo, item->Graphic);

    if (infoCheck.first != -1 && infoCheck.second != -1)
    {
        const CCustomHouseObjectPlaceInfo& info = m_ObjectsInfo[infoCheck.first];

        if ((info.CanGoW == 0) && item->GetX() == StartPos.x)
        {
            return false;
        }
        if ((info.CanGoN == 0) && item->GetY() == StartPos.y)
        {
            return false;
        }
        if ((info.CanGoNWS == 0) && item->GetX() == StartPos.x && item->GetY() == StartPos.y)
        {
            return false;
        }

        if (info.Bottom == 0)
        {
            bool found = false;

            if (info.AdjUN != 0)
            {
                found = ValidatePlaceStructure(
                    foundationItem,
                    foundationItem->GetMultiAtXY(item->GetX(), item->GetY() + 1),
                    minZ,
                    maxZ,
                    CHVCF_BOTTOM | CHVCF_N);
            }

            if (!found && (info.AdjUE != 0))
            {
                found = ValidatePlaceStructure(
                    foundationItem,
                    foundationItem->GetMultiAtXY(item->GetX() - 1, item->GetY()),
                    minZ,
                    maxZ,
                    CHVCF_BOTTOM | CHVCF_E);
            }

            if (!found && (info.AdjUS != 0))
            {
                found = ValidatePlaceStructure(
                    foundationItem,
                    foundationItem->GetMultiAtXY(item->GetX(), item->GetY() - 1),
                    minZ,
                    maxZ,
                    CHVCF_BOTTOM | CHVCF_S);
            }

            if (!found && (info.AdjUW != 0))
            {
                found = ValidatePlaceStructure(
                    foundationItem,
                    foundationItem->GetMultiAtXY(item->GetX() + 1, item->GetY()),
                    minZ,
                    maxZ,
                    CHVCF_BOTTOM | CHVCF_W);
            }

            if (!found)
            {
                return false;
            }
        }

        if (info.Top == 0)
        {
            bool found = false;

            if (info.AdjLN != 0)
            {
                found = ValidatePlaceStructure(
                    foundationItem,
                    foundationItem->GetMultiAtXY(item->GetX(), item->GetY() + 1),
                    minZ,
                    maxZ,
                    CHVCF_TOP | CHVCF_N);
            }

            if (!found && (info.AdjLE != 0))
            {
                found = ValidatePlaceStructure(
                    foundationItem,
                    foundationItem->GetMultiAtXY(item->GetX() - 1, item->GetY()),
                    minZ,
                    maxZ,
                    CHVCF_TOP | CHVCF_E);
            }

            if (!found && (info.AdjLS != 0))
            {
                found = ValidatePlaceStructure(
                    foundationItem,
                    foundationItem->GetMultiAtXY(item->GetX(), item->GetY() - 1),
                    minZ,
                    maxZ,
                    CHVCF_TOP | CHVCF_S);
            }

            if (!found && (info.AdjLW != 0))
            {
                found = ValidatePlaceStructure(
                    foundationItem,
                    foundationItem->GetMultiAtXY(item->GetX() + 1, item->GetY()),
                    minZ,
                    maxZ,
                    CHVCF_TOP | CHVCF_W);
            }

            if (!found)
            {
                return false;
            }
        }
    }

    return true;
}

bool CGumpCustomHouse::CanEraseHere(CRenderWorldObject* place, CUSTOM_HOUSE_BUILD_TYPE& type)
{
    type = CHBT_NORMAL;

    if (place != nullptr && place->IsMultiObject())
    {
        CMultiObject* multi = (CMultiObject*)place;

        if (multi->IsCustomHouseMulti() && ((multi->State & CHMOF_GENERIC_INTERNAL) == 0))
        {
            if ((multi->State & CHMOF_FLOOR) != 0)
            {
                type = CHBT_FLOOR;
            }
            else if ((multi->State & CHMOF_STAIR) != 0)
            {
                type = CHBT_STAIR;
            }
            else if ((multi->State & CHMOF_ROOF) != 0)
            {
                type = CHBT_ROOF;
            }

            return true;
        }
    }

    return false;
}

void CGumpCustomHouse::OnTargetWorld(CRenderWorldObject* place)
{
    if (place != nullptr && place->IsMultiObject()) // && place->GetZ() >= MinHouseZ)
    {
        CMultiObject* multiObject = (CMultiObject*)place;

        int zOffset = 0;

        if (CurrentFloor == 1)
        {
            zOffset = -7;
        }

        if (SeekTile)
        {
            SeekGraphic(place->Graphic);
        }
        else if (
            place->GetZ() >= g_Player->GetZ() + zOffset && place->GetZ() < g_Player->GetZ() + 20)
        {
            CGameItem* foundationItem = g_World->GetWorldItem(Serial);

            if (foundationItem == nullptr)
            {
                return;
            }

            if (Erasing)
            {
                CUSTOM_HOUSE_BUILD_TYPE type;

                if (CanEraseHere(place, type))
                {
                    CMulti* multi = foundationItem->GetMultiAtXY(place->GetX(), place->GetY());

                    if (multi == nullptr)
                    {
                        return;
                    }

                    int z = 7 + (CurrentFloor - 1) * 20;

                    if (type == CHBT_STAIR || type == CHBT_ROOF)
                    {
                        z = place->GetZ() - (foundationItem->GetZ() + z) + z;
                    }

                    if (type == CHBT_ROOF)
                    {
                        CPacketCustomHouseDeleteRoof(
                            place->Graphic,
                            place->GetX() - foundationItem->GetX(),
                            place->GetY() - foundationItem->GetY(),
                            z)
                            .Send();
                    }
                    else
                    {
                        CPacketCustomHouseDeleteItem(
                            place->Graphic,
                            place->GetX() - foundationItem->GetX(),
                            place->GetY() - foundationItem->GetY(),
                            z)
                            .Send();
                    }

                    multi->Delete(place);
                }
            }
            else if (SelectedGraphic != 0u)
            {
                std::vector<CBuildObject> list;
                CUSTOM_HOUSE_BUILD_TYPE type;

                if (CanBuildHere(list, place, type) &&
                    (static_cast<unsigned int>(!list.empty()) != 0u))
                {
                    int placeX = place->GetX();
                    int placeY = place->GetY();

                    if (type == CHBT_STAIR && CombinedStair)
                    {
                        if (Page >= 0 && Page < (int)m_Stairs.size())
                        {
                            const CCustomHouseObjectStair& stair = m_Stairs[Page];
                            u16 graphic                          = 0;

                            if (SelectedGraphic == stair.North)
                            {
                                graphic = stair.MultiNorth;
                            }
                            else if (SelectedGraphic == stair.East)
                            {
                                graphic = stair.MultiEast;
                            }
                            else if (SelectedGraphic == stair.South)
                            {
                                graphic = stair.MultiSouth;
                            }
                            else if (SelectedGraphic == stair.West)
                            {
                                graphic = stair.MultiWest;
                            }

                            if (graphic != 0u)
                            {
                                CPacketCustomHouseAddStair(
                                    graphic,
                                    placeX - foundationItem->GetX(),
                                    placeY - foundationItem->GetY())
                                    .Send();
                            }
                        }
                    }
                    else
                    {
                        const CBuildObject& item = list[0];
                        int x                    = placeX - foundationItem->GetX() + item.X;
                        int y                    = placeY - foundationItem->GetY() + item.Y;

                        CMulti* multi =
                            foundationItem->GetMultiAtXY(placeX + item.X, placeY + item.Y);

                        if (multi != nullptr)
                        {
                            if (!CombinedStair)
                            {
                                int minZ = foundationItem->GetZ() + 7 + (CurrentFloor - 1) * 20;
                                int maxZ = minZ + 20;

                                if (CurrentFloor == 1)
                                {
                                    minZ -= 7;
                                }

                                CMultiObject* nextMultiObject = nullptr;

                                for (CMultiObject* multiObject = (CMultiObject*)multi->m_Items;
                                     multiObject != nullptr;
                                     multiObject = nextMultiObject)
                                {
                                    nextMultiObject = (CMultiObject*)multiObject->m_Next;

                                    int testMinZ = minZ;

                                    if ((multiObject->State & CHMOF_ROOF) != 0)
                                    {
                                        testMinZ -= 3;
                                    }

                                    if (multiObject->GetZ() < testMinZ ||
                                        multiObject->GetZ() >= maxZ ||
                                        !multiObject->IsCustomHouseMulti() ||
                                        ((multiObject->State & CHMOF_GENERIC_INTERNAL) != 0))
                                    {
                                        continue;
                                    }

                                    if (type == CHBT_STAIR)
                                    {
                                        if ((multiObject->State & CHMOF_STAIR) != 0)
                                        {
                                            multi->Delete(multiObject);
                                        }
                                    }
                                    else if (type == CHBT_ROOF)
                                    {
                                        if ((multiObject->State & CHMOF_ROOF) != 0)
                                        {
                                            multi->Delete(multiObject);
                                        }
                                    }
                                    else if (type == CHBT_FLOOR)
                                    {
                                        if ((multiObject->State & CHMOF_FLOOR) != 0)
                                        {
                                            multi->Delete(multiObject);
                                        }
                                    }
                                    else
                                    {
                                        if ((multiObject->State &
                                             (CHMOF_STAIR | CHMOF_ROOF | CHMOF_FLOOR)) == 0)
                                        {
                                            multi->Delete(multiObject);
                                        }
                                    }
                                }

                                if (multi->m_Items == nullptr)
                                {
                                    foundationItem->Delete(multi);
                                }
                            }

                            if (type == CHBT_ROOF)
                            {
                                CPacketCustomHouseAddRoof(item.Graphic, x, y, item.Z).Send();
                            }
                            else
                            {
                                CPacketCustomHouseAddItem(item.Graphic, x, y).Send();
                            }
                        }
                    }

                    int x = placeX - foundationItem->GetX();
                    int y = placeY - foundationItem->GetY();
                    int z = foundationItem->GetZ() + 7 + (CurrentFloor - 1) * 20;

                    if (type == CHBT_STAIR && !CombinedStair)
                    {
                        z = foundationItem->GetZ();
                    }

                    for (const CBuildObject& item : list)
                    {
                        foundationItem->AddMulti(
                            item.Graphic, 0, x + item.X, y + item.Y, z + item.Z, true);
                    }
                }
            }

            GenerateFloorPlace();
            WantUpdateContent = true;
        }
    }
}

void CGumpCustomHouse::GenerateFloorPlace()
{
    CGameItem* foundationItem = g_World->GetWorldItem(Serial);

    if (foundationItem != nullptr)
    {
        foundationItem->ClearCustomHouseMultis(CHMOF_GENERIC_INTERNAL);

        QFOR(multi, foundationItem->m_Items, CMulti*)
        {
            QFOR(item, multi->m_Items, CMultiObject*)
            {
                if (!item->IsCustomHouseMulti())
                {
                    continue;
                }

                int currentFloor = -1;
                int floorZ       = foundationItem->GetZ() + 7;
                int itemZ        = item->GetZ();

                for (int i = 0; i < 4; i++)
                {
                    int offset = ((int)i != 0 ? 0 : 7);

                    if (itemZ >= floorZ - offset && itemZ < floorZ + 20)
                    {
                        currentFloor = (int)i;
                        break;
                    }

                    floorZ += 20;
                }

                if (currentFloor == -1)
                {
                    continue;
                }

                std::pair<int, int> floorCheck =
                    SeekGraphicInCustomHouseObjectList<CCustomHouseObjectFloor>(
                        m_Floors, item->Graphic);
                int state = item->State;

                if (floorCheck.first != -1 && floorCheck.second != -1)
                {
                    state |= CHMOF_FLOOR;

                    if (m_FloorVisionState[currentFloor] == CHGVS_HIDE_FLOOR)
                    {
                        state |= CHMOF_IGNORE_IN_RENDER;
                    }
                    else if (
                        m_FloorVisionState[currentFloor] == CHGVS_TRANSPARENT_FLOOR ||
                        m_FloorVisionState[currentFloor] == CHGVS_TRANSLUCENT_FLOOR)
                    {
                        state |= CHMOF_TRANSPARENT;
                    }
                }
                else
                {
                    std::pair<int, int> stairCheck =
                        SeekGraphicInCustomHouseObjectList<CCustomHouseObjectStair>(
                            m_Stairs, item->Graphic);

                    if (stairCheck.first != -1 && stairCheck.second != -1)
                    {
                        state |= CHMOF_STAIR;
                    }
                    else
                    {
                        std::pair<int, int> roofCheck =
                            SeekGraphicInCustomHouseObjectListWithCategory<
                                CCustomHouseObjectRoof,
                                CCustomHouseObjectRoofCategory>(m_Roofs, item->Graphic);

                        if (roofCheck.first != -1 && roofCheck.second != -1)
                        {
                            state |= CHMOF_ROOF;
                        }
                        else
                        {
                            std::pair<int, int> fixtureCheck =
                                SeekGraphicInCustomHouseObjectList<CCustomHouseObjectDoor>(
                                    m_Doors, item->Graphic);

                            if (fixtureCheck.first == -1 || fixtureCheck.second == -1)
                            {
                                fixtureCheck =
                                    SeekGraphicInCustomHouseObjectList<CCustomHouseObjectTeleport>(
                                        m_Teleports, item->Graphic);
                            }

                            if (fixtureCheck.first != -1 && fixtureCheck.second != -1)
                            {
                                state |= CHMOF_FIXTURE;
                            }
                        }
                    }

                    if (m_FloorVisionState[currentFloor] == CHGVS_HIDE_CONTENT)
                    {
                        state |= CHMOF_IGNORE_IN_RENDER;
                    }
                    else if (m_FloorVisionState[currentFloor] == CHGVS_TRANSPARENT_CONTENT)
                    {
                        state |= CHMOF_TRANSPARENT;
                    }
                }

                if (m_FloorVisionState[currentFloor] == CHGVS_HIDE_ALL)
                {
                    state |= CHMOF_IGNORE_IN_RENDER;
                }

                item->State = state;
            }
        }

        int z = foundationItem->GetZ() + 7;

        for (int x = StartPos.x + 1; x < EndPos.x; x++)
        {
            for (int y = StartPos.y + 1; y < EndPos.y; y++)
            {
                CMulti* multi = foundationItem->GetMultiAtXY((int)x, (int)y);

                if (multi == nullptr)
                {
                    continue;
                }

                CMultiObject* floorMulti       = nullptr;
                CMultiObject* floorCustomMulti = nullptr;

                QFOR(item, multi->m_Items, CMultiObject*)
                {
                    if (item->GetZ() != z || ((item->State & CHMOF_FLOOR) == 0))
                    {
                        continue;
                    }

                    if (item->IsCustomHouseMulti())
                    {
                        floorCustomMulti = item;
                    }
                    else
                    {
                        floorMulti = item;
                    }
                }

                if (floorMulti != nullptr && floorCustomMulti == nullptr)
                {
                    CMultiObject* mo = foundationItem->AddMulti(
                        floorMulti->Graphic,
                        0,
                        (int)x - foundationItem->GetX(),
                        (int)y - foundationItem->GetY(),
                        z,
                        true);

                    int state = CHMOF_FLOOR;

                    if (m_FloorVisionState[0] == CHGVS_HIDE_FLOOR)
                    {
                        state |= CHMOF_IGNORE_IN_RENDER;
                    }
                    else if (
                        m_FloorVisionState[0] == CHGVS_TRANSPARENT_FLOOR ||
                        m_FloorVisionState[0] == CHGVS_TRANSLUCENT_FLOOR)
                    {
                        state |= CHMOF_TRANSPARENT;
                    }

                    mo->State = state;
                }
            }
        }

        for (int i = 0; i < FloorCount; i++)
        {
            int minZ = foundationItem->GetZ() + 7 + ((int)i * 20);
            int maxZ = minZ + 20;

            for (int j = 0; j < 2; j++)
            {
                std::vector<Core::Vec2<i32>> validatedFloors;

                for (int x = StartPos.x; x < EndPos.x + 1; x++)
                {
                    for (int y = StartPos.y; y < EndPos.y + 1; y++)
                    {
                        CMulti* multi = foundationItem->GetMultiAtXY((int)x, (int)y);

                        if (multi == nullptr)
                        {
                            continue;
                        }

                        QFOR(item, multi->m_Items, CMultiObject*)
                        {
                            if (!item->IsCustomHouseMulti())
                            {
                                continue;
                            }

                            if (j == 0)
                            {
                                if ((i == 0) && item->GetZ() < minZ)
                                {
                                    item->State = item->State | CHMOF_VALIDATED_PLACE;
                                    continue;
                                }

                                if ((item->State & CHMOF_FLOOR) == 0)
                                {
                                    continue;
                                }

                                if ((i == 0) && item->GetZ() >= minZ && item->GetZ() < maxZ)
                                {
                                    item->State = item->State | CHMOF_VALIDATED_PLACE;
                                    continue;
                                }
                            }

                            if (((item->State & (CHMOF_VALIDATED_PLACE | CHMOF_GENERIC_INTERNAL)) ==
                                 0) &&
                                item->GetZ() >= minZ && item->GetZ() < maxZ)
                            {
                                if (!ValidateItemPlace(
                                        foundationItem, item, minZ, maxZ, validatedFloors))
                                {
                                    item->State =
                                        item->State | CHMOF_VALIDATED_PLACE | CHMOF_INCORRECT_PLACE;
                                }
                                else
                                {
                                    item->State = item->State | CHMOF_VALIDATED_PLACE;
                                }
                            }
                        }
                    }
                }

                if ((i != 0) && (j == 0))
                {
                    for (const Core::Vec2<i32>& point : validatedFloors)
                    {
                        CMulti* multi = foundationItem->GetMultiAtXY(point.x, point.y);

                        if (multi == nullptr)
                        {
                            continue;
                        }

                        QFOR(item, multi->m_Items, CMultiObject*)
                        {
                            if (item->IsCustomHouseMulti() && ((item->State & CHMOF_FLOOR) != 0) &&
                                item->GetZ() >= minZ && item->GetZ() < maxZ)
                            {
                                item->State = item->State & ~CHMOF_INCORRECT_PLACE;
                            }
                        }
                    }
                    for (int x = StartPos.x; x < EndPos.x + 1; x++)
                    {
                        int minY = 0;
                        int maxY = 0;

                        for (int y = StartPos.y; y < EndPos.y + 1; y++)
                        {
                            CMulti* multi = foundationItem->GetMultiAtXY((int)x, (int)y);

                            if (multi == nullptr)
                            {
                                continue;
                            }

                            QFOR(item, multi->m_Items, CMultiObject*)
                            {
                                if (item->IsCustomHouseMulti() &&
                                    ((item->State & CHMOF_FLOOR) != 0) &&
                                    ((item->State & CHMOF_VALIDATED_PLACE) != 0) &&
                                    ((item->State & CHMOF_INCORRECT_PLACE) == 0) &&
                                    item->GetZ() >= minZ && item->GetZ() < maxZ)
                                {
                                    minY = (int)y;
                                    break;
                                }
                            }

                            if (minY != 0)
                            {
                                break;
                            }
                        }

                        for (int y = EndPos.y; y >= StartPos.y; y--)
                        {
                            CMulti* multi = foundationItem->GetMultiAtXY((int)x, (int)y);

                            if (multi == nullptr)
                            {
                                continue;
                            }

                            QFOR(item, multi->m_Items, CMultiObject*)
                            {
                                if (item->IsCustomHouseMulti() &&
                                    ((item->State & CHMOF_FLOOR) != 0) &&
                                    ((item->State & CHMOF_VALIDATED_PLACE) != 0) &&
                                    ((item->State & CHMOF_INCORRECT_PLACE) == 0) &&
                                    item->GetZ() >= minZ && item->GetZ() < maxZ)
                                {
                                    maxY = (int)y;
                                    break;
                                }
                            }

                            if (maxY != 0)
                            {
                                break;
                            }
                        }

                        for (int y = minY; y < maxY; y++)
                        {
                            CMulti* multi = foundationItem->GetMultiAtXY((int)x, (int)y);

                            if (multi == nullptr)
                            {
                                continue;
                            }

                            QFOR(item, multi->m_Items, CMultiObject*)
                            {
                                if (item->IsCustomHouseMulti() &&
                                    ((item->State & CHMOF_FLOOR) != 0) &&
                                    ((item->State & CHMOF_VALIDATED_PLACE) != 0) &&
                                    item->GetZ() >= minZ && item->GetZ() < maxZ)
                                {
                                    item->State = item->State & ~CHMOF_INCORRECT_PLACE;
                                }
                            }
                        }
                    }

                    for (int y = StartPos.y; y < EndPos.y + 1; y++)
                    {
                        int minX = 0;
                        int maxX = 0;

                        for (int x = StartPos.x; x < EndPos.x + 1; x++)
                        {
                            CMulti* multi = foundationItem->GetMultiAtXY((int)x, (int)y);

                            if (multi == nullptr)
                            {
                                continue;
                            }

                            QFOR(item, multi->m_Items, CMultiObject*)
                            {
                                if (item->IsCustomHouseMulti() &&
                                    ((item->State & CHMOF_FLOOR) != 0) &&
                                    ((item->State & CHMOF_VALIDATED_PLACE) != 0) &&
                                    ((item->State & CHMOF_INCORRECT_PLACE) == 0) &&
                                    item->GetZ() >= minZ && item->GetZ() < maxZ)
                                {
                                    minX = (int)x;
                                    break;
                                }
                            }

                            if (minX != 0)
                            {
                                break;
                            }
                        }

                        for (int x = EndPos.x; x >= StartPos.x; x--)
                        {
                            CMulti* multi = foundationItem->GetMultiAtXY((int)x, (int)y);

                            if (multi == nullptr)
                            {
                                continue;
                            }

                            QFOR(item, multi->m_Items, CMultiObject*)
                            {
                                if (item->IsCustomHouseMulti() &&
                                    ((item->State & CHMOF_FLOOR) != 0) &&
                                    ((item->State & CHMOF_VALIDATED_PLACE) != 0) &&
                                    ((item->State & CHMOF_INCORRECT_PLACE) == 0) &&
                                    item->GetZ() >= minZ && item->GetZ() < maxZ)
                                {
                                    maxX = (int)x;
                                    break;
                                }
                            }

                            if (maxX != 0)
                            {
                                break;
                            }
                        }

                        for (int x = minX; x < maxX; x++)
                        {
                            CMulti* multi = foundationItem->GetMultiAtXY((int)x, (int)y);

                            if (multi == nullptr)
                            {
                                continue;
                            }

                            QFOR(item, multi->m_Items, CMultiObject*)
                            {
                                if (item->IsCustomHouseMulti() &&
                                    ((item->State & CHMOF_FLOOR) != 0) &&
                                    ((item->State & CHMOF_VALIDATED_PLACE) != 0) &&
                                    item->GetZ() >= minZ && item->GetZ() < maxZ)
                                {
                                    item->State = item->State & ~CHMOF_INCORRECT_PLACE;
                                }
                            }
                        }
                    }
                }
            }
        }

        z = foundationItem->GetZ() + 7 + 20;

        u16 color = 0x0051;

        for (int i = 1; i < CurrentFloor; i++)
        {
            for (int x = StartPos.x; x < EndPos.x; x++)
            {
                for (int y = StartPos.y; y < EndPos.y; y++)
                {
                    u16 tempColor = color;

                    if (x == StartPos.x || y == StartPos.y)
                        tempColor++;

                    CMultiObject* mo = foundationItem->AddMulti(
                        0x0496,
                        tempColor,
                        (int)x - foundationItem->GetX(),
                        (int)y - foundationItem->GetY(),
                        z,
                        true);
                    mo->State = CHMOF_GENERIC_INTERNAL | CHMOF_TRANSPARENT;
                    g_MapManager.AddRender(mo);
                }
            }

            color += 5;
            z += 20;
        }
    }
}

void CGumpCustomHouse::GUMP_TEXT_ENTRY_EVENT_C
{
    QFOR(item, m_DataBox->m_Items, CBaseGUI*)
    {
        if (item->Type == GOT_TEXTENTRY)
        {
            CGUITextEntry* entry = (CGUITextEntry*)item;
            entry->Focused       = (entry->Serial == serial);
        }
    }

    OnButton(serial);
}

void CGumpCustomHouse::OnLeftMouseButtonUp()
{
    CGump::OnLeftMouseButtonUp();

    QFOR(item, m_DataBox->m_Items, CBaseGUI*)
    {
        if (item->Type == GOT_TEXTENTRY)
        {
            ((CGUITextEntry*)item)->Focused = false;
        }
    }

    WantRedraw = true;
}

void CGumpCustomHouse::GUMP_BUTTON_EVENT_C
{
    if (serial >= ID_GCH_ITEM_IN_LIST)
    {
        int index = serial - ID_GCH_ITEM_IN_LIST;

        if (Category == -1 && (State == CHGS_WALL || State == CHGS_ROOF || State == CHGS_MISC))
        {
            int newCategory = -1;

            if (State == CHGS_WALL && index >= 0 && index < (int)m_Walls.size())
            {
                newCategory = m_Walls[index].Index;
            }
            else if (State == CHGS_ROOF && index >= 0 && index < (int)m_Roofs.size())
            {
                newCategory = m_Roofs[index].Index;
            }
            else if (State == CHGS_MISC && index >= 0 && index < (int)m_Miscs.size())
            {
                newCategory = m_Miscs[index].Index;
            }

            if (newCategory != -1)
            {
                Category          = newCategory;
                WantUpdateContent = true;
                Page              = 0;
                SelectedGraphic   = 0;
                Erasing           = false;
                SeekTile          = false;
                CombinedStair     = false;
                UpdateMaxPage();
            }
        }
        else if (index >= 0 && Page >= 0)
        {
            bool combinedStair = false;
            u16 graphic        = 0;

            if (State == CHGS_WALL || State == CHGS_ROOF || State == CHGS_MISC)
            {
                if (Category >= 0)
                {
                    if (State == CHGS_WALL && Category < (int)m_Walls.size() &&
                        index < CCustomHouseObjectWall::GRAPHICS_COUNT)
                    {
                        const std::vector<CCustomHouseObjectWall>& list = m_Walls[Category].m_Items;

                        if (Page < (int)list.size())
                        {
                            graphic =
                                (ShowWindow ? list[Page].m_WindowGraphics[index] :
                                              list[Page].m_Graphics[index]);
                        }
                    }
                    else if (
                        State == CHGS_ROOF && Category < (int)m_Roofs.size() &&
                        index < CCustomHouseObjectRoof::GRAPHICS_COUNT)
                    {
                        const std::vector<CCustomHouseObjectRoof>& list = m_Roofs[Category].m_Items;

                        if (Page < (int)list.size())
                        {
                            graphic = list[Page].m_Graphics[index];
                        }
                    }
                    else if (
                        State == CHGS_MISC && Category < (int)m_Miscs.size() &&
                        index < CCustomHouseObjectMisc::GRAPHICS_COUNT)
                    {
                        const std::vector<CCustomHouseObjectMisc>& list = m_Miscs[Category].m_Items;

                        if (Page < (int)list.size())
                        {
                            graphic = list[Page].m_Graphics[index];
                        }
                    }
                }
            }
            else
            {
                if (State == CHGS_DOOR && Page < (int)m_Doors.size() &&
                    index < CCustomHouseObjectDoor::GRAPHICS_COUNT)
                {
                    graphic = m_Doors[Page].m_Graphics[index];
                }
                else if (
                    State == CHGS_FLOOR && Page < (int)m_Floors.size() &&
                    index < CCustomHouseObjectFloor::GRAPHICS_COUNT)
                {
                    graphic = m_Floors[Page].m_Graphics[index];
                }
                else if (State == CHGS_STAIR && Page < (int)m_Stairs.size())
                {
                    if (index > 10)
                    {
                        combinedStair = true;
                        index -= 10;
                    }

                    if (index < CCustomHouseObjectStair::GRAPHICS_COUNT)
                    {
                        graphic = m_Stairs[Page].m_Graphics[index];
                    }
                }
            }

            if (graphic != 0u)
            {
                g_Target.RequestFromCustomHouse();
                CombinedStair     = combinedStair;
                WantUpdateContent = true;
                SelectedGraphic   = graphic;
            }
        }

        return;
    }

    switch (serial)
    {
        case ID_GCH_STATE_WALL:
        {
            Category          = -1;
            State             = CHGS_WALL;
            WantUpdateContent = true;
            Page              = 0;
            SelectedGraphic   = 0;
            CombinedStair     = false;
            UpdateMaxPage();
            g_Target.SendCancelTarget();
            break;
        }
        case ID_GCH_STATE_DOOR:
        {
            Category          = -1;
            State             = CHGS_DOOR;
            WantUpdateContent = true;
            Page              = 0;
            SelectedGraphic   = 0;
            CombinedStair     = false;
            UpdateMaxPage();
            g_Target.SendCancelTarget();
            break;
        }
        case ID_GCH_STATE_FLOOR:
        {
            Category          = -1;
            State             = CHGS_FLOOR;
            WantUpdateContent = true;
            Page              = 0;
            SelectedGraphic   = 0;
            CombinedStair     = false;
            UpdateMaxPage();
            g_Target.SendCancelTarget();
            break;
        }
        case ID_GCH_STATE_STAIR:
        {
            Category          = -1;
            State             = CHGS_STAIR;
            WantUpdateContent = true;
            Page              = 0;
            SelectedGraphic   = 0;
            CombinedStair     = false;
            UpdateMaxPage();
            g_Target.SendCancelTarget();
            break;
        }
        case ID_GCH_STATE_ROOF:
        {
            Category          = -1;
            State             = CHGS_ROOF;
            WantUpdateContent = true;
            Page              = 0;
            SelectedGraphic   = 0;
            CombinedStair     = false;
            UpdateMaxPage();
            g_Target.SendCancelTarget();
            break;
        }
        case ID_GCH_STATE_MISC:
        {
            Category          = -1;
            State             = CHGS_MISC;
            WantUpdateContent = true;
            Page              = 0;
            SelectedGraphic   = 0;
            CombinedStair     = false;
            UpdateMaxPage();
            g_Target.SendCancelTarget();
            break;
        }
        case ID_GCH_STATE_ERASE:
        {
            g_Target.RequestFromCustomHouse();
            Erasing           = !Erasing;
            WantUpdateContent = true;
            SelectedGraphic   = 0;
            CombinedStair     = false;
            break;
        }
        case ID_GCH_STATE_EYEDROPPER:
        {
            g_Target.RequestFromCustomHouse();
            SeekTile          = true;
            WantUpdateContent = true;
            SelectedGraphic   = 0;
            CombinedStair     = false;
            break;
        }
        case ID_GCH_STATE_MENU:
        {
            Category          = -1;
            State             = CHGS_MENU;
            WantUpdateContent = true;
            MaxPage           = 1;
            Page              = 0;
            SelectedGraphic   = 0;
            CombinedStair     = false;
            g_Target.SendCancelTarget();
            break;
        }
        case ID_GCH_VISIBILITY_STORY_1:
        case ID_GCH_VISIBILITY_STORY_2:
        case ID_GCH_VISIBILITY_STORY_3:
        case ID_GCH_VISIBILITY_STORY_4:
        {
            int selectedFloor = serial - ID_GCH_VISIBILITY_STORY_1;

            m_FloorVisionState[selectedFloor]++;

            if (m_FloorVisionState[selectedFloor] > CHGVS_HIDE_ALL)
            {
                m_FloorVisionState[selectedFloor] = CHGVS_NORMAL;
            }

            WantUpdateContent = true;
            GenerateFloorPlace();

            break;
        }
        case ID_GCH_GO_FLOOR_1:
        {
            CurrentFloor = 1;
            CPacketCustomHouseGoToFloor(1).Send();
            WantUpdateContent     = true;
            m_FloorVisionState[0] = m_FloorVisionState[1] = m_FloorVisionState[2] =
                m_FloorVisionState[3]                     = CHGVS_NORMAL;
            break;
        }
        case ID_GCH_GO_FLOOR_2:
        {
            CurrentFloor = 2;
            CPacketCustomHouseGoToFloor(2).Send();
            WantUpdateContent     = true;
            m_FloorVisionState[0] = m_FloorVisionState[1] = m_FloorVisionState[2] =
                m_FloorVisionState[3]                     = CHGVS_NORMAL;
            break;
        }
        case ID_GCH_GO_FLOOR_3:
        {
            CurrentFloor = 3;
            CPacketCustomHouseGoToFloor(3).Send();
            WantUpdateContent     = true;
            m_FloorVisionState[0] = m_FloorVisionState[1] = m_FloorVisionState[2] =
                m_FloorVisionState[3]                     = CHGVS_NORMAL;
            break;
        }
        case ID_GCH_GO_FLOOR_4:
        {
            CurrentFloor = 4;
            CPacketCustomHouseGoToFloor(4).Send();
            WantUpdateContent     = true;
            m_FloorVisionState[0] = m_FloorVisionState[1] = m_FloorVisionState[2] =
                m_FloorVisionState[3]                     = CHGVS_NORMAL;
            break;
        }
        case ID_GCH_LIST_LEFT:
        {
            Page--;

            if (Page < 0)
            {
                Page = MaxPage - 1;

                if (Page < 0)
                {
                    Page = 0;
                }
            }

            WantUpdateContent = true;

            break;
        }
        case ID_GCH_LIST_RIGHT:
        {
            Page++;

            if (Page >= MaxPage)
            {
                Page = 0;
            }

            WantUpdateContent = true;

            break;
        }
        case ID_GCH_MENU_BACKUP:
        {
            CPacketCustomHouseBackup().Send();
            break;
        }
        case ID_GCH_MENU_RESTORE:
        {
            CPacketCustomHouseRestore().Send();
            break;
        }
        case ID_GCH_MENU_SYNCH:
        {
            CPacketCustomHouseSync().Send();
            break;
        }
        case ID_GCH_MENU_CLEAR:
        {
            CPacketCustomHouseClear().Send();
            break;
        }
        case ID_GCH_MENU_COMMIT:
        {
            CPacketCustomHouseCommit().Send();
            break;
        }
        case ID_GCH_MENU_REVERT:
        {
            CPacketCustomHouseRevert().Send();
            break;
        }
        case ID_GCH_GO_CATEGORY:
        {
            Category          = -1;
            WantUpdateContent = true;
            Page              = 0;
            SelectedGraphic   = 0;
            CombinedStair     = false;
            UpdateMaxPage();
            g_Target.SendCancelTarget();
            break;
        }
        case ID_GCH_WALL_SHOW_WINDOW:
        {
            ShowWindow        = !ShowWindow;
            WantUpdateContent = true;
            break;
        }
        case ID_GCH_ROOF_Z_UP:
        {
            if (RoofZ < 6)
            {
                RoofZ++;
                WantUpdateContent = true;
            }
            break;
        }
        case ID_GCH_ROOF_Z_DOWN:
        {
            if (RoofZ > 1)
            {
                RoofZ--;
                WantUpdateContent = true;
            }

            break;
        }
        default: break;
    }
}
