#include "ConfigManager.h"
#include "Core/File.h"
#include "Core/StringUtils.h"
#include "Core/TextFileParser.h"
#include "GameVars.h"
#include "Globals.h"
#include "GumpManager.h"
#include "SoundManager.h"
#include "ObjectPropertiesManager.h"
#include "../Config.h"
#include "../OrionUO.h"
#include "../Party.h"
#include "../ServerList.h"
#include "../OrionWindow.h"
#include "../Container.h"
#include "../CharacterList.h"
#include "../TextEngine/GameConsole.h"
#include "../Gumps/Gump.h"
#include "../Gumps/GumpSpell.h"
#include "../Gumps/GumpPropertyIcon.h"
#include "../GameObjects/GameWorld.h"
#include "../GameObjects/GamePlayer.h"
#include "../Network/Packets.h"

CConfigManager g_ConfigManager;
CConfigManager g_OptionsConfig;

enum
{
    CMKC_NONE = -1,

    CMKC_SOUND = 0,
    CMKC_SOUND_VOLUME,
    CMKC_MUSIC,
    CMKC_MUSIC_VOLUME,
    CMKC_FOOTSTEPS_SOUND,
    CMKC_COMBAT_MUSIC,
    CMKC_BACKGROUND_SOUND,
    CMKC_CLIENT_FPS,
    CMKC_USE_SCALING,
    CMKC_REMOVE_TEXT_WITH_BLENDING,
    CMKC_DRAW_STATUS_STATE,
    CMKC_DRAW_STUMPS,
    CMKC_MARKING_CAVES,
    CMKC_NO_ANIMATE_FIELDS,
    CMKC_NO_VEGETATION,
    CMKC_HIDDEN_CHARACTERS_RENDER_MODE,
    CMKC_HIDDEN_ALPHA,
    CMKC_USE_HIDDEN_MODE_ONLY_FOR_SELF,
    CMKC_TRANSPARENT_SPELL_ICONS,
    CMKC_SPELL_ICON_ALPHA,
    CMKC_OLD_STYLE_STATUSBAR,
    CMKC_ORIGINAL_PARTY_STATUSBAR,
    CMKC_APPLY_STATE_COLOR_ON_CHARACTERS,
    CMKC_CHANGE_FIELDS_GRAPHIC,
    CMKC_PAPERDOLL_SLOTS,
    CMKC_DRAW_STATUS_CONDITION_STATE,
    CMKC_DRAW_STATUS_CONDITION_VALUE,
    CMKC_REMOVE_STATUSBARS_WITHOUT_OBJECTS,
    CMKC_SHOW_DEFAULT_CONSOLE_ENTRY_MODE,
    CMKC_DRAW_AURA_STATE,
    CMKC_DRAW_AURA_WITH_CTRL_PRESSED,
    CMKC_SCREENSHOT_FORMAT,
    CMKC_SCALE_IMAGES_IN_PAPERDOLL_SLOTS,
    CMKC_REMOVE_OR_CREATE_OBJECTS_WITH_BLENDING,
    CMKC_DRAW_HELMETS_ON_SHROUD,
    CMKC_USE_GLOBAL_MAP_LAYER,
    CMKC_NO_DRAW_ROOFS,
    CMKC_HIGHLIGHT_TARGET_BY_TYPE,
    CMKC_AUTO_DISPLAY_WORLD_MAP,
    CMKC_DISABLE_MACRO_IN_CHAT,
    CMKC_USE_TOOLTIPS,
    CMKC_TOOLTIPS_TEXT_COLOR,
    CMKC_TOOLTIPS_TEXT_FONT,
    CMKC_TOOLTIPS_DELAY,
    CMKC_CHAT_COLOR_INPUT_TEXT,
    CMKC_CHAT_COLOR_MENU_OPTION,
    CMKC_CHAT_COLOR_PLAYER_IN_MEMBER_LIST,
    CMKC_CHAT_COLOR_TEXT,
    CMKC_CHAT_COLOR_PLAYER_NAME_WITHOUT,
    CMKC_CHAT_COLOR_MUTED,
    CMKC_CHAT_COLOR_CHANNEL_MODERATOR_NAME,
    CMKC_CHAT_COLOR_CHANNEL_MODERATOR_TEXT,
    CMKC_CHAT_COLOR_MY_NAME,
    CMKC_CHAT_COLOR_MY_TEXT,
    CMKC_CHAT_COLOR_SYSTEM_MESSAGE,
    CMKC_CHAT_FONT,
    CMKC_CHAT_COLOR_BG_OUTPUT_TEXT,
    CMKC_CHAT_COLOR_BG_INPUT_TEXT,
    CMKC_CHAT_COLOR_BG_USER_LIST,
    CMKC_CHAT_COLOR_BG_CONF_LIST,
    CMKC_CHAT_COLOR_BG_COMMAND_LIST,
    CMKC_ENABLE_PATHFIND,
    CMKC_HOLD_TAB_FOR_COMBAT,
    CMKC_OFFSET_INTERFACE_WINDOWS,
    CMKC_AUTO_ARRANGE,
    CMKC_ALWAYS_RUN,
    CMKC_DISABLE_MENUBAR,
    CMKC_GRAY_OUT_OF_RANGE_OBJECTS,
    CMKC_DISABLE_NEW_TARGET_SYSTEM,
    CMKC_ITEMP_ROPERTIES_MODE,
    CMKC_ITEMP_ROPERTIES_ICON,
    CMKC_OBJECT_HANDLES,
    CMKC_REDUCE_FPS_UNACTIVE_WINDOW,
    CMKC_HOLD_SHIFT_FOR_CONTEXT_MENUS,
    CMKC_HOLD_SHIFT_FOR_ENABLE_PATHFIND,
    CMKC_CONTAINER_DEFAULT_X,
    CMKC_CONTAINER_DEFAULT_Y,
    CMKC_GAME_WINDOW_WIDTH,
    CMKC_GAME_WINDOW_HEIGHT,
    CMKC_SPEECH_DELAY,
    CMKC_SCALE_SPEECH_DELAY,
    CMKC_SPEECH_COLOR,
    CMKC_EMOTE_COLOR,
    CMKC_PARTY_MESSAGE_COLOR,
    CMKC_GUILD_MESSAGE_COLOR,
    CMKC_ALLIANCE_MESSAGE_COLOR,
    CMKC_IGNORE_GUILD_MESSAGE,
    CMKC_IGNORE_ALLIANCE_MESSAGE,
    CMKC_DARK_NIGHTS,
    CMKC_COLORED_LIGHTING,
    CMKC_STANDART_CHARACTERS_ANIMATION_DELAY,
    CMKC_STANDART_ITEMS_ANIMATION_DELAY,
    CMKC_LOCK_RESIZING_GAME_WINDOW,
    CMKC_LOCK_GUMPS_MOVING,
    CMKC_INNOCENT_COLOR,
    CMKC_FRIENDLY_COLOR,
    CMKC_SOMEONE_COLOR,
    CMKC_CRIMINAL_COLOR,
    CMKC_ENEMY_COLOR,
    CMKC_MURDERER_COLOR,
    CMKC_CRIMINAL_ACTIONS_QUERY,
    CMKC_SHOW_INCOMING_NAMES,
    CMKC_USE_CIRCLE_TRANS,
    CMKC_STAT_REPORT,
    CMKC_CONSOLE_NEED_ENTER,
    CMKC_CIRCLE_TRANS_RADIUS,
    CMKC_SKILL_REPORT,
    CMKC_SPEECH_FONT,
    CMKC_GAME_WINDOW_X,
    CMKC_GAME_WINDOW_Y,
    CMKC_ZOOMED,
    CMKC_REAL_X,
    CMKC_REAL_Y,
    CMKC_REAL_WIDTH,
    CMKC_REAL_HEIGHT,
    CMKC_TOGGLE_BUFFICON_WINDOW,
    CMKC_DEVELOPER_MODE,
    CMKC_LAST_SERVER,
    CMKC_LAST_CHARACTER,
    CMKC_CHARACTER_BACKPACK_STYLE,
    CMKC_CHECK_PING,
    CMKC_PING_TIMER,
    CMKC_CANCEL_NEW_TARGET_SYSTEM_ON_SHIFT_ESC,
    CMKC_DRAW_STATUS_FOR_HUMANOIDS,

    CMKC_COUNT
};

