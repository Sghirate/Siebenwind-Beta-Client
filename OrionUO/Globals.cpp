#include "Target.h"
#include "GameObjects/GameWorld.h"
#include "GameObjects/GameCharacter.h"

bool g_AltPressed         = false;
bool g_CtrlPressed        = false;
bool g_ShiftPressed       = false;
bool g_MovingFromMouse    = false;
bool g_AutoMoving         = false;
bool g_AbyssPacket03First = true;

int g_LandObjectsCount                 = 0;
int g_StaticsObjectsCount              = 0;
int g_GameObjectsCount                 = 0;
int g_MultiObjectsCount                = 0;
int g_RenderedObjectsCountInGameWindow = 0;

GLdouble g_GlobalScale = 1.0;

CGLTexture g_MapTexture[MAX_MAPS_COUNT];
CGLTexture g_AuraTexture;

bool g_LogoutAfterClick = false;
int g_FrameDelay[2]     = { FRAME_DELAY_INACTIVE_WINDOW, FRAME_DELAY_ACTIVE_WINDOW };

u32 g_LastSendTime   = 0;
u32 g_LastPacketTime = 0;
u32 g_TotalSendSize  = 0;
u32 g_TotalRecvSize  = 0;
u32 g_Ticks          = 0;

GLuint ShaderColorTable = 0;
GLuint g_ShaderDrawMode = 0;

std::string g_Language = "ENU";

GAME_STATE g_GameState = GS_MAIN;

CGLTexture g_TextureGumpState[2];

Core::Vec2<i32> g_MapSize[MAX_MAPS_COUNT] = {
    // Felucca      Trammel         Ilshenar        Malas           Tokuno          TerMur
    { 7168, 4096 }, { 7168, 4096 }, { 2304, 1600 }, { 2560, 2048 }, { 1448, 1448 }, { 1280, 4096 },
};
Core::Vec2<i32> g_MapBlockSize[MAX_MAPS_COUNT];

int g_MultiIndexCount = 0;

CGLFrameBuffer g_LightBuffer;

bool g_GumpPressed                         = false;
class CRenderObject* g_GumpSelectedElement = nullptr;
class CRenderObject* g_GumpPressedElement  = nullptr;
Core::Vec2<i32> g_GumpMovingOffset;
Core::Vec2<float> g_GumpTranslate;
bool g_ShowGumpLocker = false;

bool g_GrayedPixels = false;

bool g_ConfigLoaded = false;

u8 g_LightLevel         = 0;
u8 g_PersonalLightLevel = 0;

char g_SelectedCharName[30] = { 0 };

u8 g_CurrentMap = 0;

u8 g_ServerTimeHour   = 0;
u8 g_ServerTimeMinute = 0;
u8 g_ServerTimeSecond = 0;

bool g_PacketLoginComplete = false;

u32 g_ClientFlag = 0;

bool g_SendLogoutNotification = false;
bool g_PopupEnabled           = false;
bool g_ChatEnabled            = false;
bool g_TooltipsEnabled        = false;
bool g_PaperdollBooks         = false;

u8 g_GameSeed[4] = { 0 };

u16 g_OutOfRangeColor = 0;
char g_MaxGroundZ          = 0;
bool g_NoDrawRoof          = false;
char g_FoliageIndex        = 0;
bool g_UseCircleTrans      = false;

bool g_JournalShowSystem  = true;
bool g_JournalShowObjects = true;
bool g_JournalShowClient  = true;

u32 g_PlayerSerial        = 0;
u32 g_StatusbarUnderMouse = 0;

int g_LastSpellIndex        = 1;
int g_LastSkillIndex        = 1;
u32 g_LastUseObject    = 0;
u32 g_LastTargetObject = 0;
u32 g_LastAttackObject = 0;

CHARACTER_SPEED_TYPE g_SpeedMode = CST_NORMAL;

u32 g_DeathScreenTimer = 0;

float g_AnimCharactersDelayValue = 80.0f; //0x50

Core::Vec2<i32> g_RemoveRangeXY;

int g_GrayMenuCount = 0;

