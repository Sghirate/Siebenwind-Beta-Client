// MIT License
// Copyright (C) August 2016 Hotride

#pragma once

#include "IndexObject.h"
#include "GameObjects/GameWorld.h"
#include "plugin/plugininterface.h"

class CUopMappedFile;

class COrion
{
public:
    int TexturesDataCount = 0;
    string m_OverrideServerAddress;
    int m_OverrideServerPort = 0;

private:
    u32 m_CRC_Table[256];

    u8 m_StaticTilesFilterFlags[0x10000];

    std::vector<u16> m_StumpTiles;
    std::vector<u16> m_CaveTiles;

    std::deque<CIndexObjectStatic *> m_StaticAnimList;

    std::deque<CIndexObject *> m_UsedLandList;
    std::deque<CIndexObject *> m_UsedStaticList;
    std::deque<CIndexObject *> m_UsedGumpList;
    std::deque<CIndexObject *> m_UsedTextureList;
    std::deque<CIndexSound *> m_UsedSoundList;
    std::deque<CIndexObject *> m_UsedLightList;

    std::vector<u8> m_AnimData;

    string m_GameServerIP = "";

    void LoadAutoLoginNames();
    void LoadTiledata(int landSize, int staticsSize);
    void LoadIndexFiles();
    void UnloadIndexFiles();
    void InitStaticAnimList();
    u16 CalculateLightColor(u16 id);
    void ProcessStaticAnimList();
    void PatchFiles();
    void IndexReplaces();
    void LoadClientStartupConfig();
    void LoadShaders();
    void CreateAuraTexture();
    void CreateObjectHandlesBackground();
    void ClearUnusedTextures();
    void ReadMulIndexFile(
        size_t indexMaxCount,
        const std::function<CIndexObject *(int index)> &getIdxObj,
        size_t address,
        BASE_IDX_BLOCK *ptr,
        const std::function<BASE_IDX_BLOCK *()> &getNewPtrValue);
    void ReadUOPIndexFile(
        size_t indexMaxCount,
        const std::function<CIndexObject *(int index)> &getIdxObj,
        const char *uopFileName,
        int padding,
        const char *extesion,
        CUopMappedFile &uopFile,
        int startIndex = 0);
    u16 TextToGraphic(const char *text);
    void CheckStaticTileFilterFiles();
    string DecodeArgumentString(const char *text, int length);
    void ParseCommandLine();
    void LoadPlugin(const std::filesystem::path& a_path, const std::string& a_function, int a_flags);
    bool InstallPlugin(PluginEntry *initFunc, int flags);
    void LoadContainerOffsets();

public:
    COrion();
    ~COrion();

    static u64 CreateHash(const char *s);

    std::vector<LAND_TILES> m_LandData;
    std::vector<STATIC_TILES> m_StaticData;

    bool Install();
    void Uninstall();

    void InitScreen(GAME_STATE state);
    void ProcessDelayedClicks();

    void Process(bool rendering = false);
    void LoadStartupConfig(int serial);
    void LoadPluginConfig();
    void LoadLocalConfig(int serial);
    void SaveLocalConfig(int serial);

    CIndexObjectLand m_LandDataIndex[MAX_LAND_DATA_INDEX_COUNT];
    CIndexObjectStatic m_StaticDataIndex[MAX_STATIC_DATA_INDEX_COUNT];
    CIndexGump m_GumpDataIndex[MAX_GUMP_DATA_INDEX_COUNT];
    CIndexObject m_TextureDataIndex[MAX_LAND_TEXTURES_DATA_INDEX_COUNT];
    CIndexSound m_SoundDataIndex[MAX_SOUND_DATA_INDEX_COUNT];
    CIndexMusic m_MP3Data[MAX_MUSIC_DATA_INDEX_COUNT];
    CIndexMulti m_MultiDataIndex[MAX_MULTI_DATA_INDEX_COUNT];
    CIndexLight m_LightDataIndex[MAX_LIGHTS_DATA_INDEX_COUNT];

    u16 m_WinterTile[MAX_LAND_DATA_INDEX_COUNT];

    std::vector<std::pair<u16, u16>> m_IgnoreInFilterTiles;

    bool InTileFilter(u16 graphic);

    static string FixServerName(string name);

    void Connect();
    void Disconnect();
    int Send(u8 *buf, int size);
    int Send(const std::vector<u8> &buf) { return Send((u8 *)&buf[0], int(buf.size())); }
    void ServerSelection(int pos);
    void RelayServer(const char *ip, int port, u8 *gameSeed);
    void CharacterSelection(int pos);
    void LoginComplete(bool reload);
    void ChangeSeason(const SEASON_TYPE &season, int music);

    u16 GetLandSeasonGraphic(u16 graphic);
    u16 GetSeasonGraphic(u16 graphic);
    u16 GetSpringGraphic(u16 graphic);
    u16 GetSummerGraphic(u16 graphic);
    u16 GetFallGraphic(u16 graphic);
    u16 GetWinterGraphic(u16 graphic);
    u16 GetDesolationGraphic(u16 graphic);

    int ValueInt(const VALUE_KEY_INT &key, int value = -1);
    string ValueString(const VALUE_KEY_STRING &key, string value = "");

