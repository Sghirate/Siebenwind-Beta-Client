#include "GumpMinimap.h"
#include "Core/Time.h"
#include "Globals.h"
#include "OrionUO.h"
#include "Managers/ConfigManager.h"
#include "Managers/ColorManager.h"
#include "Managers/MapManager.h"
#include "Managers/UOFileReader.h"
#include "GameObjects/GameItem.h"
#include "GameObjects/MapBlock.h"
#include "GameObjects/GamePlayer.h"

using namespace Core::TimeLiterals;

CGumpMinimap::CGumpMinimap(short x, short y, bool minimized)
    : CGump(GT_MINIMAP, 0, x, y)
{
    Minimized = minimized;
    m_Locker.Serial = ID_GMM_LOCK_MOVING;
    GenerateMap();
}

CGumpMinimap::~CGumpMinimap()
{
    m_Texture.Clear();
}

void CGumpMinimap::CalculateGumpState()
{
    bool minimized = Minimized;
    Minimized = false;

    CGump::CalculateGumpState();

    Minimized = minimized;
}

void CGumpMinimap::GenerateMap()
{

    /*const Core::Vec2<i32> foliageOffsetTable[17 * 3] =
	{
		Core::Vec2<i32>(0, 0),
		Core::Vec2<i32>(-2, 1),
		Core::Vec2<i32>(-2, -2),
		Core::Vec2<i32>(-1, -1),
		Core::Vec2<i32>(-1, 0),
		Core::Vec2<i32>(-1, 1),
		Core::Vec2<i32>(-1, 2),
		Core::Vec2<i32>(-1, -1),
		Core::Vec2<i32>(0, 1),
		Core::Vec2<i32>(0, 2),
		Core::Vec2<i32>(0, -2),
		Core::Vec2<i32>(1, -1),
		Core::Vec2<i32>(1, 0),
		Core::Vec2<i32>(1, 1),
		Core::Vec2<i32>(1, -1),
		Core::Vec2<i32>(2, 0),
		Core::Vec2<i32>(2, 0),

		Core::Vec2<i32>(0, -1),
		Core::Vec2<i32>(-2, 0),
		Core::Vec2<i32>(-2, -1),
		Core::Vec2<i32>(-1, 0),
		Core::Vec2<i32>(-1, 1),
		Core::Vec2<i32>(-1, 2),
		Core::Vec2<i32>(-1, -2),
		Core::Vec2<i32>(0, -1),
		Core::Vec2<i32>(0, 1),
		Core::Vec2<i32>(0, 2),
		Core::Vec2<i32>(0, -2),
		Core::Vec2<i32>(1, -1),
		Core::Vec2<i32>(1, 0),
		Core::Vec2<i32>(1, 1),
		Core::Vec2<i32>(1, 0),
		Core::Vec2<i32>(2, 1),
		Core::Vec2<i32>(2, 0),

		Core::Vec2<i32>(0, -1),
		Core::Vec2<i32>(-2, 1),
		Core::Vec2<i32>(-2, -2),
		Core::Vec2<i32>(-1, -1),
		Core::Vec2<i32>(-1, 0),
		Core::Vec2<i32>(-1, 1),
		Core::Vec2<i32>(-1, 2),
		Core::Vec2<i32>(-1, -1),
		Core::Vec2<i32>(0, 1),
		Core::Vec2<i32>(0, -2),
		Core::Vec2<i32>(1, -1),
		Core::Vec2<i32>(1, 0),
		Core::Vec2<i32>(1, 1),
		Core::Vec2<i32>(1, 2),
		Core::Vec2<i32>(1, -1),
		Core::Vec2<i32>(2, 1),
		Core::Vec2<i32>(2, 0)
	};*/

    const Core::Vec2<i32> originalOffsetTable[2] = { Core::Vec2<i32>(0, 0), Core::Vec2<i32>(0, 1) };

    if (g_Player != nullptr)
    {
        LastX = g_Player->GetX();
        LastY = g_Player->GetY();
    }

    m_Texture.Clear();

    u16 gumpID = 0x1393 - (int)Minimized;
    CIndexObject &io = g_Orion.m_GumpDataIndex[gumpID];

    int gumpWidth = io.Width;
    int gumpHeight = io.Height;

    std::vector<u16> data = g_UOFileReader.GetGumpPixels(io);

    if (data.empty())
    {
        return;
    }

    int blockOffsetX = gumpWidth / 4;
    int blockOffsetY = gumpHeight / 4;

    int gumpCenterX = gumpWidth / 2;
    int gumpCenterY = gumpHeight / 2;

    //0xFF080808 - pixel32
    //0x8421 - pixel16

    int minBlockX = (LastX - blockOffsetX) / 8 - 1;
    int minBlockY = (LastY - blockOffsetY) / 8 - 1;
    int maxBlockX = ((LastX + blockOffsetX) / 8) + 1;
    int maxBlockY = ((LastY + blockOffsetY) / 8) + 1;

    if (minBlockX < 0)
    {
        minBlockX = 0;
    }

    if (minBlockY < 0)
    {
        minBlockY = 0;
    }

    int map = g_MapManager.GetActualMap();
    u32 maxBlockIndex = g_MapManager.MaxBlockIndex;
    int mapBlockHeight = g_MapBlockSize[map].y;

    for (int i = minBlockX; i <= maxBlockX; i++)
    {
        u32 blockIndexOffset = i * mapBlockHeight;

        for (int j = minBlockY; j <= maxBlockY; j++)
        {
            u32 blockIndex = blockIndexOffset + j;

            if (blockIndex >= maxBlockIndex)
            {
                break;
            }

            RADAR_MAP_BLOCK mb = {};
            g_MapManager.GetRadarMapBlock(i, j, mb);

            CMapBlock *mapBlock = g_MapManager.GetBlock(blockIndex);

            int realBlockX = (i * 8);
            int realBlockY = (j * 8);

            for (int x = 0; x < 8; x++)
            {
                int px = ((realBlockX + (int)x) - LastX) + gumpCenterX;

                for (int y = 0; y < 8; y++)
                {
                    int py = (realBlockY + (int)y) - LastY;

                    int gx = px - py;
                    int gy = px + py;

                    u32 color = mb.Cells[x][y].Graphic;
                    char &isLand = mb.Cells[x][y].IsLand;

                    if (mapBlock != nullptr)
                    {
                        u16 multiColor = mapBlock->GetRadarColor((int)x, (int)y);

                        if (multiColor != 0u)
                        {
                            color = multiColor;
                            isLand = 0;
                        }
                    }

                    if (isLand == 0)
                    {
                        color = g_Orion.GetSeasonGraphic(color) + 0x4000;
                    }
                    else
                    {
                        color = g_Orion.GetLandSeasonGraphic(color);
                    }

                    int tableSize = 2;
                    const Core::Vec2<i32> *table = &originalOffsetTable[0];

                    /*if (color > 0x4000 && ::IsFoliage(g_Orion.GetStaticFlags(color - 0x4000)))
					{
						tableSize = 17;
						table = &foliageOffsetTable[((color - 0x4000) % 3) * tableSize];
					}*/

                    color = 0x8000 | g_ColorManager.GetRadarColorData(color);

                    CreatePixels(data, color, gx, gy, gumpWidth, gumpHeight, table, tableSize);
                }
            }
        }
    }

    g_GL_BindTexture16(m_Texture, gumpWidth, gumpHeight, &data[0]);

    WantUpdateContent = true;
}

