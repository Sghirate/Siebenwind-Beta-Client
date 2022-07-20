#pragma once

#include "Constants.h"
#include "Core/Minimal.h"
#include "plugin/enumlist.h"
#include "Backend.h" // CGLTexture, GLuint
#include <assert.h>

class CGump;

#define countof(xarray) (sizeof(xarray) / sizeof(xarray[0]))

enum
{
    WINDOW_INACTIVE = 0,
    WINDOW_ACTIVE = 1,
};

extern bool g_AltPressed;
extern bool g_CtrlPressed;
extern bool g_ShiftPressed;
extern bool g_MovingFromMouse;
extern bool g_AutoMoving;
extern bool g_AbyssPacket03First;

bool CanBeDraggedByOffset(const Core::Vec2<i32> &point);
void TileOffsetOnMonitorToXY(int &ofsX, int &ofsY, int &x, int &y);

class CGameObject;
int GetDistance(CGameObject *current, CGameObject *target);
int GetDistance(CGameObject *current, const Core::Vec2<i32> &target);
bool CheckMultiDistance(const Core::Vec2<i32> &current, CGameObject *target, int maxDistance);
int GetDistance(const Core::Vec2<i32> &current, CGameObject *target);
int GetDistance(const Core::Vec2<i32> &current, const Core::Vec2<i32> &target);
int GetRemoveDistance(const Core::Vec2<i32> &current, CGameObject *target);
int GetTopObjDistance(CGameObject *current, CGameObject *target);

const char *GetReagentName(u16 id);

extern int g_LandObjectsCount;
extern int g_StaticsObjectsCount;
extern int g_GameObjectsCount;
extern int g_MultiObjectsCount;
extern int g_RenderedObjectsCountInGameWindow;

extern GLdouble g_GlobalScale;

extern CGLTexture g_MapTexture[MAX_MAPS_COUNT];

extern CGLTexture g_AuraTexture;

extern bool g_LogoutAfterClick;

extern int g_FrameDelay[2];

extern u32 g_LastSendTime;

extern u32 g_LastPacketTime;

extern u32 g_TotalSendSize;

extern u32 g_TotalRecvSize;

extern u32 g_Ticks;

extern GLuint ShaderColorTable;
extern GLuint g_ShaderDrawMode;

extern std::string g_Language;

extern GAME_STATE g_GameState;

extern CGLTexture g_TextureGumpState[2];

extern Core::Vec2<i32> g_MapSize[MAX_MAPS_COUNT];
extern Core::Vec2<i32> g_MapBlockSize[MAX_MAPS_COUNT];

extern int g_MultiIndexCount;

extern class CGLFrameBuffer g_LightBuffer;

extern bool g_GumpPressed;
extern class CRenderObject *g_GumpSelectedElement;
extern class CRenderObject *g_GumpPressedElement;
extern Core::Vec2<i32> g_GumpMovingOffset;
extern Core::Vec2<float> g_GumpTranslate;
extern bool g_ShowGumpLocker;

extern bool g_GrayedPixels;

extern bool g_ConfigLoaded;

extern u8 g_LightLevel;
extern u8 g_PersonalLightLevel;

extern char g_SelectedCharName[30];

extern u8 g_CurrentMap;

extern u8 g_ServerTimeHour;
extern u8 g_ServerTimeMinute;
extern u8 g_ServerTimeSecond;

extern bool g_PacketLoginComplete;

extern u32 g_ClientFlag;

extern bool g_SendLogoutNotification;
extern bool g_PopupEnabled;
extern bool g_ChatEnabled;
extern bool g_TooltipsEnabled;
extern bool g_PaperdollBooks;

extern u8 g_GameSeed[4];

extern u16 g_OutOfRangeColor;

extern bool g_NoDrawRoof;

extern char g_MaxGroundZ;

extern char g_FoliageIndex;

extern bool g_UseCircleTrans;

extern bool g_JournalShowSystem;

extern bool g_JournalShowObjects;

extern bool g_JournalShowClient;

extern u32 g_PlayerSerial;
extern u32 g_StatusbarUnderMouse;

extern int g_LastSpellIndex;

extern int g_LastSkillIndex;

extern u32 g_LastUseObject;

extern u32 g_LastTargetObject;

extern u32 g_LastAttackObject;

extern CHARACTER_SPEED_TYPE g_SpeedMode;

extern u32 g_DeathScreenTimer;

extern float g_AnimCharactersDelayValue;

typedef std::vector<std::pair<u32, u32>> UINTS_PAIR_LIST;

extern Core::Vec2<i32> g_RemoveRangeXY;

extern int g_GrayMenuCount;

extern PROMPT_TYPE g_ConsolePrompt;

extern u8 g_LastASCIIPrompt[11];

extern u8 g_LastUnicodePrompt[11];

extern u32 g_PartyHelperTarget;

extern u32 g_PartyHelperTimer;

extern float g_DrawColor;

extern SEASON_TYPE g_Season;
extern SEASON_TYPE g_OldSeason;
extern int g_OldSeasonMusic;

extern u32 g_LockedClientFeatures;

extern bool g_GeneratedMouseDown;

enum DEVELOPER_MODE
{
    DM_NO_DEBUG = 0,
    DM_SHOW_FPS_ONLY,
    DM_DEBUGGING
};
extern DEVELOPER_MODE g_DeveloperMode;
extern DEVELOPER_MODE g_OptionsDeveloperMode;

