#include "GumpWorldMap.h"
#include "Core/Platform.h"
#include "Core/File.h"
#include "Globals.h"
#include "../OrionUO.h"
#include "../OrionApplication.h"
#include "../PressedObject.h"
#include "../DefinitionMacro.h"
#include "../Managers/ColorManager.h"
#include "../Managers/MapManager.h"
#include "../Managers/MouseManager.h"
#include "../Managers/FileManager.h"
#include "../Network/Packets.h"
#include "../GameObjects/GamePlayer.h"

const int m_Scales[7] = { 1, 1, 1, 2, 4, 6, 10 };

CGumpWorldMap::CGumpWorldMap(short x, short y)
    : CGump(GT_WORLD_MAP, 0, x, y)
{
    Page = 2;

    Add(new CGUIPage(1));
    Add(new CGUIGumppic(0x15E8, 0, 0)); //Earth button

    Add(new CGUIPage(2));

    m_Minimizer = (CGUIButton*)Add(
        new CGUIButton(ID_GWM_MINIMIZE, 0x082D, 0x082D, 0x082D, (Width / 2) - 10, 0));
    m_Background = (CGUIResizepic*)Add(new CGUIResizepic(0, 0x0A3C, 0, 23, Width, Height));
    m_Resizer    = (CGUIResizeButton*)Add(
        new CGUIResizeButton(ID_GWM_RESIZE, 0x0837, 0x0838, 0x0838, Width - 8, Height + 13));

    //Map settings
    static const std::string mapNames[7] = { "Current map", "Britannia", "Trammel", "Illshenar",
                                             "Malas",       "Tokuno",    "TerMur" };

    //Scale settings
    static const std::string scaleNames[7] = { "4:1", "2:1", "1:1", "1:2", "1:4", "1:6", "1:10" };

    //Link with player checkbox settings
    Text = (CGUIText*)Add(new CGUIText(0x03B2, 0, 0));
    Text->CreateTextureA(3, "Link with player");
    Text->SetX(Width - Text->m_Texture.Width);

    m_Checkbox = (CGUICheckbox*)Add(
        new CGUICheckbox(ID_GWM_LINK_WITH_PLAYER, 0x00D2, 0x00D3, 0x00D2, Text->GetX() - 26, 2));
    m_Checkbox->Checked = m_LinkWithPlayer;

    m_Scissor = (CGUIScissor*)Add(new CGUIScissor(true, 0, 0, 8, 32, Width - 16, Height - 16));

    m_MapData         = (CGUIWorldMapTexture*)Add(new CGUIWorldMapTexture(8, 31));
    m_MapData->Serial = ID_GWM_MAP;
    int map           = GetCurrentMap();
    m_MapData->Index  = map;

    m_MapData->MoveOnDrag = (m_LinkWithPlayer || g_CurrentMap == map);

    LoadMap(map);

    int width   = 0;
    int height  = 0;
    int playerX = g_Player->GetX();
    int playerY = g_Player->GetY();

    GetScaledDimensions(width, height, playerX, playerY);

    m_MapData->Width  = width;
    m_MapData->Height = height;

    Add(new CGUIScissor(false));

    m_ComboboxScale = (CGUIComboBox*)Add(
        new CGUIComboBox(ID_GWM_SCALE_LIST, 0x098D, true, 0x09B5, 110, 0, 46, 7, false));
    m_ComboboxScale->TextOffsetY   = -5;
    m_ComboboxScale->SelectedIndex = m_Scale;

    for (int i = 0; i < 7; i++)
    {
        m_ComboboxScale->Add(
            new CGUIComboboxText(0, 6, scaleNames[i], 36, TS_CENTER, UOFONT_FIXED));
    }

    m_ComboboxMap = (CGUIComboBox*)Add(
        new CGUIComboBox(ID_GWM_MAP_LIST, 0x098D, true, 0x09B5, 0, 0, 0, 7, false));
    m_ComboboxMap->TextOffsetY   = -5;
    m_ComboboxMap->SelectedIndex = m_Map;

    for (int i = 0; i < 7; i++)
    {
        m_ComboboxMap->Add(new CGUIComboboxText(0, 6, mapNames[i], 98, TS_CENTER, UOFONT_FIXED));
    }
}

