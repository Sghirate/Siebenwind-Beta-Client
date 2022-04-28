#include "MapManager.h"
#include "Globals.h"
#include "GumpManager.h"
#include "FileManager.h"
#include "../OrionUO.h"
#include "../Profiler.h"
#include "../Gumps/GumpMinimap.h"
#include "../GameObjects/MapBlock.h"
#include "../GameObjects/GamePlayer.h"
#include "../GameObjects/LandObject.h"
#include "../GameObjects/StaticObject.h"
#include "plugin/enumlist.h"

CMapManager g_MapManager;

CIndexMap::CIndexMap()
{
}

CIndexMap::~CIndexMap()
{
}

CMapManager::CMapManager()

{
}

CMapManager::~CMapManager()
{
    if (m_Blocks != nullptr)
    {
        ClearUsedBlocks();

        delete[] m_Blocks;
        m_Blocks = nullptr;
    }

    MaxBlockIndex = 0;
}

void CMapManager::CreateBlocksTable()
{
    for (int map = 0; map < MAX_MAPS_COUNT; map++)
    {
        CreateBlockTable((int)map);
    }
}

void CMapManager::CreateBlockTable(int map)
{
    MAP_INDEX_LIST& list  = m_BlockData[map];
    Core::Vec2<i32>& size = g_MapBlockSize[map];

    int maxBlockCount = size.x * size.y;

    //Return and error notification?
    if (maxBlockCount < 1)
    {
        return;
    }

    list.resize(maxBlockCount);

    size_t mapAddress    = (size_t)g_FileManager.m_MapMul[map].GetBuffer();
    size_t endMapAddress = mapAddress + g_FileManager.m_MapMul[map].GetSize();

    CUopMappedFile& uopFile = g_FileManager.m_MapUOP[map];
    const bool isUop        = (uopFile.GetBuffer() != nullptr);
    if (isUop)
    {
        mapAddress    = (size_t)uopFile.GetBuffer();
        endMapAddress = mapAddress + uopFile.GetSize();
    }

    size_t staticIdxAddress    = (size_t)g_FileManager.m_StaticIdx[map].GetBuffer();
    size_t endStaticIdxAddress = staticIdxAddress + g_FileManager.m_StaticIdx[map].GetSize();
    size_t staticAddress       = (size_t)g_FileManager.m_StaticMul[map].GetBuffer();
    size_t endStaticAddress    = staticAddress + g_FileManager.m_StaticMul[map].GetSize();
    if ((mapAddress == 0u) || (staticIdxAddress == 0u) || (staticAddress == 0u))
    {
        return;
    }

    int fileNumber   = -1;
    size_t uopOffset = 0;
    for (int block = 0; block < maxBlockCount; block++)
    {
        CIndexMap& index         = list[block];
        size_t realMapAddress    = 0;
        size_t realStaticAddress = 0;
        int realStaticCount      = 0;
        int blockNumber          = (int)block;
        if (isUop)
        {
            blockNumber &= 4095;
            int shifted = (int)block >> 12;
            if (fileNumber != shifted)
            {
                fileNumber            = shifted;
                char mapFilePath[200] = { 0 };
                sprintf_s(mapFilePath, "build/map%ilegacymul/%08i.dat", map, shifted);

                CUopBlockHeader* uopBlock = uopFile.GetBlock(COrion::CreateHash(mapFilePath));
                if (uopBlock != nullptr)
                {
                    uopOffset = (size_t)uopBlock->Offset;
                }
                else
                {
                    LOG_WARNING("MapManager", "Hash not found in uop map %i file.", map);
                }
            }
        }

        size_t address = mapAddress + uopOffset + (blockNumber * sizeof(MAP_BLOCK));
        if (address < endMapAddress)
        {
            realMapAddress = address;
        }

        STAIDX_BLOCK* sidx = (STAIDX_BLOCK*)(staticIdxAddress + block * sizeof(STAIDX_BLOCK));
        if ((size_t)sidx < endStaticIdxAddress && sidx->Size > 0 && sidx->Position != 0xFFFFFFFF)
        {
            size_t address = staticAddress + sidx->Position;
            if (address < endStaticAddress)
            {
                realStaticAddress = address;
                realStaticCount   = sidx->Size / sizeof(STATICS_BLOCK);
                if (realStaticCount > 1024)
                {
                    realStaticCount = 1024;
                }
            }
        }

        index.OriginalMapAddress    = realMapAddress;
        index.OriginalStaticAddress = realStaticAddress;
        index.OriginalStaticCount   = realStaticCount;

        index.MapAddress    = realMapAddress;
        index.StaticAddress = realStaticAddress;
        index.StaticCount   = realStaticCount;
    }
}