namespace cmkc
{
struct ConfigEntry
{
    u32 key;
    const char* key_name;
};

static const ConfigEntry s_Keys[] = {
    { CMKC_SOUND, "sound" },
    { CMKC_SOUND_VOLUME, "soundvolume" },
    { CMKC_MUSIC, "music" },
    { CMKC_MUSIC_VOLUME, "musicvolume" },
    { CMKC_FOOTSTEPS_SOUND, "footstepssound" },
    { CMKC_COMBAT_MUSIC, "combatmusic" },
    { CMKC_BACKGROUND_SOUND, "backgroundsound" },
    { CMKC_CLIENT_FPS, "clientfps" },
    { CMKC_USE_SCALING, "usescaling" },
    { CMKC_REMOVE_TEXT_WITH_BLENDING, "removetextwithblending" },
    { CMKC_DRAW_STATUS_STATE, "drawstatusstate" },
    { CMKC_DRAW_STUMPS, "drawstumps" },
    { CMKC_MARKING_CAVES, "markingcaves" },
    { CMKC_NO_ANIMATE_FIELDS, "noanimatefields" },
    { CMKC_NO_VEGETATION, "novegetation" },
    { CMKC_HIDDEN_CHARACTERS_RENDER_MODE, "hiddencharactersrendermode" },
    { CMKC_HIDDEN_ALPHA, "hiddenalpha" },
    { CMKC_USE_HIDDEN_MODE_ONLY_FOR_SELF, "usehiddenmodeonlyforself" },
    { CMKC_TRANSPARENT_SPELL_ICONS, "transparentspellicons" },
    { CMKC_SPELL_ICON_ALPHA, "spelliconalpha" },
    { CMKC_OLD_STYLE_STATUSBAR, "oldstylestatusbar" },
    { CMKC_ORIGINAL_PARTY_STATUSBAR, "originalpartystatusbar" },
    { CMKC_APPLY_STATE_COLOR_ON_CHARACTERS, "applystatecoloroncharacters" },
    { CMKC_CHANGE_FIELDS_GRAPHIC, "changefieldsgraphic" },
    { CMKC_PAPERDOLL_SLOTS, "paperdollslots" },
    { CMKC_DRAW_STATUS_CONDITION_STATE, "drawstatusconditionstate" },
    { CMKC_DRAW_STATUS_CONDITION_VALUE, "drawstatusconditionvalue" },
    { CMKC_REMOVE_STATUSBARS_WITHOUT_OBJECTS, "removestatusbarswithoutobjects" },
    { CMKC_SHOW_DEFAULT_CONSOLE_ENTRY_MODE, "showdefaultconsoleentrymode" },
    { CMKC_DRAW_AURA_STATE, "drawaurastate" },
    { CMKC_DRAW_AURA_WITH_CTRL_PRESSED, "drawaurawithctrlpressed" },
    { CMKC_SCREENSHOT_FORMAT, "screenshotformat" },
    { CMKC_SCALE_IMAGES_IN_PAPERDOLL_SLOTS, "scaleimagesinpaperdollslots" },
    { CMKC_REMOVE_OR_CREATE_OBJECTS_WITH_BLENDING, "removeorcreateobjectswithblending" },
    { CMKC_DRAW_HELMETS_ON_SHROUD, "drawhelmetsonshroud" },
    { CMKC_USE_GLOBAL_MAP_LAYER, "useglobalmaplayer" },
    { CMKC_NO_DRAW_ROOFS, "nodrawroofs" },
    { CMKC_HIGHLIGHT_TARGET_BY_TYPE, "highlighttargetbytype" },
    { CMKC_AUTO_DISPLAY_WORLD_MAP, "autodisplayworldmap" },
    { CMKC_DISABLE_MACRO_IN_CHAT, "disablemacroinchat" },
    { CMKC_USE_TOOLTIPS, "usetooltips" },
    { CMKC_TOOLTIPS_TEXT_COLOR, "tooltipstextcolor" },
    { CMKC_TOOLTIPS_TEXT_FONT, "tooltipstextfont" },
    { CMKC_TOOLTIPS_DELAY, "tooltipsdelay" },
    { CMKC_CHAT_COLOR_INPUT_TEXT, "chatcolorinputtext" },
    { CMKC_CHAT_COLOR_MENU_OPTION, "chatcolormenuoption" },
    { CMKC_CHAT_COLOR_PLAYER_IN_MEMBER_LIST, "chatcolorplayerinmemberlist" },
    { CMKC_CHAT_COLOR_TEXT, "chatcolortext" },
    { CMKC_CHAT_COLOR_PLAYER_NAME_WITHOUT, "chatcolorplayernamewithout" },
    { CMKC_CHAT_COLOR_MUTED, "chatcolormuted" },
    { CMKC_CHAT_COLOR_CHANNEL_MODERATOR_NAME, "chatcolorchannelmoderatorname" },
    { CMKC_CHAT_COLOR_CHANNEL_MODERATOR_TEXT, "chatcolorchannelmoderatortext" },
    { CMKC_CHAT_COLOR_MY_NAME, "chatcolormyname" },
    { CMKC_CHAT_COLOR_MY_TEXT, "chatcolormytext" },
    { CMKC_CHAT_COLOR_SYSTEM_MESSAGE, "chatcolorsystemmessage" },
    { CMKC_CHAT_FONT, "chatfont" },
    { CMKC_CHAT_COLOR_BG_OUTPUT_TEXT, "chatcolorbgoutputtext" },
    { CMKC_CHAT_COLOR_BG_INPUT_TEXT, "chatcolorbginputtext" },
    { CMKC_CHAT_COLOR_BG_USER_LIST, "chatcolorbguserlist" },
    { CMKC_CHAT_COLOR_BG_CONF_LIST, "chatcolorbgconflist" },
    { CMKC_CHAT_COLOR_BG_COMMAND_LIST, "chatcolorbgcommandlist" },
    { CMKC_ENABLE_PATHFIND, "enablepathfind" },
    { CMKC_HOLD_TAB_FOR_COMBAT, "holdtabforcombat" },
    { CMKC_OFFSET_INTERFACE_WINDOWS, "offsetinterfacewindows" },
    { CMKC_AUTO_ARRANGE, "autoarrange" },
    { CMKC_ALWAYS_RUN, "alwaysrun" },
    { CMKC_DISABLE_MENUBAR, "disablemenubar" },
    { CMKC_GRAY_OUT_OF_RANGE_OBJECTS, "grayoutofrangeobjects" },
    { CMKC_DISABLE_NEW_TARGET_SYSTEM, "disablenewtargetsystem" },
    { CMKC_ITEMP_ROPERTIES_MODE, "itempropertiesmode" },
    { CMKC_ITEMP_ROPERTIES_ICON, "itempropertiesicon" },
    { CMKC_OBJECT_HANDLES, "objecthandles" },
    { CMKC_REDUCE_FPS_UNACTIVE_WINDOW, "reducefpsunactivewindow" },
    { CMKC_HOLD_SHIFT_FOR_CONTEXT_MENUS, "holdshiftforcontextmenus" },
    { CMKC_HOLD_SHIFT_FOR_ENABLE_PATHFIND, "holdshiftforenablepathfind" },
    { CMKC_CONTAINER_DEFAULT_X, "containerdefaultx" },
    { CMKC_CONTAINER_DEFAULT_Y, "containerdefaulty" },
    { CMKC_GAME_WINDOW_WIDTH, "gamewindowwidth" },
    { CMKC_GAME_WINDOW_HEIGHT, "gamewindowheight" },
    { CMKC_SPEECH_DELAY, "speechdelay" },
    { CMKC_SCALE_SPEECH_DELAY, "scalespeechdelay" },
    { CMKC_SPEECH_COLOR, "speechcolor" },
    { CMKC_EMOTE_COLOR, "emotecolor" },
    { CMKC_PARTY_MESSAGE_COLOR, "partymessagecolor" },
    { CMKC_GUILD_MESSAGE_COLOR, "guildmessagecolor" },
    { CMKC_ALLIANCE_MESSAGE_COLOR, "alliancemessagecolor" },
    { CMKC_IGNORE_GUILD_MESSAGE, "ignoreguildmessage" },
    { CMKC_IGNORE_ALLIANCE_MESSAGE, "ignorealliancemessage" },
    { CMKC_DARK_NIGHTS, "darknights" },
    { CMKC_COLORED_LIGHTING, "coloredlighting" },
    { CMKC_STANDART_CHARACTERS_ANIMATION_DELAY, "standartcharactersanimationdelay" },
    { CMKC_STANDART_ITEMS_ANIMATION_DELAY, "standartitemsanimationdelay" },
    { CMKC_LOCK_RESIZING_GAME_WINDOW, "lockresizinggamewindow" },
    { CMKC_LOCK_GUMPS_MOVING, "lockgumpsmoving" },
    { CMKC_INNOCENT_COLOR, "innocentcolor" },
    { CMKC_FRIENDLY_COLOR, "friendlycolor" },
    { CMKC_SOMEONE_COLOR, "someonecolor" },
    { CMKC_CRIMINAL_COLOR, "criminalcolor" },
    { CMKC_ENEMY_COLOR, "enemycolor" },
    { CMKC_MURDERER_COLOR, "murderercolor" },
    { CMKC_CRIMINAL_ACTIONS_QUERY, "criminalactionsquery" },
    { CMKC_SHOW_INCOMING_NAMES, "showincomingnames" },
    { CMKC_USE_CIRCLE_TRANS, "usecircletrans" },
    { CMKC_STAT_REPORT, "statreport" },
    { CMKC_CONSOLE_NEED_ENTER, "consoleneedenter" },
    { CMKC_CIRCLE_TRANS_RADIUS, "circletransradius" },
    { CMKC_SKILL_REPORT, "skillreport" },
    { CMKC_SPEECH_FONT, "speechfont" },
    { CMKC_GAME_WINDOW_X, "gamewindowx" },
    { CMKC_GAME_WINDOW_Y, "gamewindowy" },
    { CMKC_ZOOMED, "zoomed" },
    { CMKC_REAL_X, "realx" },
    { CMKC_REAL_Y, "realy" },
    { CMKC_REAL_WIDTH, "realwidth" },
    { CMKC_REAL_HEIGHT, "realheight" },
    { CMKC_TOGGLE_BUFFICON_WINDOW, "togglebufficonwindow" },
    { CMKC_DEVELOPER_MODE, "developermode" },
    { CMKC_LAST_SERVER, "lastserver" },
    { CMKC_LAST_CHARACTER, "lastcharacter" },
    { CMKC_CHARACTER_BACKPACK_STYLE, "characterbackpackstyle" },
    { CMKC_CHECK_PING, "checkping" },
    { CMKC_PING_TIMER, "pingtimer" },
    { CMKC_CANCEL_NEW_TARGET_SYSTEM_ON_SHIFT_ESC, "cancelnewtargetsystemonshiftesc" },
    { CMKC_DRAW_STATUS_FOR_HUMANOIDS, "drawstatusforhumanoids" },
    { CMKC_COUNT, nullptr },
};

static_assert(countof(s_Keys) == CMKC_COUNT + 1, "Missing key string for configuration option");

static u32 GetConfigKey(const std::string& key)
{
    auto str = Core::ToLowerA(key);
    for (int i = 0; s_Keys[i].key_name; i++)
    {
        if (str == s_Keys[i].key_name)
        {
            return s_Keys[i].key;
        }
    }
    return CMKC_NONE;
}

} // namespace cmkc

CConfigManager::CConfigManager()
{
}

void CConfigManager::Init()
{
    DefaultPage1();
    DefaultPage2();
    DefaultPage3();
    DefaultPage4();
    DefaultPage6();
    DefaultPage7();
    DefaultPage8();
    DefaultPage9();

    g_CircleOfTransparency.Create(CircleTransRadius);

    FilterPWOn      = false;
    ObscenityFilter = false;
    FilterPassword  = "";

    ToggleBufficonWindow = true;

    GameWindowX = 0;
    GameWindowY = 0;

    if (GameVars::GetClientVersion() >= CV_70331)
    {
        g_MaxViewRange = MAX_VIEW_RANGE_NEW;
    }
    else
    {
        g_MaxViewRange = MAX_VIEW_RANGE_OLD;
    }

    UpdateRange = g_MaxViewRange;
}

void CConfigManager::DefaultPage1()
{
    m_Sound         = true;
    m_Music         = true;
    FootstepsSound  = true;
    CombatMusic     = true;
    m_SoundVolume   = 255;
    m_MusicVolume   = 255;
    BackgroundSound = false;
}