    void ClearRemovedStaticsTextures();
    void ClearTreesTextures();
    bool IsTreeTile(u16 graphic, int &index);
    void ClearCaveTextures();
    bool IsCaveTile(u16 graphic);
    bool IsVegetation(u16 graphic);
    u64 GetLandFlags(u16 id);
    u64 GetStaticFlags(u16 id);
    u16 GetLightColor(u16 id) { return m_StaticDataIndex[id].LightColor; }
    Core::Vec2<i32> GetStaticArtDimension(u16 id);
    Core::Vec2<i32> GetGumpDimension(u16 id);
    CGLTexture *ExecuteGump(u16 id);
    CGLTexture *ExecuteLandArt(u16 id);
    CGLTexture *ExecuteStaticArt(u16 id);
    CGLTexture *ExecuteStaticArtAnimated(u16 id);
    CGLTexture *ExecuteTexture(u16 id);
    CGLTexture *ExecuteLight(u8 &id);
    std::pair<CGLTexture*, Core::Vec2<i16>> ExecuteCursor(u16 a_id);
    bool ExecuteGumpPart(u16 id, int count);
    bool ExecuteResizepic(u16 id) { return ExecuteGumpPart(id, 9); }
    bool ExecuteButton(u16 id) { return ExecuteGumpPart(id, 3); }
    void DrawGump(u16 id, u16 color, int x, int y, bool partialHue = false);
    void DrawGump(
        u16 id, u16 color, int x, int y, int width, int height, bool partialHue = false);
    void DrawResizepicGump(u16 id, int x, int y, int width, int height);
    void DrawLandTexture(class CLandObject *land, u16 color, int x, int y);
    void DrawLandArt(u16 id, u16 color, int x, int y);
    void DrawStaticArt(u16 id, u16 color, int x, int y, bool selection = false);
    void DrawStaticArtAnimated(u16 id, u16 color, int x, int y, bool selection = false);
    void DrawStaticArtRotated(u16 id, u16 color, int x, int y, float angle);
    void DrawStaticArtAnimatedRotated(u16 id, u16 color, int x, int y, float angle);
    void
    DrawStaticArtTransparent(u16 id, u16 color, int x, int y, bool selection = false);
    void DrawStaticArtAnimatedTransparent(
        u16 id, u16 color, int x, int y, bool selection = false);
    void DrawStaticArtInContainer(
        u16 id, u16 color, int x, int y, bool selection = false, bool onMouse = false);
    void DrawLight(struct LIGHT_DATA &light);
    bool PolygonePixelsInXY(int x, int y, int width, int height);
    bool GumpPixelsInXY(u16 id, int x, int y);
    bool GumpPixelsInXY(u16 id, int x, int y, int width, int height);
    bool ResizepicPixelsInXY(u16 id, int x, int y, int width, int height);
    bool StaticPixelsInXY(u16 id, int x, int y);
    bool StaticPixelsInXYAnimated(u16 id, int x, int y);
    bool StaticPixelsInXYInContainer(u16 id, int x, int y);
    bool LandPixelsInXY(u16 id, int x, int y);
    bool LandTexturePixelsInXY(int x, int y, const SDL_Rect &r);
    void CreateTextMessageF(u8 font, u16 color, const char *format, ...);
    void CreateUnicodeTextMessageF(u8 font, u16 color, const char *format, ...);
    void CreateTextMessage(
        const TEXT_TYPE &type,
        int serial,
        u8 font,
        u16 color,
        const std::string &text,
        class CRenderWorldObject *clientObj = nullptr);
    void CreateUnicodeTextMessage(
        const TEXT_TYPE &type,
        int serial,
        u8 font,
        u16 color,
        const std::wstring &text,
        class CRenderWorldObject *clientObj = nullptr);
    void AddSystemMessage(class CTextData *msg);
    void AddJournalMessage(class CTextData *msg, const std::string &name);
    void ChangeMap(u8 newmap);
    void PickupItem(class CGameItem *obj, int count = 0, bool isGameFigure = false);
    void DropItem(int container, u16 x, u16 y, char z);
    void EquipItem(u32 container = 0);
    void ChangeWarmode(u8 status = 0xFF);
    void Click(u32 serial);
    void DoubleClick(u32 serial);
    void PaperdollReq(u32 serial);
    void Attack(u32 serial);
    void AttackReq(u32 serial);
    void SendASCIIText(const char *str, SPEECH_TYPE type);
    void CastSpell(int index);
    void CastSpellFromBook(int index, u32 serial);
    void UseSkill(int index);
    void OpenDoor();
    void EmoteAction(const char *action);
    void AllNames();
    u32 GetFileHashCode(u8 *ptr, size_t size);
    void LoadLogin(string &login, int &port);
    void GoToWebLink(const std::string &url);
    void RemoveRangedObjects();
    void ClearWorld();
    void LogOut();
    void ConsolePromptSend();
    void ConsolePromptCancel();

    void PlayMusic(int index, bool warmode = false);
    void PlaySoundEffect(u16 id, float volume = -1);
    void PlaySoundEffectAtPosition(u16 id, int x, int y);
    void AdjustSoundEffects(int ticks, float volume = -1);
    void PauseSound() const;
    void ResumeSound() const;

    void OpenStatus(u32 serial);
    void DisplayStatusbarGump(int serial, int x, int y);
    void OpenMinimap();
    void OpenWorldMap();
    void OpenJournal();
    void OpenSkills();
    void OpenBackpack();
    void OpenLogOut();
    void OpenChat();
    void OpenConfiguration();
    void OpenMail();
    void OpenPartyManifest();
    void OpenProfile(u32 serial = 0);
    void DisconnectGump();
    void OpenCombatBookGump();
    void OpenRacialAbilitiesBookGump();
    void StartReconnect();
};

extern COrion g_Orion;