void CMapManager::SetPatchedMapBlock(size_t block, size_t address)
{
    MAP_INDEX_LIST& list  = m_BlockData[0];
    Core::Vec2<i32>& size = g_MapBlockSize[0];
    int maxBlockCount     = size.x * size.y;
    if (maxBlockCount < 1)
        return;

    list[block].OriginalMapAddress = address;
    list[block].MapAddress         = address;
}

void CMapManager::ResetPatchesInBlockTable()
{
    for (int map = 0; map < MAX_MAPS_COUNT; map++)
    {
        MAP_INDEX_LIST& list  = m_BlockData[map];
        Core::Vec2<i32>& size = g_MapBlockSize[map];
        int maxBlockCount     = size.x * size.y;
        if (maxBlockCount < 1)
            return;

        if (g_FileManager.m_MapMul[map].GetBuffer() == nullptr ||
            g_FileManager.m_StaticIdx[map].GetBuffer() == nullptr ||
            g_FileManager.m_StaticMul[map].GetBuffer() == nullptr)
        {
            return;
        }

        for (int block = 0; block < maxBlockCount; block++)
        {
            CIndexMap& index = list[block];

            index.MapAddress    = index.OriginalMapAddress;
            index.StaticAddress = index.OriginalStaticAddress;
            index.StaticCount   = index.OriginalStaticCount;
        }
    }
}

void CMapManager::ApplyPatches(Core::StreamReader& stream)
{
    ResetPatchesInBlockTable();

    PatchesCount = stream.ReadBE<u32>();

    if (PatchesCount < 0)
        PatchesCount = 0;

    if (PatchesCount > MAX_MAPS_COUNT)
        PatchesCount = MAX_MAPS_COUNT;

    memset(&m_MapPatchCount[0], 0, sizeof(m_MapPatchCount));
    memset(&m_StaticPatchCount[0], 0, sizeof(m_StaticPatchCount));

    for (int i = 0; i < PatchesCount; i++)
    {
        if (g_FileManager.m_MapMul[i].GetBuffer() == nullptr)
        {
            stream.Move(8);
            continue;
        }

        intptr_t mapPatchesCount     = stream.ReadBE<u32>();
        m_MapPatchCount[i]           = (int)mapPatchesCount;
        intptr_t staticsPatchesCount = stream.ReadBE<u32>();
        m_StaticPatchCount[i]        = (int)staticsPatchesCount;

        MAP_INDEX_LIST& list  = m_BlockData[i];
        Core::Vec2<i32>& size = g_MapBlockSize[i];

        u32 maxBlockCount = size.x * size.y;
        if (mapPatchesCount != 0)
        {
            Core::MappedFile& difl = g_FileManager.m_MapDifl[i];
            Core::MappedFile& dif  = g_FileManager.m_MapDif[i];

            mapPatchesCount = Core::Min(mapPatchesCount, (intptr_t)difl.GetSize() / 4);

            difl.ResetPtr();
            dif.ResetPtr();

            for (int j = 0; j < mapPatchesCount; j++)
            {
                u32 blockIndex = difl.ReadLE<u32>();
                if (blockIndex < maxBlockCount)
                    list[blockIndex].MapAddress = (size_t)dif.GetPtr();

                dif.Move(sizeof(MAP_BLOCK));
            }
        }

        if (staticsPatchesCount != 0)
        {
            Core::MappedFile& difl = g_FileManager.m_StaDifl[i];
            Core::MappedFile& difi = g_FileManager.m_StaDifi[i];
            size_t startAddress    = (size_t)g_FileManager.m_StaDif[i].GetBuffer();

            staticsPatchesCount = Core::Min(staticsPatchesCount, (intptr_t)difl.GetSize() / 4);

            difl.ResetPtr();
            difi.ResetPtr();

            for (int j = 0; j < staticsPatchesCount; j++)
            {
                u32 blockIndex = difl.ReadLE<u32>();

                STAIDX_BLOCK* sidx = (STAIDX_BLOCK*)difi.GetPtr();

                difi.Move(sizeof(STAIDX_BLOCK));

                if (blockIndex < maxBlockCount)
                {
                    size_t realStaticAddress = 0;
                    int realStaticCount      = 0;

                    if (sidx->Size > 0 && sidx->Position != 0xFFFFFFFF)
                    {
                        realStaticAddress = startAddress + sidx->Position;
                        realStaticCount   = sidx->Size / sizeof(STATICS_BLOCK);

                        if (realStaticCount > 0)
                        {
                            if (realStaticCount > 1024)
                            {
                                realStaticCount = 1024;
                            }
                        }
                    }

                    list[blockIndex].StaticAddress = realStaticAddress;
                    list[blockIndex].StaticCount   = realStaticCount;
                }
            }
        }
    }

    UpdatePatched();
}