CGumpWorldMap::~CGumpWorldMap()
{
}

int CGumpWorldMap::GetCurrentMap()
{
    int map = m_Map;

    if (map == 0)
    {
        map = g_CurrentMap;
    }
    else
    {
        map--;
    }

    return map;
}

void CGumpWorldMap::SetLinkWithPlayer(bool val)
{
    m_LinkWithPlayer      = val;
    m_Checkbox->Checked   = val;
    m_MapData->MoveOnDrag = (m_LinkWithPlayer || g_CurrentMap == GetCurrentMap());
    WantRedraw            = true;
}

void CGumpWorldMap::SetScale(int val)
{
    m_Scale                        = val;
    m_ComboboxScale->SelectedIndex = val;
    WantRedraw                     = true;
}

void CGumpWorldMap::SetMap(int val)
{
    m_Map                        = val;
    m_ComboboxMap->SelectedIndex = val;
    WantRedraw                   = true;
}

void CGumpWorldMap::CalculateGumpState()
{
    CGump::CalculateGumpState();

    if (g_GumpPressed)
    {
        if (g_PressedObject.LeftObject != nullptr &&
            ((CBaseGUI*)g_PressedObject.LeftObject)->Type == GOT_COMBOBOX)
        {
            g_GumpMovingOffset.set(0, 0);

            if (Minimized)
            {
                g_GumpTranslate.x = (float)MinimizedX;
                g_GumpTranslate.y = (float)MinimizedY;
            }
            else
            {
                g_GumpTranslate.x = (float)m_X;
                g_GumpTranslate.y = (float)m_Y;
            }
        }
        else
        {
            WantRedraw = true;
        }
    }
}

void CGumpWorldMap::GetCurrentCenter(int& x, int& y, int& mouseX, int& mouseY)
{
    x = -OffsetX + mouseX;
    y = -OffsetY + mouseY;

    int scale = m_Scale;

    if (scale == 0)
    {
        x /= 4;
        y /= 4;
    }
    else if (scale == 1)
    {
        x /= 2;
        y /= 2;
    }
    else if (scale > 2)
    {
        scale = m_Scales[scale];

        x *= scale;
        y *= scale;
    }
}

void CGumpWorldMap::ScaleOffsets(int newScale, int mouseX, int mouseY)
{
    int offsetX = 0;
    int offsetY = 0;

    GetCurrentCenter(offsetX, offsetY, mouseX, mouseY);

    int width  = 0;
    int height = 0;

    m_Scale = newScale;

    GetScaledDimensions(width, height, offsetX, offsetY);

    offsetX = (Width / 2) - offsetX;
    if (offsetX > 0)
    {
        offsetX = 0;
    }

    offsetY = ((Height - 30) / 2) - offsetY;
    if (offsetY > 0)
    {
        offsetY = 0;
    }

    OffsetX = offsetX;
    OffsetY = offsetY;

    FixOffsets(OffsetX, OffsetY, Width, Height);
}

void CGumpWorldMap::GetScaledDimensions(int& width, int& height, int& playerX, int& playerY)
{
    int map = GetCurrentMap();

    width  = g_MapSize[map].x;
    height = g_MapSize[map].y;

    int scale = m_Scale;

    if (scale == 0)
    {
        width *= 4;
        height *= 4;
        playerX *= 4;
        playerY *= 4;
    }
    else if (scale == 1)
    {
        width *= 2;
        height *= 2;
        playerX *= 2;
        playerY *= 2;
    }
    else if (scale > 2)
    {
        scale = m_Scales[scale];

        width /= scale;
        height /= scale;
        playerX /= scale;
        playerY /= scale;
    }
}

void CGumpWorldMap::FixOffsets(int& offsetX, int& offsetY, int& width, int& height)
{
    int mapWidth  = 0;
    int mapHeight = 0;
    int playerX   = 0;
    int playerY   = 0;

    GetScaledDimensions(mapWidth, mapHeight, playerX, playerY);

    if (offsetX + mapWidth < width)
    {
        offsetX = width - mapWidth;
    }

    if (offsetY + mapHeight < height)
    {
        offsetY = height - mapHeight;
    }

    if (offsetX > 0)
    {
        offsetX = 0;
    }

    if (offsetY > 0)
    {
        offsetY = 0;
    }
}

