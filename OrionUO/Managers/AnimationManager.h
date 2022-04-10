#pragma once

#include "Core/DataStream.h"
#include "../IndexObject.h"
#include <deque>
#include <unordered_map>
#include <vector>

class CTargetGump;
class CGameCharacter;

static const int MAX_LAYER_DIRECTIONS = 8;
static const float UPPER_BODY_RATIO = 0.35f;
static const float MID_BODY_RATIO = 0.60f;
static const float LOWER_BODY_RATIO = 0.94f;

struct FRAME_OUTPUT_INFO
{
    int StartX = 0;
    int StartY = 0;
    int EndX = 0;
    int EndY = 0;
};

struct ANIMATION_DIMENSIONS
{
    int Width = 0;
    int Height = 0;
    int CenterX = 0;
    int CenterY = 0;
};
struct UOPFrameData
{
    u8 *dataStart = nullptr;
    short frameId = 0;
    u32 pixelDataOffset = 0;
};

class CEquipConvData
{
public:
    u16 Graphic = 0;
    u16 Gump = 0;
    u16 Color = 0;

    CEquipConvData(u16 graphic, u16 gump, u16 color)
        : Graphic(graphic)
        , Gump(gump)
        , Color(color)
    {
    }
    ~CEquipConvData() {}
};

typedef std::unordered_map<u16, CEquipConvData> EQUIP_CONV_DATA_MAP;
typedef std::unordered_map<u16, EQUIP_CONV_DATA_MAP> EQUIP_CONV_BODY_MAP;

class CAnimationManager : public Core::StreamReader
{
public:
    u16 Color = 0;
    u8 AnimGroup = 0;
    u8 Direction = 0;
    u16 AnimID = 0;
    u8 AnimGroupCount = PAG_ANIMATION_COUNT;

private:
    size_t m_AddressIdx[6];
    size_t m_SizeIdx[6];

    int m_CharacterFrameHeight = 0;

    static void PrepareTargetAttackGump(
        CTargetGump &gump,
        int drawX,
        int drawY,
        u16 targetColor,
        int per,
        CGameCharacter &obj);

    int m_CharacterFrameStartY = 0;
    int m_StartCharacterWaistY = 0;
    int m_StartCharacterKneesY = 0;
    int m_StartCharacterFeetY = 0;
    int m_Sitting = 0;
    bool m_UseBlending = false;

    EQUIP_CONV_BODY_MAP m_EquipConv;

    CEquipConvData *m_EquipConvItem{ nullptr };

    static const int USED_LAYER_COUNT = 23;
    static const int m_UsedLayers[MAX_LAYER_DIRECTIONS][USED_LAYER_COUNT];

    std::vector<std::pair<u16, u8>> m_GroupReplaces[2];

    std::deque<CTextureAnimationDirection *> m_UsedAnimList;

    bool TestPixels(
        class CGameObject *obj,
        int x,
        int y,
        bool mirror,
        u8 &frameIndex,
        u16 id = 0x0000);

    void FixSittingDirection(u8 &layerDirection, bool &mirror, int &x, int &y);
    void Draw(class CGameObject *obj, int x, int y, bool mirror, u8 &frameIndex, int id = 0);
    void DrawIntoFrameBuffer(class CGameCharacter *obj, int x, int y);

    bool DrawEquippedLayers(
        bool selection,
        class CGameObject *obj,
        int drawX,
        int drawY,
        bool mirror,
        u8 layerDir,
        u8 animIndex,
        int lightOffset);

    bool IsCovered(int layer, class CGameObject *owner);
    bool m_Transform = false;

    u16 m_CharacterLayerGraphic[25];
    u16 m_CharacterLayerAnimID[25];

    bool TryReadUOPAnimDimins(CTextureAnimationDirection &direction);
    std::vector<UOPFrameData> ReadUOPFrameDataOffsets();

    void ReadUOPFrameData(
        short &imageCenterX,
        short &imageCenterY,
        short &imageWidth,
        short &imageHeight,
        u16 *&palette,
        UOPFrameData &frameData);

    u8 GetObjectNewAnimationType_0(CGameCharacter *obj, u16 action, u8 mode);
    u8 GetObjectNewAnimationType_1_2(CGameCharacter *obj, u16 action, u8 mode);
    u8 GetObjectNewAnimationType_3(CGameCharacter *obj, u16 action, u8 mode);
    u8 GetObjectNewAnimationType_4(CGameCharacter *obj, u16 action, u8 mode);
    u8 GetObjectNewAnimationType_5(CGameCharacter *obj, u16 action, u8 mode);
    u8 GetObjectNewAnimationType_6_14(CGameCharacter *obj, u16 action, u8 mode);
    u8 GetObjectNewAnimationType_7(CGameCharacter *obj, u16 action, u8 mode);
    u8 GetObjectNewAnimationType_8(CGameCharacter *obj, u16 action, u8 mode);
    u8 GetObjectNewAnimationType_9_10(CGameCharacter *obj, u16 action, u8 mode);
    u8 GetObjectNewAnimationType_11(CGameCharacter *obj, u16 action, u8 mode);

    void ReadFrameDimensionData(ANIMATION_DIMENSIONS &result, u8 frameIndex, bool isCorpse);
    void ReadFramesPixelData(CTextureAnimationDirection &direction);

public:
    CAnimationManager();
    ~CAnimationManager();

    CIndexAnimation m_DataIndex[MAX_ANIMATIONS_DATA_INDEX_COUNT];

    void Init(int graphic, size_t addressIdx, size_t sizeIdx)
    {
        m_AddressIdx[graphic] = addressIdx;
        m_SizeIdx[graphic] = sizeIdx;
    }

    EQUIP_CONV_BODY_MAP &GetEquipConv() { return m_EquipConv; }
    void InitIndexReplaces(u32 *verdata);
    void UpdateAnimationAddressTable();
    void Load(u32 *verdata);
    void ClearUnusedTextures(u32 ticks);
    bool LoadDirectionGroup(CTextureAnimationDirection &direction);
    void GetAnimDirection(u8 &dir, bool &mirror);
    void GetSittingAnimDirection(u8 &dir, bool &mirror, int &x, int &y);
    void DrawCharacter(class CGameCharacter *obj, int x, int y);
    bool CharacterPixelsInXY(class CGameCharacter *obj, int x, int y);
    void DrawCorpse(class CGameItem *obj, int x, int y);
    bool CorpsePixelsInXY(class CGameItem *obj, int x, int y);
    u8 GetDieGroupIndex(u16 id, bool second);
    ANIMATION_GROUPS GetGroupIndex(u16 id);
    bool AnimationExists(u16 graphic, u8 group);

    ANIMATION_DIMENSIONS
    GetAnimationDimensions(
        u8 frameIndex, u16 id, u8 dir, u8 animGroup, bool isCorpse);

    ANIMATION_DIMENSIONS GetAnimationDimensions(
        class CGameObject *obj,
        u8 frameIndex = 0xFF,
        u8 defaultDirection = 0,
        u8 defaultGroup = 0);

    void CalculateFrameInformation(
        FRAME_OUTPUT_INFO &info, class CGameObject *obj, bool mirror, u8 animIndex);

    struct DRAW_FRAME_INFORMATION
    CollectFrameInformation(class CGameObject *gameObject, bool checkLayers = true);

    u8 GetReplacedObjectAnimation(CGameCharacter *obj, u16 index);

    u8
    GetObjectNewAnimation(CGameCharacter *obj, u16 type, u16 action, u8 mode);
};

extern CAnimationManager g_AnimationManager;