const int g_ObjectHandlesWidth = 142;
const int g_ObjectHandlesWidthOffset = g_ObjectHandlesWidth / 2;
const int g_ObjectHandlesHeight = 20;
extern u16 g_ObjectHandlesBackgroundPixels[g_ObjectHandlesWidth * g_ObjectHandlesHeight];

extern u32 g_Ping;

extern bool g_DrawAura;

static const int MAX_ABILITIES_COUNT = 32;
extern u16 g_AbilityList[MAX_ABILITIES_COUNT];
extern u8 g_Ability[2];

extern bool g_DrawStatLockers;

extern u32 g_SelectedGameObjectHandle;

extern bool g_ShowWarnings;

extern u32 g_ProcessStaticAnimationTimer;
extern u32 g_ProcessRemoveRangedTimer;
extern int g_MaxViewRange;
extern u32 g_OrionFeaturesFlags;
inline bool Int32TryParse(const std::string &str, int &result)
{
    char *end = nullptr;
    auto v = strtol(str.c_str(), &end, 10);
    if (!*end)
    {
        result = v;
        return true;
    }
    result = 0;
    return false;
    /*
    std::istringstream convert(str);
    try
    {
        convert >> result;
    }
    catch (int)
    {
        result = 0;
        return false;
    }
    if (!convert.eof())
    {
        result = 0;
        return false;
    }
    return true;
    */
}

inline bool IsBackground(i64 flags)
{
    return (flags & 0x00000001) != 0;
}
inline bool IsWeapon(i64 flags)
{
    return (flags & 0x00000002) != 0;
}
inline bool IsTransparent(i64 flags)
{
    return (flags & 0x00000004) != 0;
}
inline bool IsTranslucent(i64 flags)
{
    return (flags & 0x00000008) != 0;
}
inline bool IsWall(i64 flags)
{
    return (flags & 0x00000010) != 0;
}
inline bool IsDamaging(i64 flags)
{
    return (flags & 0x00000020) != 0;
}
inline bool IsImpassable(i64 flags)
{
    return (flags & 0x00000040) != 0;
}
inline bool IsWet(i64 flags)
{
    return (flags & 0x00000080) != 0;
}
inline bool IsUnknown(i64 flags)
{
    return (flags & 0x00000100) != 0;
}
inline bool IsSurface(i64 flags)
{
    return (flags & 0x00000200) != 0;
}
inline bool IsBridge(i64 flags)
{
    return (flags & 0x00000400) != 0;
}
inline bool IsStackable(i64 flags)
{
    return (flags & 0x00000800) != 0;
}
inline bool IsWindow(i64 flags)
{
    return (flags & 0x00001000) != 0;
}
inline bool IsNoShoot(i64 flags)
{
    return (flags & 0x00002000) != 0;
}
inline bool IsPrefixA(i64 flags)
{
    return (flags & 0x00004000) != 0;
}
inline bool IsPrefixAn(i64 flags)
{
    return (flags & 0x00008000) != 0;
}
inline bool IsInternal(i64 flags)
{
    return (flags & 0x00010000) != 0;
}
inline bool IsFoliage(i64 flags)
{
    return (flags & 0x00020000) != 0;
}
inline bool IsPartialHue(i64 flags)
{
    return (flags & 0x00040000) != 0;
}
inline bool IsUnknown1(i64 flags)
{
    return (flags & 0x00080000) != 0;
}
inline bool IsMap(i64 flags)
{
    return (flags & 0x00100000) != 0;
}
inline bool IsContainer(i64 flags)
{
    return (flags & 0x00200000) != 0;
}
inline bool IsWearable(i64 flags)
{
    return (flags & 0x00400000) != 0;
}
inline bool IsLightSource(i64 flags)
{
    return (flags & 0x00800000) != 0;
}
inline bool IsAnimated(i64 flags)
{
    return (flags & 0x01000000) != 0;
}
inline bool IsNoDiagonal(i64 flags)
{
    return (flags & 0x02000000) != 0;
}
inline bool IsUnknown2(i64 flags)
{
    return (flags & 0x04000000) != 0;
}
inline bool IsArmor(i64 flags)
{
    return (flags & 0x08000000) != 0;
}
inline bool IsRoof(i64 flags)
{
    return (flags & 0x10000000) != 0;
}
inline bool IsDoor(i64 flags)
{
    return (flags & 0x20000000) != 0;
}
inline bool IsStairBack(i64 flags)
{
    return (flags & 0x40000000) != 0;
}
inline bool IsStairRight(i64 flags)
{
    return (flags & 0x80000000) != 0;
}

template <typename T, typename U>
static inline T checked_cast(U value)
{
    auto result = static_cast<T>(value);
    assert(static_cast<U>(result) == value && "Type conversion loses information");
    return result;
}

template <typename T, typename U>
static inline T checked_cast(U *value)
{
    auto result = checked_cast<T>((intptr_t)value);
    assert((static_cast<intptr_t>(result) == (intptr_t)value) && "Type conversion loses information");
    return result;
}

template <typename T>
static inline std::unique_ptr<T> unique_cast(void *value)
{
    return std::unique_ptr<T>((T *)value);
}

struct AutoFree
{
    AutoFree(void *p)
        : _p(p)
    {
    }
    ~AutoFree()
    {
        if (_p)
        {
            free(_p);
        }
    }

private:
    void *_p = nullptr;
};