void CGumpWorldMap::LoadMap(int map)
{
    if (!Called ||
        (g_FileManager.m_MapUOP[map].GetBuffer() == nullptr &&
         g_FileManager.m_MapMul[map].GetBuffer() == nullptr) ||
        g_FileManager.m_StaticIdx[map].GetBuffer() == nullptr ||
        g_FileManager.m_StaticMul[map].GetBuffer() == nullptr)
    {
        return;
    }

    if (g_MapTexture[map].Texture == 0)
    {
        u32 crc32 = 0;

        if (g_FileManager.m_MapUOP[map].GetBuffer() == nullptr)
        {
            crc32 = g_Orion.GetFileHashCode(
                g_FileManager.m_MapMul[map].GetBuffer(), g_FileManager.m_MapMul[map].GetSize());
        }
        else
        {
            crc32 = g_Orion.GetFileHashCode(
                g_FileManager.m_MapUOP[map].GetBuffer(), g_FileManager.m_MapUOP[map].GetSize());
        }

        crc32 ^= g_Orion.GetFileHashCode(
            g_FileManager.m_StaticIdx[map].GetBuffer(), g_FileManager.m_StaticIdx[map].GetSize());
        crc32 ^= g_Orion.GetFileHashCode(
            g_FileManager.m_StaticMul[map].GetBuffer(), g_FileManager.m_StaticMul[map].GetSize());

        if (g_FileManager.m_MapMul[map].GetBuffer() != nullptr)
        {
            for (int i = 0; i < g_MapManager.PatchesCount; i++)
            {
                if (g_MapManager.m_MapPatchCount[i] != 0)
                {
                    crc32 ^= g_Orion.GetFileHashCode(
                        g_FileManager.m_MapDifl[i].GetBuffer(), g_FileManager.m_MapDifl[i].GetSize());
                    crc32 ^= g_Orion.GetFileHashCode(
                        g_FileManager.m_MapDif[i].GetBuffer(), g_FileManager.m_MapDif[i].GetSize());
                }

                if (g_MapManager.m_StaticPatchCount[i] != 0)
                {
                    crc32 ^= g_Orion.GetFileHashCode(
                        g_FileManager.m_StaDifl[i].GetBuffer(), g_FileManager.m_StaDifl[i].GetSize());
                    crc32 ^= g_Orion.GetFileHashCode(
                        g_FileManager.m_StaDifi[i].GetBuffer(), g_FileManager.m_StaDifi[i].GetSize());
                }
            }
        }

        char fileName[64];
        std::snprintf(fileName, 64, "OrionData/WorldMap%08X.cuo", crc32);
        std::filesystem::path path = g_App.GetGameDir() / fileName;
        bool fromFile              = false;
        std::vector<u16> buf;
        if (std::filesystem::exists(path))
        {
            fromFile = true;
            Core::File file(path, "rb");
            if (file.IsOpen())
            {
                long size = file.Size();
                buf.resize(size, 0);
                if (buf.size() != size)
                {
                    LOG_ERROR("GumpWorldMap", "Allocation pixels memory for World Map failed (want size: %i)", size);
                    file.Close();
                    return;
                }

                size_t read = file.Read(&buf[0], sizeof(short), size);
                file.Close();
                if (read != size)
                {
                    LOG_ERROR("GumpWorldMap", "Error reading world map file, want=%i, read=%zi", size, read);
                    fromFile = false;
                }
                else
                {
                    LOG_INFO("GumpWorldMap", "World map read from file!");
                }
            }
            else
            {
                LOG_ERROR("GumpWorldMap", "Error open world map file: %s", path.string().c_str());
            }
        }

        int wantSize = g_MapSize[map].x * g_MapSize[map].y;

        if (!fromFile)
        {
            buf.resize(wantSize, 0);

            if (buf.size() != wantSize)
            {
                LOG_ERROR("GumpWorldMap", "Allocation pixels memory for World Map failed (want size: %i)\n", wantSize);
                return;
            }

            int maxBlock = wantSize - 1;

            for (int bx = 0; bx < g_MapBlockSize[map].x; bx++)
            {
                int mapX = (int)bx * 8;

                for (int by = 0; by < g_MapBlockSize[map].y; by++)
                {
                    CIndexMap* indexMap = g_MapManager.GetIndex(map, (int)bx, (int)by);

                    if (indexMap == nullptr || indexMap->MapAddress == 0)
                    {
                        continue;
                    }

                    int mapY       = (int)by * 8;
                    MAP_BLOCK info = {};

                    MAP_BLOCK* mapBlock = (MAP_BLOCK*)indexMap->MapAddress;

                    int pos = 0;

                    for (int y = 0; y < 8; y++)
                    {
                        for (int x = 0; x < 8; x++)
                        {
                            MAP_CELLS& cell     = mapBlock->Cells[pos];
                            MAP_CELLS& infoCell = info.Cells[pos];
                            infoCell.TileID     = cell.TileID;
                            infoCell.Z          = cell.Z;
                            pos++;
                        }
                    }

                    STATICS_BLOCK* sb = (STATICS_BLOCK*)indexMap->StaticAddress;

                    if (sb != nullptr)
                    {
                        int count = indexMap->StaticCount;

                        for (int c = 0; c < count; c++)
                        {
                            STATICS_BLOCK& staticBlock = sb[c];

                            if ((staticBlock.Color != 0u) && staticBlock.Color != 0xFFFF &&
                                !CRenderStaticObject::IsNoDrawTile(staticBlock.Color))
                            {
                                pos = (staticBlock.Y * 8) + staticBlock.X;
                                //if (pos > 64) continue;

                                MAP_CELLS& infoCell = info.Cells[pos];

                                if (infoCell.Z <= staticBlock.Z)
                                {
                                    infoCell.TileID = staticBlock.Color + 0x4000;
                                    infoCell.Z      = staticBlock.Z;
                                }
                            }
                        }
                    }

                    pos = 0;

                    for (int y = 0; y < 8; y++)
                    {
                        int block = ((mapY + (int)y) * g_MapSize[map].x) + mapX;

                        for (int x = 0; x < 8; x++)
                        {
                            u16 color =
                                0x8000 | g_ColorManager.GetRadarColorData(info.Cells[pos].TileID);

                            buf[block] = color;

                            if (y < 7 && x < 7 && block < maxBlock)
                            {
                                buf[block + 1] = color;
                            }

                            block++;
                            pos++;
                        }
                    }
                }
            }

            std::filesystem::create_directory(g_App.GetExeDir() / "OrionData");
            Core::File file(path, "wb");
            if (file.IsOpen())
            {
                size_t written = file.Write(&buf[0], sizeof(short), buf.size());
                LOG_INFO("GumpWorldMap", "Write world map file, want=%zi, written=%zi", buf.size(), written);
            }
        }

        if (buf.size() == wantSize)
        {
            g_GL.IgnoreHitMap = true;
            g_GL_BindTexture16(
                g_MapTexture[map], g_MapSize[map].x, g_MapSize[map].y, &buf[0]);
            g_GL.IgnoreHitMap = false;
        }
        else
        {
            LOG_ERROR("GumpWorldMap", "World map build error: buffer=%zi, want=%i",
                buf.size(),
                g_MapSize[map].x * g_MapSize[map].y);
        }
    }
}