PROMPT_TYPE g_ConsolePrompt     = PT_NONE;
u8 g_LastASCIIPrompt[11]   = { 0 };
u8 g_LastUnicodePrompt[11] = { 0 };

u32 g_PartyHelperTarget = 0;
u32 g_PartyHelperTimer  = 0;

float g_DrawColor = 1.0f;

SEASON_TYPE g_Season    = ST_SUMMER;
SEASON_TYPE g_OldSeason = ST_SUMMER;
int g_OldSeasonMusic    = 0;

u32 g_LockedClientFeatures = 0;

bool g_GeneratedMouseDown = false;

DEVELOPER_MODE g_DeveloperMode        = DM_SHOW_FPS_ONLY;
DEVELOPER_MODE g_OptionsDeveloperMode = DM_SHOW_FPS_ONLY;

u16 g_ObjectHandlesBackgroundPixels[g_ObjectHandlesWidth * g_ObjectHandlesHeight] = { 0 };

u32 g_Ping = 0;

bool g_DrawAura = false;

u16 g_AbilityList[MAX_ABILITIES_COUNT] = {
    AT_ARMOR_IGNORE,     AT_BLEED_ATTACK,    AT_CONCUSSION_BLOW,    AT_CRUSHING_BLOW,
    AT_DISARM,           AT_DISMOUNT,        AT_DOUBLE_STRIKE,      AT_INFECTING,
    AT_MORTAL_STRIKE,    AT_MOVING_SHOT,     AT_PARALYZING_BLOW,    AT_SHADOW_STRIKE,
    AT_WHIRLWIND_ATTACK, AT_RIDING_SWIPE,    AT_FRENZIED_WHIRLWIND, AT_BLOCK,
    AT_DEFENSE_MASTERY,  AT_NERVE_STRIKE,    AT_TALON_STRIKE,       AT_FEINT,
    AT_DUAL_WIELD,       AT_DOUBLE_SHOT,     AT_ARMOR_PIERCE,       AT_BLADEWEAVE,
    AT_FORCE_ARROW,      AT_LIGHTNING_ARROW, AT_PSYCHIC_ATTACK,     AT_SERPENT_ARROW,
    AT_FORCE_OF_NATURE,  AT_INFUSED_THROW,   AT_MYSTIC_ARC,         AT_DISROBE
};

u8 g_Ability[2] = { AT_DISARM, AT_PARALYZING_BLOW };

bool g_DrawStatLockers = false;

u32 g_SelectedGameObjectHandle = 0;

bool g_ShowWarnings = true;

u32 g_ProcessStaticAnimationTimer = 0;
u32 g_ProcessRemoveRangedTimer    = 0;
int g_MaxViewRange                     = MAX_VIEW_RANGE_OLD;
u32 g_OrionFeaturesFlags          = OFF_ALL_FLAGS;

bool CanBeDraggedByOffset(const Core::Vec2<i32>& point)
{
    if (g_Target.IsTargeting())
    {
        return (
            abs(point.x) >= DRAG_PIXEL_RANGE_WITH_TARGET ||
            abs(point.y) >= DRAG_PIXEL_RANGE_WITH_TARGET);
    }

    return (abs(point.x) >= DRAG_ITEMS_PIXEL_RANGE || abs(point.y) >= DRAG_ITEMS_PIXEL_RANGE);
}

void TileOffsetOnMonitorToXY(int& ofsX, int& ofsY, int& x, int& y)
{
    if (ofsX == 0)
    {
        x = y = ofsY / 2;
    }
    else if (ofsY == 0)
    {
        x = ofsX / 2;
        y = -x;
    }
    else //if (ofsX && ofsY)
    {
        int absX = abs(ofsX);
        int absY = abs(ofsY);
        x        = ofsX;

        if (ofsY > ofsX)
        {
            if (ofsX < 0 && ofsY < 0)
            {
                y = absX - absY;
            }
            else if (ofsX > 0 && ofsY > 0)
            {
                y = absY - absX;
            }
        }
        else if (ofsX > ofsY)
        {
            if (ofsX < 0 && ofsY < 0)
            {
                y = -(absY - absX);
            }
            else if (ofsX > 0 && ofsY > 0)
            {
                y = -(absX - absY);
            }
        }

        if ((y == 0) && ofsY != ofsX)
        {
            if (ofsY < 0)
            {
                y = -(absX + absY);
            }
            else
            {
                y = absX + absY;
            }
        }

        y /= 2;
        x += y;
    }
}