void CMapManager::UpdatePatched()
{
    if (g_Player == nullptr)
    {
        return;
    }

    std::deque<CRenderWorldObject*> objectsList;

    if (m_Blocks != nullptr)
    {
        QFOR(block, m_Items, CMapBlock*)
        {
            for (int x = 0; x < 8; x++)
            {
                for (int y = 0; y < 8; y++)
                {
                    for (CRenderWorldObject* item = block->GetRender((int)x, (int)y);
                         item != nullptr;
                         item = item->m_NextXY)
                    {
                        if (!item->IsLandObject() && !item->IsStaticObject())
                        {
                            objectsList.push_back(item);
                        }
                    }
                }
            }
        }
    }

    Init(false);

    for (CRenderWorldObject* item : objectsList)
    {
        AddRender(item);
    }

    CGumpMinimap* gump = (CGumpMinimap*)g_GumpManager.UpdateGump(0, 0, GT_MINIMAP);

    if (gump != nullptr)
    {
        gump->LastX = 0;
    }
}

CIndexMap* CMapManager::GetIndex(int map, int blockX, int blockY)
{
    if (map >= MAX_MAPS_COUNT)
    {
        return nullptr;
    }

    u32 block            = (blockX * g_MapBlockSize[map].y) + blockY;
    MAP_INDEX_LIST& list = m_BlockData[map];

    if (block >= list.size())
    {
        return nullptr;
    }

    return &list[block];
}

void CMapManager::ClearBlockAccess()
{
    memset(&m_BlockAccessList[0], 0, sizeof(m_BlockAccessList));
}

char CMapManager::CalculateNearZ(char defaultZ, int x, int y, int z)
{
    int blockX = x / 8;
    int blockY = y / 8;
    u32 index  = (blockX * g_MapBlockSize[g_CurrentMap].y) + blockY;

    bool& accessBlock = m_BlockAccessList[(x & 0x3F) + ((y & 0x3F) << 6)];

    if (accessBlock)
    {
        return defaultZ;
    }

    accessBlock      = true;
    CMapBlock* block = GetBlock(index);

    if (block != nullptr)
    {
        CMapObject* item = block->Block[x % 8][y % 8];

        for (; item != nullptr; item = (CMapObject*)item->m_Next)
        {
            if (!item->IsGameObject())
            {
                if (!item->IsStaticObject() && !item->IsMultiObject())
                {
                    continue;
                }
            }
            else if (((CGameObject*)item)->NPC)
            {
                continue;
            }

            if (!item->IsRoof() || abs(z - item->GetZ()) > 6)
            {
                continue;
            }

            break;
        }

        if (item == nullptr)
        {
            return defaultZ;
        }

        char tileZ = item->GetZ();

        if (tileZ < defaultZ)
        {
            defaultZ = tileZ;
        }

        defaultZ = CalculateNearZ(defaultZ, x - 1, y, tileZ);
        defaultZ = CalculateNearZ(defaultZ, x + 1, y, tileZ);
        defaultZ = CalculateNearZ(defaultZ, x, y - 1, tileZ);
        defaultZ = CalculateNearZ(defaultZ, x, y + 1, tileZ);
    }

    return defaultZ;
}