void CGumpWorldMap::GenerateFrame(bool stop)
{
    CGump::GenerateFrame(false);

    //Player drawing
    if (!Minimized && g_CurrentMap == GetCurrentMap())
    {
        int width   = 0;
        int height  = 0;
        int playerX = g_Player->GetX();
        int playerY = g_Player->GetY();

        GetScaledDimensions(width, height, playerX, playerY);

        m_Scissor->Draw(false);

        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

        g_GL.DrawPolygone(
            m_MapData->OffsetX + playerX + 0, m_MapData->OffsetY + playerY + 30, 16, 2);
        g_GL.DrawPolygone(
            m_MapData->OffsetX + playerX + 7, m_MapData->OffsetY + playerY + 23, 2, 16);
        g_GL.DrawCircle(
            m_MapData->OffsetX + playerX + 8.0f, m_MapData->OffsetY + playerY + 31.0f, 3.0f);

        g_GL.PopScissor();
    }
}

void CGumpWorldMap::PrepareContent()
{
    CurrentOffsetX = OffsetX;
    CurrentOffsetY = OffsetY;

    int map = GetCurrentMap();

    LoadMap(map);

    int mapWidth  = 0;
    int mapHeight = 0;
    int playerX   = 0;
    int playerY   = 0;

    if (g_Player != nullptr)
    {
        playerX = g_Player->GetX();
        playerY = g_Player->GetY();
    }

    GetScaledDimensions(mapWidth, mapHeight, playerX, playerY);

    m_MapData->Width  = mapWidth;
    m_MapData->Height = mapHeight;

    int oldX = CurrentOffsetX;
    int oldY = CurrentOffsetY;

    if (m_LinkWithPlayer && g_CurrentMap == map && g_Player != nullptr)
    {
        CurrentOffsetX = (Width / 2) - playerX;

        if (CurrentOffsetX > 0)
        {
            CurrentOffsetX = 0;
        }

        CurrentOffsetY = ((Height - 30) / 2) - playerY;

        if (CurrentOffsetY > 0)
        {
            CurrentOffsetY = 0;
        }

        FixOffsets(CurrentOffsetY, CurrentOffsetY, Width, Height);

        if (OffsetX != CurrentOffsetX || OffsetY != CurrentOffsetY)
        {
            OffsetX = CurrentOffsetX;
            OffsetY = CurrentOffsetY;

            WantRedraw = true;
        }
    }
    else if (m_MapMoving)
    {
        Core::Vec2<i32> offset = g_MouseManager.GetLeftDroppedOffset();

        CurrentOffsetX += offset.x;
        CurrentOffsetY += offset.y;

        if (CurrentOffsetX > 0)
        {
            CurrentOffsetX = 0;
        }

        if (CurrentOffsetY > 0)
        {
            CurrentOffsetY = 0;
        }

        FixOffsets(CurrentOffsetX, CurrentOffsetY, Width, Height);
    }

    m_MapData->OffsetX = CurrentOffsetX;
    m_MapData->OffsetY = CurrentOffsetY;

    if (oldX != CurrentOffsetX || oldY != CurrentOffsetY)
    {
        WantRedraw = true;
    }
}

