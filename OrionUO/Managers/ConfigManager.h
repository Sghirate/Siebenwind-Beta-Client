#pragma once

class CConfigManager
{
protected:
    bool m_Sound                       = true;
    bool m_Music                       = false;
    u8 m_SoundVolume              = 255;
    u8 m_MusicVolume              = 255;
    u8 m_ClientFPS                = 32;
    bool m_UseScaling                  = false;
    u8 m_DrawStatusState          = 0;
    bool m_DrawStumps                  = false;
    bool m_MarkingCaves                = false;
    bool m_NoVegetation                = false;
    bool m_NoAnimateFields             = false;
    bool m_ReduceFPSUnactiveWindow     = false;
    bool m_ConsoleNeedEnter            = false;
    u8 m_SpellIconAlpha           = 0;
    bool m_OldStyleStatusbar           = false;
    bool m_OriginalPartyStatusbar      = false;
    bool m_ApplyStateColorOnCharacters = false;
    bool m_ChangeFieldsGraphic         = false;
    bool m_PaperdollSlots              = false;
    u8 m_DrawAuraState            = 0;
    bool m_ScaleImagesInPaperdollSlots = false;
    bool m_UseGlobalMapLayer           = false;
    bool m_NoDrawRoofs                 = false;
    u8 m_PingTimer                = 10;
    u8 m_ItemPropertiesMode       = OPM_FOLLOW_MOUSE;
    bool m_ItemPropertiesIcon          = false;
    u8 m_CharacterBackpackStyle   = CBS_DEFAULT;

public:
    bool FootstepsSound                    = true;
    bool CombatMusic                       = true;
    bool BackgroundSound                   = false;
    bool RemoveTextWithBlending            = false;
    u8 DrawStatusConditionState       = 0;
    u8 DrawStatusConditionValue       = 0;
    bool StandartCharactersAnimationDelay  = false;
    bool StandartItemsAnimationDelay       = false;
    bool LockGumpsMoving                   = false;
    u8 HiddenCharactersRenderMode     = 0;
    u8 HiddenAlpha                    = 0;
    bool UseHiddenModeOnlyForSelf          = false;
    u8 TransparentSpellIcons          = 0;
    bool RemoveStatusbarsWithoutObjects    = false;
    bool ShowDefaultConsoleEntryMode       = false;
    bool DrawAuraWithCtrlPressed           = false;
    u8 ScreenshotFormat               = 0;
    bool RemoveOrCreateObjectsWithBlending = false;
    bool DrawHelmetsOnShroud               = false;
    bool HighlightTargetByType             = true;
    bool AutoDisplayWorldMap               = false;
    bool DisableMacroInChat                = false;
    bool CheckPing                         = true;
    bool CancelNewTargetSystemOnShiftEsc   = false;
    bool DrawStatusForHumanoids            = true;

    bool UseToolTips           = false;
    u16 ToolTipsTextColor = 0;
    u16 ToolTipsTextFont  = 0;
    u16 ToolTipsDelay     = 0;

    u16 ChatColorInputText            = 0;
    u16 ChatColorMenuOption           = 0;
    u16 ChatColorPlayerInMemberList   = 0;
    u16 ChatColorText                 = 0;
    u16 ChatColorPlayerNameWithout    = 0;
    u16 ChatColorMuted                = 0;
    u16 ChatColorChannelModeratorName = 0;
    u16 ChatColorChannelModeratorText = 0;
    u16 ChatColorMyName               = 0;
    u16 ChatColorMyText               = 0;
    u16 ChatColorSystemMessage        = 0;
    u16 ChatFont                      = 0;
    u16 ChatColorBGOutputText         = 0;
    u16 ChatColorBGInputText          = 0;
    u16 ChatColorBGUserList           = 0;
    u16 ChatColorBGConfList           = 0;
    u16 ChatColorBGCommandList        = 0;

    bool EnablePathfind             = false;
    bool HoldTabForCombat           = false;
    bool OffsetInterfaceWindows     = false;
    bool AutoArrange                = false;
    bool AlwaysRun                  = false;
    bool DisableMenubar             = false;
    bool GrayOutOfRangeObjects      = false;
    bool DisableNewTargetSystem     = false;
    bool ObjectHandles              = false;
    bool HoldShiftForContextMenus   = false;
    bool HoldShiftForEnablePathfind = false;
    int GameWindowWidth             = 0;
    int GameWindowHeight            = 0;
    u16 SpeechDelay            = 0;
    bool ScaleSpeechDelay           = false;
    u16 SpeechColor            = 0;
    u16 EmoteColor             = 0;
    u16 PartyMessageColor      = 0;
    u16 GuildMessageColor      = 0;
    u16 AllianceMessageColor   = 0;
    bool IgnoreGuildMessage         = false;
    bool IgnoreAllianceMessage      = false;
    bool DarkNights                 = false;
    bool ColoredLighting            = false;
    bool LockResizingGameWindow     = false;