void CMapManager::GetRadarMapBlock(int blockX, int blockY, RADAR_MAP_BLOCK& mb)
{
    CIndexMap* indexMap = GetIndex(GetActualMap(), blockX, blockY);

    if (indexMap == nullptr || indexMap->MapAddress == 0)
    {
        return;
    }

    MAP_BLOCK* pmb = (MAP_BLOCK*)indexMap->MapAddress;

    for (int x = 0; x < 8; x++)
    {
        for (int y = 0; y < 8; y++)
        {
            MAP_CELLS& inCell        = pmb->Cells[(y * 8) + x];
            RADAR_MAP_CELLS& outCell = mb.Cells[x][y];
            outCell.Graphic          = inCell.TileID;
            outCell.Z                = inCell.Z;
            outCell.IsLand           = 1;
        }
    }

    STATICS_BLOCK* sb = (STATICS_BLOCK*)indexMap->StaticAddress;

    if (sb != nullptr)
    {
        int count = indexMap->StaticCount;

        for (int c = 0; c < count; c++)
        {
            if ((sb->Color != 0u) && sb->Color != 0xFFFF &&
                !CRenderStaticObject::IsNoDrawTile(sb->Color))
            {
                RADAR_MAP_CELLS& outCell = mb.Cells[sb->X][sb->Y];

                //int pos = (sb->GetY() * 8) + sb->GetX();
                //if (pos > 64) continue;

                if (outCell.Z <= sb->Z)
                {
                    outCell.Graphic = sb->Color;
                    outCell.Z       = sb->Z;
                    outCell.IsLand  = 0;
                }
            }

            sb++;
        }
    }
}

void CMapManager::GetMapZ(int x, int y, int& groundZ, int& staticZ)
{
    int blockX = x / 8;
    int blockY = y / 8;
    u32 index  = (blockX * g_MapBlockSize[g_CurrentMap].y) + blockY;

    if (index < MaxBlockIndex)
    {
        CMapBlock* block = GetBlock(index);

        if (block == nullptr)
        {
            block    = AddBlock(index);
            block->X = blockX;
            block->Y = blockY;
            LoadBlock(block);
        }

        CMapObject* item = block->Block[x % 8][y % 8];

        while (item != nullptr)
        {
            if (item->IsLandObject())
            {
                groundZ = item->GetZ();
            }
            else if (staticZ < item->GetZ())
            {
                staticZ = item->GetZ();
            }

            item = (CMapObject*)item->m_Next;
        }
    }
}

void CMapManager::ClearUnusedBlocks()
{
    CMapBlock* block = (CMapBlock*)m_Items;
    int count        = 0;
    Core::TimeStamp clearBefore =
        Core::FrameTimer::Now() - Core::TimeDiff::FromMilliseconds(CLEAR_TEXTURES_DELAY);

    while (block != nullptr)
    {
        CMapBlock* next = (CMapBlock*)block->m_Next;

        if (block->LastAccessed < clearBefore && block->HasNoExternalData())
        {
            u32 index = block->Index;
            Delete(block);

            m_Blocks[index] = nullptr;

            if (++count >= MAX_MAP_OBJECT_REMOVED_BY_GARBAGE_COLLECTOR)
            {
                break;
            }
        }

        block = next;
    }
}

void CMapManager::ClearUsedBlocks()
{
    CMapBlock* block = (CMapBlock*)m_Items;

    while (block != nullptr)
    {
        CMapBlock* next = (CMapBlock*)block->m_Next;

        u32 index = block->Index;
        Delete(block);

        m_Blocks[index] = nullptr;

        block = next;
    }
}

void CMapManager::Init(bool delayed)
{
    PROFILER_EVENT();
    if (g_Player == nullptr)
    {
        return;
    }

    int map = GetActualMap();
    if (!delayed)
    {
        if (m_Blocks != nullptr)
        {
            ClearUsedBlocks();

            delete[] m_Blocks;
            m_Blocks = nullptr;
        }

        MaxBlockIndex = g_MapBlockSize[map].x * g_MapBlockSize[map].y;
        m_Blocks      = new CMapBlock*[MaxBlockIndex];
        memset(&m_Blocks[0], 0, sizeof(CMapBlock*) * MaxBlockIndex);
        ClearBlockAccess();
        PatchesCount = 0;
        memset(&m_MapPatchCount[0], 0, sizeof(m_MapPatchCount));
        memset(&m_StaticPatchCount[0], 0, sizeof(m_StaticPatchCount));
    }

    const int XY_Offset = 30; //70;
    int minBlockX       = (g_Player->GetX() - XY_Offset) / 8 - 1;
    int minBlockY       = (g_Player->GetY() - XY_Offset) / 8 - 1;
    int maxBlockX       = ((g_Player->GetX() + XY_Offset) / 8) + 1;
    int maxBlockY       = ((g_Player->GetY() + XY_Offset) / 8) + 1;

    if (minBlockX < 0)
        minBlockX = 0;

    if (minBlockY < 0)
        minBlockY = 0;

    if (maxBlockX >= g_MapBlockSize[map].x)
        maxBlockX = g_MapBlockSize[map].x - 1;

    if (maxBlockY >= g_MapBlockSize[map].y)
        maxBlockY = g_MapBlockSize[map].y - 1;

    u32 ticks    = g_Ticks;
    u32 maxDelay = g_FrameDelay[WINDOW_ACTIVE] / 2;
    for (int i = minBlockX; i <= maxBlockX; i++)
    {
        u32 index = i * g_MapBlockSize[map].y;
        for (int j = minBlockY; j <= maxBlockY; j++)
        {
            u32 realIndex = index + j;
            if (realIndex < MaxBlockIndex)
            {
                CMapBlock* block = GetBlock(realIndex);
                if (block == nullptr)
                {
                    if (delayed && g_Ticks - ticks >= maxDelay)
                        return;

                    block    = AddBlock(realIndex);
                    block->X = i;
                    block->Y = j;
                    LoadBlock(block);
                }
            }
        }
    }
}