void CGumpWorldMap::OnLeftMouseButtonDown()
{
    CGump::OnLeftMouseButtonDown();

    if (g_PressedObject.LeftObject == m_MapData)
    {
        if (!m_LinkWithPlayer || g_CurrentMap != GetCurrentMap())
        {
            m_MapData->MoveOnDrag = false;
            m_MapMoving           = true;
        }
    }
}

void CGumpWorldMap::OnLeftMouseButtonUp()
{
    CGump::OnLeftMouseButtonUp();

    if (g_PressedObject.LeftObject == m_MapData)
    {
        if (m_MapMoving)
        {
            Core::Vec2<i32> offset = g_MouseManager.GetLeftDroppedOffset();
            OffsetX += offset.x;
            OffsetY += offset.y;
            FixOffsets(OffsetX, OffsetY, Width, Height);
        }

        m_MapMoving = false;
    }
}

void CGumpWorldMap::GUMP_BUTTON_EVENT_C
{
    if (serial == ID_GWM_MINIMIZE)
    {
        Minimized         = true;
        Page              = 1;
        WantUpdateContent = true;
    }
}

void CGumpWorldMap::GUMP_CHECKBOX_EVENT_C
{
    if (serial == ID_GWM_LINK_WITH_PLAYER)
    {
        m_LinkWithPlayer      = state;
        m_MapData->MoveOnDrag = (m_LinkWithPlayer || g_CurrentMap == GetCurrentMap());
    }
}