    u16 InnocentColor    = 0;
    u16 FriendlyColor    = 0;
    u16 SomeoneColor     = 0;
    u16 CriminalColor    = 0;
    u16 EnemyColor       = 0;
    u16 MurdererColor    = 0;
    bool CriminalActionsQuery = false;

    bool ShowIncomingNames    = false;
    bool UseCircleTrans       = false;
    bool StatReport           = false;
    u8 CircleTransRadius = 0;
    u8 SkillReport       = 0;
    u16 SpeechFont       = 0;

    bool FilterPWOn       = false;
    bool ObscenityFilter  = false;
    string FilterPassword = "";

    bool ToggleBufficonWindow = false;
    int GameWindowX           = 0;
    int GameWindowY           = 0;
    int UpdateRange           = g_MaxViewRange;

public:
    CConfigManager();
    ~CConfigManager() {}

    bool GetSound() { return m_Sound; };
    void SetSound(bool val);

    bool GetMusic() { return m_Music; };
    void SetMusic(bool val);

    u8 GetSoundVolume() { return m_SoundVolume; };
    void SetSoundVolume(u8 val);

    u8 GetMusicVolume() { return m_MusicVolume; };
    void SetMusicVolume(u8 val);

    u8 GetClientFPS() { return m_ClientFPS; };
    void SetClientFPS(u8 val);

    bool GetUseScaling() { return m_UseScaling; };
    void SetUseScaling(bool val);

    u8 GetDrawStatusState() { return m_DrawStatusState; };
    void SetDrawStatusState(u8 val);

    bool GetDrawStumps() { return m_DrawStumps; };
    void SetDrawStumps(bool val);

    bool GetMarkingCaves() { return m_MarkingCaves; };
    void SetMarkingCaves(bool val);

    bool GetNoVegetation() { return m_NoVegetation; };
    void SetNoVegetation(bool val);

    bool GetNoAnimateFields() { return m_NoAnimateFields; };
    void SetNoAnimateFields(bool val);

    bool GetReduceFPSUnactiveWindow() { return m_ReduceFPSUnactiveWindow; };
    void SetReduceFPSUnactiveWindow(bool val);

    bool GetConsoleNeedEnter() { return m_ConsoleNeedEnter; };
    void SetConsoleNeedEnter(bool val);

    u8 GetSpellIconAlpha() { return m_SpellIconAlpha; };
    void SetSpellIconAlpha(u8 val);

    bool GetOldStyleStatusbar() { return m_OldStyleStatusbar; };
    void SetOldStyleStatusbar(bool val);

    bool GetOriginalPartyStatusbar() { return m_OriginalPartyStatusbar; };
    void SetOriginalPartyStatusbar(bool val);

    bool GetApplyStateColorOnCharacters() { return m_ApplyStateColorOnCharacters; };
    void SetApplyStateColorOnCharacters(bool val);

    bool GetChangeFieldsGraphic() { return m_ChangeFieldsGraphic; };
    void SetChangeFieldsGraphic(bool val);

    // Siebenwind does not use these extra slots.
    bool GetPaperdollSlots() { return false; };
    void SetPaperdollSlots(bool val);

    u8 GetDrawAuraState() { return m_DrawAuraState; };
    void SetDrawAuraState(u8 val);

    bool GetScaleImagesInPaperdollSlots() { return m_ScaleImagesInPaperdollSlots; };
    void SetScaleImagesInPaperdollSlots(bool val);

    bool GetUseGlobalMapLayer() { return m_UseGlobalMapLayer; };
    void SetUseGlobalMapLayer(bool val);

    bool GetNoDrawRoofs() { return m_NoDrawRoofs; };
    void SetNoDrawRoofs(bool val);

    u8 GetPingTimer() { return m_PingTimer; };
    void SetPingTimer(u8 val);

    bool GetItemPropertiesIcon() { return m_ItemPropertiesIcon; };
    void SetItemPropertiesIcon(bool val);

    u8 GetItemPropertiesMode() { return m_ItemPropertiesMode; };
    void SetItemPropertiesMode(u8 val);

    u8 GetCharacterBackpackStyle() { return m_CharacterBackpackStyle; };
    void SetCharacterBackpackStyle(u8 val);

    void Init();

    //!Проставление значений по-умолчанию
    void DefaultPage1();
    void DefaultPage2();
    void DefaultPage3();
    void DefaultPage4();
    //5 нету
    void DefaultPage6();
    void DefaultPage7();
    void DefaultPage8();
    void DefaultPage9();
    //10 нету

    void UpdateFeatures();
    u16 GetColorByNotoriety(u8 a_notoriety);
    bool LoadBin(const std::filesystem::path& a_path);
    int GetConfigKeyCode(const std::string& a_key);

    bool Load(const std::filesystem::path& a_path);
    void Save(const std::filesystem::path& a_path);
};

extern CConfigManager g_ConfigManager;
extern CConfigManager g_OptionsConfig;
