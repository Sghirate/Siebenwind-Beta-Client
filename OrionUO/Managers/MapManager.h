#pragma once

#include "Core/Minimal.h"
#include "Core/DataStream.h"
#include "BaseQueue.h"
#include "Globals.h"
#include <vector>

class CMapBlock;
class CRenderWorldObject;
struct RADAR_MAP_BLOCK;

class CIndexMap
{
public:
    size_t OriginalMapAddress    = 0;
    size_t OriginalStaticAddress = 0;
    u32 OriginalStaticCount      = 0;

    size_t MapAddress    = 0;
    size_t StaticAddress = 0;
    u32 StaticCount      = 0;

    CIndexMap();
    virtual ~CIndexMap();
};

typedef std::vector<CIndexMap> MAP_INDEX_LIST;

class CMapManager : public CBaseQueue
{
public:
    u32 MaxBlockIndex = 0;
    int PatchesCount  = 0;

protected:
    CMapBlock** m_Blocks{ nullptr };
    MAP_INDEX_LIST m_BlockData[MAX_MAPS_COUNT];
    bool m_BlockAccessList[0x1000];
    void ResetPatchesInBlockTable();

public:
    CMapManager();
    virtual ~CMapManager();

    int m_MapPatchCount[MAX_MAPS_COUNT];
    int m_StaticPatchCount[MAX_MAPS_COUNT];

    CIndexMap* GetIndex(int map, int blockX, int blockY);
    void CreateBlockTable(int map);
    void CreateBlocksTable();
    void ApplyPatches(Core::StreamReader& stream);
    void UpdatePatched();
    void ClearBlockAccess();
    char CalculateNearZ(char defaultZ, int x, int y, int z);
    int GetActualMap();
    void SetPatchedMapBlock(size_t block, size_t address);
    void LoadBlock(CMapBlock* block);
    void GetRadarMapBlock(int blockX, int blockY, RADAR_MAP_BLOCK& mb);
    void GetMapZ(int x, int y, int& groundZ, int& staticZ);
    void Init(bool delayed = false);
    CMapBlock* GetBlock(u32 index);
    CMapBlock* AddBlock(u32 index);
    void DeleteBlock(u32 index);
    void ClearUnusedBlocks();
    void ClearUsedBlocks();
    void AddRender(CRenderWorldObject* item);
};

extern CMapManager g_MapManager;