void CConfigManager::DefaultPage2()
{
    m_ClientFPS                       = 32;
    m_ReduceFPSUnactiveWindow         = true;
    StandartCharactersAnimationDelay  = false;
    StandartItemsAnimationDelay       = true;
    m_UseScaling                      = false;
    RemoveTextWithBlending            = true;
    m_DrawStatusState                 = DCSS_NO_DRAW;
    DrawStatusConditionState          = DCSCS_LOWER;
    DrawStatusConditionValue          = 70;
    m_DrawStumps                      = false;
    m_MarkingCaves                    = false;
    m_NoVegetation                    = false;
    m_NoAnimateFields                 = false;
    LockGumpsMoving                   = true;
    m_ConsoleNeedEnter                = false;
    HiddenCharactersRenderMode        = 0;
    HiddenAlpha                       = 0x7F;
    UseHiddenModeOnlyForSelf          = true;
    TransparentSpellIcons             = 1u;
    m_SpellIconAlpha                  = 0x7F;
    m_OldStyleStatusbar               = false;
    m_ApplyStateColorOnCharacters     = false;
    m_OriginalPartyStatusbar          = false;
    m_ChangeFieldsGraphic             = false;
    m_PaperdollSlots                  = true;
    RemoveStatusbarsWithoutObjects    = false;
    ShowDefaultConsoleEntryMode       = true;
    m_DrawAuraState                   = DAS_NEVER;
    DrawAuraWithCtrlPressed           = true;
    ScreenshotFormat                  = SF_PNG;
    m_ScaleImagesInPaperdollSlots     = true;
    RemoveOrCreateObjectsWithBlending = true;
    DrawHelmetsOnShroud               = false;
    m_UseGlobalMapLayer               = false;
    m_NoDrawRoofs                     = false;
    HighlightTargetByType             = true;
    AutoDisplayWorldMap               = false;
    DisableMacroInChat                = false;
    CheckPing                         = true;
    m_PingTimer                       = 10;
    CancelNewTargetSystemOnShiftEsc   = false;
    DrawStatusForHumanoids            = true;
}

void CConfigManager::DefaultPage3()
{
    UseToolTips       = true;
    ToolTipsTextColor = 0xFFFF;
    ToolTipsTextFont  = 0;
    ToolTipsDelay     = 200;
}

void CConfigManager::DefaultPage4()
{
    ChatColorInputText            = 0xFFFF;
    ChatColorMenuOption           = 0xFFFF;
    ChatColorPlayerInMemberList   = 0xFFFF;
    ChatColorText                 = 0xFFFF;
    ChatColorPlayerNameWithout    = 0xFFFF;
    ChatColorMuted                = 0xFFFF;
    ChatColorChannelModeratorName = 0xFFFF;
    ChatColorChannelModeratorText = 0xFFFF;
    ChatColorMyName               = 0xFFFF;
    ChatColorMyText               = 0xFFFF;
    ChatColorSystemMessage        = 0xFFFF;
    ChatColorBGOutputText         = 0xFFFF;
    ChatColorBGInputText          = 0xFFFF;
    ChatColorBGUserList           = 0xFFFF;
    ChatColorBGConfList           = 0xFFFF;
    ChatColorBGCommandList        = 0xFFFF;

    ChatFont = 0;
}

void CConfigManager::DefaultPage6()
{
    EnablePathfind             = true;
    HoldTabForCombat           = true;
    OffsetInterfaceWindows     = true;
    AutoArrange                = true;
    AlwaysRun                  = false;
    DisableMenubar             = false;
    GrayOutOfRangeObjects      = false;
    DisableNewTargetSystem     = true;
    m_ItemPropertiesMode       = OPM_FOLLOW_MOUSE;
    m_ItemPropertiesIcon       = false;
    ObjectHandles              = false;
    HoldShiftForContextMenus   = false;
    HoldShiftForEnablePathfind = false;
    m_CharacterBackpackStyle   = CBS_DEFAULT;
}

void CConfigManager::DefaultPage7()
{
    GameWindowWidth        = 800;
    GameWindowHeight       = 600;
    SpeechDelay            = 500;
    ScaleSpeechDelay       = true;
    SpeechColor            = 0x02B2;
    EmoteColor             = 0x0021;
    PartyMessageColor      = 0x0044;
    GuildMessageColor      = 0x0044;
    AllianceMessageColor   = 0x0057;
    IgnoreGuildMessage     = false;
    IgnoreAllianceMessage  = false;
    DarkNights             = false;
    ColoredLighting        = true;
    LockResizingGameWindow = false;
}

void CConfigManager::DefaultPage8()
{
    InnocentColor        = 0x005A;
    FriendlyColor        = 0x0044;
    SomeoneColor         = 0x03B2;
    CriminalColor        = 0x03B2;
    EnemyColor           = 0x0031;
    MurdererColor        = 0x0023;
    CriminalActionsQuery = true;
}

void CConfigManager::DefaultPage9()
{
    ShowIncomingNames = true;
    UseCircleTrans    = false;
    StatReport        = true;
    CircleTransRadius = 64;
    SkillReport       = 1;
    SpeechFont        = 0;
}

void CConfigManager::UpdateFeatures()
{
    SetDrawStatusState(m_DrawStatusState);
    SetDrawStumps(m_DrawStumps);
    SetMarkingCaves(m_MarkingCaves);
    SetNoVegetation(m_NoVegetation);
    SetNoAnimateFields(m_NoAnimateFields);
    SetApplyStateColorOnCharacters(m_ApplyStateColorOnCharacters);
    SetChangeFieldsGraphic(m_ChangeFieldsGraphic);
    SetDrawAuraState(m_DrawAuraState);
    SetNoDrawRoofs(m_NoDrawRoofs);
}

void CConfigManager::SetSound(bool val)
{
    m_Sound = val;
    if (this == &g_ConfigManager && !val)
    {
        g_Orion.AdjustSoundEffects(g_Ticks + 100000);
    }
}

void CConfigManager::SetMusic(bool val)
{
    m_Music = val;
    if (this == &g_ConfigManager && !val)
    {
        g_SoundManager.StopMusic();
    }
}

void CConfigManager::SetSoundVolume(u8 val)
{
    if (this == &g_ConfigManager && m_SoundVolume != val)
    {
        g_Orion.AdjustSoundEffects(g_Ticks + 100000, val);
    }

    m_SoundVolume = val;
}

void CConfigManager::SetMusicVolume(u8 val)
{
    if (this == &g_ConfigManager && m_MusicVolume != val)
    {
        m_MusicVolume = val;
        g_SoundManager.SetMusicVolume(m_MusicVolume);
    }
    else
    {
        m_MusicVolume = val;
    }
}

void CConfigManager::SetClientFPS(u8 val)
{
    m_ClientFPS = val;
    if (this == &g_ConfigManager)
    {
        if (m_ClientFPS < MIN_FPS_LIMIT)
        {
            m_ClientFPS = MIN_FPS_LIMIT;
        }
        else if (m_ClientFPS > MAX_FPS_LIMIT)
        {
            m_ClientFPS = MAX_FPS_LIMIT;
        }

        g_FrameDelay[WINDOW_ACTIVE] = 1000 / m_ClientFPS;
        if (!m_ReduceFPSUnactiveWindow)
        {
            g_FrameDelay[WINDOW_INACTIVE] = g_FrameDelay[WINDOW_ACTIVE];
        }
        g_OrionWindow.SetRenderTimerDelay(g_FrameDelay[g_OrionWindow.IsActive()]);
    }
}

void CConfigManager::SetUseScaling(bool val)
{
    m_UseScaling = val;
    if (!val && this == &g_ConfigManager)
    {
        g_GlobalScale = 1.0;
    }
}

void CConfigManager::SetDrawStatusState(u8 val)
{
    u8 state = val;
    if ((g_OrionFeaturesFlags & OFF_DRAW_CHARACTERS_STATUS_IN_WORLD) == 0u)
    {
        state = DCSS_NO_DRAW;
    }

    if (this == &g_ConfigManager)
    {
        if ((state != 0u) && (m_DrawStatusState == 0u))
        {
            QFOR(item, g_World->m_Items, CGameObject*)
            {
                if (item->NPC)
                {
                    CPacketStatusRequest(item->Serial).Send();
                }
            }
        }
    }

    m_DrawStatusState = state;
}

void CConfigManager::SetDrawStumps(bool val)
{
    bool state = val;
    if ((g_OrionFeaturesFlags & OFF_CHANGE_TREES_TO_STUMPS) == 0u)
    {
        state = false;
    }

    if (m_DrawStumps != state && this == &g_ConfigManager)
    {
        g_Orion.ClearTreesTextures();
    }

    m_DrawStumps = state;
}

void CConfigManager::SetMarkingCaves(bool val)
{
    bool state = val;
    if ((g_OrionFeaturesFlags & OFF_MARKING_CAVES) == 0u)
    {
        state = false;
    }

    if (m_MarkingCaves != state && this == &g_ConfigManager)
    {
        g_Orion.ClearCaveTextures();
    }

    m_MarkingCaves = state;
}

void CConfigManager::SetNoVegetation(bool val)
{
    bool state = val;
    if ((g_OrionFeaturesFlags & OFF_NO_VEGETATION) == 0u)
    {
        state = false;
    }

    m_NoVegetation = state;
}

void CConfigManager::SetNoAnimateFields(bool val)
{
    bool state = val;
    if ((g_OrionFeaturesFlags & OFF_NO_FIELDS_ANIMATION) == 0u)
    {
        state = false;
    }

    m_NoAnimateFields = state;
}

void CConfigManager::SetApplyStateColorOnCharacters(bool val)
{
    bool state = val;
    if ((g_OrionFeaturesFlags & OFF_COLORED_CHARACTERS_STATE) == 0u)
    {
        state = false;
    }

    m_ApplyStateColorOnCharacters = state;
}

void CConfigManager::SetDrawAuraState(u8 val)
{
    u8 state = val;
    if ((g_OrionFeaturesFlags & OFF_DRAW_AURA) == 0u)
    {
        state = DAS_NEVER;
    }

    m_DrawAuraState = state;
}

void CConfigManager::SetReduceFPSUnactiveWindow(bool val)
{
    if (this == &g_ConfigManager)
    {
        if (val)
        {
            g_FrameDelay[WINDOW_INACTIVE] = FRAME_DELAY_INACTIVE_WINDOW;
        }
        else
        {
            g_FrameDelay[WINDOW_INACTIVE] = g_FrameDelay[WINDOW_ACTIVE];
        }
        g_OrionWindow.SetRenderTimerDelay(g_FrameDelay[g_OrionWindow.IsActive()]);
    }

    m_ReduceFPSUnactiveWindow = val;
}

void CConfigManager::SetConsoleNeedEnter(bool val)
{
    if (this == &g_ConfigManager && val && g_EntryPointer == &g_GameConsole)
    {
        g_EntryPointer = nullptr;
    }

    m_ConsoleNeedEnter = val;
}

void CConfigManager::SetSpellIconAlpha(u8 val)
{
    if (this == &g_ConfigManager && val != m_SpellIconAlpha)
    {
        float alpha = val / 255.0f;
        bool redraw = g_ConfigManager.TransparentSpellIcons != 0u;

        QFOR(gump, g_GumpManager.m_Items, CGump*)
        {
            if (gump->GumpType == GT_SPELL)
            {
                ((CGumpSpell*)gump)->m_Blender->Alpha = alpha;

                if (redraw)
                {
                    gump->WantRedraw = true;
                }
            }
        }
    }

    m_SpellIconAlpha = val;
}

void CConfigManager::SetOldStyleStatusbar(bool val)
{
    m_OldStyleStatusbar = val;
    if (this == &g_ConfigManager)
    {
        CGump* gump = g_GumpManager.UpdateGump(g_PlayerSerial, 0, GT_STATUSBAR);

        if (gump != nullptr && !gump->Minimized)
        {
            gump->WantUpdateContent = true;
        }
    }
}