void CGumpMinimap::CreatePixels(
    std::vector<u16> &data,
    int color,
    int x,
    int y,
    int width,
    int height,
    const Core::Vec2<i32> *table,
    int count)
{
    int px = x;
    int py = y;

    for (int i = 0; i < count; i++)
    {
        px += table[i].x;
        py += table[i].y;
        int gx = px;

        if (gx < 0 || gx >= width)
        {
            continue;
        }

        int gy = py;

        if (gy < 0 || gy >= height)
        {
            break;
        }

        int block = (gy * width) + gx;

        if (data[block] == 0x8421)
        {
            data[block] = color;
        }
    }
}

void CGumpMinimap::PrepareContent()
{
    if (g_Player->GetX() != LastX || g_Player->GetY() != LastY || m_Texture.Texture == 0)
    {
        GenerateMap();
    }
    else if ((m_Count == 0u) || m_Count == 6 || WantRedraw)
    {
        WantUpdateContent = true;
    }

    Core::TimeStamp now = Core::FrameTimer::Now();
    static Core::TimeStamp nextInc = now;
    static const Core::TimeDiff kIncDelayMs = 300_ms;
    if (nextInc <= now)
    {
        m_Count += 7;
        nextInc = now + kIncDelayMs;
    }

    if (m_Count > 12)
    {
        m_Count = 0;
    }
}

void CGumpMinimap::UpdateContent()
{
    u16 graphic = 0x1393 - (int)Minimized;

    CGLTexture *th = g_Orion.ExecuteGump(graphic);

    if (th == nullptr)
    {
        return;
    }

    if (m_Items == nullptr)
    {
        m_DataBox = (CGUIDataBox *)Add(new CGUIDataBox());
        m_Body = (CGUIGumppic *)Add(new CGUIGumppic(graphic, 0, 0));
        m_Body->SelectOnly = true;
    }
    else
    {
        m_DataBox->Clear();
    }

    m_Body->Graphic = graphic;

    int playerX = g_Player->GetX();
    int playerY = g_Player->GetY();

    int gumpWidth = th->Width;
    int gumpHeight = th->Height;

    int gumpCenterX = (gumpWidth / 2) - 1;
    int gumpCenterY = (gumpHeight / 2) - 1;

    m_DataBox->Add(new CGUIExternalTexture(&m_Texture, false, 0, 0));

    if (m_Count < 6)
    {
        QFOR(go, g_World->m_Items, CGameObject *)
        {
            if (go->Container != 0xFFFFFFFF || ((CGameItem *)go)->MultiBody)
            {
                continue; //multi
            }

            if (go->NPC && !go->IsPlayer())
            {
                u16 color =
                    g_ConfigManager.GetColorByNotoriety(go->GameCharacterPtr()->Notoriety);

                if (color != 0u)
                {
                    u32 pcl = g_ColorManager.GetPolygoneColor(16, color);

                    int x = go->GetX() - playerX;
                    int y = go->GetY() - playerY;

                    int gx = x - y;
                    int gy = x + y;

                    m_DataBox->Add(new CGUIColoredPolygone(
                        0, 0, gumpCenterX + gx, gumpCenterY + gy, 2, 2, pcl));
                }
            }
        }

        m_DataBox->Add(new CGUIColoredPolygone(0, 0, gumpCenterX, gumpCenterY, 2, 2, 0xFFFFFFFF));
    }
}

void CGumpMinimap::GUMP_BUTTON_EVENT_C
{
    if (serial == ID_GMM_LOCK_MOVING)
    {
        LockMoving = !LockMoving;
    }
}

bool CGumpMinimap::OnLeftMouseButtonDoubleClick()
{

    g_Orion.OpenMinimap();

    return true;
}