void CGumpWorldMap::GUMP_COMBOBOX_SELECTION_EVENT_C
{
    if (serial >= ID_GWM_SCALE_LIST)
    {
        int index = serial - ID_GWM_SCALE_LIST;

        ScaleOffsets(index, (Width / 2), ((Height - 30) / 2));
    }
    else if (serial >= ID_GWM_MAP_LIST)
    {
        int index = serial - ID_GWM_MAP_LIST;

        int mapTest = index;

        if (mapTest == 0)
        {
            mapTest = g_CurrentMap;
        }
        else
        {
            mapTest--;
        }

        if (g_MapTexture[mapTest].Texture == 0)
        {
            LoadMap(mapTest);
        }

        if (g_MapTexture[mapTest].Texture != 0)
        {
            if (mapTest != m_Map)
            {
                OffsetX = 0;
                OffsetY = 0;
            }

            m_Map            = index;
            m_MapData->Index = GetCurrentMap();
        }
        else
        {
            m_ComboboxMap->SelectedIndex = m_Map;
        }

        m_MapData->MoveOnDrag = (m_LinkWithPlayer || g_CurrentMap == GetCurrentMap());
    }

    int width   = 0;
    int height  = 0;
    int playerX = g_Player->GetX();
    int playerY = g_Player->GetY();

    GetScaledDimensions(width, height, playerX, playerY);

    m_MapData->Width  = width;
    m_MapData->Height = height;
}

bool CGumpWorldMap::OnLeftMouseButtonDoubleClick()
{
    bool result = false;

    if (Page == 1)
    {
        Minimized  = false;
        Page       = 2;
        WantRedraw = true;

        result = true;
    }

    return result;
}

void CGumpWorldMap::OnMidMouseButtonScroll(bool up)
{
    if (!Minimized && !g_MouseManager.LeftButtonPressed && !g_MouseManager.RightButtonPressed &&
        g_Orion.PolygonePixelsInXY(m_X + 8, m_Y + 31, Width - 16, Height - 16))
    {
        int ofs = 0;

        if (!up && m_Scale > 0)
        {
            ofs = -1;
        }
        else if (up && m_Scale < 6)
        {
            ofs = 1;
        }

        if (ofs != 0)
        {
            m_ComboboxScale->SelectedIndex += ofs;
            int mouseX = (Width / 2);         //g_MouseX - X + 8;
            int mouseY = ((Height - 30) / 2); //g_MouseY - Y + 31;

            ScaleOffsets(m_Scale + ofs, mouseX, mouseY);
            WantRedraw = true;

            int width   = 0;
            int height  = 0;
            int playerX = g_Player->GetX();
            int playerY = g_Player->GetY();

            GetScaledDimensions(width, height, playerX, playerY);

            m_MapData->Width  = width;
            m_MapData->Height = height;
        }
    }
}

void CGumpWorldMap::UpdateSize()
{
    Core::Rect<int> display = Core::Platform::GetDisplayArea();
    int screenX = display.size.x - 50;
    int screenY = display.size.y - 50;

    if (Height < MIN_WORLD_MAP_HEIGHT)
    {
        Height = MIN_WORLD_MAP_HEIGHT;
    }

    if (Height >= screenX)
    {
        Height = screenX;
    }

    if (Width < MIN_WORLD_MAP_WIDTH)
    {
        Width = MIN_WORLD_MAP_WIDTH;
    }

    if (Width >= screenY)
    {
        Width = screenY;
    }

    m_Minimizer->SetX((Width / 2) - 10);
    m_Background->Width  = Width;
    m_Background->Height = Height;
    m_Resizer->SetX(Width - 8);
    m_Resizer->SetY(Height + 13);
    Text->SetX(Width - Text->m_Texture.Width);
    m_Checkbox->SetX(Text->GetX() - 26);
    m_Scissor->Width  = Width - 16;
    m_Scissor->Height = Height - 16;
    WantRedraw        = true;
    WantUpdateContent = true;
}

void CGumpWorldMap::GUMP_RESIZE_START_EVENT_C
{
    m_StartResizeWidth  = Width;
    m_StartResizeHeight = Height;
}

void CGumpWorldMap::GUMP_RESIZE_EVENT_C
{
    if ((m_StartResizeWidth != 0) && (m_StartResizeHeight != 0))
    {
        Core::Vec2<i32> offset = g_MouseManager.GetLeftDroppedOffset();
        Width                  = m_StartResizeWidth + offset.x;
        Height                 = m_StartResizeHeight + offset.y;
        UpdateSize();
    }
}

void CGumpWorldMap::GUMP_RESIZE_END_EVENT_C
{
    m_StartResizeWidth  = 0;
    m_StartResizeHeight = 0;
}