void CConfigManager::SetOriginalPartyStatusbar(bool val)
{
    m_OriginalPartyStatusbar = val;
    if (this == &g_ConfigManager)
    {
        if (g_Party.Leader != 0)
        {
            QFOR(gump, g_GumpManager.m_Items, CGump*)
            {
                if (gump->GumpType == GT_STATUSBAR &&
                    (g_Party.Leader == gump->Serial || g_Party.Contains(gump->Serial)))
                {
                    gump->WantRedraw        = true;
                    gump->WantUpdateContent = true;
                }
            }
        }
    }
}

void CConfigManager::SetChangeFieldsGraphic(bool val)
{
    m_ChangeFieldsGraphic = val;
    if ((g_OrionFeaturesFlags & OFF_TILED_FIELDS) == 0u)
    {
        m_ChangeFieldsGraphic = false;
    }

    if (this == &g_ConfigManager && g_World != nullptr)
    {
        QFOR(item, g_World->m_Items, CGameObject*)
        {
            if (!item->NPC)
            {
                ((CGameItem*)item)->CalculateFieldColor();
            }
        }
    }
}

void CConfigManager::SetPaperdollSlots(bool val)
{
    m_PaperdollSlots = val;
    if (this == &g_ConfigManager && g_World != nullptr)
    {
        QFOR(gump, g_GumpManager.m_Items, CGump*)
        {
            if (gump->GumpType == GT_PAPERDOLL)
            {
                gump->WantRedraw        = true;
                gump->WantUpdateContent = true;
            }
        }
    }
}

void CConfigManager::SetScaleImagesInPaperdollSlots(bool val)
{
    m_ScaleImagesInPaperdollSlots = val;
    if (this == &g_ConfigManager && g_World != nullptr)
    {
        QFOR(gump, g_GumpManager.m_Items, CGump*)
        {
            if (gump->GumpType == GT_PAPERDOLL)
            {
                gump->WantRedraw        = true;
                gump->WantUpdateContent = true;
            }
        }
    }
}

void CConfigManager::SetUseGlobalMapLayer(bool val)
{
    m_UseGlobalMapLayer = val;
    if (this == &g_ConfigManager && val)
    {
        QFOR(gump, g_GumpManager.m_Items, CGump*)
        {
            if (gump->GumpType == GT_WORLD_MAP)
            {
                g_GumpManager.MoveToFront(gump);
            }
        }
    }
}

void CConfigManager::SetNoDrawRoofs(bool val)
{
    m_NoDrawRoofs = val;
    if ((g_OrionFeaturesFlags & OFF_TILED_FIELDS) == 0u)
    {
        m_NoDrawRoofs = false;
    }

    if (this == &g_ConfigManager && g_Player != nullptr)
    {
        g_Player->OldX = 0;
        g_Player->OldY = 0;
    }
}

void CConfigManager::SetPingTimer(u8 val)
{
    m_PingTimer = Core::Max(Core::Min(val, u8(120)), u8(10));
    g_PingTimer = 0;
}

void CConfigManager::SetItemPropertiesMode(u8 val)
{
    m_ItemPropertiesMode = val;
    if (this == &g_ConfigManager && g_World != nullptr)
    {
        CGumpPropertyIcon* gump =
            (CGumpPropertyIcon*)g_GumpManager.UpdateContent(0, 0, GT_PROPERTY_ICON);

        if (gump != nullptr && (val == OPM_AT_ICON || val == OPM_ALWAYS_UP))
        {
            gump->SetTextW(gump->GetTextW());
        }

        g_ObjectPropertiesManager.Reset();
    }
}

void CConfigManager::SetItemPropertiesIcon(bool val)
{
    m_ItemPropertiesIcon = val;
    if (this == &g_ConfigManager && g_World != nullptr)
    {
        if (val)
        {
            CGump* gump = g_GumpManager.UpdateContent(0, 0, GT_PROPERTY_ICON);

            if (gump == nullptr)
            {
                Core::Vec2<i32> windowSize = g_OrionWindow.GetSize();

                int x = GameWindowX + (int)(GameWindowWidth * 0.9f);
                int y = GameWindowY + GameWindowHeight;

                if (x + 100 >= windowSize.x)
                    x = windowSize.x - 100;

                if (y + 60 >= windowSize.y)
                    y = windowSize.y - 60;

                g_GumpManager.AddGump(new CGumpPropertyIcon(x, y));
            }
        }
        else
        {
            g_GumpManager.CloseGump(0, 0, GT_PROPERTY_ICON);
            g_ObjectPropertiesManager.Reset();
        }
    }
}

void CConfigManager::SetCharacterBackpackStyle(u8 val)
{
    m_CharacterBackpackStyle = val;
    if (this == &g_ConfigManager && g_World != nullptr)
    {
        g_GumpManager.UpdateContent(g_PlayerSerial, 0, GT_PAPERDOLL);

        CGameItem* backpack = g_Player->FindLayer(OL_BACKPACK);

        if (backpack != nullptr)
        {
            g_GumpManager.UpdateContent(backpack->Serial, 0, GT_CONTAINER);
        }
    }
}

u16 CConfigManager::GetColorByNotoriety(u8 notoriety)
{
    u16 color = 0;
    switch ((NOTORIETY_TYPE)notoriety)
    {
        case NT_INNOCENT: //Blue
        {
            color = InnocentColor;
            break;
        }
        case NT_FRIENDLY: //Green
        {
            color = FriendlyColor;
            break;
        }
        case NT_SOMEONE_GRAY: //Someone gray
        {
            color = SomeoneColor;
            break;
        }
        case NT_CRIMINAL: //Criminal
        {
            color = CriminalColor;
            break;
        }
        case NT_ENEMY: //Orange
        {
            color = EnemyColor;
            break;
        }
        case NT_MURDERER: //Red*
        {
            color = MurdererColor;
            break;
        }
        case NT_INVULNERABLE:
        {
            color = 0x0034;
            break;
        }
        default: break;
    }

    return color;
}