void CMapManager::LoadBlock(CMapBlock* block)
{
    int map = GetActualMap();

    CIndexMap* indexMap = GetIndex(GetActualMap(), block->X, block->Y);

    if (indexMap == nullptr || indexMap->MapAddress == 0)
    {
        return;
    }

    MAP_BLOCK* pmb = (MAP_BLOCK*)indexMap->MapAddress;

    int bx = block->X * 8;
    int by = block->Y * 8;

    for (int x = 0; x < 8; x++)
    {
        for (int y = 0; y < 8; y++)
        {
            int pos         = (int)y * 8 + (int)x;
            CMapObject* obj = new CLandObject(
                pos,
                pmb->Cells[pos].TileID & 0x3FFF,
                0,
                bx + (int)x,
                by + (int)y,
                pmb->Cells[pos].Z);
            block->AddObject(obj, (int)x, (int)y);
        }
    }

    STATICS_BLOCK* sb = (STATICS_BLOCK*)indexMap->StaticAddress;

    if (sb != nullptr)
    {
        int count = indexMap->StaticCount;

        for (int c = 0; c < count; c++, sb++)
        {
            if ((sb->Color != 0u) && sb->Color != 0xFFFF)
            {
                int x = sb->X;
                int y = sb->Y;

                int pos = (y * 8) + x;

                if (pos >= 64)
                {
                    continue;
                }

                CRenderStaticObject* obj =
                    new CStaticObject(pos, sb->Color, sb->Hue, bx + x, by + y, sb->Z);

                block->AddObject(obj, x, y);
            }
        }
    }

    block->CreateLandTextureRect();
}

int CMapManager::GetActualMap()
{
    if (g_CurrentMap == 1 && (((g_FileManager.m_MapUOP[1].GetBuffer() == nullptr) &&
                               (g_FileManager.m_MapMul[1].GetBuffer() == nullptr)) ||
                              (g_FileManager.m_StaticIdx[1].GetBuffer() == nullptr) ||
                              (g_FileManager.m_StaticMul[1].GetBuffer() == nullptr)))
    {
        return 0;
    }

    return g_CurrentMap;
}

void CMapManager::AddRender(CRenderWorldObject* item)
{
    int itemX = item->GetX();
    int itemY = item->GetY();

    int x = itemX / 8;
    int y = itemY / 8;

    u32 index = (x * g_MapBlockSize[g_CurrentMap].y) + y;

    if (index < MaxBlockIndex)
    {
        CMapBlock* block = GetBlock(index);

        if (block == nullptr)
        {
            block    = AddBlock(index);
            block->X = x;
            block->Y = y;
            LoadBlock(block);
        }

        x = itemX % 8;
        y = itemY % 8;

        block->AddRender(item, x, y);
    }
}

CMapBlock* CMapManager::GetBlock(u32 index)
{
    CMapBlock* block = nullptr;
    if (index < MaxBlockIndex)
    {
        block = m_Blocks[index];
        if (block != nullptr)
            block->LastAccessed = Core::FrameTimer::Now();
    }
    return block;
}

CMapBlock* CMapManager::AddBlock(u32 index)
{
    CMapBlock* block = (CMapBlock*)Add(new CMapBlock(index));
    m_Blocks[index] = block;
    return block;
}

void CMapManager::DeleteBlock(u32 index)
{
    CMapBlock* block = (CMapBlock*)m_Items;

    while (block != nullptr)
    {
        if (block->Index == index)
        {
            Delete(block);
            m_Blocks[index] = nullptr;

            break;
        }

        block = (CMapBlock*)block->m_Next;
    }
}