std::string ToCamelCase(std::string text)
{
    bool lastSpace = true;

    for (char& c : text)
    {
        if (lastSpace && (c >= 'a' && c <= 'z'))
        {
            c = 'A' + (c - 'a');
        }

        lastSpace = (c == ' ');
    }

    return text;
}

int GetDistance(CGameObject* current, CGameObject* target)
{
    if (current != nullptr && target != nullptr)
    {
        int distx = abs(target->GetX() - current->GetX());
        int disty = abs(target->GetY() - current->GetY());

        if (disty > distx)
        {
            distx = disty;
        }

        return distx;
    }

    return 100500;
}

int GetDistance(CGameObject* current, const Core::Vec2<i32>& target)
{
    if (current != nullptr)
    {
        int distx = abs(target.x - current->GetX());
        int disty = abs(target.y - current->GetY());

        if (disty > distx)
        {
            distx = disty;
        }

        return distx;
    }

    return 100500;
}

int GetDistance(const Core::Vec2<i32>& current, CGameObject* target)
{
    if (target != nullptr)
    {
        int distx = abs(target->GetX() - current.x);
        int disty = abs(target->GetY() - current.y);

        if (disty > distx)
        {
            distx = disty;
        }

        return distx;
    }

    return 100500;
}

int GetRemoveDistance(const Core::Vec2<i32>& current, CGameObject* target)
{
    if (target != nullptr)
    {
        Core::Vec2<i32> targetPoint(target->GetX(), target->GetY());

        if (target->NPC && !((CGameCharacter*)target)->m_Steps.empty())
        {
            CWalkData& wd = ((CGameCharacter*)target)->m_Steps.back();

            targetPoint = Core::Vec2<i32>(wd.X, wd.Y);
        }

        int distx = abs(targetPoint.x - current.x);
        int disty = abs(targetPoint.y - current.y);

        if (disty > distx)
        {
            distx = disty;
        }

        return distx;
    }

    return 100500;
}

bool CheckMultiDistance(const Core::Vec2<i32>& current, CGameObject* target, int maxDistance)
{
    bool result = false;

    if (target != nullptr)
    {
        maxDistance += ((CGameItem*)target)->MultiDistanceBonus;

        result =
            ((abs(target->GetX() - current.x) <= maxDistance) &&
             (abs(target->GetY() - current.y) <= maxDistance));
    }

    return result;
}

int GetDistance(const Core::Vec2<i32>& current, const Core::Vec2<i32>& target)
{
    int distx = abs(target.x - current.x);
    int disty = abs(target.y - current.y);

    if (disty > distx)
    {
        distx = disty;
    }

    return distx;
}

int GetTopObjDistance(CGameObject* current, CGameObject* target)
{
    if (current != nullptr && target != nullptr)
    {
        while (target != nullptr && target->Container != 0xFFFFFFFF)
        {
            target = g_World->FindWorldObject(target->Container);
        }

        if (target != nullptr)
        {
            int distx = abs(target->GetX() - current->GetX());
            int disty = abs(target->GetY() - current->GetY());

            if (disty > distx)
            {
                distx = disty;
            }

            return distx;
        }
    }

    return 100500;
}

const char* GetReagentName(u16 id)
{
    switch (id)
    {
        case 0x0F7A: return "Black pearl";
        case 0x0F7B: return "Bloodmoss";
        case 0x0F84: return "Garlic";
        case 0x0F85: return "Ginseng";
        case 0x0F86: return "Mandrake root";
        case 0x0F88: return "Nightshade";
        case 0x0F8C: return "Sulfurous ash";
        case 0x0F8D: return "Spiders silk";
        default: break;
    }

    return "";
}