bool CConfigManager::LoadBin(const std::filesystem::path& path)
{
    int screenX, screenY;
    GetDisplaySize(&screenX, &screenY);
    screenX -= 20;
    screenY -= 60;

    bool result     = false;
    g_DeveloperMode = DM_SHOW_FPS_ONLY;
    Core::MappedFile file;
    if (file.Load(path) && (file.GetSize() != 0u))
    {
        UpdateRange = g_MaxViewRange;
        u8 version  = file.ReadBE<u8>();

        //Page 1
        u8* next       = file.GetPtr();
        char blockSize = file.ReadBE<i8>();
        next += blockSize;

        if (file.ReadBE<i8>() == 1)
        {
            SetSound(file.ReadBE<u8>() != 0u);
            SetMusic(file.ReadBE<u8>() != 0u);
            FootstepsSound  = (file.ReadBE<u8>() != 0u);
            CombatMusic     = (file.ReadBE<u8>() != 0u);
            m_SoundVolume   = file.ReadBE<u8>();
            m_MusicVolume   = file.ReadBE<u8>();
            BackgroundSound = (file.ReadBE<u8>() != 0u);
        }

        file.SetPtr(next);

        //Page 2
        next      = file.GetPtr();
        blockSize = file.ReadBE<i8>();
        next += blockSize;

        m_UseScaling                     = false;
        RemoveTextWithBlending           = true;
        m_DrawStatusState                = DCSS_NO_DRAW;
        DrawStatusConditionState         = DCSCS_LOWER;
        DrawStatusConditionValue         = 70;
        bool drawStumps                  = false;
        bool markingCaves                = false;
        bool noAnimateFields             = false;
        bool noVegetation                = false;
        TransparentSpellIcons            = 1u;
        m_SpellIconAlpha                 = 0x7F;
        m_OldStyleStatusbar              = false;
        m_OriginalPartyStatusbar         = false;
        bool applyStateColorOnCharacters = false;
        bool changeFieldsGraphic         = false;
        bool paperdollSlots              = true;
        RemoveStatusbarsWithoutObjects   = false;
        ShowDefaultConsoleEntryMode      = true;
        u8 drawAuraState                 = DAS_NEVER;
        DrawAuraWithCtrlPressed          = true;
        ScreenshotFormat                 = SF_PNG;
        bool scaleImagesInPaperdollSlots = true;
        DrawHelmetsOnShroud              = false;
        bool useGlobalMapLayer           = false;
        bool noDrawRoofs                 = false;
        HighlightTargetByType            = true;
        AutoDisplayWorldMap              = false;
        DisableMacroInChat               = false;
        CheckPing                        = true;
        m_PingTimer                      = 10;
        CancelNewTargetSystemOnShiftEsc  = false;
        DrawStatusForHumanoids           = true;

        if (file.ReadBE<i8>() == 2)
        {
            if (blockSize > 23)
            {
                SetClientFPS(file.ReadBE<u8>());
                m_UseScaling                   = (file.ReadBE<u8>() != 0u);
                RemoveTextWithBlending         = (file.ReadBE<u8>() != 0u);
                m_DrawStatusState              = file.ReadBE<u8>();
                drawStumps                     = (file.ReadBE<u8>() != 0u);
                markingCaves                   = (file.ReadBE<u8>() != 0u);
                noAnimateFields                = (file.ReadBE<u8>() != 0u);
                noVegetation                   = (file.ReadBE<u8>() != 0u);
                HiddenCharactersRenderMode     = file.ReadBE<u8>();
                HiddenAlpha                    = file.ReadBE<u8>();
                UseHiddenModeOnlyForSelf       = (file.ReadBE<u8>() != 0u);
                TransparentSpellIcons          = file.ReadBE<u8>();
                m_SpellIconAlpha               = file.ReadBE<u8>();
                m_OldStyleStatusbar            = (file.ReadBE<u8>() != 0u);
                m_OriginalPartyStatusbar       = (file.ReadBE<u8>() != 0u);
                applyStateColorOnCharacters    = (file.ReadBE<u8>() != 0u);
                changeFieldsGraphic            = (file.ReadBE<u8>() != 0u);
                paperdollSlots                 = (file.ReadBE<u8>() != 0u);
                DrawStatusConditionState       = file.ReadBE<u8>();
                DrawStatusConditionValue       = file.ReadBE<u8>();
                RemoveStatusbarsWithoutObjects = (file.ReadBE<u8>() != 0u);

                ShowDefaultConsoleEntryMode = (file.ReadBE<u8>() != 0u);

                if (blockSize > 24)
                {
                    u8 auraState = file.ReadBE<u8>();

                    drawAuraState           = auraState & 0x7F;
                    DrawAuraWithCtrlPressed = ((auraState & 0x80) != 0);

                    if (blockSize > 25)
                    {
                        ScreenshotFormat = file.ReadBE<u8>();

                        if (blockSize > 26)
                        {
                            scaleImagesInPaperdollSlots = (file.ReadBE<u8>() != 0u);

                            if (blockSize > 27)
                            {
                                RemoveOrCreateObjectsWithBlending = (file.ReadBE<u8>() != 0u);

                                if (blockSize > 28)
                                {
                                    DrawHelmetsOnShroud = (file.ReadBE<u8>() != 0u);

                                    if (blockSize > 29)
                                    {
                                        useGlobalMapLayer = (file.ReadBE<u8>() != 0u);

                                        if (blockSize > 30)
                                        {
                                            noDrawRoofs = (file.ReadBE<u8>() != 0u);

                                            if (blockSize > 31)
                                            {
                                                HighlightTargetByType = (file.ReadBE<u8>() != 0u);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            else if (blockSize > 2)
            {
                SetClientFPS(file.ReadBE<u8>());

                if (blockSize > 3)
                {
                    m_UseScaling = (file.ReadBE<u8>() != 0u);

                    if (blockSize > 4)
                    {
                        RemoveTextWithBlending = (file.ReadBE<u8>() != 0u);
                        m_DrawStatusState      = file.ReadBE<u8>();

                        if (blockSize > 6)
                        {
                            drawStumps      = (file.ReadBE<u8>() != 0u);
                            markingCaves    = (file.ReadBE<u8>() != 0u);
                            noAnimateFields = (file.ReadBE<u8>() != 0u);

                            if (blockSize > 9)
                            {
                                noVegetation               = (file.ReadBE<u8>() != 0u);
                                HiddenCharactersRenderMode = file.ReadBE<u8>();
                                HiddenAlpha                = file.ReadBE<u8>();
                                UseHiddenModeOnlyForSelf   = (file.ReadBE<u8>() != 0u);

                                if (blockSize > 13)
                                {
                                    TransparentSpellIcons = file.ReadBE<u8>();
                                    m_SpellIconAlpha      = file.ReadBE<u8>();

                                    if (blockSize > 15)
                                    {
                                        m_OldStyleStatusbar         = (file.ReadBE<u8>() != 0u);
                                        m_OriginalPartyStatusbar    = (file.ReadBE<u8>() != 0u);
                                        applyStateColorOnCharacters = (file.ReadBE<u8>() != 0u);

                                        if (blockSize > 18)
                                        {
                                            changeFieldsGraphic = (file.ReadBE<u8>() != 0u);

                                            if (blockSize > 19)
                                            {
                                                paperdollSlots = (file.ReadBE<u8>() != 0u);

                                                if (blockSize > 21)
                                                {
                                                    DrawStatusConditionState = file.ReadBE<u8>();
                                                    DrawStatusConditionValue = file.ReadBE<u8>();

                                                    if (blockSize > 22)
                                                    {
                                                        RemoveStatusbarsWithoutObjects =
                                                            (file.ReadBE<u8>() != 0u);
                                                    }
                                                }
                                            }
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
                SetClientFPS(32);
            }
        }
        else
        {
            SetClientFPS(32);
        }

        SetDrawStumps(drawStumps);
        SetMarkingCaves(markingCaves);
        SetChangeFieldsGraphic(changeFieldsGraphic);
        SetPaperdollSlots(paperdollSlots);
        SetScaleImagesInPaperdollSlots(scaleImagesInPaperdollSlots);
        SetUseGlobalMapLayer(useGlobalMapLayer);
        SetNoDrawRoofs(noDrawRoofs);
        SetApplyStateColorOnCharacters(applyStateColorOnCharacters);
        SetDrawAuraState(drawAuraState);
        SetNoVegetation(noVegetation);
        SetNoAnimateFields(noAnimateFields);

        file.SetPtr(next);

        //Page 3
        next      = file.GetPtr();
        blockSize = file.ReadBE<i8>();
        next += blockSize;

        if (file.ReadBE<i8>() == 3)
        {
            UseToolTips       = (file.ReadBE<u8>() != 0u);
            ToolTipsTextColor = file.ReadLE<u16>();
            ToolTipsTextFont  = file.ReadLE<u16>();
            ToolTipsDelay     = file.ReadLE<u16>();
        }

        file.SetPtr(next);

        //Page 4
        next      = file.GetPtr();
        blockSize = file.ReadBE<i8>();
        next += blockSize;

        if (file.ReadBE<i8>() == 4)
        {
            ChatColorInputText            = file.ReadLE<u16>();
            ChatColorMenuOption           = file.ReadLE<u16>();
            ChatColorPlayerInMemberList   = file.ReadLE<u16>();
            ChatColorText                 = file.ReadLE<u16>();
            ChatColorPlayerNameWithout    = file.ReadLE<u16>();
            ChatColorMuted                = file.ReadLE<u16>();
            ChatColorChannelModeratorName = file.ReadLE<u16>();
            ChatColorChannelModeratorText = file.ReadLE<u16>();
            ChatColorMyName               = file.ReadLE<u16>();
            ChatColorMyText               = file.ReadLE<u16>();
            ChatColorSystemMessage        = file.ReadLE<u16>();
            ChatFont                      = file.ReadLE<u16>();
            ChatColorBGOutputText         = file.ReadLE<u16>();
            ChatColorBGInputText          = file.ReadLE<u16>();
            ChatColorBGUserList           = file.ReadLE<u16>();
            ChatColorBGConfList           = file.ReadLE<u16>();
            ChatColorBGCommandList        = file.ReadLE<u16>();
        }

        file.SetPtr(next);

        //Page 6
        next      = file.GetPtr();
        blockSize = file.ReadBE<i8>();
        next += blockSize;

        if (file.ReadBE<i8>() == 6)
        {
            EnablePathfind             = (file.ReadBE<u8>() != 0u);
            HoldTabForCombat           = (file.ReadBE<u8>() != 0u);
            OffsetInterfaceWindows     = (file.ReadBE<u8>() != 0u);
            AutoArrange                = (file.ReadBE<u8>() != 0u);
            AlwaysRun                  = (file.ReadBE<u8>() != 0u);
            DisableMenubar             = (file.ReadBE<u8>() != 0u);
            GrayOutOfRangeObjects      = (file.ReadBE<u8>() != 0u);
            HoldShiftForContextMenus   = true;
            HoldShiftForEnablePathfind = false;

            if (blockSize > 9)
            {
                DisableNewTargetSystem = (file.ReadBE<u8>() != 0u);
                m_ItemPropertiesMode   = file.ReadBE<u8>();
                m_ItemPropertiesIcon   = (file.ReadBE<u8>() != 0u);
                ObjectHandles          = (file.ReadBE<u8>() != 0u);
                SetReduceFPSUnactiveWindow(file.ReadBE<u8>() != 0u);

                if (blockSize > 14)
                {
                    HoldShiftForContextMenus = (file.ReadBE<u8>() != 0u);

                    if (blockSize > 15)
                    {
                        HoldShiftForEnablePathfind = (file.ReadBE<u8>() != 0u);

                        if (blockSize > 16)
                        {
                            g_ContainerRect.DefaultX = file.ReadLE<u16>();
                            g_ContainerRect.DefaultY = file.ReadLE<u16>();
                        }
                    }
                }
            }
            else
            {
                DisableNewTargetSystem = true;
                m_ItemPropertiesMode   = OPM_FOLLOW_MOUSE;
                m_ItemPropertiesIcon   = false;
                ObjectHandles          = false;
                SetReduceFPSUnactiveWindow(true);
            }
        }

        file.SetPtr(next);

        //Page 7
        next      = file.GetPtr();
        blockSize = file.ReadBE<i8>();
        next += blockSize;

        if (file.ReadBE<i8>() == 7)
        {
            GameWindowWidth                  = file.ReadLE<u16>();
            GameWindowHeight                 = file.ReadLE<u16>();
            SpeechDelay                      = file.ReadLE<u16>();
            ScaleSpeechDelay                 = (file.ReadLE<u16>() != 0u);
            SpeechColor                      = file.ReadLE<u16>();
            EmoteColor                       = file.ReadLE<u16>();
            PartyMessageColor                = file.ReadLE<u16>();
            GuildMessageColor                = file.ReadLE<u16>();
            AllianceMessageColor             = file.ReadLE<u16>();
            IgnoreGuildMessage               = (file.ReadBE<u8>() != 0u);
            IgnoreAllianceMessage            = (file.ReadBE<u8>() != 0u);
            DarkNights                       = (file.ReadBE<u8>() != 0u);
            ColoredLighting                  = (file.ReadBE<u8>() != 0u);
            StandartCharactersAnimationDelay = (file.ReadBE<u8>() != 0u);
            StandartItemsAnimationDelay      = (file.ReadBE<u8>() != 0u);
            LockResizingGameWindow           = (file.ReadBE<u8>() != 0u);
            LockGumpsMoving                  = (file.ReadBE<u8>() != 0u);
        }

        file.SetPtr(next);

        //Page 8
        next      = file.GetPtr();
        blockSize = file.ReadBE<i8>();
        next += blockSize;

        if (file.ReadBE<i8>() == 8)
        {
            InnocentColor        = file.ReadLE<u16>();
            FriendlyColor        = file.ReadLE<u16>();
            SomeoneColor         = file.ReadLE<u16>();
            CriminalColor        = file.ReadLE<u16>();
            EnemyColor           = file.ReadLE<u16>();
            MurdererColor        = file.ReadLE<u16>();
            CriminalActionsQuery = (file.ReadBE<u8>() != 0u);
        }

        file.SetPtr(next);

        //Page 9
        next      = file.GetPtr();
        blockSize = file.ReadBE<i8>();
        next += blockSize;

        if (file.ReadBE<i8>() == 9)
        {
            ShowIncomingNames = (file.ReadBE<u8>() != 0u);
            UseCircleTrans    = (file.ReadBE<u8>() != 0u);
            StatReport        = (file.ReadBE<u8>() != 0u);
            SetConsoleNeedEnter(file.ReadBE<u8>() != 0u);
            CircleTransRadius = file.ReadBE<u8>();
            SkillReport       = file.ReadBE<u8>();
            SpeechFont        = file.ReadLE<u16>();
        }

        g_CircleOfTransparency.Create(CircleTransRadius);

        file.SetPtr(next);

        //Page 10
        next      = file.GetPtr();
        blockSize = file.ReadBE<i8>();
        next += blockSize;

        if (file.ReadBE<i8>() == 10)
        {
        }

        file.SetPtr(next);

        //No page
        blockSize = file.ReadBE<i8>();

        if (file.ReadBE<i8>() == 0)
        {
            GameWindowX = file.ReadLE<i16>();

            if (GameWindowX < 0)
            {
                GameWindowX = 0;
            }

            GameWindowY = file.ReadLE<i16>();

            if (GameWindowY < 0)
            {
                GameWindowY = 0;
            }

            if (blockSize > 6)
            {
                bool zoomed = file.ReadBE<u8>() != 0u;

                int windowX = file.ReadLE<i16>();

                if (windowX < 0)
                {
                    windowX = 0;
                }

                int windowY = file.ReadLE<i16>();

                if (windowY < 0)
                {
                    windowY = 0;
                }

                int windowWidth  = file.ReadLE<i16>();
                int windowHeight = file.ReadLE<i16>();

                if (g_GameState >= GS_GAME)
                {
                    if (windowWidth < 640)
                    {
                        windowWidth = 640;
                    }

                    if (windowWidth >= screenX)
                    {
                        windowWidth = screenX;
                    }

                    if (windowHeight < 480)
                    {
                        windowHeight = 480;
                    }

                    if (windowHeight >= screenY)
                    {
                        windowHeight = screenY;
                    }
                }
                else
                {
                    windowWidth  = 640;
                    windowHeight = 480;
                }

                if (zoomed)
                {
                    g_OrionWindow.MaximizeWindow();
                }
                else
                {
                    g_OrionWindow.SetPositionSize(windowX, windowY, windowWidth, windowHeight);
                }

                g_GL.UpdateRect();

                if (blockSize > 15)
                {
                    ToggleBufficonWindow = (file.ReadBE<u8>() != 0u);

                    if (blockSize == 17)
                    {
                        g_DeveloperMode = (DEVELOPER_MODE)file.ReadBE<u8>();
                    }
                }
            }
            else
            {
                g_OrionWindow.MaximizeWindow();
            }
        }

        result = true;
    }

    if (g_GameState >= GS_GAME)
    {
        if (GameWindowWidth < 640)
        {
            GameWindowWidth = 640;
        }

        if (GameWindowWidth >= screenX)
        {
            GameWindowWidth = screenX;
        }

        if (GameWindowHeight < 480)
        {
            GameWindowHeight = 480;
        }

        if (GameWindowHeight >= screenY)
        {
            GameWindowHeight = screenY;
        }
    }
    else
    {
        GameWindowWidth  = 640;
        GameWindowHeight = 480;
    }

    file.Unload();

    return result;
}

bool CConfigManager::Load(const std::filesystem::path& a_path)
{
    if (!std::filesystem::exists(a_path))
        return false;

    int screenX, screenY;
    GetDisplaySize(&screenX, &screenY);
    screenX -= 20;
    screenY -= 60;

    Core::TextFileParser file(a_path, "=", "#;", "");
    bool zoomed      = false;
    int windowX      = -1;
    int windowY      = -1;
    int windowWidth  = -1;
    int windowHeight = -1;
    UpdateRange      = g_MaxViewRange;

    while (!file.IsEOF())
    {
        auto strings = file.ReadTokens();
        if (strings.size() >= 2)
        {
            const int code = cmkc::GetConfigKey(strings[0]);
            if (code == CMKC_NONE)
            {
                continue;
            }

            switch (code)
            {
                //Page 1
                case CMKC_SOUND: SetSound(Core::ToBool(strings[1])); break;
                case CMKC_SOUND_VOLUME: m_SoundVolume = atoi(strings[1].c_str()); break;
                case CMKC_MUSIC: SetMusic(Core::ToBool(strings[1])); break;
                case CMKC_MUSIC_VOLUME: m_MusicVolume = atoi(strings[1].c_str()); break;
                case CMKC_FOOTSTEPS_SOUND: FootstepsSound = Core::ToBool(strings[1]); break;
                case CMKC_COMBAT_MUSIC: CombatMusic = Core::ToBool(strings[1]); break;
                case CMKC_BACKGROUND_SOUND: BackgroundSound = Core::ToBool(strings[1]); break;

                //Page 2
                case CMKC_CLIENT_FPS: SetClientFPS(atoi(strings[1].c_str())); break;
                case CMKC_USE_SCALING: m_UseScaling = Core::ToBool(strings[1]); break;
                case CMKC_REMOVE_TEXT_WITH_BLENDING:
                    RemoveTextWithBlending = Core::ToBool(strings[1]);
                    break;
                case CMKC_DRAW_STATUS_STATE: m_DrawStatusState = atoi(strings[1].c_str()); break;
                case CMKC_DRAW_STUMPS: SetDrawStumps(Core::ToBool(strings[1])); break;
                case CMKC_MARKING_CAVES: SetMarkingCaves(Core::ToBool(strings[1])); break;
                case CMKC_NO_ANIMATE_FIELDS: SetNoAnimateFields(Core::ToBool(strings[1])); break;
                case CMKC_NO_VEGETATION: SetNoVegetation(Core::ToBool(strings[1])); break;
                case CMKC_HIDDEN_CHARACTERS_RENDER_MODE:
                    HiddenCharactersRenderMode = atoi(strings[1].c_str());
                    break;
                case CMKC_HIDDEN_ALPHA: HiddenAlpha = atoi(strings[1].c_str()); break;
                case CMKC_USE_HIDDEN_MODE_ONLY_FOR_SELF:
                    UseHiddenModeOnlyForSelf = Core::ToBool(strings[1]);
                    break;
                case CMKC_TRANSPARENT_SPELL_ICONS:
                    TransparentSpellIcons = atoi(strings[1].c_str());
                    break;
                case CMKC_SPELL_ICON_ALPHA: m_SpellIconAlpha = atoi(strings[1].c_str()); break;
                case CMKC_OLD_STYLE_STATUSBAR: m_OldStyleStatusbar = Core::ToBool(strings[1]); break;
                case CMKC_ORIGINAL_PARTY_STATUSBAR:
                    m_OriginalPartyStatusbar = Core::ToBool(strings[1]);
                    break;
                case CMKC_APPLY_STATE_COLOR_ON_CHARACTERS:
                    SetApplyStateColorOnCharacters(Core::ToBool(strings[1]));
                    break;
                case CMKC_CHANGE_FIELDS_GRAPHIC: SetChangeFieldsGraphic(Core::ToBool(strings[1])); break;
                case CMKC_PAPERDOLL_SLOTS: SetPaperdollSlots(Core::ToBool(strings[1])); break;
                case CMKC_DRAW_STATUS_CONDITION_STATE:
                    DrawStatusConditionState = atoi(strings[1].c_str());
                    break;
                case CMKC_DRAW_STATUS_CONDITION_VALUE:
                    DrawStatusConditionValue = atoi(strings[1].c_str());
                    break;
                case CMKC_REMOVE_STATUSBARS_WITHOUT_OBJECTS:
                    RemoveStatusbarsWithoutObjects = Core::ToBool(strings[1]);
                    break;
                case CMKC_SHOW_DEFAULT_CONSOLE_ENTRY_MODE:
                    ShowDefaultConsoleEntryMode = Core::ToBool(strings[1]);
                    break;
                case CMKC_DRAW_AURA_STATE: SetDrawAuraState(atoi(strings[1].c_str())); break;
                case CMKC_DRAW_AURA_WITH_CTRL_PRESSED:
                    DrawAuraWithCtrlPressed = Core::ToBool(strings[1]);
                    break;
                case CMKC_SCREENSHOT_FORMAT: ScreenshotFormat = atoi(strings[1].c_str()); break;
                case CMKC_SCALE_IMAGES_IN_PAPERDOLL_SLOTS:
                    SetScaleImagesInPaperdollSlots(Core::ToBool(strings[1]));
                    break;
                case CMKC_REMOVE_OR_CREATE_OBJECTS_WITH_BLENDING:
                    RemoveOrCreateObjectsWithBlending = Core::ToBool(strings[1]);
                    break;
                case CMKC_DRAW_HELMETS_ON_SHROUD: DrawHelmetsOnShroud = Core::ToBool(strings[1]); break;
                case CMKC_USE_GLOBAL_MAP_LAYER: SetUseGlobalMapLayer(Core::ToBool(strings[1])); break;
                case CMKC_NO_DRAW_ROOFS: SetNoDrawRoofs(Core::ToBool(strings[1])); break;
                case CMKC_HIGHLIGHT_TARGET_BY_TYPE:
                    HighlightTargetByType = Core::ToBool(strings[1]);
                    break;
                case CMKC_AUTO_DISPLAY_WORLD_MAP: AutoDisplayWorldMap = Core::ToBool(strings[1]); break;
                case CMKC_DISABLE_MACRO_IN_CHAT: DisableMacroInChat = Core::ToBool(strings[1]); break;
                case CMKC_CHECK_PING: CheckPing = Core::ToBool(strings[1]); break;
                case CMKC_PING_TIMER: SetPingTimer(atoi(strings[1].c_str())); break;
                case CMKC_CANCEL_NEW_TARGET_SYSTEM_ON_SHIFT_ESC:
                    CancelNewTargetSystemOnShiftEsc = Core::ToBool(strings[1]);
                    break;
                case CMKC_DRAW_STATUS_FOR_HUMANOIDS:
                    DrawStatusForHumanoids = Core::ToBool(strings[1]);
                    break;

                //Page 3
                case CMKC_USE_TOOLTIPS: UseToolTips = Core::ToBool(strings[1]); break;
                case CMKC_TOOLTIPS_TEXT_COLOR: ToolTipsTextColor = atoi(strings[1].c_str()); break;
                case CMKC_TOOLTIPS_TEXT_FONT: ToolTipsTextFont = atoi(strings[1].c_str()); break;
                case CMKC_TOOLTIPS_DELAY: ToolTipsDelay = atoi(strings[1].c_str()); break;

                //Page 4
                case CMKC_CHAT_COLOR_INPUT_TEXT:
                    ChatColorInputText = atoi(strings[1].c_str());
                    break;
                case CMKC_CHAT_COLOR_MENU_OPTION:
                    ChatColorMenuOption = atoi(strings[1].c_str());
                    break;
                case CMKC_CHAT_COLOR_PLAYER_IN_MEMBER_LIST:
                    ChatColorPlayerInMemberList = atoi(strings[1].c_str());
                    break;
                case CMKC_CHAT_COLOR_TEXT: ChatColorText = atoi(strings[1].c_str()); break;
                case CMKC_CHAT_COLOR_PLAYER_NAME_WITHOUT:
                    ChatColorPlayerNameWithout = atoi(strings[1].c_str());
                    break;
                case CMKC_CHAT_COLOR_MUTED: ChatColorMuted = atoi(strings[1].c_str()); break;
                case CMKC_CHAT_COLOR_CHANNEL_MODERATOR_NAME:
                    ChatColorChannelModeratorName = atoi(strings[1].c_str());
                    break;
                case CMKC_CHAT_COLOR_CHANNEL_MODERATOR_TEXT:
                    ChatColorChannelModeratorText = atoi(strings[1].c_str());
                    break;
                case CMKC_CHAT_COLOR_MY_NAME: ChatColorMyName = atoi(strings[1].c_str()); break;
                case CMKC_CHAT_COLOR_MY_TEXT: ChatColorMyText = atoi(strings[1].c_str()); break;
                case CMKC_CHAT_COLOR_SYSTEM_MESSAGE:
                    ChatColorSystemMessage = atoi(strings[1].c_str());
                    break;
                case CMKC_CHAT_FONT: ChatFont = atoi(strings[1].c_str()); break;
                case CMKC_CHAT_COLOR_BG_OUTPUT_TEXT:
                    ChatColorBGOutputText = atoi(strings[1].c_str());
                    break;
                case CMKC_CHAT_COLOR_BG_INPUT_TEXT:
                    ChatColorBGInputText = atoi(strings[1].c_str());
                    break;
                case CMKC_CHAT_COLOR_BG_USER_LIST:
                    ChatColorBGUserList = atoi(strings[1].c_str());
                    break;
                case CMKC_CHAT_COLOR_BG_CONF_LIST:
                    ChatColorBGConfList = atoi(strings[1].c_str());
                    break;
                case CMKC_CHAT_COLOR_BG_COMMAND_LIST:
                    ChatColorBGCommandList = atoi(strings[1].c_str());
                    break;

                //Page 6
                case CMKC_ENABLE_PATHFIND: EnablePathfind = Core::ToBool(strings[1]); break;
                case CMKC_HOLD_TAB_FOR_COMBAT: HoldTabForCombat = Core::ToBool(strings[1]); break;
                case CMKC_OFFSET_INTERFACE_WINDOWS:
                    OffsetInterfaceWindows = Core::ToBool(strings[1]);
                    break;
                case CMKC_AUTO_ARRANGE: AutoArrange = Core::ToBool(strings[1]); break;
                case CMKC_ALWAYS_RUN: AlwaysRun = Core::ToBool(strings[1]); break;
                case CMKC_DISABLE_MENUBAR: DisableMenubar = Core::ToBool(strings[1]); break;
                case CMKC_GRAY_OUT_OF_RANGE_OBJECTS:
                    GrayOutOfRangeObjects = Core::ToBool(strings[1]);
                    break;
                case CMKC_DISABLE_NEW_TARGET_SYSTEM:
                    DisableNewTargetSystem = Core::ToBool(strings[1]);
                    break;
                case CMKC_ITEMP_ROPERTIES_MODE:
                    m_ItemPropertiesMode = atoi(strings[1].c_str());
                    break;
                case CMKC_ITEMP_ROPERTIES_ICON: m_ItemPropertiesIcon = Core::ToBool(strings[1]); break;
                case CMKC_OBJECT_HANDLES: ObjectHandles = Core::ToBool(strings[1]); break;
                case CMKC_REDUCE_FPS_UNACTIVE_WINDOW:
                    SetReduceFPSUnactiveWindow(Core::ToBool(strings[1]));
                    break;
                case CMKC_HOLD_SHIFT_FOR_CONTEXT_MENUS:
                    HoldShiftForContextMenus = Core::ToBool(strings[1]);
                    break;
                case CMKC_HOLD_SHIFT_FOR_ENABLE_PATHFIND:
                    HoldShiftForEnablePathfind = Core::ToBool(strings[1]);
                    break;
                case CMKC_CONTAINER_DEFAULT_X:
                    g_ContainerRect.DefaultX = atoi(strings[1].c_str());
                    break;
                case CMKC_CONTAINER_DEFAULT_Y:
                    g_ContainerRect.DefaultY = atoi(strings[1].c_str());
                    break;

                //Page 7
                case CMKC_GAME_WINDOW_WIDTH: GameWindowWidth = atoi(strings[1].c_str()); break;
                case CMKC_GAME_WINDOW_HEIGHT: GameWindowHeight = atoi(strings[1].c_str()); break;
                case CMKC_SPEECH_DELAY: SpeechDelay = atoi(strings[1].c_str()); break;
                case CMKC_SCALE_SPEECH_DELAY: ScaleSpeechDelay = Core::ToBool(strings[1]); break;
                case CMKC_SPEECH_COLOR: SpeechColor = atoi(strings[1].c_str()); break;
                case CMKC_EMOTE_COLOR: EmoteColor = atoi(strings[1].c_str()); break;
                case CMKC_PARTY_MESSAGE_COLOR: PartyMessageColor = atoi(strings[1].c_str()); break;
                case CMKC_GUILD_MESSAGE_COLOR: GuildMessageColor = atoi(strings[1].c_str()); break;
                case CMKC_ALLIANCE_MESSAGE_COLOR:
                    AllianceMessageColor = atoi(strings[1].c_str());
                    break;
                case CMKC_IGNORE_GUILD_MESSAGE: IgnoreGuildMessage = Core::ToBool(strings[1]); break;
                case CMKC_IGNORE_ALLIANCE_MESSAGE:
                    IgnoreAllianceMessage = Core::ToBool(strings[1]);
                    break;
                case CMKC_DARK_NIGHTS: DarkNights = Core::ToBool(strings[1]); break;
                case CMKC_COLORED_LIGHTING: ColoredLighting = Core::ToBool(strings[1]); break;
                case CMKC_STANDART_CHARACTERS_ANIMATION_DELAY:
                    StandartCharactersAnimationDelay = Core::ToBool(strings[1]);
                    break;
                case CMKC_STANDART_ITEMS_ANIMATION_DELAY:
                    StandartItemsAnimationDelay = Core::ToBool(strings[1]);
                    break;
                case CMKC_LOCK_RESIZING_GAME_WINDOW:
                    LockResizingGameWindow = Core::ToBool(strings[1]);
                    break;
                case CMKC_LOCK_GUMPS_MOVING: LockGumpsMoving = Core::ToBool(strings[1]); break;

                //Page 8
                case CMKC_INNOCENT_COLOR: InnocentColor = atoi(strings[1].c_str()); break;
                case CMKC_FRIENDLY_COLOR: FriendlyColor = atoi(strings[1].c_str()); break;
                case CMKC_SOMEONE_COLOR: SomeoneColor = atoi(strings[1].c_str()); break;
                case CMKC_CRIMINAL_COLOR: CriminalColor = atoi(strings[1].c_str()); break;
                case CMKC_ENEMY_COLOR: EnemyColor = atoi(strings[1].c_str()); break;
                case CMKC_MURDERER_COLOR: MurdererColor = atoi(strings[1].c_str()); break;
                case CMKC_CRIMINAL_ACTIONS_QUERY: CriminalActionsQuery = Core::ToBool(strings[1]); break;

                //Page 9
                case CMKC_SHOW_INCOMING_NAMES: ShowIncomingNames = Core::ToBool(strings[1]); break;
                case CMKC_USE_CIRCLE_TRANS: UseCircleTrans = Core::ToBool(strings[1]); break;
                case CMKC_STAT_REPORT: StatReport = Core::ToBool(strings[1]); break;
                case CMKC_CONSOLE_NEED_ENTER: SetConsoleNeedEnter(Core::ToBool(strings[1])); break;
                case CMKC_CIRCLE_TRANS_RADIUS:
                    CircleTransRadius = atoi(strings[1].c_str());
                    g_CircleOfTransparency.Create(CircleTransRadius);
                    break;
                case CMKC_SKILL_REPORT: SkillReport = atoi(strings[1].c_str()); break;
                case CMKC_SPEECH_FONT: SpeechFont = atoi(strings[1].c_str()); break;

                //No page
                case CMKC_GAME_WINDOW_X: GameWindowX = atoi(strings[1].c_str()); break;
                case CMKC_GAME_WINDOW_Y: GameWindowY = atoi(strings[1].c_str()); break;
                case CMKC_ZOOMED: zoomed = Core::ToBool(strings[1]); break;
                case CMKC_REAL_X: windowX = atoi(strings[1].c_str()); break;
                case CMKC_REAL_Y: windowY = atoi(strings[1].c_str()); break;
                case CMKC_REAL_WIDTH: windowWidth = atoi(strings[1].c_str()); break;
                case CMKC_REAL_HEIGHT: windowHeight = atoi(strings[1].c_str()); break;
                case CMKC_TOGGLE_BUFFICON_WINDOW: ToggleBufficonWindow = Core::ToBool(strings[1]); break;
                case CMKC_DEVELOPER_MODE:
                    g_DeveloperMode = (DEVELOPER_MODE)atoi(strings[1].c_str());
                    break;
                case CMKC_LAST_SERVER:
                    if (g_World == nullptr)
                    {
                        g_ServerList.LastServerName = strings[1];
                    }
                    break;
                case CMKC_LAST_CHARACTER:
                    if (g_World == nullptr)
                    {
                        g_CharacterList.LastCharacterName = strings[1];
                    }
                    break;
                case CMKC_CHARACTER_BACKPACK_STYLE:
                    m_CharacterBackpackStyle = atoi(strings[1].c_str());
                    break;
                default: break;
            }
        }
    }

    if (GameWindowX < 0)
    {
        GameWindowX = 0;
    }

    if (GameWindowY < 0)
    {
        GameWindowY = 0;
    }

    if (windowX != -1 && windowY != -1 && windowWidth != -1 && windowHeight != -1)
    {
        if (windowX < 0)
        {
            windowX = 0;
        }

        if (windowY < 0)
        {
            windowY = 0;
        }

        if (g_GameState >= GS_GAME)
        {
            if (windowWidth < 640)
            {
                windowWidth = 640;
            }

            if (windowWidth >= screenX)
            {
                windowWidth = screenX;
            }

            if (windowHeight < 480)
            {
                windowHeight = 480;
            }

            if (windowHeight >= screenY)
            {
                windowHeight = screenY;
            }
        }
        else
        {
            windowWidth  = 640;
            windowHeight = 480;
        }

        if (zoomed)
        {
            g_OrionWindow.MaximizeWindow();
        }
        else
        {
            g_OrionWindow.SetPositionSize(windowX, windowY, windowWidth, windowHeight);
        }

        g_GL.UpdateRect();
    }
    else
    {
        g_OrionWindow.MaximizeWindow();
    }

    return true;
}

void CConfigManager::Save(const std::filesystem::path& a_path)
{
    Core::File file(a_path, "w");
    if (file)
    {
        //Page 1
        file.Print("Sound=%s\n", (m_Sound ? "yes" : "no"));
        file.Print("SoundVolume=%i\n", m_SoundVolume);
        file.Print("Music=%s\n", (m_Music ? "yes" : "no"));
        file.Print("MusicVolume=%i\n", m_MusicVolume);
        file.Print("FootstepsSound=%s\n", (FootstepsSound ? "yes" : "no"));
        file.Print("CombatMusic=%s\n", (CombatMusic ? "yes" : "no"));
        file.Print("BackgroundSound=%s\n", (BackgroundSound ? "yes" : "no"));

        //Page 2
        file.Print("ClientFPS=%i\n", m_ClientFPS);
        file.Print("UseScaling=%s\n", (m_UseScaling ? "yes" : "no"));
        file.Print("RemoveTextWithBlending=%s\n", (RemoveTextWithBlending ? "yes" : "no"));
        file.Print("DrawStatusState=%i\n", m_DrawStatusState);
        file.Print("DrawStumps=%s\n", (m_DrawStumps ? "yes" : "no"));
        file.Print("MarkingCaves=%s\n", (m_MarkingCaves ? "yes" : "no"));
        file.Print("NoAnimateFields=%s\n", (m_NoAnimateFields ? "yes" : "no"));
        file.Print("NoVegetation=%s\n", (m_NoVegetation ? "yes" : "no"));
        file.Print("HiddenCharactersRenderMode=%i\n", HiddenCharactersRenderMode);
        file.Print("HiddenAlpha=%i\n", HiddenAlpha);
        file.Print("UseHiddenModeOnlyForSelf=%s\n", (UseHiddenModeOnlyForSelf ? "yes" : "no"));
        file.Print("TransparentSpellIcons=%i\n", TransparentSpellIcons);
        file.Print("SpellIconAlpha=%i\n", m_SpellIconAlpha);
        file.Print("OldStyleStatusbar=%s\n", (m_OldStyleStatusbar ? "yes" : "no"));
        file.Print("OriginalPartyStatusbar=%s\n", (m_OriginalPartyStatusbar ? "yes" : "no"));
        file.Print(
            "ApplyStateColorOnCharacters=%s\n", (m_ApplyStateColorOnCharacters ? "yes" : "no"));
        file.Print("ChangeFieldsGraphic=%s\n", (m_ChangeFieldsGraphic ? "yes" : "no"));
        file.Print("PaperdollSlots=%s\n", (m_PaperdollSlots ? "yes" : "no"));
        file.Print("DrawStatusConditionState=%i\n", DrawStatusConditionState);
        file.Print("DrawStatusConditionValue=%i\n", DrawStatusConditionValue);
        file.Print(
            "RemoveStatusbarsWithoutObjects=%s\n", (RemoveStatusbarsWithoutObjects ? "yes" : "no"));
        file.Print(
            "ShowDefaultConsoleEntryMode=%s\n", (ShowDefaultConsoleEntryMode ? "yes" : "no"));
        file.Print("DrawAuraState=%i\n", m_DrawAuraState);
        file.Print("DrawAuraWithCtrlPressed=%s\n", (DrawAuraWithCtrlPressed ? "yes" : "no"));
        file.Print("ScreenshotFormat=%i\n", ScreenshotFormat);
        file.Print(
            "ScaleImagesInPaperdollSlots=%s\n", (m_ScaleImagesInPaperdollSlots ? "yes" : "no"));
        file.Print(
            "RemoveOrCreateObjectsWithBlending=%s\n",
            (RemoveOrCreateObjectsWithBlending ? "yes" : "no"));
        file.Print("DrawHelmetsOnShroud=%s\n", (DrawHelmetsOnShroud ? "yes" : "no"));
        file.Print("UseGlobalMapLayer=%s\n", (m_UseGlobalMapLayer ? "yes" : "no"));
        file.Print("NoDrawRoofs=%s\n", (m_NoDrawRoofs ? "yes" : "no"));
        file.Print("HighlightTargetByType=%s\n", (HighlightTargetByType ? "yes" : "no"));
        file.Print("AutoDisplayWorldMap=%s\n", (AutoDisplayWorldMap ? "yes" : "no"));
        file.Print("DisableMacroInChat=%s\n", (DisableMacroInChat ? "yes" : "no"));
        file.Print("CheckPing=%s\n", (CheckPing ? "yes" : "no"));
        file.Print("PingTimer=%i\n", m_PingTimer);
        file.Print(
            "CancelNewTargetSystemOnShiftEsc=%s\n",
            (CancelNewTargetSystemOnShiftEsc ? "yes" : "no"));
        file.Print("DrawStatusForHumanoids=%s\n", (DrawStatusForHumanoids ? "yes" : "no"));

        //Page 3
        file.Print("UseToolTips=%s\n", (UseToolTips ? "yes" : "no"));
        file.Print("ToolTipsTextColor=%i\n", ToolTipsTextColor);
        file.Print("ToolTipsTextFont=%i\n", ToolTipsTextFont);
        file.Print("ToolTipsDelay=%i\n", ToolTipsDelay);

        //Page 4
        file.Print("ChatColorInputText=%i\n", ChatColorInputText);
        file.Print("ChatColorMenuOption=%i\n", ChatColorMenuOption);
        file.Print("ChatColorPlayerInMemberList=%i\n", ChatColorPlayerInMemberList);
        file.Print("ChatColorText=%i\n", ChatColorText);
        file.Print("ChatColorPlayerNameWithout=%i\n", ChatColorPlayerNameWithout);
        file.Print("ChatColorMuted=%i\n", ChatColorMuted);
        file.Print("ChatColorChannelModeratorName=%i\n", ChatColorChannelModeratorName);
        file.Print("ChatColorChannelModeratorText=%i\n", ChatColorChannelModeratorText);
        file.Print("ChatColorMyName=%i\n", ChatColorMyName);
        file.Print("ChatColorMyText=%i\n", ChatColorMyText);
        file.Print("ChatColorSystemMessage=%i\n", ChatColorSystemMessage);
        file.Print("ChatFont=%i\n", ChatFont);
        file.Print("ChatColorBGOutputText=%i\n", ChatColorBGOutputText);
        file.Print("ChatColorBGInputText=%i\n", ChatColorBGInputText);
        file.Print("ChatColorBGUserList=%i\n", ChatColorBGUserList);
        file.Print("ChatColorBGConfList=%i\n", ChatColorBGConfList);
        file.Print("ChatColorBGCommandList=%i\n", ChatColorBGCommandList);

        //Page 6
        file.Print("EnablePathfind=%s\n", (EnablePathfind ? "yes" : "no"));
        file.Print("HoldTabForCombat=%s\n", (HoldTabForCombat ? "yes" : "no"));
        file.Print("OffsetInterfaceWindows=%s\n", (OffsetInterfaceWindows ? "yes" : "no"));
        file.Print("AutoArrange=%s\n", (AutoArrange ? "yes" : "no"));
        file.Print("AlwaysRun=%s\n", (AlwaysRun ? "yes" : "no"));
        file.Print("DisableMenubar=%s\n", (DisableMenubar ? "yes" : "no"));
        file.Print("GrayOutOfRangeObjects=%s\n", (GrayOutOfRangeObjects ? "yes" : "no"));
        file.Print("DisableNewTargetSystem=%s\n", (DisableNewTargetSystem ? "yes" : "no"));
        file.Print("ItemPropertiesMode=%i\n", m_ItemPropertiesMode);
        file.Print("ItemPropertiesIcon=%s\n", (m_ItemPropertiesIcon ? "yes" : "no"));
        file.Print("ObjectHandles=%s\n", (ObjectHandles ? "yes" : "no"));
        file.Print("ReduceFPSUnactiveWindow=%s\n", (m_ReduceFPSUnactiveWindow ? "yes" : "no"));
        file.Print("HoldShiftForContextMenus=%s\n", (HoldShiftForContextMenus ? "yes" : "no"));
        file.Print("HoldShiftForEnablePathfind=%s\n", (HoldShiftForEnablePathfind ? "yes" : "no"));
        file.Print("ContainerDefaultX=%i\n", g_ContainerRect.DefaultX);
        file.Print("ContainerDefaultY=%i\n", g_ContainerRect.DefaultY);
        file.Print("CharacterBackpackStyle=%i\n", GetCharacterBackpackStyle());

        //Page 7
        file.Print("GameWindowWidth=%i\n", GameWindowWidth);
        file.Print("GameWindowHeight=%i\n", GameWindowHeight);
        file.Print("SpeechDelay=%i\n", SpeechDelay);
        file.Print("ScaleSpeechDelay=%s\n", (ScaleSpeechDelay ? "yes" : "no"));
        file.Print("SpeechColor=%i\n", SpeechColor);
        file.Print("EmoteColor=%i\n", EmoteColor);
        file.Print("PartyMessageColor=%i\n", PartyMessageColor);
        file.Print("GuildMessageColor=%i\n", GuildMessageColor);
        file.Print("AllianceMessageColor=%i\n", AllianceMessageColor);
        file.Print("IgnoreGuildMessage=%s\n", (IgnoreGuildMessage ? "yes" : "no"));
        file.Print("IgnoreAllianceMessage=%s\n", (IgnoreAllianceMessage ? "yes" : "no"));
        file.Print("DarkNights=%s\n", (DarkNights ? "yes" : "no"));
        file.Print("ColoredLighting=%s\n", (ColoredLighting ? "yes" : "no"));
        file.Print(
            "StandartCharactersAnimationDelay=%s\n",
            (StandartCharactersAnimationDelay ? "yes" : "no"));
        file.Print(
            "StandartItemsAnimationDelay=%s\n", (StandartItemsAnimationDelay ? "yes" : "no"));
        file.Print("LockResizingGameWindow=%s\n", (LockResizingGameWindow ? "yes" : "no"));
        file.Print("LockGumpsMoving=%s\n", (LockGumpsMoving ? "yes" : "no"));

        //Page 8
        file.Print("InnocentColor=%i\n", InnocentColor);
        file.Print("FriendlyColor=%i\n", FriendlyColor);
        file.Print("SomeoneColor=%i\n", SomeoneColor);
        file.Print("CriminalColor=%i\n", CriminalColor);
        file.Print("EnemyColor=%i\n", EnemyColor);
        file.Print("MurdererColor=%i\n", MurdererColor);
        file.Print("CriminalActionsQuery=%s\n", (CriminalActionsQuery ? "yes" : "no"));

        //Page 9
        file.Print("ShowIncomingNames=%s\n", (ShowIncomingNames ? "yes" : "no"));
        file.Print("UseCircleTrans=%s\n", (UseCircleTrans ? "yes" : "no"));
        file.Print("StatReport=%s\n", (StatReport ? "yes" : "no"));
        file.Print("ConsoleNeedEnter=%s\n", (m_ConsoleNeedEnter ? "yes" : "no"));
        file.Print("CircleTransRadius=%i\n", CircleTransRadius);
        file.Print("SkillReport=%i\n", SkillReport);
        file.Print("SpeechFont=%i\n", SpeechFont);

        //No page
        file.Print("GameWindowX=%i\n", GameWindowX);
        file.Print("GameWindowY=%i\n", GameWindowY);

        file.Print("Zoomed=%s\n", (g_OrionWindow.IsMaximizedWindow() ? "yes" : "no"));

        int x, y, w, h;
        g_OrionWindow.GetPositionSize(&x, &y, &w, &h);

        file.Print("RealX=%i\n", x);
        file.Print("RealY=%i\n", y);
        file.Print("RealWidth=%i\n", w);
        file.Print("RealHeight=%i\n", h);

        file.Print("ToggleBufficonWindow=%s\n", (ToggleBufficonWindow ? "yes" : "no"));
        file.Print("DeveloperMode=%i\n", g_DeveloperMode);
    }
}
