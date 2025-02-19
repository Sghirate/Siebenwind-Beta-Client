#include "OrionUO.h"

#include <SDL.h>

#include "GitRevision.h"
#include "Config.h"
#include "GameVars.h"
#include "Globals.h"

#include "Core/CommandLine.h"
#include "Core/File.h"
#include "Core/Input.h"
#include "Core/Log.h"
#include "Core/Minimal.h"
#include "Core/MappedFile.h"
#include "Core/Platform.h"
#include "Core/StringUtils.h"
#include "Core/TextFileParser.h"
#include "Core/Time.h"
#include "Core/Vars.h"

#include "GameWindow.h"
#include "Macro.h"
#include "DefinitionMacro.h"
#include "CityList.h"
#include "Target.h"
#include "Weather.h"
#include "TargetGump.h"
#include "StumpsData.h"
#include "Party.h"
#include "ServerList.h"
#include "PressedObject.h"
#include "SelectedObject.h"
#include "ClickObject.h"
#include "UseItemsList.h"
#include "ContainerStack.h"
#include "Container.h"
#include "CharacterList.h"
#include "DateTimeStamp.h"
#include "OrionApplication.h"
#include "SiebenwindClient.h"

#include "Utility/Pinger.h"
#include "Crypt/CryptEntry.h"

#include "Profiler.h"

#include "Walker/Walker.h"
#include "Walker/PathFinder.h"

#include "GameObjects/LandObject.h"
#include "GameObjects/GamePlayer.h"
#include "GameObjects/ObjectOnCursor.h"
#include "GameObjects/MapBlock.h"
#include "GameObjects/GameCharacter.h"

#include "TextEngine/EntryText.h"
#include "TextEngine/GameConsole.h"
#include "TextEngine/Journal.h"
#include "TextEngine/RenderTextObject.h"
#include "TextEngine/TextData.h"
#include "TextEngine/TextRenderer.h"

#include "Network/UOHuffman.h"
#include "Network/Packets.h"
#include "Network/PluginPackets.h"
#include "Network/Connection.h"

#include "Managers/FileManager.h"
#include "Managers/AnimationManager.h"
#include "Managers/CityManager.h"
#include "Managers/ClilocManager.h"
#include "Managers/CustomHousesManager.h"
#include "Managers/IntlocManager.h"
#include "Managers/ObjectPropertiesManager.h"
#include "Managers/ColorManager.h"
#include "Managers/ConfigManager.h"
#include "Managers/ConnectionManager.h"
#include "Managers/CreateCharacterManager.h"
#include "Managers/EffectManager.h"
#include "Managers/FontsManager.h"
#include "Managers/GumpManager.h"
#include "Managers/MacroManager.h"
#include "Managers/MapManager.h"
#include "Managers/MouseManager.h"
#include "Managers/OptionsMacroManager.h"
#include "Managers/PacketManager.h"
#include "Managers/ProfessionManager.h"
#include "Managers/ScreenEffectManager.h"
#include "Managers/SkillGroupManager.h"
#include "Managers/SoundManager.h"
#include "Managers/SpeechManager.h"
#include "Managers/UOFileReader.h"
#include "Managers/CorpseManager.h"
#include "Managers/SkillsManager.h"

#include "ScreenStages/BaseScreen.h"
#include "ScreenStages/CharacterListScreen.h"
#include "ScreenStages/ConnectionScreen.h"
#include "ScreenStages/CreateCharacterScreen.h"
#include "ScreenStages/GameBlockedScreen.h"
#include "ScreenStages/GameScreen.h"
#include "ScreenStages/MainScreen.h"
#include "ScreenStages/SelectProfessionScreen.h"
#include "ScreenStages/SelectTownScreen.h"
#include "ScreenStages/ServerScreen.h"

#include "Gumps/GumpAbility.h"
#include "Gumps/GumpBaseScroll.h"
#include "Gumps/GumpBook.h"
#include "Gumps/GumpBuff.h"
#include "Gumps/GumpBulletinBoard.h"
#include "Gumps/GumpBulletinBoardItem.h"
#include "Gumps/GumpCombatBook.h"
#include "Gumps/GumpConsoleType.h"
#include "Gumps/GumpContainer.h"
#include "Gumps/GumpCustomHouse.h"
#include "Gumps/GumpDrag.h"
#include "Gumps/GumpSelectColor.h"
#include "Gumps/GumpDye.h"
#include "Gumps/GumpGeneric.h"
#include "Gumps/GumpGrayMenu.h"
#include "Gumps/GumpJournal.h"
#include "Gumps/GumpMap.h"
#include "Gumps/GumpMenu.h"
#include "Gumps/GumpMenubar.h"
#include "Gumps/GumpMinimap.h"
#include "Gumps/GumpNotify.h"
#include "Gumps/GumpOptions.h"
#include "Gumps/GumpPaperdoll.h"
#include "Gumps/GumpPartyManifest.h"
#include "Gumps/GumpPopupMenu.h"
#include "Gumps/GumpProfile.h"
#include "Gumps/GumpQuestion.h"
#include "Gumps/GumpRacialAbilitiesBook.h"
#include "Gumps/GumpRacialAbility.h"
#include "Gumps/GumpScreenCharacterList.h"
#include "Gumps/GumpScreenConnection.h"
#include "Gumps/GumpScreenCreateCharacter.h"
#include "Gumps/GumpScreenGame.h"
#include "Gumps/GumpScreenMain.h"
#include "Gumps/GumpScreenSelectProfession.h"
#include "Gumps/GumpScreenSelectTown.h"
#include "Gumps/GumpScreenServer.h"
#include "Gumps/GumpSecureTrading.h"
#include "Gumps/GumpSelectFont.h"
#include "Gumps/GumpShop.h"
#include "Gumps/GumpSkill.h"
#include "Gumps/GumpSkills.h"
#include "Gumps/GumpSpell.h"
#include "Gumps/GumpSpellbook.h"
#include "Gumps/GumpStatusbar.h"
#include "Gumps/GumpTargetSystem.h"
#include "Gumps/GumpTextEntryDialog.h"
#include "Gumps/GumpTip.h"
#include "Gumps/GumpWorldMap.h"
#include "Gumps/GumpProperty.h"
#include "Gumps/GumpPropertyIcon.h"

#include <resources/shaders.hpp>
#include <deque>

namespace
{
enum { kGameServerId = 0xFFFFFFFF };
Core::TimeStamp g_nextGameServerPing = Core::TimeStamp::Now();
PingResult g_gameServerPingInfo;
std::string g_pingString;

static void OnPingResult(const PingResult& a_pingResult)
{
    if (a_pingResult.serverId == kGameServerId)
    {
        memcpy(&g_gameServerPingInfo, &a_pingResult, sizeof(g_gameServerPingInfo));
        char ping[50] = { 0 };
        sprintf_s(
            ping,
            "ping(min:%i max:%i avg:%i lost:%i) ",
            g_gameServerPingInfo.min,
            g_gameServerPingInfo.max,
            g_gameServerPingInfo.avg,
            g_gameServerPingInfo.lost);
        g_pingString = ping;
    }
}
static void TickGameServerPing(const std::string& a_gameServerIp)
{
    if (ping_disable.GetValue() == 0 && g_ConfigManager.CheckPing && g_nextGameServerPing < Core::TimeStamp::Now())
    {
        enum { kGameServerId = 0xFFFFFFFF };
        Pinger::Get().Ping(kGameServerId, a_gameServerIp, 10, OnPingResult);
        g_nextGameServerPing = Core::TimeStamp::Now() + Core::TimeDiff::FromSeconds(g_ConfigManager.GetPingTimer());
    }
}

} // namespace <anonymous>

COrion g_Orion;

COrion::COrion()
{
}

COrion::~COrion()
{
}

u32 Reflect(u32 source, int c)
{
    u32 value = 0;

    for (int i = 1; i < c + 1; i++)
    {
        if ((source & 0x1) != 0u)
        {
            value |= (1 << (c - i));
        }

        source >>= 1;
    }

    return value;
}

u32 COrion::GetFileHashCode(u8* ptr, size_t size)
{
    u32 crc = 0xFFFFFFFF;

    while (size > 0)
    {
        crc = (crc >> 8) ^ m_CRC_Table[(crc & 0xFF) ^ *ptr];

        ptr++;
        size--;
    }

    return (crc & 0xFFFFFFFF);
}

std::string COrion::DecodeArgumentString(const char* text, int length)
{
    std::string result{};

    for (int i = 0; i < length; i += 2)
    {
        char buf[5] = { '0', 'x', text[i], text[i + 1], 0 };

        char* end = nullptr;
        result += (char)strtoul(buf, &end, 16);
    }

    return result;
}

void COrion::ParseCommandLine() // FIXME: move this out
{
    bool fastLogin         = false;
    u32 defaultPluginFlags = 0xFFFFFFFF;

    int argc                                = Core::CommandLine::GetArgc();
    char** argv                             = Core::CommandLine::GetArgv();
    std::filesystem::path defaultPluginPath = g_App.GetExeDir() / "OrionAssistant.dll";
    std::string defaultPluginFunction       = "Install";

    for (int i = 0; i < argc; i++)
    {
        if ((argv[i] == nullptr) || *argv[i] != '-')
        {
            continue;
        }

        std::string str = std::string(argv[i] + 1);
        Core::TextFileParser parser({}, " ,:", "", "''");
        std::vector<std::string> strings = parser.GetTokens(str.c_str());
        if (strings.empty())
        {
            continue;
        }

        str             = Core::ToLowerA(strings[0]);
        bool haveParam  = (strings.size() > 1);
        bool have2Param = (strings.size() > 2);
        if (have2Param)
        {
            if (str == "login")
            {
                m_OverrideServerAddress = strings[1];
                m_OverrideServerPort    = atoi(strings[2].c_str());
            }
            else if (str == "proxyhost")
            {
                g_ConnectionManager.SetUseProxy(true);
                g_ConnectionManager.SetProxyAddress(strings[1]);
                g_ConnectionManager.SetProxyPort(atoi(strings[2].c_str()));
            }
            else if (str == "proxyaccount")
            {
                g_ConnectionManager.SetProxySocks5(true);
                g_ConnectionManager.SetProxyAccount(
                    DecodeArgumentString(strings[1].c_str(), (int)strings[1].length()));
                g_ConnectionManager.SetProxyPassword(
                    DecodeArgumentString(strings[2].c_str(), (int)strings[2].length()));
            }
            else if (str == "account")
            {
                g_MainScreen.SetAccounting(
                    DecodeArgumentString(strings[1].c_str(), (int)strings[1].length()),
                    DecodeArgumentString(strings[2].c_str(), (int)strings[2].length()));
            }
#if defined(ORION_WINDOWS)
            else if (str == "plugin")
            {
                strings = Core::TextFileParser({}, ",:", "", "").GetTokens(argv[i] + 1, false);
                if (strings.size() > 4)
                {
                    defaultPluginFlags = 0;
                    if (Core::ToLowerA(strings[4]).find("0x") == 0)
                    {
                        char* end          = nullptr;
                        defaultPluginFlags = strtoul(strings[4].c_str(), &end, 16);
                    }
                    else
                    {
                        defaultPluginFlags = atoi(strings[4].c_str());
                    }
                    defaultPluginPath     = strings[1] + ":" + strings[2];
                    defaultPluginFunction = strings[3];
                }
            }
#endif
        }
        else if (str == "autologin")
        {
            bool enabled = true;
            if (haveParam)
            {
                enabled = (atoi(strings[1].c_str()) != 0);
            }
            g_MainScreen.m_AutoLogin->Checked = enabled;
        }
        else if (str == "savepassword")
        {
            bool enabled = true;
            if (haveParam)
            {
                enabled = (atoi(strings[1].c_str()) != 0);
            }
            g_MainScreen.m_SavePassword->Checked = enabled;
        }
        else if (str == "fastlogin")
        {
            fastLogin = true;
        }
        else if (str == "autologinname")
        {
            if (g_PacketManager.AutoLoginNames.length() != 0u)
            {
                g_PacketManager.AutoLoginNames =
                    std::string("|") +
                    DecodeArgumentString(strings[1].c_str(), (int)strings[1].length()) +
                    g_PacketManager.AutoLoginNames;
            }
            else
            {
                g_PacketManager.AutoLoginNames =
                    std::string("|") +
                    DecodeArgumentString(strings[1].c_str(), (int)strings[1].length());
            }
        }
        else if (str == "nowarnings")
        {
            g_ShowWarnings = false;
        }
        else if (str == "nocrypt")
        {
            g_Config.EncryptionType = ET_NOCRYPT;
        }
    }
}

bool COrion::Install()
{
    LOG_INFO("Client", "COrion::Install()");
    auto buildStamp = GetBuildDateTimeStamp();
    LOG_INFO(
        "Client", "Orion version is: %s (build %s)", RC_PRODUCE_VERSION_STR, buildStamp.c_str());
    //CRASHLOG("Orion version is: %s (build %s)\n", RC_PRODUCE_VERSION_STR, buildStamp.c_str());

    for (int i = 0; i < 256; i++)
    {
        m_CRC_Table[i] = Reflect((int)i, 8) << 24;
        for (int j = 0; j < 8; j++)
        {
            m_CRC_Table[i] =
                (m_CRC_Table[i] << 1) ^ ((m_CRC_Table[i] & (1 << 31)) != 0u ? 0x04C11DB7 : 0);
        }
        m_CRC_Table[i] = Reflect(m_CRC_Table[i], 32);
    }

    for (int i = 0; i < MAX_MAPS_COUNT; i++)
    {
        g_MapBlockSize[i].x = g_MapSize[i].x / 8;
        g_MapBlockSize[i].y = g_MapSize[i].y / 8;
    }

    Platform::SetLanguageFromSystemLocale();
    std::filesystem::create_directory(g_App.GetExeDir() / "screenshots");

    LOG_INFO("Client", "Client config loaded!");
    if (GameVars::GetClientVersion() >= CV_305D)
    {
        CGumpSpellbook::m_SpellReagents1[4]  = "Sulfurous ash";                //Magic Arrow
        CGumpSpellbook::m_SpellReagents1[17] = "Black pearl";                  //Fireball
        CGumpSpellbook::m_SpellReagents1[29] = "Mandrake root\nSulfurous ash"; //Lightning
        CGumpSpellbook::m_SpellReagents1[42] = "Bloodmoss\nMandrake root";     //Explosion
    }

    LoadAutoLoginNames();

    LOG_INFO("Client", "Load files");

    if (GameVars::GetClientVersion() >= CV_7000)
    {
        g_FileManager.TryReadUOPAnimations();
    }

    if (!g_FileManager.Load())
    {
        LOG_ERROR("Game", "Error loading a memmapped file. Please check the log for more info.\nUsing UO search path: %s", g_App.GetGameDir().c_str());
        return false;
    }

    g_SpeechManager.LoadSpeech();

    CGumpSpellbook::InitStaticData();

    m_AnimData.resize(g_FileManager.m_AnimdataMul.GetSize());
    memcpy(
        &m_AnimData[0],
        g_FileManager.m_AnimdataMul.GetBuffer(),
        g_FileManager.m_AnimdataMul.GetSize());

    g_ColorManager.Init();

    LOG_INFO("Client", "Load tiledata");

    int staticsCount = 512;

    if (GameVars::GetClientVersion() >= CV_7090)
        staticsCount =
            (int)(g_FileManager.m_TiledataMul.GetSize() - (512 * sizeof(LAND_GROUP_NEW))) /
            sizeof(STATIC_GROUP_NEW);
    else
        staticsCount =
            (int)(g_FileManager.m_TiledataMul.GetSize() - (512 * sizeof(LAND_GROUP_OLD))) /
            sizeof(STATIC_GROUP_OLD);

    if (staticsCount > 2048)
        staticsCount = 2048;

    LOG_INFO("Client", "staticsCount=%i", staticsCount);
    LoadTiledata(512, staticsCount);
    LOG_INFO("Client", "Load indexes");
    LoadIndexFiles();
    InitStaticAnimList();

    LOG_INFO("Client", "Load fonts.");
    if (!g_FontManager.LoadFonts())
    {
        LOG_ERROR("Client", "Error loading fonts");

        return false;
    }

    LOG_INFO("Client", "Load skills.");
    if (!g_SkillsManager.Load())
    {
        LOG_ERROR("Client", "Error loading skills");
        return false;
    }

    LOG_INFO("Client", "Create map blocksTable");
    g_MapManager.CreateBlocksTable();

    LOG_INFO("Client", "Patch files");
    PatchFiles();
    LOG_INFO("Client", "Replaces...");
    IndexReplaces();

    CheckStaticTileFilterFiles();

    Core::Vec2<i32> statusbarDims = GetGumpDimension(0x0804);

    CGumpStatusbar::m_StatusbarDefaultWidth  = statusbarDims.x;
    CGumpStatusbar::m_StatusbarDefaultHeight = statusbarDims.y;

    LOG_INFO("Client", "Sort skills...");
    g_SkillsManager.Sort();

    LOG_INFO("Client", "Load cursors.");
    if (!g_MouseManager.LoadCursorTextures())
    {
        LOG_ERROR("Client", "Error loading cursors");
        return false;
    }

    if (core_no_sound.GetValue() == 0) {
        if (!g_SoundManager.Init() && g_ShowWarnings)
        {
            LOG_WARNING("Client", "Failed to init audio system");
        }
    }

    LoadContainerOffsets();

    g_CityManager.Init();

    g_EntryPointer = nullptr;

    LOG_INFO("Client", "Load prof.");
    g_ProfessionManager.Load();
    g_ProfessionManager.Selected = (CBaseProfession*)g_ProfessionManager.m_Items;

    ExecuteStaticArt(0x0EED); //gp 1
    ExecuteStaticArt(0x0EEE); //gp 2-5
    ExecuteStaticArt(0x0EEF); //gp 6+

    g_CreateCharacterManager.Init();

    for (int i = 0; i < 6; i++)
    {
        g_AnimationManager.Init(
            (int)i,
            (size_t)g_FileManager.m_AnimIdx[i].GetBuffer(),
            (size_t)g_FileManager.m_AnimIdx[i].GetSize());
    }
    g_AnimationManager.InitIndexReplaces((u32*)g_FileManager.m_VerdataMul.GetBuffer());

    LOG_INFO("Client", "Load client startup.");
    LoadClientStartupConfig();

    u16 b = 0x0000;
    u16 r = 0xFC00; // 0xFF0000FF;
    u16 g = 0x83E0; // 0xFF00FF00;

    u16 pdwlt[2][140] = {
        { b, b, b, g, g, g, g, g, g, g, b, b, b, g, g, g, g, g, g, g, b, b, b, b, g, g, b, b,
          b, b, b, b, b, b, g, g, b, b, b, b, b, b, b, b, g, g, b, b, b, b, g, g, g, g, g, g,
          g, g, g, b, g, g, g, g, g, g, g, g, g, b, g, g, b, b, b, b, b, g, g, b, g, g, b, g,
          g, b, b, g, g, b, g, g, b, b, g, b, b, g, g, b, g, g, b, b, g, b, b, g, g, b, g, g,
          g, b, b, b, g, g, g, b, b, g, g, g, g, g, g, g, b, b, b, b, g, g, g, g, g, b, b, b },
        { b, r, r, r, r, r, r, r, b, b, b, r, r, r, r, r, r, r, b, b, b, b, r, r, b, r, r, b,
          b, b, b, b, r, r, b, r, r, b, b, b, b, b, r, r, b, r, r, b, b, b, r, r, r, r, r, r,
          r, r, r, b, r, r, r, r, r, r, r, r, r, b, r, r, b, b, b, b, b, r, r, b, r, r, b, r,
          r, b, b, r, r, b, r, r, b, b, r, b, b, r, r, b, r, r, b, b, r, b, b, r, r, b, r, r,
          r, b, b, b, r, r, r, b, b, r, r, r, r, r, r, r, b, b, b, b, r, r, r, r, r, b, b, b }
    };

    for (int i = 0; i < 2; i++)
    {
        g_GL_BindTexture16(g_TextureGumpState[i], 10, 14, &pdwlt[i][0]);
    }

    memset(&m_WinterTile[0], 0, sizeof(m_WinterTile));

    m_WinterTile[196]  = 0x011A;
    m_WinterTile[197]  = 0x011B;
    m_WinterTile[198]  = 0x011C;
    m_WinterTile[199]  = 0x011D;
    m_WinterTile[206]  = 0x05C0;
    m_WinterTile[248]  = 0x011A;
    m_WinterTile[249]  = 0x011B;
    m_WinterTile[250]  = 0x011C;
    m_WinterTile[251]  = 0x011D;
    m_WinterTile[804]  = 0x0391;
    m_WinterTile[805]  = 0x0392;
    m_WinterTile[806]  = 0x0393;
    m_WinterTile[807]  = 0x0394;
    m_WinterTile[808]  = 0x0395;
    m_WinterTile[809]  = 0x0396;
    m_WinterTile[1521] = 0x011A;
    m_WinterTile[1522] = 0x011B;
    m_WinterTile[1523] = 0x011C;
    m_WinterTile[1524] = 0x011D;
    m_WinterTile[1529] = 0x011A;
    m_WinterTile[1530] = 0x011B;
    m_WinterTile[1531] = 0x011C;
    m_WinterTile[1532] = 0x011D;
    m_WinterTile[1533] = 0x011B;
    m_WinterTile[1534] = 0x011C;
    m_WinterTile[1535] = 0x011D;
    m_WinterTile[1536] = 0x011B;
    m_WinterTile[1537] = 0x011C;
    m_WinterTile[1538] = 0x011D;
    m_WinterTile[1539] = 0x011C;
    m_WinterTile[1540] = 0x011D;

    LOG_INFO("Client", "Init light buffer.");
    g_LightBuffer.Init(640, 480);

    LOG_INFO("Client", "Create object handles.");
    CreateObjectHandlesBackground();

    LOG_INFO("Client", "Create aura.");
    CreateAuraTexture();

    LOG_INFO("Client", "Load shaders.");
    LoadShaders();

    LOG_INFO("Client", "Update main screen content");
    g_MainScreen.UpdateContent();

    LOG_INFO("Client", "Init screen...");

    InitScreen(GS_MAIN);

    if (GameVars::GetClientVersion() >= CV_7000)
    {
        LOG_INFO("Client", "Waiting for FileManager to try & load AnimationFrame files");
        g_FileManager.m_AutoResetEvent.WaitOne();
        LOG_INFO("Client", "FileManager.TryReadUOPAnimations() done!");
    }

    LOG_INFO("Client", "Installation completed!");

    return true;
}

void COrion::Uninstall()
{
    LOG_INFO("Client", "COrion::Uninstall()");
    SaveLocalConfig(g_PacketManager.ConfigSerial);
    g_MainScreen.Save();
    SaveGlobalConfig();
    g_GumpManager.OnDelete();

    Disconnect();

    UnloadIndexFiles();

    g_EntryPointer  = nullptr;
    g_CurrentScreen = nullptr;

    g_AuraTexture.Clear();

    for (int i = 0; i < MAX_MAPS_COUNT; i++)
    {
        g_MapTexture[i].Clear();
    }

    for (int i = 0; i < 2; i++)
    {
        g_TextureGumpState[i].Clear();
    }

    g_SoundManager.Free();
    g_FileManager.Unload();
    g_LightBuffer.Free();

    g_GL.Uninstall();
}

void COrion::InitScreen(GAME_STATE state)
{
    g_GameState = state;
    g_SelectedObject.Clear();
    g_LastSelectedObject.Clear();
    g_PressedObject.ClearAll();

    switch (state)
    {
        case GS_MAIN:
        {
            g_CurrentScreen = &g_MainScreen;
            break;
        }
        case GS_MAIN_CONNECT:
        {
            g_CurrentScreen = &g_ConnectionScreen;
            break;
        }
        case GS_SERVER:
        {
            g_CurrentScreen = &g_ServerScreen;
            break;
        }
        case GS_SERVER_CONNECT:
        {
            g_CurrentScreen = &g_ConnectionScreen;
            break;
        }
        case GS_CHARACTER:
        {
            g_CurrentScreen = &g_CharacterListScreen;
            break;
        }
        case GS_DELETE:
        {
            g_CurrentScreen = &g_ConnectionScreen;
            break;
        }
        case GS_PROFESSION_SELECT:
        {
            g_CurrentScreen = &g_SelectProfessionScreen;
            break;
        }
        case GS_CREATE:
        {
            g_CurrentScreen = &g_CreateCharacterScreen;
            break;
        }
        case GS_SELECT_TOWN:
        {
            g_CurrentScreen = &g_SelectTownScreen;
            break;
        }
        case GS_GAME_CONNECT:
        {
            g_CurrentScreen = &g_ConnectionScreen;
            break;
        }
        case GS_GAME:
        {
            g_CurrentScreen = &g_GameScreen;
            break;
        }
        case GS_GAME_BLOCKED:
        {
            g_CurrentScreen = &g_GameBlockedScreen;
            break;
        }
        default: break;
    }

    if (g_CurrentScreen != nullptr)
    {
        g_CurrentScreen->Init();
    }
}

u16 COrion::TextToGraphic(const char* text)
{
    if (strlen(text) > 2 && text[0] == '0' && (text[1] == 'x' || text[1] == 'X'))
    {
        long l = strtol(text + 2, nullptr, 16);

        if (l < 0 || l > 0xFFFF)
        {
            return 0xFFFF;
        }

        return (u16)l;
    }
    if (text[0] >= '0' && text[0] <= '9')
    {
        return atoi(text);
    }

    return 0;
}

void COrion::CheckStaticTileFilterFiles()
{
    memset(&m_StaticTilesFilterFlags[0], 0, sizeof(m_StaticTilesFilterFlags));

    std::filesystem::path dataPath = g_App.GetExeDir() / "OrionData";
    std::filesystem::create_directory(dataPath);

    std::filesystem::path filePath = dataPath / "cave.txt";
    if (!std::filesystem::exists(filePath))
    {
        Core::File file(filePath, "w");
        file.Print("#Format: graphic");
        for (int i = 0x053B; i < 0x0553 + 1; i++)
        {
            if (i != 0x0550)
            {
                file.Print("0x%04X\n", i);
            }
        }
    }

    filePath = dataPath / "vegetation.txt";
    Core::File vegetationFile;
    if (!std::filesystem::exists(filePath))
    {
        vegetationFile.Open(filePath, "w");
        vegetationFile.Print("#Format: graphic");

        static const int vegetationTilesCount = 178;

        static const u16 vegetationTiles[vegetationTilesCount] = {
            0x0D45, 0x0D46, 0x0D47, 0x0D48, 0x0D49, 0x0D4A, 0x0D4B, 0x0D4C, 0x0D4D, 0x0D4E, 0x0D4F,
            0x0D50, 0x0D51, 0x0D52, 0x0D53, 0x0D54, 0x0D5C, 0x0D5D, 0x0D5E, 0x0D5F, 0x0D60, 0x0D61,
            0x0D62, 0x0D63, 0x0D64, 0x0D65, 0x0D66, 0x0D67, 0x0D68, 0x0D69, 0x0D6D, 0x0D73, 0x0D74,
            0x0D75, 0x0D76, 0x0D77, 0x0D78, 0x0D79, 0x0D7A, 0x0D7B, 0x0D7C, 0x0D7D, 0x0D7E, 0x0D7F,
            0x0D80, 0x0D83, 0x0D87, 0x0D88, 0x0D89, 0x0D8A, 0x0D8B, 0x0D8C, 0x0D8D, 0x0D8E, 0x0D8F,
            0x0D90, 0x0D91, 0x0D93, 0x12B6, 0x12B7, 0x12BC, 0x12BD, 0x12BE, 0x12BF, 0x12C0, 0x12C1,
            0x12C2, 0x12C3, 0x12C4, 0x12C5, 0x12C6, 0x12C7, 0x0CB9, 0x0CBC, 0x0CBD, 0x0CBE, 0x0CBF,
            0x0CC0, 0x0CC1, 0x0CC3, 0x0CC5, 0x0CC6, 0x0CC7, 0x0CF3, 0x0CF4, 0x0CF5, 0x0CF6, 0x0CF7,
            0x0D04, 0x0D06, 0x0D07, 0x0D08, 0x0D09, 0x0D0A, 0x0D0B, 0x0D0C, 0x0D0D, 0x0D0E, 0x0D0F,
            0x0D10, 0x0D11, 0x0D12, 0x0D13, 0x0D14, 0x0D15, 0x0D16, 0x0D17, 0x0D18, 0x0D19, 0x0D28,
            0x0D29, 0x0D2A, 0x0D2B, 0x0D2D, 0x0D34, 0x0D36, 0x0DAE, 0x0DAF, 0x0DBA, 0x0DBB, 0x0DBC,
            0x0DBD, 0x0DBE, 0x0DC1, 0x0DC2, 0x0DC3, 0x0C83, 0x0C84, 0x0C85, 0x0C86, 0x0C87, 0x0C88,
            0x0C89, 0x0C8A, 0x0C8B, 0x0C8C, 0x0C8D, 0x0C8E, 0x0C93, 0x0C94, 0x0C98, 0x0C9F, 0x0CA0,
            0x0CA1, 0x0CA2, 0x0CA3, 0x0CA4, 0x0CA7, 0x0CAC, 0x0CAD, 0x0CAE, 0x0CAF, 0x0CB0, 0x0CB1,
            0x0CB2, 0x0CB3, 0x0CB4, 0x0CB5, 0x0CB6, 0x0C45, 0x0C46, 0x0C49, 0x0C47, 0x0C48, 0x0C4A,
            0x0C4B, 0x0C4C, 0x0C4D, 0x0C4E, 0x0C37, 0x0C38, 0x0CBA, 0x0D2F, 0x0D32, 0x0D33, 0x0D3F,
            0x0D40, 0x0CE9
        };

        for (int i = 0; i < vegetationTilesCount; i++)
        {
            i64 flags = g_Orion.GetStaticFlags(vegetationTiles[i]);
            if ((flags & 0x00000040) != 0)
                continue;

            vegetationFile.Print("0x%04X\n", vegetationTiles[i]);
        }
    }
    else
    {
        vegetationFile.Open(filePath, "a");
    }

    filePath = dataPath / "stumps.txt";
    if (!std::filesystem::exists(filePath))
    {
        Core::File file(filePath, "w");
        file.Print("#Format: graphic hatched");

        static const int treeTilesCount = 53;

        static const u16 treeTiles[treeTilesCount] = {
            0x0CCA, 0x0CCB, 0x0CCC, 0x0CCD, 0x0CD0, 0x0CD3, 0x0CD6, 0x0CD8, 0x0CDA, 0x0CDD, 0x0CE0,
            0x0CE3, 0x0CE6, 0x0D41, 0x0D42, 0x0D43, 0x0D44, 0x0D57, 0x0D58, 0x0D59, 0x0D5A, 0x0D5B,
            0x0D6E, 0x0D6F, 0x0D70, 0x0D71, 0x0D72, 0x0D84, 0x0D85, 0x0D86, 0x0D94, 0x0D98, 0x0D9C,
            0x0DA0, 0x0DA4, 0x0DA8, 0x0C9E, 0x0CA8, 0x0CAA, 0x0CAB, 0x0CC9, 0x0CF8, 0x0CFB, 0x0CFE,
            0x0D01, 0x12B6, 0x12B7, 0x12B8, 0x12B9, 0x12BA, 0x12BB, 0x12BC, 0x12BD
        };

        for (int i = 0; i < treeTilesCount; i++)
        {
            u16 graphic = treeTiles[i];
            u8 hatched  = 1;

            switch (graphic)
            {
                case 0x0C9E:
                case 0x0CA8:
                case 0x0CAA:
                case 0x0CAB:
                case 0x0CC9:
                case 0x0CF8:
                case 0x0CFB:
                case 0x0CFE:
                case 0x0D01:
                case 0x12B6:
                case 0x12B7:
                case 0x12B8:
                case 0x12B9:
                case 0x12BA:
                case 0x12BB: hatched = 0;
                default: break;
            }

            i64 flags = g_Orion.GetStaticFlags(graphic);

            if ((flags & 0x00000040) == 0)
                vegetationFile.Print("0x%04X\n", graphic);
            else
                file.Print("0x%04X\t%i\n", graphic, hatched);
        }
    }

    filePath = dataPath / "cave.txt";
    {
        Core::TextFileParser parser(filePath, " \t", "#;//", "");
        while (!parser.IsEOF())
        {
            std::vector<std::string> strings = parser.ReadTokens();
            if (!strings.empty())
            {
                u16 graphic = TextToGraphic(strings[0].c_str());
                m_StaticTilesFilterFlags[graphic] |= STFF_CAVE;
                m_CaveTiles.push_back(graphic);
            }
        }
    }

    filePath = dataPath / "stumps.txt";
    {
        Core::TextFileParser parser(filePath, " \t", "#;//", "");
        while (!parser.IsEOF())
        {
            std::vector<std::string> strings = parser.ReadTokens();
            if (strings.size() >= 2)
            {
                u8 flag = STFF_STUMP;
                if (atoi(strings[1].c_str()) != 0)
                    flag |= STFF_STUMP_HATCHED;
                u16 graphic = TextToGraphic(strings[0].c_str());
                m_StaticTilesFilterFlags[graphic] |= flag;
                m_StumpTiles.push_back(graphic);
            }
        }
    }

    filePath = dataPath / "vegetation.txt";
    {
        Core::TextFileParser parser(filePath, " \t", "#;//", "");
        while (!parser.IsEOF())
        {
            std::vector<std::string> strings = parser.ReadTokens();
            if (!strings.empty())
                m_StaticTilesFilterFlags[TextToGraphic(strings[0].c_str())] |= STFF_VEGETATION;
        }
    }
}

void COrion::LoadContainerOffsets()
{
    std::filesystem::path path = g_App.GetExeDir() / "OrionData";
    std::filesystem::create_directory(path);

    std::filesystem::path filePath = path / "containers.txt";
    LOG_INFO("Client", "Containers: %s", filePath.string().c_str());
    if (!std::filesystem::exists(filePath))
    {
        //												Gump   OpenSnd  CloseSnd					minX minY maxX maxY
        g_ContainerOffset.push_back(CContainerOffset(
            0x0009, 0x0000, 0x0000, CContainerOffsetRect(20, 85, 124, 196))); //corpse
        g_ContainerOffset.push_back(
            CContainerOffset(0x003C, 0x0048, 0x0058, CContainerOffsetRect(44, 65, 186, 159)));
        g_ContainerOffset.push_back(
            CContainerOffset(0x003D, 0x0048, 0x0058, CContainerOffsetRect(29, 34, 137, 128)));
        g_ContainerOffset.push_back(
            CContainerOffset(0x003E, 0x002F, 0x002E, CContainerOffsetRect(33, 36, 142, 148)));
        g_ContainerOffset.push_back(
            CContainerOffset(0x003F, 0x004F, 0x0058, CContainerOffsetRect(19, 47, 182, 123)));
        g_ContainerOffset.push_back(
            CContainerOffset(0x0040, 0x002D, 0x002C, CContainerOffsetRect(16, 51, 150, 140)));
        g_ContainerOffset.push_back(
            CContainerOffset(0x0041, 0x004F, 0x0058, CContainerOffsetRect(35, 38, 145, 116)));
        g_ContainerOffset.push_back(
            CContainerOffset(0x0042, 0x002D, 0x002C, CContainerOffsetRect(18, 105, 162, 178)));
        g_ContainerOffset.push_back(
            CContainerOffset(0x0043, 0x002D, 0x002C, CContainerOffsetRect(16, 51, 181, 124)));
        g_ContainerOffset.push_back(
            CContainerOffset(0x0044, 0x002D, 0x002C, CContainerOffsetRect(20, 10, 170, 100)));
        g_ContainerOffset.push_back(
            CContainerOffset(0x0048, 0x002F, 0x002E, CContainerOffsetRect(16, 10, 154, 94)));
        g_ContainerOffset.push_back(
            CContainerOffset(0x0049, 0x002D, 0x002C, CContainerOffsetRect(18, 105, 162, 178)));
        g_ContainerOffset.push_back(
            CContainerOffset(0x004A, 0x002D, 0x002C, CContainerOffsetRect(18, 105, 162, 178)));
        g_ContainerOffset.push_back(
            CContainerOffset(0x004B, 0x002D, 0x002C, CContainerOffsetRect(16, 51, 184, 124)));
        g_ContainerOffset.push_back(
            CContainerOffset(0x004C, 0x002D, 0x002C, CContainerOffsetRect(46, 74, 196, 184)));
        g_ContainerOffset.push_back(
            CContainerOffset(0x004D, 0x002F, 0x002E, CContainerOffsetRect(76, 12, 140, 68)));
        g_ContainerOffset.push_back(CContainerOffset(
            0x004E, 0x002D, 0x002C, CContainerOffsetRect(24, 96, 140, 152))); //bugged
        g_ContainerOffset.push_back(CContainerOffset(
            0x004F, 0x002D, 0x002C, CContainerOffsetRect(24, 96, 140, 152))); //bugged
        g_ContainerOffset.push_back(
            CContainerOffset(0x0051, 0x002F, 0x002E, CContainerOffsetRect(16, 10, 154, 94)));
        g_ContainerOffset.push_back(CContainerOffset(
            0x091A, 0x0000, 0x0000, CContainerOffsetRect(1, 13, 260, 199))); //game board
        g_ContainerOffset.push_back(CContainerOffset(
            0x092E, 0x0000, 0x0000, CContainerOffsetRect(1, 13, 260, 199))); //backgammon game
        g_ContainerOffset.push_back(
            CContainerOffset(0x0104, 0x002F, 0x002E, CContainerOffsetRect(0, 20, 168, 115)));
        g_ContainerOffset.push_back(
            CContainerOffset(0x0105, 0x002F, 0x002E, CContainerOffsetRect(0, 20, 168, 115)));
        g_ContainerOffset.push_back(
            CContainerOffset(0x0106, 0x002F, 0x002E, CContainerOffsetRect(0, 20, 168, 115)));
        g_ContainerOffset.push_back(
            CContainerOffset(0x0107, 0x002F, 0x002E, CContainerOffsetRect(0, 20, 168, 115)));
        g_ContainerOffset.push_back(
            CContainerOffset(0x0108, 0x004F, 0x0058, CContainerOffsetRect(0, 35, 150, 105)));
        g_ContainerOffset.push_back(
            CContainerOffset(0x0109, 0x002F, 0x002E, CContainerOffsetRect(0, 20, 175, 105)));
        g_ContainerOffset.push_back(
            CContainerOffset(0x010A, 0x002F, 0x002E, CContainerOffsetRect(0, 20, 175, 105)));
        g_ContainerOffset.push_back(
            CContainerOffset(0x010B, 0x002F, 0x002E, CContainerOffsetRect(0, 20, 175, 105)));
        g_ContainerOffset.push_back(
            CContainerOffset(0x010C, 0x002F, 0x002E, CContainerOffsetRect(0, 20, 168, 115)));
        g_ContainerOffset.push_back(
            CContainerOffset(0x010D, 0x002F, 0x002E, CContainerOffsetRect(0, 20, 168, 115)));
        g_ContainerOffset.push_back(
            CContainerOffset(0x010E, 0x002F, 0x002E, CContainerOffsetRect(0, 20, 168, 115)));
        //present boxes
        g_ContainerOffset.push_back(
            CContainerOffset(0x0102, 0x004F, 0x0058, CContainerOffsetRect(15, 10, 245, 120)));
        g_ContainerOffset.push_back(
            CContainerOffset(0x011B, 0x004F, 0x0058, CContainerOffsetRect(15, 10, 220, 120)));
        g_ContainerOffset.push_back(
            CContainerOffset(0x011C, 0x004F, 0x0058, CContainerOffsetRect(10, 10, 220, 145)));
        g_ContainerOffset.push_back(
            CContainerOffset(0x011D, 0x004F, 0x0058, CContainerOffsetRect(10, 10, 220, 130)));
        g_ContainerOffset.push_back(
            CContainerOffset(0x011E, 0x004F, 0x0058, CContainerOffsetRect(15, 10, 290, 130)));
        g_ContainerOffset.push_back(
            CContainerOffset(0x011F, 0x004F, 0x0058, CContainerOffsetRect(15, 10, 220, 120)));
        //secret chest
        g_ContainerOffset.push_back(
            CContainerOffset(0x058E, 0x002D, 0x002C, CContainerOffsetRect(16, 51, 184, 124)));
    }
    else
    {
        Core::TextFileParser parser(filePath, " \t", "#;//", "");
        while (!parser.IsEOF())
        {
            std::vector<std::string> strings = parser.ReadTokens();
            if (strings.size() >= 7)
            {
                u16 gump       = TextToGraphic(strings[0].c_str());
                u16 openSound  = TextToGraphic(strings[1].c_str());
                u16 closeSound = TextToGraphic(strings[2].c_str());
                u16 minX       = atoi(strings[3].c_str());
                u16 minY       = atoi(strings[4].c_str());
                u16 maxX       = atoi(strings[5].c_str());
                u16 maxY       = atoi(strings[6].c_str());
                g_ContainerOffset.push_back(CContainerOffset(
                    gump, openSound, closeSound, CContainerOffsetRect(minX, minY, maxX, maxY)));
            }
        }
    }

    if (!std::filesystem::exists(filePath))
    {
        Core::File file(filePath, "w");
        file.Print("#Format: gump open_sound close_sound minX minY maxX maxY");
        for (const CContainerOffset& item : g_ContainerOffset)
        {
            file.Print(
                "0x%04X 0x%04X 0x%04X %i %i %i %i\n",
                item.Gump,
                item.OpenSound,
                item.CloseSound,
                item.Rect.MinX,
                item.Rect.MinY,
                item.Rect.MaxX,
                item.Rect.MaxY);
        }
    }

    LOG_INFO("Client", "g_ContainerOffset.size()=%zd\n", g_ContainerOffset.size());
}

void COrion::LoadAutoLoginNames()
{
    Core::TextFileParser file(g_App.GetGameDir() / "AutoLoginNames.cfg", "", "#;", "");
    std::string names = g_PacketManager.AutoLoginNames + "|";
    while (!file.IsEOF())
    {
        std::vector<std::string> strings = file.ReadTokens(false);
        if (static_cast<unsigned int>(!strings.empty()) != 0u)
            names += strings[0] + "|";
    }
    g_PacketManager.AutoLoginNames = names;
}

void COrion::ProcessDelayedClicks()
{
    PROFILER_EVENT();
    if (g_ClickObject.Enabled && g_ClickObject.Timer < g_Ticks)
    {
        u32 serial = 0;

        if (g_ClickObject.Object != nullptr)
        {
            serial = g_ClickObject.Object->Serial;
        }

        if (g_ClickObject.Gump == nullptr)
        {
            if (serial != 0u)
            {
                CGameObject* go = (CGameObject*)g_ClickObject.Object;

                if (!g_TooltipsEnabled || (!go->NPC && go->Locked()))
                {
                    Click(serial);
                }

                g_ObjectPropertiesManager.OnItemClicked(serial);

                if (g_PopupEnabled && (!g_ConfigManager.HoldShiftForContextMenus || g_ShiftPressed))
                {
                    CPacketRequestPopupMenu(serial).Send();
                }
            }
        }
        else
        {
            g_ClickObject.Gump->DelayedClick(g_ClickObject.Object);
        }

        g_ClickObject.Clear();
    }
}

void COrion::Process(bool rendering)
{
    PROFILER_EVENT();
    if (g_CurrentScreen == nullptr)
    {
        return;
    }

    static u32 removeUnusedTexturesTime          = 0;
    static u32 removeUnusedAnimationTexturesTime = 0;

    if (g_GameState >= GS_CHARACTER && (g_LastSendTime + SEND_TIMEOUT_DELAY) < g_Ticks)
    {
        g_LastSendTime = g_Ticks;
        u8 ping[2]     = { 0x73, 0 };
        Send(ping, 2);
    }

    const bool oldCtrl  = g_CtrlPressed;
    const bool oldShift = g_ShiftPressed;

    auto mod       = SDL_GetModState();
    g_AltPressed   = ((mod & KMOD_ALT) != 0);
    g_CtrlPressed  = ((mod & KMOD_CTRL) != 0);
    g_ShiftPressed = ((mod & KMOD_SHIFT) != 0);

    if (g_GameState >= GS_GAME) // || g_GameState == GS_GAME_BLOCKED)
    {
        if (g_LogoutAfterClick)
        {
            g_LogoutAfterClick = false;
            LogOut();
            return;
        }

        TickGameServerPing(m_GameServerIP);

        g_UseItemActions.Process();
        g_ShowGumpLocker = g_ConfigManager.LockGumpsMoving && g_AltPressed && g_CtrlPressed;
        ProcessStaticAnimList();

        g_EffectManager.UpdateEffects();
        CGumpBuff* gumpBuff = (CGumpBuff*)g_GumpManager.GetGump(0, 0, GT_BUFF);
        if (gumpBuff != nullptr)
        {
            gumpBuff->UpdateBuffIcons();
        }

        if (g_World != nullptr)
        {
            g_World->ProcessAnimation();
        }

        g_PathFinder.ProcessAutowalk();
        bool canRenderSelect = false;
        if (g_GameState == GS_GAME)
        {
            g_MouseManager.ProcessWalking();
            g_MacroManager.Execute();
            ProcessDelayedClicks();
            canRenderSelect = true;
            if (g_PressedObject.LeftGump == nullptr && g_PressedObject.LeftObject != nullptr &&
                g_PressedObject.LeftObject->IsGUI())
            {
                canRenderSelect = false;
            }
        }

        if (g_World != nullptr)
        {
            if (g_World->ObjectToRemove != 0)
            {
                CGameObject* removeObj  = g_World->FindWorldObject(g_World->ObjectToRemove);
                g_World->ObjectToRemove = 0;
                if (removeObj != nullptr)
                {
                    CGameCharacter* character = g_World->FindWorldCharacter(removeObj->Container);
                    g_World->RemoveObject(removeObj);
                    if (character != nullptr)
                    {
                        character->m_FrameInfo =
                            g_AnimationManager.CollectFrameInformation(character);
                        g_GumpManager.UpdateContent(g_ObjectInHand.Container, 0, GT_PAPERDOLL);
                    }
                }
            }

            if (g_ProcessRemoveRangedTimer < g_Ticks)
            {
                g_Orion.RemoveRangedObjects();
                g_ProcessRemoveRangedTimer = g_Ticks + 50;
            }

            if (g_ConfigManager.ObjectHandles && g_CtrlPressed && g_ShiftPressed &&
                (oldCtrl != g_CtrlPressed || oldShift != g_ShiftPressed))
            {
                g_World->ResetObjectHandlesState();
            }

            if (rendering)
            {
                g_GameScreen.CalculateGameWindowBounds();
                g_GameScreen.CalculateRenderList();
                g_GameScreen.RenderListInitalized = true;
                g_SelectedObject.Clear();
                g_SelectedGameObjectHandle = 0;
                if (!g_gameWindow.IsMinimized())
                {
                    if (canRenderSelect)
                    {
                        g_GameScreen.SelectObject();
                    }

                    CGump::ProcessListing();
                    g_GameScreen.PrepareContent();
                    g_GameScreen.Render();
                }
                g_Target.UnloadMulti();
                g_GameScreen.RenderListInitalized = false;
                g_MapManager.Init(true);
            }
        }
    }
    else if (rendering)
    {
        g_SelectedObject.Clear();
        if (!g_gameWindow.IsMaximized())
        {
            g_CurrentScreen->SelectObject();
            g_CurrentScreen->PrepareContent();
            CGump::ProcessListing();
            g_CurrentScreen->Render();
        }
    }

    if (removeUnusedTexturesTime < g_Ticks)
    {
        ClearUnusedTextures();
        removeUnusedTexturesTime = g_Ticks + CLEAR_TEXTURES_DELAY;
    }

    if (removeUnusedAnimationTexturesTime < g_Ticks)
    {
        g_AnimationManager.ClearUnusedTextures(g_Ticks);
        removeUnusedAnimationTexturesTime = g_Ticks + CLEAR_ANIMATION_TEXTURES_DELAY;
    }
}

void COrion::LoadStartupConfig(int serial)
{
    PROFILER_EVENT();
    char buf[MAX_PATH] = { 0 };
    CServer* server    = g_ServerList.GetSelectedServer();
    if (server != nullptr)
    {
        sprintf_s(
            buf,
            "Desktop/%s/%s/0x%08X",
            g_MainScreen.m_Account->c_str(),
            FixServerName(server->Name).c_str(),
            serial);
    }
    else
    {
        sprintf_s(buf, "Desktop/%s/0x%08X", g_MainScreen.m_Account->c_str(), serial);
    }

    std::filesystem::path exeFilesPath = g_App.GetExeDir() / buf;
    if (!g_ConfigManager.Load(exeFilesPath / "sot_options.cfg"))
    {
        std::filesystem::path uoFilesPath = g_App.GetGameDir() / buf;
        if (!g_ConfigManager.Load(uoFilesPath / "sot_options.cfg"))
        {
            if (!g_ConfigManager.LoadBin(exeFilesPath / "options_debug.cuo"))
            {
                g_ConfigManager.LoadBin(uoFilesPath / "/options_debug.cuo");
            }
        }
    }

    g_SoundManager.SetMusicVolume(g_ConfigManager.GetMusicVolume());
    if (!g_ConfigManager.GetSound())
    {
        AdjustSoundEffects(g_Ticks + 100000);
    }

    if (!g_ConfigManager.GetMusic())
    {
        g_SoundManager.StopMusic();
    }
}

std::string COrion::FixServerName(std::string name)
{
    size_t i = 0;
    while ((i = name.find(':')) != std::string::npos)
    {
        name.erase(i, 1);
    }
    return name;
}

void COrion::LoadLocalConfig(int serial)
{
    PROFILER_EVENT();
    if (g_ConfigLoaded)
    {
        return;
    }

    g_CheckContainerStackTimer = g_Ticks + 30000;

    char buf[MAX_PATH] = { 0 };
    CServer* server    = g_ServerList.GetSelectedServer();
    if (server != nullptr)
    {
        sprintf_s(
            buf,
            "Desktop/%s/%s/0x%08X",
            g_MainScreen.m_Account->c_str(),
            FixServerName(server->Name).c_str(),
            serial);
    }
    else
    {
        sprintf_s(buf, "Desktop/%s/0x%08X", g_MainScreen.m_Account->c_str(), serial);
    }

    std::filesystem::path path = g_App.GetExeDir() / buf;
    if (!g_ConfigManager.Load(path / "sot_options.cfg") &&
        !g_ConfigManager.Load(g_App.GetGameDir() / "sot_options.cfg") &&
        !g_ConfigManager.LoadBin(path / "options_debug.cuo") &&
        !g_ConfigManager.LoadBin(g_App.GetGameDir() / "options_debug.cuo"))
    {
        g_ConfigManager.Init();
        if (g_GameState >= GS_GAME)
            g_gameWindow.Maximize();
    }

    if (!g_SkillGroupManager.Load(path / "skills_debug.cuo"))
        g_SkillGroupManager.Load(g_App.GetGameDir() / "skills_debug.cuo");

    if (!g_MacroManager.Load(path / "macros_debug.cuo", path / "macros.txt"))
        g_MacroManager.Load(
            g_App.GetGameDir() / "macros_debug.cuo", g_App.GetGameDir() / "macros.txt");

    g_GumpManager.Load(path / "gumps_debug.cuo");
    g_CustomHousesManager.Load(path / "customhouses_debug.cuo");
    if (g_ConfigManager.OffsetInterfaceWindows)
    {
        g_ContainerRect.MakeDefault();
    }

    if (g_ConfigManager.GetConsoleNeedEnter())
    {
        g_EntryPointer = nullptr;
    }
    else
    {
        g_EntryPointer = &g_GameConsole;
    }

    g_SoundManager.SetMusicVolume(g_ConfigManager.GetMusicVolume());
    if (!g_ConfigManager.GetSound())
    {
        AdjustSoundEffects(g_Ticks + 100000);
    }

    if (!g_ConfigManager.GetMusic())
    {
        g_SoundManager.StopMusic();
    }
    g_ConfigLoaded = true;
}

void COrion::SaveLocalConfig(int serial)
{
    PROFILER_EVENT();
    if (!g_ConfigLoaded)
    {
        return;
    }

    std::filesystem::path path = g_App.GetExeDir() / "Desktop";
    if (!std::filesystem::exists(path))
    {
        LOG_INFO("Client", "%s Does not exist, creating.", path.string().c_str());
        std::filesystem::create_directory(path);
    }
    path /= g_MainScreen.m_Account->c_str();
    if (!std::filesystem::exists(path))
    {
        LOG_INFO("Client", "%s Does not exist, creating.", path.string().c_str());
        std::filesystem::create_directory(path);
    }
    CServer* server = g_ServerList.GetSelectedServer();
    if (server != nullptr)
        path /= FixServerName(server->Name);
    if (!std::filesystem::exists(path))
    {
        LOG_INFO("Client", "%s Does not exist, creating.", path.string().c_str());
        std::filesystem::create_directory(path);
    }
    char serbuf[20] = { 0 };
    sprintf_s(serbuf, "0x%08X", g_PlayerSerial);
    path /= serbuf;
    if (!std::filesystem::exists(path))
    {
        LOG_INFO("Client", "%s Does not exist, creating.", path.string().c_str());
        std::filesystem::create_directory(path);
    }
    else
    {
        LOG_INFO("Client", "SaveLocalConfig using path: %s", path.string().c_str());
    }

    LOG_INFO("Client", "managers:saving");
    g_ConfigManager.Save(path / "sot_options.cfg");
    g_SkillGroupManager.Save(path / "skills_debug.cuo");
    g_MacroManager.Save(path / "macros_debug.cuo");
    g_GumpManager.Save(path / "gumps_debug.cuo");
    g_CustomHousesManager.Save(path / "customhouses_debug.cuo");

    LOG_INFO("Client", "managers:saving in to root");
    g_ConfigManager.Save(g_App.GetGameDir() / "sot_options.cfg");
    g_MacroManager.Save(g_App.GetGameDir() / "macros_debug.cuo");

    if (g_Player != nullptr)
    {
        LOG_INFO("Client", "player exists");
        LOG_INFO("Client", "name len: %zd", g_Player->GetName().length());
        path /= ("_" + g_Player->GetName() + ".cuo");
        if (!std::filesystem::exists(path))
        {
            LOG_INFO("Client", "file saving");
            Core::File file(path, "wb");
            LOG_INFO("Client", "file closing");
        }
    }
    LOG_INFO("Client", "SaveLocalConfig end");
}

void COrion::ClearUnusedTextures()
{
    if (g_GameState < GS_GAME)
    {
        return;
    }

    g_MapManager.ClearUnusedBlocks();

    g_GumpManager.PrepareTextures();

    g_Ticks -= CLEAR_TEXTURES_DELAY;

    void* lists[5] = {
        &m_UsedLandList, &m_UsedStaticList, &m_UsedGumpList, &m_UsedTextureList, &m_UsedLightList
    };

    int counts[5] = { MAX_ART_OBJECT_REMOVED_BY_GARBAGE_COLLECTOR,
                      MAX_ART_OBJECT_REMOVED_BY_GARBAGE_COLLECTOR,
                      MAX_GUMP_OBJECT_REMOVED_BY_GARBAGE_COLLECTOR,
                      MAX_ART_OBJECT_REMOVED_BY_GARBAGE_COLLECTOR,
                      100 };

    for (int i = 0; i < 5; i++)
    {
        int count     = 0;
        auto* list    = (std::deque<CIndexObject*>*)lists[i];
        int& maxCount = counts[i];

        for (auto it = list->begin(); it != list->end();)
        {
            CIndexObject* obj = *it;
            if (obj->LastAccessed < Core::FrameTimer::Now())
            {
                if (obj->Texture != nullptr)
                {
                    delete obj->Texture;
                    obj->Texture = nullptr;
                }

                it = list->erase(it);
                if (++count >= maxCount)
                {
                    break;
                }
            }
            else
            {
                ++it;
            }
        }
    }

    int count = 0;
    for (auto it = m_UsedSoundList.begin(); it != m_UsedSoundList.end();)
    {
        CIndexSound* obj = *it;
        if (obj->LastAccessed < Core::FrameTimer::Now())
        {
            if (obj->m_Stream != SOUND_NULL)
            {
                g_SoundManager.UpdateSoundEffect(obj->m_Stream, -1);
                obj->m_Stream = SOUND_NULL;
            }

            it = m_UsedSoundList.erase(it);
            if (++count >= MAX_SOUND_OBJECT_REMOVED_BY_GARBAGE_COLLECTOR)
            {
                break;
            }
        }
        else
        {
            ++it;
        }
    }

    AdjustSoundEffects(g_Ticks);
    g_Ticks += CLEAR_TEXTURES_DELAY;
}

void COrion::Connect()
{
    PROFILER_EVENT();
    InitScreen(GS_MAIN_CONNECT);
    Process(true);

    g_ConnectionManager.Disconnect();
    g_ConnectionManager.Init(); //Configure

    std::string login{};
    int port;
    LoadLogin(login, port);
    if (g_ConnectionManager.Connect(login, port, g_GameSeed))
    {
        g_ConnectionScreen.SetConnected(true);
        CPacketFirstLogin().Send();
    }
    else
    {
        g_ConnectionScreen.SetConnectionFailed(true);
        g_ConnectionScreen.SetErrorCode(8);
    }
}

void COrion::Disconnect()
{
    PROFILER_EVENT();
    g_AbyssPacket03First = true;;

    g_ConnectionManager.Disconnect();

    ClearWorld();
}

int COrion::Send(u8* buf, int size)
{
    u32 ticks = g_Ticks;
    g_TotalSendSize += size;
    CPacketInfo& type = g_PacketManager.GetInfo(*buf);
    if (type.save)
    {
#if defined(ORION_WINDOWS)
        time_t rawtime;
        struct tm timeinfo;
        char buffer[80];

        time(&rawtime);
        localtime_s(&timeinfo, &rawtime);
        strftime(buffer, sizeof(buffer), "%d-%m-%Y %H:%M:%S", &timeinfo);
        LOG_INFO(
            "Client",
            "--- ^(%d) s(+%d => %d) %s Client:: %s\n",
            ticks - g_LastPacketTime,
            size,
            g_TotalSendSize,
            buffer,
            type.Name);
#endif

        if (*buf == 0x80 || *buf == 0x91)
        {
            //LOG_DUMP(buf, 1);
            //SAFE_LOG_DUMP(buf, size);
            LOG_INFO("Client", "**** ACCOUNT AND PASSWORD CENSORED ****");
        }
        else
        {
            //LOG_DUMP(buf, size);
        }
    }

    int result = 0;

    if (type.Direction != DIR_SEND && type.Direction != DIR_BOTH)
    {
        LOG_ERROR("Client", "Warning!!! Message direction invalid: 0x%02X\n", *buf);
    }
    else
    {
        result = g_ConnectionManager.Send(buf, size);
    }

    if (result != 0)
    {
        g_LastPacketTime = ticks;

        // Siebenwind issue #1:
        // Uncommenting this results in more frequently sent pings/"keep alive"
        // packets in the COrion::Process(..) method.
        // TODO: Fix the server, so that it is no longer necessary
        // to change the client behavior.
        // g_LastSendTime = ticks;
    }

    return result;
}

void COrion::ServerSelection(int pos)
{
    InitScreen(GS_SERVER_CONNECT);
    Process(true);
    CServer* server = g_ServerList.Select(pos);
    if (server != nullptr)
        CPacketSelectServer((u8)server->Index).Send();
}

void COrion::RelayServer(const char* ip, int port, u8* gameSeed)
{
    memcpy(&g_GameSeed[0], &gameSeed[0], 4);
    g_ConnectionManager.Init(gameSeed);
    m_GameServerIP = ip;
    g_gameServerPingInfo.reset();
    if (g_ConnectionManager.Connect(ip, port, gameSeed))
    {
        g_ConnectionScreen.SetConnected(true);
        CPacketSecondLogin().Send();
    }
    else
    {
        g_ConnectionScreen.SetConnectionFailed(true);
        g_ConnectionScreen.SetErrorCode(8);
    }
}

void COrion::CharacterSelection(int pos)
{
    InitScreen(GS_GAME_CONNECT);
    g_ConnectionScreen.SetType(CST_GAME);
    g_CharacterList.LastCharacterName = g_CharacterList.GetName(pos);
    CPacketSelectCharacter(pos, g_CharacterList.LastCharacterName).Send();
}

void COrion::LoginComplete(bool reload)
{
    PROFILER_EVENT();
    bool load = reload;
    if (!load && !g_ConnectionScreen.GetCompleted())
    {
        load = true;
        g_ConnectionScreen.SetCompleted(true);
        InitScreen(GS_GAME);
    }

    if (load && g_Player != nullptr)
    {
        std::string title = SiebenwindClient::GetWindowTitle() + " - " + g_Player->GetName();
        CServer* server   = g_ServerList.GetSelectedServer();
        if (server != nullptr)
            title += " (" + server->Name + ")";

        g_gameWindow.SetTitle(title.c_str());
        CPacketSkillsRequest(g_PlayerSerial).Send();
        g_UseItemActions.Add(g_PlayerSerial);

        //CPacketOpenChat({}).Send();
        //CPacketRazorAnswer().Send();
        if (GameVars::GetClientVersion() >= CV_306E)
        {
            CPacketClientType().Send();
        }

        if (GameVars::GetClientVersion() >= CV_305D)
        {
            CPacketClientViewRange(g_ConfigManager.UpdateRange).Send();
        }
        LoadLocalConfig(g_PacketManager.ConfigSerial);
    }
}

void COrion::ChangeSeason(const SEASON_TYPE& season, int music)
{
    PROFILER_EVENT();

    g_Season = season;
    QFOR(item, g_MapManager.m_Items, CMapBlock*)
    {
        for (int x = 0; x < 8; x++)
        {
            for (int y = 0; y < 8; y++)
            {
                QFOR(obj, item->GetRender((int)x, (int)y), CRenderWorldObject*)
                {
                    obj->UpdateGraphicBySeason();
                }
            }
        }
    }

    CGumpMinimap* gump = (CGumpMinimap*)g_GumpManager.UpdateGump(0, 0, GT_MINIMAP);

    if (gump != nullptr)
    {
        gump->LastX = 0;
    }

    if (music != 0)
    {
        g_Orion.PlayMusic(music, true);
    }
}

u16 COrion::GetLandSeasonGraphic(u16 graphic)
{
    if (g_Season == ST_WINTER)
    {
        u16 buf = m_WinterTile[graphic];

        if (buf != 0)
        {
            graphic = buf;
        }
    }

    return graphic;
}

u16 COrion::GetSeasonGraphic(u16 graphic)
{
    switch (g_Season)
    {
        case ST_SPRING: return GetSpringGraphic(graphic);
        //case ST_SUMMER:
        //	return GetSummerGraphic(graphic);
        case ST_FALL: return GetFallGraphic(graphic);
        //case ST_WINTER:
        //	return GetWinterGraphic(graphic);
        case ST_DESOLATION: return GetDesolationGraphic(graphic);
        default: break;
    }

    return graphic;
}

u16 COrion::GetSpringGraphic(u16 graphic)
{
    switch (graphic)
    {
        case 0x0CA7: graphic = 0x0C84; break;
        case 0x0CAC: graphic = 0x0C46; break;
        case 0x0CAD: graphic = 0x0C48; break;
        case 0x0CAE:
        case 0x0CB5: graphic = 0x0C4A; break;
        case 0x0CAF: graphic = 0x0C4E; break;
        case 0x0CB0: graphic = 0x0C4D; break;
        case 0x0CB6:
        case 0x0D0D:
        case 0x0D14: graphic = 0x0D2B; break;
        case 0x0D0C: graphic = 0x0D29; break;
        case 0x0D0E: graphic = 0x0CBE; break;
        case 0x0D0F: graphic = 0x0CBF; break;
        case 0x0D10: graphic = 0x0CC0; break;
        case 0x0D11: graphic = 0x0C87; break;
        case 0x0D12: graphic = 0x0C38; break;
        case 0x0D13: graphic = 0x0D2F; break;
        default: break;
    }

    return graphic;
}

u16 COrion::GetSummerGraphic(u16 graphic)
{
    return graphic;
}

u16 COrion::GetFallGraphic(u16 graphic)
{
    switch (graphic)
    {
        case 0x0CD1: graphic = 0x0CD2; break;
        case 0x0CD4: graphic = 0x0CD5; break;
        case 0x0CDB: graphic = 0x0CDC; break;
        case 0x0CDE: graphic = 0x0CDF; break;
        case 0x0CE1: graphic = 0x0CE2; break;
        case 0x0CE4: graphic = 0x0CE5; break;
        case 0x0CE7: graphic = 0x0CE8; break;
        case 0x0D95: graphic = 0x0D97; break;
        case 0x0D99: graphic = 0x0D9B; break;
        case 0x0CCE: graphic = 0x0CCF; break;
        case 0x0CE9:
        case 0x0C9E: graphic = 0x0D3F; break;
        case 0x0CEA: graphic = 0x0D40; break;
        case 0x0C84:
        case 0x0CB0: graphic = 0x1B22; break;
        case 0x0C8B:
        case 0x0C8C:
        case 0x0C8D:
        case 0x0C8E: graphic = 0x0CC6; break;
        case 0x0CA7: graphic = 0x0C48; break;
        case 0x0CAC: graphic = 0x1B1F; break;
        case 0x0CAD: graphic = 0x1B20; break;
        case 0x0CAE: graphic = 0x1B21; break;
        case 0x0CAF: graphic = 0x0D0D; break;
        case 0x0CB5: graphic = 0x0D10; break;
        case 0x0CB6: graphic = 0x0D2B; break;
        case 0x0CC7: graphic = 0x0C4E; break;
        default: break;
    }

    return graphic;
}

u16 COrion::GetWinterGraphic(u16 graphic)
{
    return graphic;
}

u16 COrion::GetDesolationGraphic(u16 graphic)
{
    switch (graphic)
    {
        case 0x1B7E: graphic = 0x1E34; break;
        case 0x0D2B: graphic = 0x1B15; break;
        case 0x0D11:
        case 0x0D14:
        case 0x0D17: graphic = 0x122B; break;
        case 0x0D16:
        case 0x0CB9:
        case 0x0CBA:
        case 0x0CBB:
        case 0x0CBC:
        case 0x0CBD:
        case 0x0CBE: graphic = 0x1B8D; break;
        case 0x0CC7: graphic = 0x1B0D; break;
        case 0x0CE9: graphic = 0x0ED7; break;
        case 0x0CEA: graphic = 0x0D3F; break;
        case 0x0D0F: graphic = 0x1B1C; break;
        case 0x0CB8: graphic = 0x1CEA; break;
        case 0x0C84:
        case 0x0C8B: graphic = 0x1B84; break;
        case 0x0C9E: graphic = 0x1182; break;
        case 0x0CAD: graphic = 0x1AE1; break;
        case 0x0C4C: graphic = 0x1B16; break;
        case 0x0C8E:
        case 0x0C99:
        case 0x0CAC: graphic = 0x1B8D; break;
        case 0x0C46:
        case 0x0C49:
        case 0x0CB6: graphic = 0x1B9D; break;
        case 0x0C45:
        case 0x0C48:
        case 0x0C4E:
        case 0x0C85:
        case 0x0CA7:
        case 0x0CAE:
        case 0x0CAF:
        case 0x0CB5:
        case 0x0D15:
        case 0x0D29: graphic = 0x1B9C; break;
        case 0x0C37:
        case 0x0C38:
        case 0x0C47:
        case 0x0C4A:
        case 0x0C4B:
        case 0x0C4D:
        case 0x0C8C:
        case 0x0C8D:
        case 0x0C93:
        case 0x0C94:
        case 0x0C98:
        case 0x0C9F:
        case 0x0CA0:
        case 0x0CA1:
        case 0x0CA2:
        case 0x0CA3:
        case 0x0CA4:
        case 0x0CB0:
        case 0x0CB1:
        case 0x0CB2:
        case 0x0CB3:
        case 0x0CB4:
        case 0x0CB7:
        case 0x0CC5:
        case 0x0D0C:
        case 0x0D0D:
        case 0x0D0E:
        case 0x0D10:
        case 0x0D12:
        case 0x0D13:
        case 0x0D18:
        case 0x0D19:
        case 0x0D2D:
        case 0x0D2F: graphic = 0x1BAE; break;
        default: break;
    }

    return graphic;
}

int COrion::ValueInt(const VALUE_KEY_INT& key, int value)
{
    switch (key)
    {
        case VKI_SOUND:
        {
            if (value == -1)
            {
                value = static_cast<int>(g_ConfigManager.GetSound());
            }
            else
            {
                g_ConfigManager.SetSound(value != 0);
            }

            break;
        }
        case VKI_SOUND_VALUE:
        {
            if (value == -1)
            {
                value = g_ConfigManager.GetSoundVolume();
            }
            else
            {
                if (value < 0)
                {
                    value = 0;
                }
                else if (value > 255)
                {
                    value = 255;
                }

                g_ConfigManager.SetSoundVolume(value);
            }

            break;
        }
        case VKI_MUSIC:
        {
            if (value == -1)
            {
                value = static_cast<int>(g_ConfigManager.GetMusic());
            }
            else
            {
                g_ConfigManager.SetMusic(value != 0);
            }

            break;
        }
        case VKI_MUSIC_VALUE:
        {
            if (value == -1)
            {
                value = g_ConfigManager.GetMusicVolume();
            }
            else
            {
                if (value < 0)
                {
                    value = 0;
                }
                else if (value > 255)
                {
                    value = 255;
                }

                g_ConfigManager.SetMusicVolume(value);
            }

            break;
        }
        case VKI_USE_TOOLTIPS:
        {
            if (value == -1)
            {
                value = static_cast<int>(g_ConfigManager.UseToolTips);
            }
            else
            {
                g_ConfigManager.UseToolTips = (value != 0);
            }

            break;
        }
        case VKI_ALWAYS_RUN:
        {
            if (value == -1)
            {
                value = static_cast<int>(g_ConfigManager.AlwaysRun);
            }
            else
            {
                g_ConfigManager.AlwaysRun = (value != 0);
            }

            break;
        }
        case VKI_NEW_TARGET_SYSTEM:
        {
            if (value == -1)
            {
                value = static_cast<int>(g_ConfigManager.DisableNewTargetSystem);
            }
            else
            {
                g_ConfigManager.DisableNewTargetSystem =
                    (value == 0); //������ == 0!!! �.�. � ������� ��� Target System enable/disable
            }

            break;
        }
        case VKI_OBJECT_HANDLES:
        {
            if (value == -1)
            {
                value = static_cast<int>(g_ConfigManager.ObjectHandles);
            }
            else
            {
                g_ConfigManager.ObjectHandles = (value != 0);
            }

            break;
        }
        case VKI_SCALE_SPEECH_DELAY:
        {
            if (value == -1)
            {
                value = static_cast<int>(g_ConfigManager.ScaleSpeechDelay);
            }
            else
            {
                g_ConfigManager.ScaleSpeechDelay = (value != 0);
            }

            break;
        }
        case VKI_SPEECH_DELAY:
        {
            if (value == -1)
            {
                value = g_ConfigManager.SpeechDelay;
            }
            else
            {
                if (value < 0)
                {
                    value = 0;
                }
                else if (value > 999)
                {
                    value = 999;
                }

                g_ConfigManager.SpeechDelay = value;
            }

            break;
        }
        case VKI_IGNORE_GUILD_MESSAGES:
        {
            if (value == -1)
            {
                value = static_cast<int>(g_ConfigManager.IgnoreGuildMessage);
            }
            else
            {
                g_ConfigManager.IgnoreGuildMessage = (value != 0);
            }

            break;
        }
        case VKI_IGNORE_ALLIANCE_MESSAGES:
        {
            if (value == -1)
            {
                value = static_cast<int>(g_ConfigManager.IgnoreAllianceMessage);
            }
            else
            {
                g_ConfigManager.IgnoreAllianceMessage = (value != 0);
            }

            break;
        }
        case VKI_DARK_NIGHTS:
        {
            if (value == -1)
            {
                value = static_cast<int>(g_ConfigManager.DarkNights);
            }
            else
            {
                g_ConfigManager.DarkNights = (value != 0);
            }

            break;
        }
        case VKI_COLORED_LIGHTING:
        {
            if (value == -1)
            {
                value = static_cast<int>(g_ConfigManager.ColoredLighting);
            }
            else
            {
                g_ConfigManager.ColoredLighting = (value != 0);
            }

            break;
        }
        case VKI_CRIMINAL_ACTIONS_QUERY:
        {
            if (value == -1)
            {
                value = static_cast<int>(g_ConfigManager.CriminalActionsQuery);
            }
            else
            {
                g_ConfigManager.CriminalActionsQuery = (value != 0);
            }

            break;
        }
        case VKI_CIRCLETRANS:
        {
            if (value == -1)
            {
                value = static_cast<int>(g_ConfigManager.UseCircleTrans);
            }
            else
            {
                g_ConfigManager.UseCircleTrans = (value != 0);
            }

            break;
        }
        case VKI_CIRCLETRANS_VALUE:
        {
            if (value == -1)
            {
                value = g_ConfigManager.CircleTransRadius;
            }
            else
            {
                if (value < 0)
                {
                    value = 0;
                }
                else if (value > 255)
                {
                    value = 255;
                }

                g_ConfigManager.CircleTransRadius = value;
            }

            break;
        }
        case VKI_LOCK_RESIZING_GAME_WINDOW:
        {
            if (value == -1)
            {
                value = static_cast<int>(g_ConfigManager.LockResizingGameWindow);
            }
            else
            {
                g_ConfigManager.LockResizingGameWindow = (value != 0);
            }

            break;
        }
        case VKI_CLIENT_FPS_VALUE:
        {
            if (value == -1)
            {
                value = g_ConfigManager.GetClientFPS();
            }
            else
            {
                if (value < MIN_FPS_LIMIT)
                {
                    value = MIN_FPS_LIMIT;
                }
                else if (value > MAX_FPS_LIMIT)
                {
                    value = MAX_FPS_LIMIT;
                }

                g_ConfigManager.SetClientFPS(value);
            }

            break;
        }
        case VKI_DRAW_STATUS_STATE:
        {
            if (value == -1)
            {
                value = g_ConfigManager.GetDrawStatusState();
            }
            else
            {
                if (value < 0)
                {
                    value = 0;
                }
                else if (value > DCSS_UNDER)
                {
                    value = DCSS_UNDER;
                }

                g_ConfigManager.SetDrawStatusState(value);
            }

            break;
        }
        case VKI_DRAW_STUMPS:
        {
            if (value == -1)
            {
                value = static_cast<int>(g_ConfigManager.GetDrawStumps());
            }
            else
            {
                g_ConfigManager.SetDrawStumps((value != 0));
            }

            break;
        }
        case VKI_MARKING_CAVES:
        {
            if (value == -1)
            {
                value = static_cast<int>(g_ConfigManager.GetMarkingCaves());
            }
            else
            {
                g_ConfigManager.SetMarkingCaves((value != 0));
            }

            break;
        }
        case VKI_NO_VEGETATION:
        {
            if (value == -1)
            {
                value = static_cast<int>(g_ConfigManager.GetNoVegetation());
            }
            else
            {
                g_ConfigManager.SetNoVegetation((value != 0));
            }

            break;
        }
        case VKI_NO_ANIMATE_FIELDS:
        {
            if (value == -1)
            {
                value = static_cast<int>(g_ConfigManager.GetNoAnimateFields());
            }
            else
            {
                g_ConfigManager.SetNoAnimateFields((value != 0));
            }

            break;
        }
        case VKI_STANDARD_CHARACTERS_DELAY:
        {
            if (value == -1)
            {
                value = static_cast<int>(g_ConfigManager.StandartCharactersAnimationDelay);
            }
            else
            {
                g_ConfigManager.StandartCharactersAnimationDelay = (value != 0);
            }

            break;
        }
        case VKI_STANDARD_ITEMS_DELAY:
        {
            if (value == -1)
            {
                value = static_cast<int>(g_ConfigManager.StandartItemsAnimationDelay);
            }
            else
            {
                g_ConfigManager.StandartItemsAnimationDelay = (value != 0);
            }

            break;
        }
        case VKI_LOCK_GUMPS_MOVING:
        {
            if (value == -1)
            {
                value = static_cast<int>(g_ConfigManager.LockGumpsMoving);
            }
            else
            {
                g_ConfigManager.LockGumpsMoving = (value != 0);
            }

            break;
        }
        case VKI_CONSOLE_NEED_ENTER:
        {
            if (value == -1)
            {
                value = static_cast<int>(g_ConfigManager.GetConsoleNeedEnter());
            }
            else
            {
                g_ConfigManager.SetConsoleNeedEnter(value != 0);
            }

            break;
        }
        case VKI_HIDDEN_CHARACTERS_MODE:
        {
            if (value == -1)
            {
                value = g_ConfigManager.HiddenCharactersRenderMode;
            }
            else
            {
                if (value < 0)
                {
                    value = 0;
                }
                else if (value > HCRM_SPECIAL_SPECTRAL_COLOR)
                {
                    value = HCRM_SPECIAL_SPECTRAL_COLOR;
                }

                g_ConfigManager.HiddenCharactersRenderMode = value;
            }

            break;
        }
        case VKI_HIDDEN_CHARACTERS_ALPHA:
        {
            if (value == -1)
            {
                value = g_ConfigManager.HiddenAlpha;
            }
            else
            {
                if (value < 20)
                {
                    value = 20;
                }
                else if (value > 255)
                {
                    value = 255;
                }

                g_ConfigManager.HiddenAlpha = value;
            }

            break;
        }
        case VKI_HIDDEN_CHARACTERS_MODE_ONLY_FOR_SELF:
        {
            if (value == -1)
            {
                value = static_cast<int>(g_ConfigManager.UseHiddenModeOnlyForSelf);
            }
            else
            {
                g_ConfigManager.UseHiddenModeOnlyForSelf = (value != 0);
            }

            break;
        }
        case VKI_TRANSPARENT_SPELL_ICONS:
        {
            if (value == -1)
            {
                value = g_ConfigManager.TransparentSpellIcons;
            }
            else
            {
                g_ConfigManager.TransparentSpellIcons = static_cast<u8>(value != 0);
            }

            break;
        }
        case VKI_SPELL_ICONS_ALPHA:
        {
            if (value == -1)
            {
                value = g_ConfigManager.GetSpellIconAlpha();
            }
            else
            {
                if (value < 30)
                {
                    value = 30;
                }
                else if (value > 255)
                {
                    value = 255;
                }

                g_ConfigManager.SetSpellIconAlpha(value);
            }

            break;
        }
        case VKI_SKILLS_COUNT:
        {
            value = g_SkillsManager.Count;
            break;
        }
        case VKI_SKILL_CAN_BE_USED:
        {
            CSkill* skill = g_SkillsManager.Get(value);

            if (skill != nullptr)
            {
                value = static_cast<int>(skill->Button);
            }

            break;
        }
        case VKI_STATIC_ART_ADDRESS:
        {
            if (value >= 0 && value < (int)m_StaticData.size())
            {
                value = (int)m_StaticDataIndex[value].Address;
            }

            break;
        }
        case VKI_USED_LAYER:
        {
            if (value >= 0 && value < (int)m_StaticData.size())
            {
                value = m_StaticData[value].Layer;
            }

            break;
        }
        case VKI_SPELLBOOK_COUNT:
        {
            if (value >= 0 && value < 7)
            {
                switch (value)
                {
                    case 1:
                    {
                        value = CGumpSpellbook::SPELLBOOK_1_SPELLS_COUNT;
                        break;
                    }
                    case 2:
                    {
                        value = CGumpSpellbook::SPELLBOOK_2_SPELLS_COUNT;
                        break;
                    }
                    case 3:
                    {
                        value = CGumpSpellbook::SPELLBOOK_3_SPELLS_COUNT;
                        break;
                    }
                    case 4:
                    {
                        value = CGumpSpellbook::SPELLBOOK_4_SPELLS_COUNT;
                        break;
                    }
                    case 5:
                    {
                        value = CGumpSpellbook::SPELLBOOK_5_SPELLS_COUNT;
                        break;
                    }
                    case 6:
                    {
                        value = CGumpSpellbook::SPELLBOOK_6_SPELLS_COUNT;
                        break;
                    }
                    case 7:
                    {
                        value = CGumpSpellbook::SPELLBOOK_7_SPELLS_COUNT;
                        break;
                    }
                    default: break;
                }
            }

            break;
        }
        case VKI_BLOCK_MOVING:
        {
            g_PathFinder.BlockMoving = (value != 0);

            break;
        }
        case VKI_SET_PLAYER_GRAPHIC:
        {
            if (g_Player != nullptr && g_Player->Graphic != value)
            {
                g_Player->Graphic = value;
                g_Player->OnGraphicChange(1000);
            }

            break;
        }
        case VKI_FAST_ROTATION:
        {
            g_PathFinder.FastRotation = (value != 0);

            break;
        }
        case VKI_IGNORE_STAMINA_CHECK:
        {
            g_PathFinder.IgnoreStaminaCheck = (value != 0);

            break;
        }
        case VKI_LAST_TARGET:
        {
            if (value == -1)
            {
                value = g_LastTargetObject;
            }
            else
            {
                g_LastTargetObject = value;
                g_Target.SetLastTargetObject(g_LastTargetObject);
            }

            break;
        }
        case VKI_LAST_ATTACK:
        {
            if (value == -1)
            {
                value = g_LastAttackObject;
            }
            else
            {
                g_LastAttackObject = value;
            }

            break;
        }
        case VKI_NEW_TARGET_SYSTEM_SERIAL:
        {
            if (value == -1)
            {
                value = g_NewTargetSystem.Serial;
            }
            else
            {
                g_NewTargetSystem.Serial = value;
            }

            break;
        }
        case VKI_GET_MAP_SIZE:
        {
            if (value >= 0 && value < 6)
            {
                value = (g_MapSize[value].x << 16) | g_MapSize[value].y;
            }

            break;
        }
        case VKI_GET_MAP_BLOCK_SIZE:
        {
            if (value >= 0 && value < 6)
            {
                value = (g_MapBlockSize[value].x << 16) | g_MapBlockSize[value].y;
            }

            break;
        }
        case VKI_MAP_MUL_ADDRESS:
        {
            if (value >= 0 && value < 6)
            {
                value = checked_cast<int>(g_FileManager.m_MapMul[value].GetBuffer());
            }

            break;
        }
        case VKI_MAP_MUL_SIZE:
        {
            if (value >= 0 && value < 6)
            {
                value = (int)g_FileManager.m_MapMul[value].GetSize();
            }

            break;
        }
        case VKI_STATIC_IDX_ADDRESS:
        {
            if (value >= 0 && value < 6)
            {
                value = checked_cast<int>(g_FileManager.m_StaticIdx[value].GetBuffer());
            }

            break;
        }
        case VKI_STATIC_IDX_SIZE:
        {
            if (value >= 0 && value < 6)
            {
                value = (int)g_FileManager.m_StaticIdx[value].GetSize();
            }

            break;
        }
        case VKI_STATIC_MUL_ADDRESS:
        {
            if (value >= 0 && value < 6)
            {
                value = checked_cast<int>(g_FileManager.m_StaticMul[value].GetBuffer());
            }

            break;
        }
        case VKI_STATIC_MUL_SIZE:
        {
            if (value >= 0 && value < 6)
            {
                value = (int)g_FileManager.m_StaticMul[value].GetSize();
            }

            break;
        }
        case VKI_MAP_DIFL_ADDRESS:
        {
            if (value >= 0 && value < 6)
            {
                value = checked_cast<int>(g_FileManager.m_MapDifl[value].GetBuffer());
            }

            break;
        }
        case VKI_MAP_DIFL_SIZE:
        {
            if (value >= 0 && value < 6)
            {
                value = (int)g_FileManager.m_MapDifl[value].GetSize();
            }

            break;
        }
        case VKI_MAP_DIF_ADDRESS:
        {
            if (value >= 0 && value < 6)
            {
                value = checked_cast<int>(g_FileManager.m_MapDif[value].GetBuffer());
            }

            break;
        }
        case VKI_MAP_DIF_SIZE:
        {
            if (value >= 0 && value < 6)
            {
                value = (int)g_FileManager.m_MapDif[value].GetSize();
            }

            break;
        }
        case VKI_STATIC_DIFL_ADDRESS:
        {
            if (value >= 0 && value < 6)
            {
                value = checked_cast<int>(g_FileManager.m_StaDifl[value].GetBuffer());
            }

            break;
        }
        case VKI_STATIC_DIFL_SIZE:
        {
            if (value >= 0 && value < 6)
            {
                value = (int)g_FileManager.m_StaDifl[value].GetSize();
            }

            break;
        }
        case VKI_STATIC_DIFI_ADDRESS:
        {
            if (value >= 0 && value < 6)
            {
                value = checked_cast<int>(g_FileManager.m_StaDifi[value].GetBuffer());
            }

            break;
        }
        case VKI_STATIC_DIFI_SIZE:
        {
            if (value >= 0 && value < 6)
            {
                value = (int)g_FileManager.m_StaDifi[value].GetSize();
            }

            break;
        }
        case VKI_STATIC_DIF_ADDRESS:
        {
            if (value >= 0 && value < 6)
            {
                value = checked_cast<int>(g_FileManager.m_StaDif[value].GetBuffer());
            }

            break;
        }
        case VKI_STATIC_DIF_SIZE:
        {
            if (value >= 0 && value < 6)
            {
                value = (int)g_FileManager.m_StaDif[value].GetSize();
            }

            break;
        }
        case VKI_VERDATA_ADDRESS:
        {
            value = checked_cast<int>(g_FileManager.m_VerdataMul.GetBuffer());

            break;
        }
        case VKI_VERDATA_SIZE:
        {
            value = (int)g_FileManager.m_VerdataMul.GetSize();

            break;
        }
        case VKI_MAP_UOP_ADDRESS:
        {
            if (value >= 0 && value < 6)
            {
                value = checked_cast<int>(g_FileManager.m_MapUOP[value].GetBuffer());
            }

            break;
        }
        case VKI_MAP_UOP_SIZE:
        {
            if (value >= 0 && value < 6)
            {
                value = (int)g_FileManager.m_MapUOP[value].GetSize();
            }

            break;
        }
        case VKI_MAP_X_UOP_ADDRESS:
        {
            if (value >= 0 && value < 6)
            {
                value = checked_cast<int>(g_FileManager.m_MapXUOP[value].GetBuffer());
            }

            break;
        }
        case VKI_MAP_X_UOP_SIZE:
        {
            if (value >= 0 && value < 6)
            {
                value = (int)g_FileManager.m_MapXUOP[value].GetSize();
            }

            break;
        }
        case VKI_CLILOC_ENU_ADDRESS:
        {
            value = checked_cast<int>(g_ClilocManager.GetCliloc("enu")->GetFile().GetBuffer());

            break;
        }
        case VKI_CLILOC_ENU_SIZE:
        {
            value = (int)g_ClilocManager.GetCliloc("enu")->GetFile().GetSize();

            break;
        }
        case VKI_GUMP_ART_ADDRESS:
        {
            if (value >= 0 && value < MAX_GUMP_DATA_INDEX_COUNT)
            {
                value = checked_cast<int>(m_GumpDataIndex[value].Address);
            }

            break;
        }
        case VKI_VIEW_RANGE:
        {
            value = g_ConfigManager.UpdateRange;

            break;
        }
        case VKI_SET_PVPCALLER:
        {
            CGameCharacter* obj = g_World->FindWorldCharacter(value);
            if (obj == nullptr)
            {
                break;
            }

            if (!obj->pvpCaller)
            {
                obj->pvpCaller = true;
            }
            else
            {
                obj->pvpCaller = false;
            }

            break;
        }
        default: break;
    }

    return value;
}

std::string COrion::ValueString(const VALUE_KEY_STRING& key, std::string value)
{
    switch (key)
    {
        case VKS_SKILL_NAME:
        {
            int index = atoi(value.c_str());

            CSkill* skill = g_SkillsManager.Get(index);

            if (skill != nullptr)
            {
                value = skill->Name;
            }

            break;
        }
        case VKS_SERVER_NAME:
        {
            CServer* server = g_ServerList.GetSelectedServer();

            if (server != nullptr)
            {
                value = server->Name;
            }

            break;
        }
        case VKS_CHARACTER_NAME:
        {
            if (g_Player != nullptr)
            {
                value = g_Player->GetName();
            }

            break;
        }
        case VKS_SPELLBOOK_1_SPELL_NAME:
        {
            int index = atoi(value.c_str());

            if (index >= 0 && index < CGumpSpellbook::SPELLBOOK_1_SPELLS_COUNT)
            {
                value = CGumpSpellbook::m_SpellName1[index][0];
            }

            break;
        }
        case VKS_SPELLBOOK_2_SPELL_NAME:
        {
            int index = atoi(value.c_str());

            if (index >= 0 && index < CGumpSpellbook::SPELLBOOK_2_SPELLS_COUNT)
            {
                value = CGumpSpellbook::m_SpellName2[index][0];
            }

            break;
        }
        case VKS_SPELLBOOK_3_SPELL_NAME:
        {
            int index = atoi(value.c_str());

            if (index >= 0 && index < CGumpSpellbook::SPELLBOOK_3_SPELLS_COUNT)
            {
                value = CGumpSpellbook::m_SpellName3[index][0];
            }

            break;
        }
        case VKS_SPELLBOOK_4_SPELL_NAME:
        {
            int index = atoi(value.c_str());

            if (index >= 0 && index < CGumpSpellbook::SPELLBOOK_4_SPELLS_COUNT)
            {
                value = CGumpSpellbook::m_SpellName4[index];
            }

            break;
        }
        case VKS_SPELLBOOK_5_SPELL_NAME:
        {
            int index = atoi(value.c_str());

            if (index >= 0 && index < CGumpSpellbook::SPELLBOOK_5_SPELLS_COUNT)
            {
                value = CGumpSpellbook::m_SpellName5[index];
            }

            break;
        }
        case VKS_SPELLBOOK_6_SPELL_NAME:
        {
            int index = atoi(value.c_str());

            if (index >= 0 && index < CGumpSpellbook::SPELLBOOK_6_SPELLS_COUNT)
            {
                value = CGumpSpellbook::m_SpellName6[index][0];
            }

            break;
        }
        case VKS_SPELLBOOK_7_SPELL_NAME:
        {
            int index = atoi(value.c_str());

            if (index >= 0 && index < CGumpSpellbook::SPELLBOOK_7_SPELLS_COUNT)
            {
                value = CGumpSpellbook::m_SpellName7[index][0];
            }

            break;
        }
        default: break;
    }

    return value;
}

void COrion::ClearRemovedStaticsTextures()
{
    for (std::deque<CIndexObject*>::iterator it = m_UsedStaticList.begin();
         it != m_UsedStaticList.end();)
    {
        CIndexObject* obj = *it;

        if (!obj->LastAccessed.IsSet())
        {
            if (obj->Texture != nullptr)
            {
                delete obj->Texture;
                obj->Texture = nullptr;
            }

            it = m_UsedStaticList.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void COrion::ClearTreesTextures()
{
    for (u16 graphic : m_StumpTiles)
        m_StaticDataIndex[graphic].LastAccessed.Reset();
    ClearRemovedStaticsTextures();
}

bool COrion::InTileFilter(u16 graphic)
{
    if (!m_IgnoreInFilterTiles.empty())
    {
        for (const std::pair<u16, u16>& i : m_IgnoreInFilterTiles)
        {
            if (i.first == graphic || ((i.second != 0u) && IN_RANGE(graphic, i.first, i.second)))
            {
                return true;
            }
        }
    }

    return false;
}

bool COrion::IsTreeTile(u16 graphic, int& index)
{
    if (!g_ConfigManager.GetDrawStumps() || InTileFilter(graphic))
    {
        return false;
    }

    u8 flags = m_StaticTilesFilterFlags[graphic];

    if ((flags & STFF_STUMP) != 0)
    {
        if ((flags & STFF_STUMP_HATCHED) != 0)
        {
            index = g_StumpHatchedID;
        }
        else
        {
            index = g_StumpID;
        }

        return true;
    }

    return false;
}

void COrion::ClearCaveTextures()
{
    for (u16 graphic : m_CaveTiles)
        m_StaticDataIndex[graphic].LastAccessed.Reset();
    ClearRemovedStaticsTextures();
}

bool COrion::IsCaveTile(u16 graphic)
{
    return (
        g_ConfigManager.GetMarkingCaves() &&
        ((m_StaticTilesFilterFlags[graphic] & STFF_CAVE) != 0));
}

bool COrion::IsVegetation(u16 graphic)
{
    return (m_StaticTilesFilterFlags[graphic] & STFF_VEGETATION) != 0;
}

void COrion::LoadLogin(std::string& login, int& port)
{
    login = m_OverrideServerAddress;
    port  = m_OverrideServerPort;
    if (m_OverrideServerPort != 0)
    {
        return;
    }

    login = uo_server_address.GetValue();
    port = uo_server_port.GetValue();
    if (uo_server_port.GetValue() > 0)
    {
        return;
    }

    Core::TextFileParser file(g_App.GetGameDir() / "login.cfg", "=,", "#;", "");
    while (!file.IsEOF())
    {
        auto strings = file.ReadTokens();
        if (strings.size() >= 3)
        {
            std::string lo = Core::ToLowerA(strings[0]);
            if (lo == "loginserver")
            {
                login = strings[1];
                port  = atoi(strings[2].c_str());
            }
        }
    }
}

void COrion::GoToWebLink(const std::string& url)
{
    if (url.length() != 0u)
    {
        std::size_t found = url.find("http://");
        if (found == std::string::npos)
        {
            found = url.find("https://");
        }
        const std::string header = "http://";
        if (found != std::string::npos)
        {
            Platform::OpenBrowser(url.c_str());
        }
        else
        {
            Platform::OpenBrowser((header + url).c_str());
        }
    }
}

void COrion::LoadTiledata(int landSize, int staticsSize)
{
    Core::MappedFile& file = g_FileManager.m_TiledataMul;
    if (file.GetSize() != 0u)
    {
        bool isOldVersion = (GameVars::GetClientVersion() < CV_7090);
        file.ResetPtr();
        m_LandData.resize(landSize * 32);
        m_StaticData.resize(staticsSize * 32);
        for (int i = 0; i < landSize; i++)
        {
            file.ReadLE<u32>();
            for (int j = 0; j < 32; j++)
            {
                LAND_TILES& tile = m_LandData[(i * 32) + j];
                if (isOldVersion)
                    tile.Flags = file.ReadLE<u32>();
                else
                    tile.Flags = file.ReadLE<u64>();

                tile.TexID = file.ReadLE<u16>();
                tile.Name  = file.ReadString(20);
            }
        }

        for (int i = 0; i < staticsSize; i++)
        {
            file.ReadLE<u32>();
            for (int j = 0; j < 32; j++)
            {
                STATIC_TILES& tile = m_StaticData[(i * 32) + j];
                if (isOldVersion)
                    tile.Flags = file.ReadLE<u32>();
                else
                    tile.Flags = file.ReadLE<i64>();

                tile.Weight     = file.ReadLE<i8>();
                tile.Layer      = file.ReadLE<i8>();
                tile.Count      = file.ReadLE<i32>();
                tile.AnimID     = file.ReadLE<i16>();
                tile.Hue        = file.ReadLE<i16>();
                tile.LightIndex = file.ReadLE<i16>();
                tile.Height     = file.ReadLE<i8>();
                tile.Name       = file.ReadString(20);
            }
        }
        file.Unload();
    }
}

void COrion::ReadMulIndexFile(
    size_t indexMaxCount,
    const std::function<CIndexObject*(int index)>& getIdxObj,
    size_t address,
    BASE_IDX_BLOCK* ptr,
    const std::function<BASE_IDX_BLOCK*()>& getNewPtrValue)
{
    for (int i = 0; i < (int)indexMaxCount; i++)
    {
        CIndexObject* obj = getIdxObj((int)i);
        obj->ReadIndexFile(address, ptr, (u16)i);
        ptr = getNewPtrValue();
    }
}

void COrion::ReadUOPIndexFile(
    size_t indexMaxCount,
    const std::function<CIndexObject*(int)>& getIdxObj,
    const char* uopFileName,
    int padding,
    const char* extesion,
    CUopMappedFile& uopFile,
    int startIndex)
{
    std::string p = uopFileName;
    std::transform(p.begin(), p.end(), p.begin(), ::tolower);

    bool isGump        = (std::string("gumpartlegacymul") == p);
    char basePath[200] = { 0 };
    sprintf_s(basePath, "build/%s/%%0%ii%s", p.c_str(), padding, extesion);

    for (int i = startIndex; i < (int)indexMaxCount; i++)
    {
        char hashString[200] = { 0 };
        sprintf_s(hashString, basePath, (int)i);

        CUopBlockHeader* block = uopFile.GetBlock(CreateHash(hashString));

        if (block != nullptr)
        {
            CIndexObject* obj = getIdxObj((int)i);
            obj->Address      = (uintptr_t)uopFile.GetBuffer() + (u32)block->Offset;
            obj->DataSize     = block->DecompressedSize;
            obj->UopBlock     = block;
            obj->ID           = -1;

            if (isGump)
            {
                obj->Address += 8;
                obj->DataSize -= 8;

                uopFile.ResetPtr();
                uopFile.Move((int)block->Offset);

                obj->Width  = uopFile.ReadLE<u32>();
                obj->Height = uopFile.ReadLE<u32>();
            }
        }
    }
}

u64 COrion::CreateHash(const char* s)
{
    const auto l = (u32)strlen(s);
    u32 eax, ecx, edx, ebx, esi, edi;

    eax = ecx = edx = ebx = esi = edi = 0;
    ebx = edi = esi = l + 0xDEADBEEF;

    u32 i = 0;
    for (i = 0; i + 12 < l; i += 12)
    {
        edi = (u32)((s[i + 7] << 24) | (s[i + 6] << 16) | (s[i + 5] << 8) | s[i + 4]) + edi;
        esi = (u32)((s[i + 11] << 24) | (s[i + 10] << 16) | (s[i + 9] << 8) | s[i + 8]) + esi;
        edx = (u32)((s[i + 3] << 24) | (s[i + 2] << 16) | (s[i + 1] << 8) | s[i]) - esi;

        edx = (edx + ebx) ^ (esi >> 28) ^ (esi << 4);
        esi += edi;
        edi = (edi - edx) ^ (edx >> 26) ^ (edx << 6);
        edx += esi;
        esi = (esi - edi) ^ (edi >> 24) ^ (edi << 8);
        edi += edx;
        ebx = (edx - esi) ^ (esi >> 16) ^ (esi << 16);
        esi += edi;
        edi = (edi - ebx) ^ (ebx >> 13) ^ (ebx << 19);
        ebx += esi;
        esi = (esi - edi) ^ (edi >> 28) ^ (edi << 4);
        edi += ebx;
    }

    if (l - i > 0)
    {
        switch (l - i)
        {
            case 12:
                esi += static_cast<u32>(s[i + 11]) << 24;
                goto case_11;
                break;
            case 11:
            case_11:
                esi += static_cast<u32>(s[i + 10]) << 16;
                goto case_10;
                break;
            case 10:
            case_10:
                esi += static_cast<u32>(s[i + 9]) << 8;
                goto case_9;
                break;
            case 9:
            case_9:
                esi += s[i + 8];
                goto case_8;
                break;
            case 8:
            case_8:
                edi += static_cast<u32>(s[i + 7]) << 24;
                goto case_7;
                break;
            case 7:
            case_7:
                edi += static_cast<u32>(s[i + 6]) << 16;
                goto case_6;
                break;
            case 6:
            case_6:
                edi += static_cast<u32>(s[i + 5]) << 8;
                goto case_5;
                break;
            case 5:
            case_5:
                edi += s[i + 4];
                goto case_4;
                break;
            case 4:
            case_4:
                ebx += static_cast<u32>(s[i + 3]) << 24;
                goto case_3;
                break;
            case 3:
            case_3:
                ebx += static_cast<u32>(s[i + 2]) << 16;
                goto case_2;
                break;
            case 2:
            case_2:
                ebx += static_cast<u32>(s[i + 1]) << 8;
                goto case_1;
            case 1:
            case_1:
                ebx += s[i];
                break;
            default: break;
        }

        esi = (esi ^ edi) - ((edi >> 18) ^ (edi << 14));
        ecx = (esi ^ ebx) - ((esi >> 21) ^ (esi << 11));
        edi = (edi ^ ecx) - ((ecx >> 7) ^ (ecx << 25));
        esi = (esi ^ edi) - ((edi >> 16) ^ (edi << 16));
        edx = (esi ^ ecx) - ((esi >> 28) ^ (esi << 4));
        edi = (edi ^ edx) - ((edx >> 18) ^ (edx << 14));
        eax = (esi ^ edi) - ((edi >> 8) ^ (edi << 24));

        return (static_cast<u64>(edi) << 32) | eax;
    }

    return (static_cast<u64>(esi) << 32) | eax;
}

void COrion::LoadIndexFiles()
{
    ART_IDX_BLOCK* LandArtPtr = (ART_IDX_BLOCK*)g_FileManager.m_ArtIdx.GetBuffer();
    ART_IDX_BLOCK* StaticArtPtr =
        (ART_IDX_BLOCK*)((size_t)g_FileManager.m_ArtIdx.GetBuffer() + (m_LandData.size() * sizeof(ART_IDX_BLOCK)));
    GUMP_IDX_BLOCK* GumpArtPtr    = (GUMP_IDX_BLOCK*)g_FileManager.m_GumpIdx.GetBuffer();
    TEXTURE_IDX_BLOCK* TexturePtr = (TEXTURE_IDX_BLOCK*)g_FileManager.m_TextureIdx.GetBuffer();
    MULTI_IDX_BLOCK* MultiPtr     = (MULTI_IDX_BLOCK*)g_FileManager.m_MultiIdx.GetBuffer();
    SOUND_IDX_BLOCK* SoundPtr     = (SOUND_IDX_BLOCK*)g_FileManager.m_SoundIdx.GetBuffer();
    LIGHT_IDX_BLOCK* LightPtr     = (LIGHT_IDX_BLOCK*)g_FileManager.m_LightIdx.GetBuffer();

    if (g_FileManager.m_MultiCollection.GetBuffer() != nullptr)
    {
        g_MultiIndexCount = MAX_MULTI_DATA_INDEX_COUNT;
    }
    else
    {
        g_MultiIndexCount = (int)(g_FileManager.m_MultiIdx.GetSize() / sizeof(MULTI_IDX_BLOCK));

        if (g_MultiIndexCount > MAX_MULTI_DATA_INDEX_COUNT)
        {
            g_MultiIndexCount = MAX_MULTI_DATA_INDEX_COUNT;
        }
    }

    int maxGumpsCount = (int)(g_FileManager.m_GumpIdx.GetBuffer() == nullptr ? MAX_GUMP_DATA_INDEX_COUNT : g_FileManager.m_GumpIdx.GetSize() / sizeof(GUMP_IDX_BLOCK));
    if (g_FileManager.m_ArtMul.GetBuffer() != nullptr)
    {
        ReadMulIndexFile(
            MAX_LAND_DATA_INDEX_COUNT,
            [&](int i) { return &m_LandDataIndex[i]; },
            (size_t)g_FileManager.m_ArtMul.GetBuffer(),
            LandArtPtr,
            [&LandArtPtr]() { return ++LandArtPtr; });
        ReadMulIndexFile(
            m_StaticData.size(),
            [&](int i) { return &m_StaticDataIndex[i]; },
            (size_t)g_FileManager.m_ArtMul.GetBuffer(),
            StaticArtPtr,
            [&StaticArtPtr]() { return ++StaticArtPtr; });
    }
    else
    {
        ReadUOPIndexFile(
            MAX_LAND_DATA_INDEX_COUNT,
            [&](int i) { return &m_LandDataIndex[i]; },
            "artLegacyMUL",
            8,
            ".tga",
            g_FileManager.m_ArtLegacyMUL);
        ReadUOPIndexFile(
            m_StaticData.size() + MAX_LAND_DATA_INDEX_COUNT,
            [&](int i) { return &m_StaticDataIndex[i - MAX_LAND_DATA_INDEX_COUNT]; },
            "artLegacyMUL",
            8,
            ".tga",
            g_FileManager.m_ArtLegacyMUL,
            MAX_LAND_DATA_INDEX_COUNT);
    }

    if (g_FileManager.m_SoundMul.GetBuffer() != nullptr)
    {
        ReadMulIndexFile(
            MAX_SOUND_DATA_INDEX_COUNT,
            [&](int i) { return &m_SoundDataIndex[i]; },
            (size_t)g_FileManager.m_SoundMul.GetBuffer(),
            SoundPtr,
            [&SoundPtr]() { return ++SoundPtr; });
    }
    else
    {
        ReadUOPIndexFile(
            MAX_SOUND_DATA_INDEX_COUNT,
            [&](int i) { return &m_SoundDataIndex[i]; },
            "soundLegacyMUL",
            8,
            ".dat",
            g_FileManager.m_SoundLegacyMUL);
    }

    if (g_FileManager.m_GumpMul.GetBuffer() != nullptr)
    {
        ReadMulIndexFile(
            maxGumpsCount,
            [&](int i) { return &m_GumpDataIndex[i]; },
            (size_t)g_FileManager.m_GumpMul.GetBuffer(),
            GumpArtPtr,
            [&GumpArtPtr]() { return ++GumpArtPtr; });
    }
    else
    {
        ReadUOPIndexFile(
            maxGumpsCount,
            [&](int i) { return &m_GumpDataIndex[i]; },
            "gumpartLegacyMUL",
            8,
            ".tga",
            g_FileManager.m_GumpartLegacyMUL);
    }

    ReadMulIndexFile(
        g_FileManager.m_TextureIdx.GetSize() / sizeof(TEXTURE_IDX_BLOCK),
        [&](int i) { return &m_TextureDataIndex[i]; },
        (size_t)g_FileManager.m_TextureMul.GetBuffer(),
        TexturePtr,
        [&TexturePtr]() { return ++TexturePtr; });
    ReadMulIndexFile(
        MAX_LIGHTS_DATA_INDEX_COUNT,
        [&](int i) { return &m_LightDataIndex[i]; },
        (size_t)g_FileManager.m_LightMul.GetBuffer(),
        LightPtr,
        [&LightPtr]() { return ++LightPtr; });

    if (g_FileManager.m_MultiMul.GetBuffer() != nullptr)
    {
        ReadMulIndexFile(
            g_MultiIndexCount,
            [&](int i) { return &m_MultiDataIndex[i]; },
            (size_t)g_FileManager.m_MultiMul.GetBuffer(),
            MultiPtr,
            [&MultiPtr]() { return ++MultiPtr; });
    }
    else
    {
        CUopMappedFile& file = g_FileManager.m_MultiCollection;
        for (std::unordered_map<u64, CUopBlockHeader>::iterator i = file.m_Map.begin();
             i != file.m_Map.end();
             ++i)
        {
            CUopBlockHeader& block = i->second;
            std::vector<u8> data   = file.GetData(block);

            if (data.empty())
            {
                continue;
            }

            Core::StreamReader reader(&data[0], data.size());

            u32 id = reader.ReadLE<u32>();

            if (id < MAX_MULTI_DATA_INDEX_COUNT)
            {
                CIndexMulti& index = m_MultiDataIndex[id];

                index.Address  = (size_t)file.GetBuffer() + (size_t)block.Offset;
                index.DataSize = block.DecompressedSize;
                index.UopBlock = &i->second;
                index.ID       = -1;
                index.Count    = reader.ReadLE<u32>();
            }
        }

        //ReadUOPIndexFile(g_MultiIndexCount, [&](int i){ return &m_MultiDataIndex[i]; }, "MultiCollection", 6, ".bin", g_FileManager.m_MultiCollection);
    }
}

void COrion::UnloadIndexFiles()
{
    std::deque<CIndexObject*>* lists[5] = {
        &m_UsedLandList, &m_UsedStaticList, &m_UsedGumpList, &m_UsedTextureList, &m_UsedLightList
    };

    for (int i = 0; i < 5; i++)
    {
        auto& list = *lists[i];
        for (auto it = list.begin(); it != list.end(); ++it)
        {
            CIndexObject* obj = *it;
            if (obj->Texture != nullptr)
            {
                delete obj->Texture;
                obj->Texture = nullptr;
            }
        }
        list.clear();
    }

    for (auto it = m_UsedSoundList.begin(); it != m_UsedSoundList.end(); ++it)
    {
        CIndexSound* obj = *it;
        if (obj->m_Stream != SOUND_NULL)
        {
            g_SoundManager.UpdateSoundEffect(obj->m_Stream, -1);
            obj->m_Stream = SOUND_NULL;
        }
    }
    m_UsedSoundList.clear();
}

void COrion::InitStaticAnimList()
{
    if (static_cast<unsigned int>(!m_AnimData.empty()) != 0u)
    {
        uintptr_t lastElement = (uintptr_t)(&m_AnimData[0] + m_AnimData.size() - sizeof(ANIM_DATA));

        for (int i = 0; i < (int)m_StaticData.size(); i++)
        {
            m_StaticDataIndex[i].Index = (u16)i;

            m_StaticDataIndex[i].LightColor = CalculateLightColor((u16)i);

            bool isField = false;

            //fire field
            if (IN_RANGE(i, 0x398C, 0x399F))
            {
                isField = true;
                //paralyze field
            }
            else if (IN_RANGE(i, 0x3967, 0x397A))
            {
                isField = true;
                //energy field
            }
            else if (IN_RANGE(i, 0x3946, 0x3964))
            {
                isField = true;
                //poison field
            }
            else if (IN_RANGE(i, 0x3914, 0x3929))
            {
                isField = true;
            }

            m_StaticDataIndex[i].IsFiled = isField;

            if (IsAnimated(m_StaticData[i].Flags))
            {
                uintptr_t addr   = (uintptr_t)((i * 68) + 4 * ((i / 8) + 1));
                uintptr_t offset = (uintptr_t)(&m_AnimData[0] + addr);

                if (offset <= lastElement)
                {
                    m_StaticAnimList.push_back(&m_StaticDataIndex[i]);
                }
            }
        }
    }
}

u16 COrion::CalculateLightColor(u16 id)
{
    u16 color = 0;

    if (id < 0x3E27)
    {
        //color = ???;
    }
    else
    {
        color = 666;

        //if (id > 0x3E3A)
        //	color = ???;
    }

    switch (id)
    {
        case 0x088C: color = 31; break;
        case 0x0FAC: color = 30; break;
        case 0x0FB1: color = 60; break;
        case 0x1647: color = 61; break;
        case 0x19BB:
        case 0x1F2B: color = 40; break;
        default: break;
    };

    if (id < 0x09FB || id > 0x0A14)
    {
        if (id < 0x0A15 || id > 0x0A29)
        {
            if (id < 0x0B1A || id > 0x0B1F)
            {
                if (id < 0x0B20 || id > 0x0B25)
                {
                    if (id < 0x0B26 || id > 0x0B28)
                    {
                        if (id < 0x0DE1 || id > 0x0DEA)
                        {
                            if (id < 0x1849 || id > 0x1850)
                            {
                                if (id < 0x1853 || id > 0x185A)
                                {
                                    if (id < 0x197A || id > 0x19A9)
                                    {
                                        if (id < 0x19AB || id > 0x19B6)
                                        {
                                            if ((id >= 0x1ECD && id <= 0x1ECF) ||
                                                (id >= 0x1ED0 && id <= 0x1ED2))
                                            {
                                                color = 1;
                                            }
                                        }
                                        else
                                        {
                                            color = 60;
                                        }
                                    }
                                    else
                                    {
                                        color = 60;
                                    }
                                }
                                else
                                {
                                    color = 61;
                                }
                            }
                            else
                            {
                                color = 61;
                            }
                        }
                        else
                        {
                            color = 31;
                        }
                    }
                    else
                    {
                        color = 666;
                    }
                }
                else
                {
                    color = 666;
                }
            }
            else
            {
                color = 666;
            }
        }
        else
        {
            color = 666;
        }
    }
    else
    {
        color = 30;
    }

    if (id == 0x1FD4 || id == 0x0F6C)
    {
        color = 2;
    }

    if (id < 0x0E2D || id > 0x0E30)
    {
        if (id < 0x0E31 || id > 0x0E33)
        {
            if (id < 0x0E5C || id > 0x0E6A)
            {
                if (id < 0x12EE || id > 0x134D)
                {
                    if (id < 0x306A || id > 0x329B)
                    {
                        if (id < 0x343B || id > 0x346C)
                        {
                            if (id < 0x3547 || id > 0x354C)
                            {
                                if (id < 0x3914 || id > 0x3929)
                                {
                                    if (id < 0x3946 || id > 0x3964)
                                    {
                                        if (id < 0x3967 || id > 0x397A)
                                        {
                                            if (id < 0x398C || id > 0x399F)
                                            {
                                                if (id < 0x3E02 || id > 0x3E0B)
                                                {
                                                    if (id < 0x3E27 || id > 0x3E3A)
                                                    {
                                                        switch (id)
                                                        {
                                                            case 0x40FE: color = 40; break;
                                                            case 0x40FF: color = 10; break;
                                                            case 0x4100: color = 20; break;
                                                            case 0x4101: color = 32; break;
                                                            default:
                                                                if ((id >= 0x983B &&
                                                                     id <= 0x983D) ||
                                                                    (id >= 0x983F && id <= 0x9841))
                                                                {
                                                                    color = 30;
                                                                }
                                                                break;
                                                        }
                                                    }
                                                    else
                                                    {
                                                        color = 31;
                                                    }
                                                }
                                                else
                                                {
                                                    color = 1;
                                                }
                                            }
                                            else
                                            {
                                                color = 31;
                                            }
                                        }
                                        else
                                        {
                                            color = 6;
                                        }
                                    }
                                    else
                                    {
                                        color = 6;
                                    }
                                }
                                else
                                {
                                    color = 1;
                                }
                            }
                            else
                            {
                                color = 31;
                            }
                        }
                        else
                        {
                            color = 31;
                        }
                    }
                    else
                    {
                        color = 31;
                    }
                }
                else
                {
                    color = 31;
                }
            }
            else
            {
                color = 6;
            }
        }
        else
        {
            color = 40;
        }
    }
    else
    {
        color = 62;
    }

    /*u16 color = m_StaticData[id].Hue;

	if (!color)
	{
		switch (id)
		{
			case 0x0B1A:
			case 0x0B1B:
			case 0x0B1C:
			case 0x0B1D:
			case 0x0B1E:
			case 0x0B1F:
			case 0x0B20:
			case 0x0B21:
			case 0x0B22:
			case 0x0B23:
			case 0x0B24:
			case 0x0B25:
			case 0x0B26:
			case 0x0B27:
			case 0x0B28:
			{
				color = 0x029A;
				break;
			}
			case 0x0E2D:
			case 0x0E2E:
			case 0x0E2F:
			case 0x0E30:
			{
				color = 0x003E;
				break;
			}
			case 0x088C:
			{
				color = 0x001F;
				break;
			}
			//fire pit
			case 0x0FAC:
			{
				color = 0x001E;
				break;
			}
			//forge
			case 0x0FB1:
			{
				color = 0x003C;
				break;
			}
			case 0x1647:
			{
				color = 0x003D;
				break;
			}
			//blue moongate
			case 0x0F6C:
			//moongate
			case 0x1FD4:
			{
				color = 0x0002;
				break;
			}
			//brazier
			case 0x0E31:
			case 0x0E32:
			case 0x0E33:
			case 0x19BB:
			case 0x1F2B:
			{
				color = 0x0028;
				break;
			}
			//lava
			case 0x3547:
			case 0x3548:
			case 0x3549:
			case 0x354A:
			case 0x354B:
			case 0x354C:
			{
				color = 0x001F;
				break;
			}
			default:
				break;
		}

		if (!color)
		{
			//wall sconce
			if (IN_RANGE(id, 0x09FB, 0x0A14))
				color = 0x001E;
			//lava
			else if (IN_RANGE(id, 0x12EE, 0x134D) || IN_RANGE(id, 0x306A, 0x329B) || IN_RANGE(id, 0x343B, 0x346C))
				color = 0x001F;
			//?
			else if (IN_RANGE(id, 0x3E27, 0x3E3A))
				color = 0x001F;
			//?
			else if (IN_RANGE(id, 0x3E02, 0x3E0B))
				color = 0x0001;
			//fire field
			else if (IN_RANGE(id, 0x398C, 0x399F))
				color = 0x001F;
			//paralyze field
			else if (IN_RANGE(id, 0x3967, 0x397A))
				color = 0x0006;
			//energy field
			else if (IN_RANGE(id, 0x3946, 0x3964))
				color = 0x0006;
			//poison field
			else if (IN_RANGE(id, 0x3914, 0x3929))
				color = 0x0001;
			//glowing runes
			else if (IN_RANGE(id, 0x0E5C, 0x0E6A))
				color = 0x0006;
			//?
			else if (IN_RANGE(id, 0x3E02, 0x3E0B))
				color = 0x0002;
			else if (IN_RANGE(id, 0x3E27, 0x3E3A) || IN_RANGE(id, 0x398C, 0x399F))
				color = 0x029A;
			else if (IN_RANGE(id, 0x1ECD, 0x1ECF) || IN_RANGE(id, 0x1ED0, 0x1ED2))
				color = 0x0001;
			else if (IN_RANGE(id, 0x19AB, 0x19B6) || IN_RANGE(id, 0x197A, 0x19A9))
				color = 0x003C;
			else if (IN_RANGE(id, 0x1853, 0x185A) || IN_RANGE(id, 0x1849, 0x1850))
				color = 0x003D;
			else if (IN_RANGE(id, 0x0DE1, 0x0DEA))
				color = 0x001F;
			else if (IN_RANGE(id, 0x0A15, 0x0A29))
				color = 0x029A;
			else if (IN_RANGE(id, 0x09FB, 0x0A14))
				color = 0x001E;
		}
	}*/

    return color;
}

void COrion::ProcessStaticAnimList()
{
    PROFILER_EVENT();
    if ((static_cast<unsigned int>(!m_AnimData.empty()) != 0u) &&
        g_ProcessStaticAnimationTimer < g_Ticks)
    {
        int delay =
            (g_ConfigManager.StandartItemsAnimationDelay ? ORIGINAL_ITEMS_ANIMATION_DELAY :
                                                           ORION_ITEMS_ANIMATION_DELAY);
        bool noAnimateFields = g_ConfigManager.GetNoAnimateFields();
        u32 nextTime         = g_Ticks + 500;

        for (std::deque<CIndexObjectStatic*>::iterator i = m_StaticAnimList.begin();
             i != m_StaticAnimList.end();
             ++i)
        {
            CIndexObjectStatic& obj = *(*i);

            if (noAnimateFields && obj.IsFiled)
            {
                obj.AnimIndex = 0;
                continue;
            }

            if (obj.ChangeTime < g_Ticks)
            {
                u32 addr       = (obj.Index * 68) + 4 * ((obj.Index / 8) + 1);
                ANIM_DATA& pad = *(ANIM_DATA*)(&m_AnimData[0] + addr);

                int offset = obj.AnimIndex;

                if (pad.FrameInterval > 0)
                {
                    obj.ChangeTime = g_Ticks + (pad.FrameInterval * delay);
                }
                else
                {
                    obj.ChangeTime = g_Ticks + delay;
                }

                if (offset < pad.FrameCount)
                {
                    obj.Offset = pad.FrameData[offset++];
                }

                if (offset >= pad.FrameCount)
                {
                    offset = 0;
                }

                obj.AnimIndex = offset;
            }

            if (obj.ChangeTime < nextTime)
            {
                nextTime = obj.ChangeTime;
            }
        }

        g_ProcessStaticAnimationTimer = nextTime;
    }
}

void COrion::PatchFiles()
{
    /*
	-map0 = 0;
	-staidx0 = 1;
	-statics0 = 2;
	+artidx = 3;
	+art = 4;
	-animidx = 5;
	-anim = 6;
	-soundidx = 7;
	-sound = 8;
	-texidx = 9;
	-texmaps = 10;
	+gumpidx = 11;
	+gumpart = 12;
	+multiidx = 13;
	+multi = 14;
	-skillsidx = 15;
	-skills = 16;
	+tiledata = 30;
	-animdata = 31;
	+hues = 32;
	*/

    Core::MappedFile& file = g_FileManager.m_VerdataMul;
    if (!g_Config.UseVerdata || (file.GetSize() == 0u))
    {
        g_ColorManager.CreateHuesPalette();
        return;
    }

    int dataCount = *(i32*)file.GetBuffer();
    size_t vAddr  = (size_t)file.GetBuffer();
    for (int i = 0; i < dataCount; i++)
    {
        VERDATA_HEADER* vh = (VERDATA_HEADER*)(vAddr + 4 + (i * sizeof(VERDATA_HEADER)));

        if (vh->FileID == 0) //Map0
        {
            g_MapManager.SetPatchedMapBlock(vh->BlockID, vAddr + vh->Position);
        }
        else if (vh->FileID == 4) //Art
        {
            if (vh->BlockID >= MAX_LAND_DATA_INDEX_COUNT) //Run
            {
                u16 ID                         = (u16)vh->BlockID - MAX_LAND_DATA_INDEX_COUNT;
                m_StaticDataIndex[ID].Address  = vAddr + vh->Position;
                m_StaticDataIndex[ID].DataSize = vh->Size;
            }
            else //Raw
            {
                m_LandDataIndex[vh->BlockID].Address  = vAddr + vh->Position;
                m_LandDataIndex[vh->BlockID].DataSize = vh->Size;
            }
        }
        else if (vh->FileID == 12) //Gumpart
        {
            m_GumpDataIndex[vh->BlockID].Address  = vAddr + vh->Position;
            m_GumpDataIndex[vh->BlockID].DataSize = vh->Size;
            m_GumpDataIndex[vh->BlockID].Width    = vh->GumpData >> 16;
            m_GumpDataIndex[vh->BlockID].Height   = vh->GumpData & 0xFFFF;
        }
        else if (vh->FileID == 14 && (int)vh->BlockID < g_MultiIndexCount) //Multi
        {
            m_MultiDataIndex[vh->BlockID].Address  = vAddr + vh->Position;
            m_MultiDataIndex[vh->BlockID].DataSize = vh->Size;
            m_MultiDataIndex[vh->BlockID].Count    = u16(vh->Size / sizeof(MULTI_IDX_BLOCK));
        }
        else if (vh->FileID == 16 && (int)vh->BlockID < g_SkillsManager.Count) //Skills
        {
            CSkill* skill = g_SkillsManager.Get(vh->BlockID);

            if (skill != nullptr)
            {
                Core::StreamReader reader((u8*)vAddr + vh->Position, vh->Size);
                skill->Button = (reader.ReadLE<u8>() != 0);
                skill->Name   = reader.ReadString(vh->Size - 1);
            }
        }
        else if (vh->FileID == 30) //Tiledata
        {
            file.ResetPtr();
            file.Move(vh->Position);

            if (vh->Size == 836)
            {
                int offset = vh->BlockID * 32;
                if (offset + 32 > (int)m_LandData.size())
                    continue;
                file.ReadLE<u32>();
                for (int j = 0; j < 32; j++)
                {
                    LAND_TILES& tile = m_LandData[offset + j];
                    if (GameVars::GetClientVersion() < CV_7090)
                        tile.Flags = file.ReadLE<u32>();
                    else
                        tile.Flags = file.ReadLE<i64>();
                    tile.TexID = file.ReadLE<u16>();
                    tile.Name  = file.ReadString(20);
                }
            }
            else if (vh->Size == 1188)
            {
                int offset = (vh->BlockID - 0x0200) * 32;

                if (offset + 32 > (int)m_StaticData.size())
                {
                    continue;
                }

                file.ReadLE<u32>();
                for (int j = 0; j < 32; j++)
                {
                    STATIC_TILES& tile = m_StaticData[offset + j];
                    if (GameVars::GetClientVersion() < CV_7090)
                        tile.Flags = file.ReadLE<u32>();
                    else
                        tile.Flags = file.ReadLE<i64>();
                    tile.Weight     = file.ReadLE<i8>();
                    tile.Layer      = file.ReadLE<i8>();
                    tile.Count      = file.ReadLE<i32>();
                    tile.AnimID     = file.ReadLE<i16>();
                    tile.Hue        = file.ReadLE<i16>();
                    tile.LightIndex = file.ReadLE<i16>();
                    tile.Height     = file.ReadLE<i8>();
                    tile.Name       = file.ReadString(20);
                }
            }
        }
        else if (vh->FileID == 32) //Hues
        {
            if ((int)vh->BlockID < g_ColorManager.GetHuesCount())
            {
                VERDATA_HUES_GROUP* group = (VERDATA_HUES_GROUP*)(vAddr + vh->Position);
                g_ColorManager.SetHuesBlock(vh->BlockID, group);
            }
        }
        else if (vh->FileID != 5 && vh->FileID != 6) //no Anim / Animidx
        {
            LOG_WARNING(
                "Client",
                "Unused verdata block (fileID) = %i (BlockID+ %i\n",
                vh->FileID,
                vh->BlockID);
        }
    }

    g_ColorManager.CreateHuesPalette();
}

void COrion::IndexReplaces()
{
    if (GameVars::GetClientVersion() < CV_305D)
    { //CV_204C
        return;
    }

    Core::TextFileParser newDataParser({}, " \t,{}", "#;//", "");
    Core::TextFileParser artParser(
        g_App.GetGameDir() / "art.def", " \t", "#;//", "{}"); // FIXME: case insensitive
    Core::TextFileParser textureParser(g_App.GetGameDir() / "TexTerr.def", " \t", "#;//", "{}");
    Core::TextFileParser gumpParser(
        g_App.GetGameDir() / "gump.def", " \t", "#;//", "{}"); // FIXME: case insensitive
    Core::TextFileParser multiParser(g_App.GetGameDir() / "Multi.def", " \t", "#;//", "{}");
    Core::TextFileParser soundParser(g_App.GetGameDir() / "Sound.def", " \t", "#;//", "{}");
    Core::TextFileParser mp3Parser(g_App.GetGameDir() / "Music/Digital/Config.txt", " ,", "#;", "");

    LOG_INFO("Client", "Replace arts");
    while (!artParser.IsEOF())
    {
        std::vector<std::string> strings = artParser.ReadTokens();
        if (strings.size() >= 3)
        {
            int index = atoi(strings[0].c_str());
            if (index < 0 || index >= MAX_LAND_DATA_INDEX_COUNT + (int)m_StaticData.size())
                continue;

            std::vector<std::string> newArt = newDataParser.GetTokens(strings[1].c_str());
            int size                        = (int)newArt.size();
            for (int i = 0; i < size; i++)
            {
                int checkIndex = atoi(newArt[i].c_str());
                if (checkIndex < 0 ||
                    checkIndex >= MAX_LAND_DATA_INDEX_COUNT + (int)m_StaticData.size())
                    continue;

                if (index < MAX_LAND_DATA_INDEX_COUNT && checkIndex < MAX_LAND_DATA_INDEX_COUNT &&
                    m_LandDataIndex[checkIndex].Address != 0 && m_LandDataIndex[index].Address == 0)
                {
                    m_LandDataIndex[index]         = m_LandDataIndex[checkIndex];
                    m_LandDataIndex[index].Texture = 0;
                    m_LandDataIndex[index].Color   = atoi(strings[2].c_str());
                    break;
                }
                if (index >= MAX_LAND_DATA_INDEX_COUNT && checkIndex >= MAX_LAND_DATA_INDEX_COUNT)
                {
                    checkIndex -= MAX_LAND_DATA_INDEX_COUNT;
                    checkIndex &= 0x3FFF;
                    index -= MAX_LAND_DATA_INDEX_COUNT;

                    if (m_StaticDataIndex[index].Address == 0 &&
                        m_StaticDataIndex[checkIndex].Address != 0)
                    {
                        m_StaticDataIndex[index]         = m_StaticDataIndex[checkIndex];
                        m_StaticDataIndex[index].Texture = 0;
                        m_StaticDataIndex[index].Color   = atoi(strings[2].c_str());
                        break;
                    }
                }
            }
        }
    }

    LOG_INFO("Client", "Replace textures");
    while (!textureParser.IsEOF())
    {
        std::vector<std::string> strings = textureParser.ReadTokens();
        if (strings.size() >= 3)
        {
            int index = atoi(strings[0].c_str());
            if (index < 0 || index >= MAX_LAND_TEXTURES_DATA_INDEX_COUNT ||
                m_TextureDataIndex[index].Address != 0)
                continue;

            std::vector<std::string> newTexture = newDataParser.GetTokens(strings[1].c_str());
            int size                            = (int)newTexture.size();
            for (int i = 0; i < size; i++)
            {
                int checkIndex = atoi(newTexture[i].c_str());
                if (checkIndex < 0)
                    continue;

                if (index < TexturesDataCount && checkIndex < TexturesDataCount &&
                    m_TextureDataIndex[checkIndex].Address != 0)
                {
                    m_TextureDataIndex[index]         = m_TextureDataIndex[checkIndex];
                    m_TextureDataIndex[index].Texture = 0;
                    m_TextureDataIndex[index].Color   = atoi(strings[2].c_str());
                    break;
                }
            }
        }
    }

    LOG_INFO("Client", "Replace gumps");
    while (!gumpParser.IsEOF())
    {
        std::vector<std::string> strings = gumpParser.ReadTokens();
        if (strings.size() >= 3)
        {
            int index = atoi(strings[0].c_str());
            if (index < 0 || index >= MAX_GUMP_DATA_INDEX_COUNT ||
                m_GumpDataIndex[index].Address != 0)
                continue;

            std::vector<std::string> newGump = newDataParser.GetTokens(strings[1].c_str());
            int size                         = (int)newGump.size();
            for (int i = 0; i < size; i++)
            {
                int checkIndex = atoi(newGump[i].c_str());
                if (checkIndex < 0 || checkIndex >= MAX_GUMP_DATA_INDEX_COUNT ||
                    m_GumpDataIndex[checkIndex].Address == 0)
                    continue;

                m_GumpDataIndex[index]         = m_GumpDataIndex[checkIndex];
                m_GumpDataIndex[index].Texture = 0;
                m_GumpDataIndex[index].Color   = atoi(strings[2].c_str());
                break;
            }
        }
    }

    LOG_INFO("Client", "Replace multi");
    while (!multiParser.IsEOF())
    {
        std::vector<std::string> strings = multiParser.ReadTokens();
        if (strings.size() >= 3)
        {
            int index = atoi(strings[0].c_str());
            if (index < 0 || index >= g_MultiIndexCount || m_MultiDataIndex[index].Address != 0)
                continue;

            std::vector<std::string> newMulti = newDataParser.GetTokens(strings[1].c_str());
            int size                          = (int)newMulti.size();
            for (int i = 0; i < size; i++)
            {
                int checkIndex = atoi(newMulti[i].c_str());
                if (checkIndex < 0 || checkIndex >= g_MultiIndexCount ||
                    m_MultiDataIndex[checkIndex].Address == 0)
                    continue;

                m_MultiDataIndex[index] = m_MultiDataIndex[checkIndex];
                break;
            }
        }
    }

    LOG_INFO("Client", "Replace sounds");
    while (!soundParser.IsEOF())
    {
        std::vector<std::string> strings = soundParser.ReadTokens();

        if (strings.size() >= 2)
        {
            int index = atoi(strings[0].c_str());

            if (index < 0 || index >= MAX_SOUND_DATA_INDEX_COUNT ||
                m_SoundDataIndex[index].Address != 0)
            {
                continue;
            }

            std::vector<std::string> newSound = newDataParser.GetTokens(strings[1].c_str());

            int size = (int)newSound.size();

            for (int i = 0; i < size; i++)
            {
                int checkIndex = atoi(newSound[i].c_str());

                if (checkIndex < -1 || checkIndex >= MAX_SOUND_DATA_INDEX_COUNT)
                {
                    continue;
                }

                CIndexSound& in = m_SoundDataIndex[index];

                if (checkIndex == -1)
                {
                    in.Address  = 0;
                    in.DataSize = 0;
                    in.Delay    = 0;
                    in.LastAccessed.Reset();
                }
                else
                {
                    CIndexSound& out = m_SoundDataIndex[checkIndex];

                    if (out.Address == 0)
                    {
                        continue;
                    }

                    in.Address      = out.Address;
                    in.DataSize     = out.DataSize;
                    in.Delay        = out.Delay;
                    in.LastAccessed = out.LastAccessed;
                }

                free(in.m_WaveFile);
                in.m_WaveFile = nullptr;
                in.m_Stream   = nullptr;

                break;
            }
        }
    }

    LOG_INFO("Client", "Loading mp3 config");
    while (!mp3Parser.IsEOF())
    {
        std::vector<std::string> strings = mp3Parser.ReadTokens();
        size_t size                      = strings.size();
        if (size > 0)
        {
            u32 index             = std::atoi(strings[0].c_str());
            CIndexMusic& mp3      = m_MP3Data[index];
            std::string name      = "music/digital/" + strings[1];
            std::string extension = ".mp3";
            if (name.find(extension) == std::string::npos)
                name += extension;
            if (size > 1)
                mp3.FilePath = (g_App.GetGameDir() / name).string();
            if (size > 2)
                mp3.Loop = true;
        }
    }
}

void COrion::CreateAuraTexture()
{
    std::vector<u32> pixels;
    short width  = 0;
    short height = 0;

    CGLTextureCircleOfTransparency::CreatePixels(30, width, height, pixels);

    for (int i = 0; i < (int)pixels.size(); i++)
    {
        u32& pixel = pixels[i];

        if (pixel != 0u)
        {
            u16 value = pixel << 3;

            if (value > 0xFF)
            {
                value = 0xFF;
            }

            pixel = (value << 24) | (value << 16) | (value << 8) | value;
        }
    }

    g_GL_BindTexture32(g_AuraTexture, width, height, &pixels[0]);
}

void COrion::CreateObjectHandlesBackground()
{
    CGLTexture* th[9] = { nullptr };
    u16 gumpID[9]     = { 0 };

    for (int i = 0; i < 9; i++)
    {
        CGLTexture* pth = ExecuteGump(0x24EA + (u16)i);

        if (pth == nullptr)
        {
            LOG_ERROR("Client", "Error!!! Failed to create Object Handles background data!");
            return;
        }

        if (i == 4)
        {
            th[8]     = pth;
            gumpID[8] = 0x24EA + (u16)i;
        }
        else if (i > 4)
        {
            th[i - 1]     = pth;
            gumpID[i - 1] = 0x24EA + (u16)i;
        }
        else
        {
            th[i]     = pth;
            gumpID[i] = 0x24EA + (u16)i;
        }
    }

    for (int i = 0; i < 8; i++)
    {
        if (i == 3 || i == 4)
        {
            continue;
        }

        CIndexObject& io = m_GumpDataIndex[gumpID[i]];

        int drawWidth  = io.Width;
        int drawHeight = io.Height;
        int drawX      = 0;
        int drawY      = 0;

        switch (i)
        {
            case 1:
            {
                drawX = th[0]->Width;

                drawWidth = g_ObjectHandlesWidth - th[0]->Width - th[2]->Width;

                break;
            }
            case 2:
            {
                drawX = g_ObjectHandlesWidth - drawWidth;

                break;
            }
            case 3:
            {
                drawY = th[0]->Height;

                drawHeight = g_ObjectHandlesHeight - th[0]->Height - th[5]->Height;

                break;
            }
            case 4:
            {
                drawX = g_ObjectHandlesWidth - drawWidth;
                drawY = th[2]->Height;

                drawHeight = g_ObjectHandlesHeight - th[2]->Height - th[7]->Height;

                break;
            }
            case 5:
            {
                drawY = g_ObjectHandlesHeight - drawHeight;

                break;
            }
            case 6:
            {
                drawX = th[5]->Width;
                drawY = g_ObjectHandlesHeight - drawHeight;

                drawWidth = g_ObjectHandlesWidth - th[5]->Width - th[7]->Width;

                break;
            }
            case 7:
            {
                drawX = g_ObjectHandlesWidth - drawWidth;
                drawY = g_ObjectHandlesHeight - drawHeight;

                break;
            }
            case 8:
            {
                drawX = th[0]->Width;
                drawY = th[0]->Height;

                drawWidth  = g_ObjectHandlesWidth - th[0]->Width - th[2]->Width;
                drawHeight = g_ObjectHandlesHeight - th[2]->Height - th[7]->Height;

                break;
            }
            default: break;
        }

        if (drawX < 0)
        {
            drawX = 0;
        }

        if (drawY < 0)
        {
            drawY = 0;
        }

        drawWidth += drawX;
        if (drawWidth > g_ObjectHandlesWidth)
        {
            drawWidth = g_ObjectHandlesWidth - drawX;
        }

        drawHeight += drawY;
        if (drawHeight > g_ObjectHandlesHeight)
        {
            drawHeight = g_ObjectHandlesHeight - drawY;
        }

        std::vector<u16> pixels = g_UOFileReader.GetGumpPixels(io);

        if (static_cast<unsigned int>(!pixels.empty()) != 0u)
        {
            int gumpWidth  = io.Width;
            int gumpHeight = io.Height;
            int srcX       = 0;

            for (int x = drawX; x < drawWidth; x++)
            {
                int srcY = 0;

                for (int y = drawY; y < drawHeight; y++)
                {
                    u16& pixel = g_ObjectHandlesBackgroundPixels[(y * g_ObjectHandlesWidth) + x];

                    if (pixel == 0)
                    {
                        pixel = pixels[((srcY % gumpHeight) * gumpWidth) + (srcX % gumpWidth)];
                    }

                    srcY++;
                }

                srcX++;
            }
        }
    }
}

void COrion::LoadShaders()
{
    g_DeathShader.Init(resources::shaders::Shader_vert_data, resources::shaders::DeathShader_frag_data);
    g_ColorizerShader.Init(resources::shaders::Shader_vert_data, resources::shaders::ColorizerShader_frag_data);
    g_FontColorizerShader.Init(resources::shaders::Shader_vert_data, resources::shaders::FontColorizerShader_frag_data);
    g_LightColorizerShader.Init(resources::shaders::Shader_vert_data, resources::shaders::LightColorizerShader_frag_data);
}

void COrion::LoadClientStartupConfig()
{
    if (!g_ConfigManager.Load(g_App.GetExeDir() / "sot_options.cfg") &&
        !g_ConfigManager.Load(g_App.GetGameDir() / "sot_options.cfg") &&
        !g_ConfigManager.LoadBin(g_App.GetExeDir() / "options_debug.cuo"))
        g_ConfigManager.LoadBin(g_App.GetGameDir() / "options_debug.cuo");

    g_SoundManager.SetMusicVolume(g_ConfigManager.GetMusicVolume());
    if (g_ConfigManager.GetMusic())
    {
        if (GameVars::GetClientVersion() >= CV_7000)
            PlayMusic(78);
        else if (GameVars::GetClientVersion() > CV_308Z)
            PlayMusic(0); //from 4.x the music played is 0, the first one
        else
            PlayMusic(8);
    }
}

void COrion::PlayMusic(int index, bool warmode)
{
    if (!g_ConfigManager.GetMusic() || index >= MAX_MUSIC_DATA_INDEX_COUNT)
    {
        return;
    }
    if (!warmode && g_SoundManager.CurrentMusicIndex == index &&
        g_SoundManager.IsPlayingNormalMusic())
    {
        return;
    }

    if (GameVars::GetClientVersion() >= CV_306E)
    {
        CIndexMusic& mp3Info = m_MP3Data[index];
        g_SoundManager.PlayMP3(mp3Info.FilePath, index, mp3Info.Loop, warmode);
    }
    else
    {
        g_SoundManager.PlayMidi(index, warmode);
    }
}

void COrion::PlaySoundEffectAtPosition(u16 id, int x, int y)
{
    auto distance = GetDistance(g_Player, Core::Vec2<i32>(x, y));
    g_Orion.PlaySoundEffect(id, g_SoundManager.GetVolumeValue(distance));
}

void COrion::PlaySoundEffect(u16 id, float volume)
{
    if (id >= 0x0800 || !g_ConfigManager.GetSound())
    {
        return;
    }

    CIndexSound& is = m_SoundDataIndex[id];
    if (is.Address == 0)
    {
        return;
    }

    if (is.m_Stream == SOUND_NULL)
    {
        is.m_Stream = g_SoundManager.LoadSoundEffect(is);
        if (is.m_Stream == SOUND_NULL)
        {
            return;
        }
        m_UsedSoundList.push_back(&m_SoundDataIndex[id]);
    }
    else
    {
        if (is.LastAccessed + Core::TimeDiff::FromMilliseconds(is.Delay) > Core::FrameTimer::Now())
        {
            return;
        }
        g_SoundManager.FreeSound(is.m_Stream);
        is.m_Stream = g_SoundManager.LoadSoundEffect(is);
    }

    if (volume <= 0)
    {
        volume = g_SoundManager.GetVolumeValue();
    }

    if (volume > 0)
    {
        g_SoundManager.PlaySoundEffect(is.m_Stream, volume);
        is.LastAccessed = Core::FrameTimer::Now();
    }
}

void COrion::AdjustSoundEffects(int ticks, float volume)
{
    for (auto it = m_UsedSoundList.begin(); it != m_UsedSoundList.end();)
    {
        CIndexSound* obj = *it;
        if (obj->m_Stream == SOUND_NULL)
        {
            ++it;
            continue;
        }
        if (obj->LastAccessed + Core::TimeDiff::FromMilliseconds(obj->Delay) < Core::FrameTimer::Now())
        {
            if (!g_SoundManager.UpdateSoundEffect(obj->m_Stream, volume))
            {
                obj->m_Stream = SOUND_NULL;
            }
            it = m_UsedSoundList.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void COrion::PauseSound() const
{
    g_SoundManager.PauseSound();
}

void COrion::ResumeSound() const
{
    g_SoundManager.ResumeSound();
}

CGLTexture* COrion::ExecuteGump(u16 id)
{
    if (id >= MAX_GUMP_DATA_INDEX_COUNT)
    {
        return nullptr;
    }

    CIndexObject& io = m_GumpDataIndex[id];
    if (io.Texture == 0)
    {
        if (io.Address == 0u)
        {
            return nullptr;
        }

        io.Texture = g_UOFileReader.ReadGump(io);

        if (io.Texture != 0)
        {
            m_UsedGumpList.push_back(&m_GumpDataIndex[id]);
        }
    }

    io.LastAccessed = Core::FrameTimer::Now();

    return io.Texture;
}

CGLTexture* COrion::ExecuteLandArt(u16 id)
{
    if (id >= MAX_LAND_DATA_INDEX_COUNT)
    {
        return nullptr;
    }
    CIndexObject& io = m_LandDataIndex[id];

    if (io.Texture == 0)
    {
        if (io.Address == 0u)
        { //nodraw tiles banned
            return nullptr;
        }

        io.Texture = g_UOFileReader.ReadArt(id, io, false);

        if (io.Texture != 0)
        {
            m_UsedLandList.push_back(&m_LandDataIndex[id]);
        }
    }

    io.LastAccessed = Core::FrameTimer::Now();

    return io.Texture;
}

CGLTexture* COrion::ExecuteStaticArtAnimated(u16 id)
{
    return ExecuteStaticArt(id + m_StaticDataIndex[id].Offset);
}

CGLTexture* COrion::ExecuteStaticArt(u16 id)
{
    if (id >= MAX_STATIC_DATA_INDEX_COUNT)
    {
        return nullptr;
    }
    CIndexObject& io = m_StaticDataIndex[id];

    if (io.Texture == 0)
    {
        if (io.Address == 0u)
        { //nodraw tiles banned
            return nullptr;
        }

        io.Texture = g_UOFileReader.ReadArt(id, io, true);

        if (io.Texture != 0)
        {
            io.Width  = ((io.Texture->Width / 2) + 1);
            io.Height = io.Texture->Height - 20;

            m_UsedStaticList.push_back(&m_StaticDataIndex[id]);
        }
    }

    io.LastAccessed = Core::FrameTimer::Now();

    return io.Texture;
}

CGLTexture* COrion::ExecuteTexture(u16 id)
{
    id = m_LandData[id].TexID;

    if ((id == 0u) || id >= MAX_LAND_TEXTURES_DATA_INDEX_COUNT)
    {
        return nullptr;
    }

    CIndexObject& io = m_TextureDataIndex[id];

    if (io.Texture == 0)
    {
        if (io.Address == 0u)
        {
            return nullptr;
        }

        io.Texture = g_UOFileReader.ReadTexture(io);

        if (io.Texture != 0)
        {
            m_UsedTextureList.push_back(&m_TextureDataIndex[id]);
        }
    }

    io.LastAccessed = Core::FrameTimer::Now();

    return io.Texture;
}

CGLTexture* COrion::ExecuteLight(u8& id)
{
    if (id >= MAX_LIGHTS_DATA_INDEX_COUNT)
    {
        id = 0;
    }

    CIndexObject& io = m_LightDataIndex[id];

    if (io.Texture == 0)
    {
        if (io.Address == 0u)
        {
            return nullptr;
        }

        io.Texture = g_UOFileReader.ReadLight(io);

        if (io.Texture != 0)
        {
            m_UsedLightList.push_back(&m_LightDataIndex[id]);
        }
    }

    io.LastAccessed = Core::FrameTimer::Now();

    return io.Texture;
}

std::pair<CGLTexture*, Core::Vec2<i16>> COrion::ExecuteCursor(u16 a_id)
{
    if (a_id >= MAX_STATIC_DATA_INDEX_COUNT)
        return { nullptr, {} };

    CIndexObject& io = m_StaticDataIndex[a_id];
    auto cursor      = g_UOFileReader.ReadCursor(a_id, io, true);
    if (io.Texture == 0)
    {
        io.Texture = cursor.first;
        if (io.Texture != 0)
        {
            io.Width  = ((io.Texture->Width / 2) + 1);
            io.Height = io.Texture->Height - 20;
            m_UsedStaticList.push_back(&m_StaticDataIndex[a_id]);
        }
    }
    io.LastAccessed = Core::TimeStamp::Max();
    return cursor;
}

bool COrion::ExecuteGumpPart(u16 id, int count)
{
    bool result = true;

    for (int i = 0; i < count; i++)
    {
        if (ExecuteGump(id + (u16)i) == nullptr)
        {
            result = false;
        }
    }

    return result;
}

void COrion::DrawGump(u16 id, u16 color, int x, int y, bool partialHue)
{
    CGLTexture* th = ExecuteGump(id);

    if (th != nullptr)
    {
        if (!g_GrayedPixels && (color != 0u))
        {
            if (partialHue)
            {
                glUniform1iARB(g_ShaderDrawMode, SDM_PARTIAL_HUE);
            }
            else
            {
                glUniform1iARB(g_ShaderDrawMode, SDM_COLORED);
            }

            g_ColorManager.SendColorsToShader(color);
        }
        else
        {
            glUniform1iARB(g_ShaderDrawMode, SDM_NO_COLOR);
        }

        th->Draw(x, y);
    }
}

void COrion::DrawGump(u16 id, u16 color, int x, int y, int width, int height, bool partialHue)
{
    CGLTexture* th = ExecuteGump(id);

    if (th != nullptr)
    {
        if (!g_GrayedPixels && (color != 0u))
        {
            if (partialHue)
            {
                glUniform1iARB(g_ShaderDrawMode, SDM_PARTIAL_HUE);
            }
            else
            {
                glUniform1iARB(g_ShaderDrawMode, SDM_COLORED);
            }

            g_ColorManager.SendColorsToShader(color);
        }
        else
        {
            glUniform1iARB(g_ShaderDrawMode, SDM_NO_COLOR);
        }

        th->Draw(x, y, width, height);
    }
}

void COrion::DrawResizepicGump(u16 id, int x, int y, int width, int height)
{
    CGLTexture* th[9] = { nullptr };

    for (int i = 0; i < 9; i++)
    {
        CGLTexture* pth = ExecuteGump(id + (u16)i);

        if (pth == nullptr)
        {
            return;
        }

        if (i == 4)
        {
            th[8] = pth;
        }
        else if (i > 4)
        {
            th[i - 1] = pth;
        }
        else
        {
            th[i] = pth;
        }
    }

    g_GL_DrawResizepic(th, x, y, width, height);
}

void COrion::DrawLandTexture(CLandObject* land, u16 color, int x, int y)
{
    u16 id = land->Graphic;

    CGLTexture* th = ExecuteTexture(id);

    if (th == nullptr)
    {
        DrawLandArt(id, color, x, y);
    }
    else
    {
        if (g_OutOfRangeColor != 0u)
        {
            color = g_OutOfRangeColor;
        }

        if (!g_GrayedPixels && (color != 0u))
        {
            glUniform1iARB(g_ShaderDrawMode, SDM_LAND_COLORED);
            g_ColorManager.SendColorsToShader(color);
        }
        else
        {
            glUniform1iARB(g_ShaderDrawMode, SDM_LAND);
        }

        g_GL_DrawLandTexture(*th, x, y + (land->GetZ() * 4), land);
    }
}

void COrion::DrawLandArt(u16 id, u16 color, int x, int y)
{
    CGLTexture* th = ExecuteLandArt(id);

    if (th != nullptr)
    {
        if (g_OutOfRangeColor != 0u)
        {
            color = g_OutOfRangeColor;
        }

        if (!g_GrayedPixels && (color != 0u))
        {
            glUniform1iARB(g_ShaderDrawMode, SDM_COLORED);
            g_ColorManager.SendColorsToShader(color);
        }
        else
        {
            glUniform1iARB(g_ShaderDrawMode, SDM_NO_COLOR);
        }

        th->Draw(x - 22, y - 22);
    }
}

void COrion::DrawStaticArt(u16 id, u16 color, int x, int y, bool selection)
{
    CGLTexture* th = ExecuteStaticArt(id);

    if (th != nullptr && id > 1)
    {
        if (g_OutOfRangeColor != 0u)
        {
            color = g_OutOfRangeColor;
        }

        if (!g_GrayedPixels && (color != 0u))
        {
            if (!selection && IsPartialHue(GetStaticFlags(id)))
            {
                glUniform1iARB(g_ShaderDrawMode, SDM_PARTIAL_HUE);
            }
            else
            {
                glUniform1iARB(g_ShaderDrawMode, SDM_COLORED);
            }

            g_ColorManager.SendColorsToShader(color);
        }
        else
        {
            glUniform1iARB(g_ShaderDrawMode, SDM_NO_COLOR);
        }

        th->Draw(x - m_StaticDataIndex[id].Width, y - m_StaticDataIndex[id].Height);
    }
}

void COrion::DrawStaticArtAnimated(u16 id, u16 color, int x, int y, bool selection)
{
    DrawStaticArt(id + m_StaticDataIndex[id].Offset, color, x, y, selection);
}

void COrion::DrawStaticArtRotated(u16 id, u16 color, int x, int y, float angle)
{
    CGLTexture* th = ExecuteStaticArt(id);

    if (th != nullptr && id > 1)
    {
        if (g_OutOfRangeColor != 0u)
        {
            color = g_OutOfRangeColor;
        }

        if (!g_GrayedPixels && (color != 0u))
        {
            glUniform1iARB(g_ShaderDrawMode, SDM_COLORED);
            g_ColorManager.SendColorsToShader(color);
        }
        else
        {
            glUniform1iARB(g_ShaderDrawMode, SDM_NO_COLOR);
        }

        th->DrawRotated(x, y, angle);
    }
}

void COrion::DrawStaticArtAnimatedRotated(u16 id, u16 color, int x, int y, float angle)
{
    DrawStaticArtRotated(id + m_StaticDataIndex[id].Offset, color, x, y, angle);
}

void COrion::DrawStaticArtTransparent(u16 id, u16 color, int x, int y, bool selection)
{
    CGLTexture* th = ExecuteStaticArt(id);

    if (th != nullptr && id > 1)
    {
        if (g_OutOfRangeColor != 0u)
        {
            color = g_OutOfRangeColor;
        }

        if (!g_GrayedPixels && (color != 0u))
        {
            if (!selection && IsPartialHue(GetStaticFlags(id)))
            {
                glUniform1iARB(g_ShaderDrawMode, SDM_PARTIAL_HUE);
            }
            else
            {
                glUniform1iARB(g_ShaderDrawMode, SDM_COLORED);
            }

            g_ColorManager.SendColorsToShader(color);
        }
        else
        {
            glUniform1iARB(g_ShaderDrawMode, SDM_NO_COLOR);
        }

        th->DrawTransparent(x - m_StaticDataIndex[id].Width, y - m_StaticDataIndex[id].Height);
    }
}

void COrion::DrawStaticArtAnimatedTransparent(u16 id, u16 color, int x, int y, bool selection)
{
    DrawStaticArtTransparent(id + m_StaticDataIndex[id].Offset, color, x, y, selection);
}

void COrion::DrawStaticArtInContainer(u16 id, u16 color, int x, int y, bool selection, bool onMouse)
{
    CGLTexture* th = ExecuteStaticArt(id);

    if (th != nullptr)
    {
        if (onMouse)
        {
            x -= th->Width / 2;
            y -= th->Height / 2;
        }

        if (!g_GrayedPixels && (color != 0u))
        {
            if (color >= 0x4000)
            {
                color = 0x1;
                glUniform1iARB(g_ShaderDrawMode, SDM_COLORED);
            }
            else if (!selection && IsPartialHue(GetStaticFlags(id)))
            {
                glUniform1iARB(g_ShaderDrawMode, SDM_PARTIAL_HUE);
            }
            else
            {
                glUniform1iARB(g_ShaderDrawMode, SDM_COLORED);
            }

            g_ColorManager.SendColorsToShader(color);
        }
        else
        {
            glUniform1iARB(g_ShaderDrawMode, SDM_NO_COLOR);
        }

        th->Draw(x, y);
    }
}

void COrion::DrawLight(LIGHT_DATA& a_light)
{
    CGLTexture* th = ExecuteLight(a_light.ID);
    if (th != nullptr)
    {
        if (a_light.Color != 0u)
        {
            glUniform1iARB(g_ShaderDrawMode, SDM_COLORED);
            g_ColorManager.SendColorsToShader(a_light.Color);
        }
        else
        {
            glUniform1iARB(g_ShaderDrawMode, SDM_NO_COLOR);
        }

        th->Draw(
            a_light.DrawX - g_RenderBounds.GameWindowPosX - (th->Width / 2),
            a_light.DrawY - g_RenderBounds.GameWindowPosY - (th->Height / 2));
    }
}

bool COrion::PolygonePixelsInXY(int a_x, int a_y, int a_width, int a_height)
{
    Core::TMousePos pos = g_MouseManager.GetPosition();
    a_x                 = pos.x - a_x;
    a_y                 = pos.y - a_y;
    return !(a_x < 0 || a_y < 0 || a_x >= a_width || a_y >= a_height);
}

bool COrion::GumpPixelsInXY(u16 a_id, int a_x, int a_y)
{
    CGLTexture* texture = m_GumpDataIndex[a_id].Texture;
    return texture ? texture->Select(a_x, a_y) : false;
}

bool COrion::GumpPixelsInXY(u16 a_id, int a_x, int a_y, int a_width, int a_height)
{
    CGLTexture* texture = m_GumpDataIndex[a_id].Texture;

    if (texture == nullptr)
        return false;

    Core::TMousePos pos = g_MouseManager.GetPosition();
    a_x                 = pos.x - a_x;
    a_y                 = pos.y - a_y;

    if (a_x < 0 || a_y < 0 || (a_width > 0 && a_x >= a_width) || (a_height > 0 && a_y >= a_height))
        return false;

    int textureWidth  = texture->Width;
    int textureHeight = texture->Height;

    if (a_width == 0)
        a_width = textureWidth;

    if (a_height == 0)
        a_height = textureHeight;

    while (a_x > textureWidth && a_width > textureWidth)
    {
        a_x -= textureWidth;
        a_width -= textureWidth;
    }

    if (a_x < 0 || a_x > a_width)
    {
        return false;
    }

    while (a_y > textureHeight && a_height > textureHeight)
    {
        a_y -= textureHeight;
        a_height -= textureHeight;
    }

    if (a_y < 0 || a_y > a_height)
    {
        return false;
    }

    int idx = (a_y * textureWidth) + a_x;
    return texture->m_hitMap.Get(idx);
}

bool COrion::ResizepicPixelsInXY(u16 a_id, int a_x, int a_y, int a_width, int a_height)
{
    Core::TMousePos pos = g_MouseManager.GetPosition();
    int tempX           = pos.x - a_x;
    int tempY           = pos.y - a_y;

    if (tempX < 0 || tempY < 0 || tempX >= a_width || tempY >= a_height)
        return false;

    CGLTexture* th[9] = { nullptr };
    for (int i = 0; i < 9; i++)
    {
        CGLTexture* pth = m_GumpDataIndex[a_id + i].Texture;

        if (pth == nullptr)
        {
            return false;
        }

        if (i == 4)
        {
            th[8] = pth;
        }
        else if (i > 4)
        {
            th[i - 1] = pth;
        }
        else
        {
            th[i] = pth;
        }
    }

    int offsetTop    = Core::Max(th[0]->Height, th[2]->Height) - th[1]->Height;
    int offsetBottom = Core::Max(th[5]->Height, th[7]->Height) - th[6]->Height;
    int offsetLeft   = Core::Max(th[0]->Width, th[5]->Width) - th[3]->Width;
    int offsetRight  = Core::Max(th[2]->Width, th[7]->Width) - th[4]->Width;

    for (int i = 0; i < 9; i++)
    {
        switch (i)
        {
            case 0:
            {
                if (GumpPixelsInXY(a_id, a_x, a_y))
                {
                    return true;
                }
                break;
            }
            case 1:
            {
                int DW = a_width - th[0]->Width - th[2]->Width;
                if (DW < 1)
                {
                    break;
                }

                if (GumpPixelsInXY(a_id + 1, a_x + th[0]->Width, a_y, DW, 0))
                {
                    return true;
                }

                break;
            }
            case 2:
            {
                if (GumpPixelsInXY(a_id + 2, a_x + a_width - th[i]->Width, a_y + offsetTop))
                {
                    return true;
                }

                break;
            }
            case 3:
            {
                int DH = a_height - th[0]->Height - th[5]->Height;
                if (DH < 1)
                {
                    break;
                }

                if (GumpPixelsInXY(a_id + 3, a_x + offsetLeft, a_y + th[0]->Height, 0, DH))
                {
                    return true;
                }

                break;
            }
            case 4:
            {
                int DH = a_height - th[2]->Height - th[7]->Height;
                if (DH < 1)
                {
                    break;
                }

                if (GumpPixelsInXY(
                        a_id + 5,
                        a_x + a_width - th[i]->Width - offsetRight,
                        a_y + th[2]->Height,
                        0,
                        DH))
                {
                    return true;
                }

                break;
            }
            case 5:
            {
                if (GumpPixelsInXY(a_id + 6, a_x, a_y + a_height - th[i]->Height))
                {
                    return true;
                }

                break;
            }
            case 6:
            {
                int DW = a_width - th[5]->Width - th[7]->Width;
                if (DW < 1)
                {
                    break;
                }

                if (GumpPixelsInXY(
                        a_id + 7,
                        a_x + th[5]->Width,
                        a_y + a_height - th[i]->Height - offsetBottom,
                        DW,
                        0))
                {
                    return true;
                }

                break;
            }
            case 7:
            {
                if (GumpPixelsInXY(
                        a_id + 8, a_x + a_width - th[i]->Width, a_y + a_height - th[i]->Height))
                {
                    return true;
                }

                break;
            }
            case 8:
            {
                int DW = a_width - th[0]->Width - th[2]->Width;

                if (DW < 1)
                {
                    break;
                }

                int DH = a_height - th[2]->Height - th[7]->Height;

                if (DH < 1)
                {
                    break;
                }

                if (GumpPixelsInXY(a_id + 4, a_x + th[0]->Width, a_y + th[0]->Height, DW, DH))
                {
                    return true;
                }

                break;
            }
            default: break;
        }
    }

    return false;
}

bool COrion::StaticPixelsInXY(u16 a_id, int a_x, int a_y)
{
    CIndexObject& io    = m_StaticDataIndex[a_id];
    CGLTexture* texture = io.Texture;
    return texture ? texture->Select(a_x - io.Width, a_y - io.Height) : false;
}

bool COrion::StaticPixelsInXYAnimated(u16 a_id, int a_x, int a_y)
{
    return StaticPixelsInXY(a_id + m_StaticDataIndex[a_id].Offset, a_x, a_y);
}

bool COrion::StaticPixelsInXYInContainer(u16 a_id, int a_x, int a_y)
{
    CGLTexture* texture = m_StaticDataIndex[a_id].Texture;
    return texture ? texture->Select(a_x, a_y) : false;
}

bool COrion::LandPixelsInXY(u16 a_id, int a_x, int a_y)
{
    CGLTexture* texture = m_LandDataIndex[a_id].Texture;
    return texture ? texture->Select(a_x - 22, a_y - 22) : false;
}

bool COrion::LandTexturePixelsInXY(int a_x, int a_y, const Core::Rect<int>& a_r)
{
    Core::TMousePos pos = g_MouseManager.GetPosition();
    a_y -= 22;
    int testX = pos.x - a_x;
    int testY = pos.y;

    int y0 = -a_r.pos.x;
    int y1 = 22 - a_r.pos.y;
    int y2 = 44 - a_r.size.x;
    int y3 = 22 - a_r.size.y;

    bool result =
        ((testY >= testX * (y1 - y0) / -22 + a_y + y0) &&
         (testY >= testX * (y3 - y0) / 22 + a_y + y0) &&
         (testY <= testX * (y3 - y2) / 22 + a_y + y2) &&
         (testY <= testX * (y1 - y2) / -22 + a_y + y2));

    return result;
}

void COrion::CreateTextMessageF(u8 a_font, u16 a_color, const char* a_format, ...)
{
    va_list arg;
    va_start(arg, a_format);
    char buf[512] = { 0 };
    vsprintf_s(buf, a_format, arg);
    CreateTextMessage(TT_SYSTEM, 0xFFFFFFFF, a_font, a_color, buf);
    va_end(arg);
}

void COrion::CreateUnicodeTextMessageF(u8 a_font, u16 a_color, const char* a_format, ...)
{
    va_list arg;
    va_start(arg, a_format);
    char buf[512] = { 0 };
    vsprintf_s(buf, a_format, arg);
    CreateUnicodeTextMessage(TT_SYSTEM, 0xFFFFFFFF, a_font, a_color, Core::ToWString(buf));
    va_end(arg);
}

void COrion::CreateTextMessage(
    const TEXT_TYPE& type,
    int serial,
    u8 font,
    u16 color,
    const std::string& text,
    CRenderWorldObject* clientObj)
{
    CTextData* td = new CTextData();
    td->Unicode   = false;
    td->Font      = font;
    td->Serial    = serial;
    td->Color     = color;
    td->Timer     = g_Ticks;
    td->Type      = type;
    td->Text      = text;

    switch (type)
    {
        case TT_SYSTEM:
        {
            td->GenerateTexture(TEXT_SYSTEM_MESSAGE_MAX_WIDTH);
            AddSystemMessage(td);

            break;
        }
        case TT_OBJECT:
        {
            CGameObject* obj = g_World->FindWorldObject(serial);

            if (obj != nullptr)
            {
                int width = g_FontManager.GetWidthA(font, text);

                td->Color = 0;

                if (width > TEXT_MESSAGE_MAX_WIDTH)
                {
                    width =
                        g_FontManager.GetWidthExA(font, text, TEXT_MESSAGE_MAX_WIDTH, TS_LEFT, 0);
                    td->GenerateTexture(width, 0, TS_LEFT);
                }
                else
                {
                    td->GenerateTexture(0, 0, TS_CENTER);
                }

                td->Color = color;

                u32 container = obj->Container;

                if (container == 0xFFFFFFFF)
                {
                    g_WorldTextRenderer.AddText(td);
                }
                else if (!obj->NPC)
                {
                    td->SetX(g_ClickObject.X);
                    td->SetY(g_ClickObject.Y);

                    CGump* gump = g_GumpManager.GetGump(container, 0, GT_CONTAINER);

                    if (gump == nullptr)
                    {
                        CGameObject* topobj = obj->GetTopObject();

                        if (((CGameItem*)obj)->Layer != OL_NONE)
                        {
                            gump = g_GumpManager.GetGump(topobj->Serial, 0, GT_PAPERDOLL);
                        }

                        if (gump == nullptr)
                        {
                            gump = g_GumpManager.GetGump(topobj->Serial, 0, GT_TRADE);

                            if (gump == nullptr)
                            {
                                topobj = (CGameObject*)topobj->m_Items;

                                while (topobj != nullptr && topobj->Graphic != 0x1E5E)
                                {
                                    topobj = (CGameObject*)topobj->m_Next;
                                }

                                if (topobj != nullptr)
                                {
                                    gump = g_GumpManager.GetGump(0, topobj->Serial, GT_TRADE);
                                }
                            }
                        }
                    }

                    if (gump != nullptr)
                    {
                        CTextRenderer* tr = gump->GetTextRenderer();

                        if (tr != nullptr)
                        {
                            tr->AddText(td);
                        }
                    }
                }

                obj->AddText(td);
            }
            else
            {
                td->GenerateTexture(TEXT_SYSTEM_MESSAGE_MAX_WIDTH);
                AddSystemMessage(td);
            }

            break;
        }
        case TT_CLIENT:
        {
            int width = g_FontManager.GetWidthA(font, text);

            if (width > TEXT_MESSAGE_MAX_WIDTH)
            {
                width = g_FontManager.GetWidthExA(font, text, TEXT_MESSAGE_MAX_WIDTH, TS_LEFT, 0);
                td->GenerateTexture(width, 0, TS_LEFT);
            }
            else
            {
                td->GenerateTexture(0, 0, TS_CENTER);
            }

            clientObj->AddText(td);
            g_WorldTextRenderer.AddText(td);

            break;
        }
        default: break;
    }
}

void COrion::CreateUnicodeTextMessage(
    const TEXT_TYPE& type,
    int serial,
    u8 font,
    u16 color,
    const std::wstring& text,
    CRenderWorldObject* clientObj)
{
    CTextData* td   = new CTextData();
    td->Unicode     = true;
    td->Font        = font;
    td->Serial      = serial;
    td->Color       = color;
    td->Timer       = g_Ticks;
    td->Type        = type;
    td->UnicodeText = text;

    switch (type)
    {
        case TT_SYSTEM:
        {
            td->GenerateTexture(TEXT_SYSTEM_MESSAGE_MAX_WIDTH, UOFONT_BLACK_BORDER);
            AddSystemMessage(td);
            break;
        }
        case TT_OBJECT:
        {
            CGameObject* obj = g_World->FindWorldObject(serial);
            if (obj != nullptr)
            {
                int width = g_FontManager.GetWidthW(font, text);
                if (width > TEXT_MESSAGE_MAX_WIDTH)
                {
                    width = g_FontManager.GetWidthExW(
                        font, text, TEXT_MESSAGE_MAX_WIDTH, TS_LEFT, UOFONT_BLACK_BORDER);
                    td->GenerateTexture(width, UOFONT_BLACK_BORDER, TS_LEFT);
                }
                else
                {
                    td->GenerateTexture(0, UOFONT_BLACK_BORDER, TS_CENTER);
                }

                u32 container = obj->Container;
                if (container == 0xFFFFFFFF)
                {
                    g_WorldTextRenderer.AddText(td);
                }
                else if (!obj->NPC)
                {
                    td->SetX(g_ClickObject.X);
                    td->SetY(g_ClickObject.Y);

                    CGump* gump = g_GumpManager.GetGump(container, 0, GT_CONTAINER);
                    if (gump == nullptr)
                    {
                        CGameObject* topobj = obj->GetTopObject();
                        if (((CGameItem*)obj)->Layer != OL_NONE)
                        {
                            gump = g_GumpManager.GetGump(topobj->Serial, 0, GT_PAPERDOLL);
                        }

                        if (gump == nullptr)
                        {
                            gump = g_GumpManager.GetGump(topobj->Serial, 0, GT_TRADE);
                            if (gump == nullptr)
                            {
                                topobj = (CGameObject*)topobj->m_Items;
                                while (topobj != nullptr && topobj->Graphic != 0x1E5E)
                                {
                                    topobj = (CGameObject*)topobj->m_Next;
                                }

                                if (topobj != nullptr)
                                {
                                    gump = g_GumpManager.GetGump(0, topobj->Serial, GT_TRADE);
                                }
                            }
                        }
                    }

                    if (gump != nullptr)
                    {
                        CTextRenderer* tr = gump->GetTextRenderer();
                        if (tr != nullptr)
                        {
                            tr->AddText(td);
                        }
                    }
                }

                obj->AddText(td);
            }
            else
            {
                td->GenerateTexture(TEXT_SYSTEM_MESSAGE_MAX_WIDTH, UOFONT_BLACK_BORDER);
                AddSystemMessage(td);
            }

            break;
        }
        case TT_CLIENT:
        {
            int width = g_FontManager.GetWidthW(font, text);
            if (width > TEXT_MESSAGE_MAX_WIDTH)
            {
                width = g_FontManager.GetWidthExW(font, text, TEXT_MESSAGE_MAX_WIDTH, TS_LEFT, 0);
                td->GenerateTexture(width, 0, TS_LEFT);
            }
            else
            {
                td->GenerateTexture(0, UOFONT_BLACK_BORDER, TS_LEFT);
            }

            clientObj->AddText(td);
            g_WorldTextRenderer.AddText(td);
            break;
        }
        default: break;
    }
}

void COrion::AddSystemMessage(CTextData* msg)
{
    g_SystemChat.Add(msg);
    AddJournalMessage(msg, "");
}

void COrion::AddJournalMessage(CTextData* msg, const std::string& name)
{
    CTextData* jmsg = new CTextData(msg);

    if (!jmsg->Unicode)
    {
        jmsg->Text = name + jmsg->Text;
        jmsg->Font = 9;
    }
    else
    {
        //if (msg->Type == TT_SYSTEM)
        //	jmsg->Color = 0;

        jmsg->UnicodeText = Core::ToWString(name) + jmsg->UnicodeText;
        jmsg->Font        = 0;
    }

    /*if (msg->Type == TT_OBJECT)
		jmsg->GenerateTexture(214, UOFONT_INDENTION | UOFONT_BLACK_BORDER);
	else
		jmsg->GenerateTexture(214, UOFONT_INDENTION);*/

    g_Journal.Add(jmsg);
}

void COrion::ChangeMap(u8 newmap)
{
    if (newmap < 0 || newmap > 5)
    {
        newmap = 0;
    }

    if (g_CurrentMap != newmap)
    {
        g_CurrentMap = newmap;

        if (g_World != nullptr && g_Player != nullptr)
        {
            g_Player->MapIndex = g_CurrentMap;
            g_Player->RemoveRender();

            CGameObject* obj = (CGameObject*)g_World->m_Items;

            while (obj != nullptr)
            {
                CGameObject* next = (CGameObject*)obj->m_Next;

                if (obj->MapIndex != g_CurrentMap)
                {
                    if (g_Party.Contains(obj->Serial))
                    {
                        obj->RemoveRender();
                        g_GumpManager.UpdateContent(obj->Serial, 0, GT_STATUSBAR);
                    }
                    else
                    {
                        g_World->RemoveObject(obj);
                    }
                }

                obj = next;
            }

            g_MapManager.Clear();
            g_MapManager.Init();

            g_MapManager.AddRender(g_Player);
        }
    }
}

void COrion::PickupItem(CGameItem* obj, int count, bool isGameFigure)
{
    if (!g_ObjectInHand.Enabled)
    {
        g_ObjectInHand.Clear();
        g_ObjectInHand.Enabled = true;

        if (count == 0)
        {
            count = obj->Count;
        }

        g_ObjectInHand.Serial       = obj->Serial;
        g_ObjectInHand.Graphic      = obj->Graphic;
        g_ObjectInHand.Color        = obj->Color;
        g_ObjectInHand.Container    = obj->Container;
        g_ObjectInHand.Layer        = obj->Layer;
        g_ObjectInHand.Flags        = obj->GetFlags();
        g_ObjectInHand.TiledataPtr  = obj->GetStaticData();
        g_ObjectInHand.Count        = count;
        g_ObjectInHand.IsGameFigure = isGameFigure;
        g_ObjectInHand.X            = obj->GetX();
        g_ObjectInHand.Y            = obj->GetY();
        g_ObjectInHand.Z            = obj->GetZ();
        g_ObjectInHand.TotalCount   = obj->Count;

        CPacketPickupRequest(g_ObjectInHand.Serial, count).Send();

        g_World->ObjectToRemove = g_ObjectInHand.Serial;
    }
}

void COrion::DropItem(int container, u16 x, u16 y, char z)
{
    if (g_ObjectInHand.Enabled && g_ObjectInHand.Serial != container)
    {
        if (GameVars::GetClientVersion() >= CV_6017)
        {
            CPacketDropRequestNew(g_ObjectInHand.Serial, x, y, z, 0, container).Send();
        }
        else
        {
            CPacketDropRequestOld(g_ObjectInHand.Serial, x, y, z, container).Send();
        }

        g_ObjectInHand.Enabled = false;
        g_ObjectInHand.Dropped = true;
    }
}

void COrion::EquipItem(u32 container)
{
    if (g_ObjectInHand.Enabled)
    {
        if (IsWearable(g_ObjectInHand.TiledataPtr->Flags))
        {
            u16 graphic = g_ObjectInHand.Graphic;

            if (container == 0u)
            {
                container = g_PlayerSerial;
            }

            CPacketEquipRequest(g_ObjectInHand.Serial, m_StaticData[graphic].Layer, container)
                .Send();

            g_ObjectInHand.Enabled = false;
            g_ObjectInHand.Dropped = true;
        }
    }
}

void COrion::ChangeWarmode(u8 status)
{
    u8 newstatus = (u8)(!g_Player->Warmode);

    if (status != 0xFF)
    {
        if (g_Player->Warmode == (bool)status)
        {
            return;
        }

        newstatus = status;
    }

    if (newstatus == 1 && g_ConfigManager.GetMusic())
    {
        PlayMusic(rand() % 3 + 38, true);
    }
    else if (newstatus == 0)
    {
        g_SoundManager.StopWarMusic();
    }

    CPacketChangeWarmode(newstatus).Send();
}

void COrion::Click(u32 serial)
{
    CPacketClickRequest(serial).Send();

    CGameObject* obj = g_World->FindWorldObject(serial);
    if (obj != nullptr)
    {
        obj->Clicked = true;
    }
}

void COrion::DoubleClick(u32 serial)
{
    if (serial >= 0x40000000)
    {
        g_LastUseObject = serial;
    }

    CPacketDoubleClickRequest(serial).Send();
}

void COrion::PaperdollReq(u32 serial)
{
    //g_LastUseObject = serial;

    CPacketDoubleClickRequest(serial | 0x80000000).Send();
}

void COrion::Attack(u32 serial)
{
    if (g_ConfigManager.CriminalActionsQuery && g_World != nullptr)
    {
        CGameCharacter* target = g_World->FindWorldCharacter(serial);

        if (target != nullptr && (NOTORIETY_TYPE)g_Player->Notoriety == NT_INNOCENT &&
            (NOTORIETY_TYPE)target->Notoriety == NT_INNOCENT)
        {
            int x = g_ConfigManager.GameWindowX + (g_ConfigManager.GameWindowWidth / 2) - 40;
            int y = g_ConfigManager.GameWindowY + (g_ConfigManager.GameWindowHeight / 2) - 20;

            CGumpQuestion* newgump =
                new CGumpQuestion(0, x, y, CGumpQuestion::ID_GQ_STATE_ATTACK_REQUEST);
            newgump->ID = serial;

            g_GumpManager.AddGump(newgump);

            InitScreen(GS_GAME_BLOCKED);
            g_GameBlockedScreen.Code = 3;

            return;
        }
    }

    AttackReq(serial);
}

void COrion::AttackReq(u32 serial)
{
    g_LastAttackObject = serial;

    //CPacketStatusRequest(serial).Send();

    CPacketAttackRequest(serial).Send();
}

void COrion::SendASCIIText(const char* str, SPEECH_TYPE type)
{
    CPacketASCIISpeechRequest(str, type, 3, g_ConfigManager.SpeechColor).Send();
}

void COrion::CastSpell(int index)
{
    if (index >= 0)
    {
        g_LastSpellIndex = index;

        CPacketCastSpell(index).Send();
    }
}

void COrion::CastSpellFromBook(int index, u32 serial)
{
    if (index >= 0)
    {
        g_LastSpellIndex = index;

        CPacketCastSpellFromBook(index, serial).Send();
    }
}

void COrion::UseSkill(int index)
{
    if (index >= 0)
    {
        g_LastSkillIndex = index;

        CPacketUseSkill(index).Send();
    }
}

void COrion::OpenDoor()
{
    CPacketOpenDoor().Send();
}

void COrion::EmoteAction(const char* action)
{
    CPacketEmoteAction(action).Send();
}

void COrion::AllNames()
{
    CGameObject* obj = g_World->m_Items;

    while (obj != nullptr)
    {
        if ((obj->NPC && !obj->IsPlayer()) || obj->IsCorpse())
        {
            CPacketClickRequest(obj->Serial).Send();
        }

        obj = (CGameObject*)obj->m_Next;
    }
}

void COrion::RemoveRangedObjects()
{
    PROFILER_EVENT();
    if (g_World != nullptr)
    {
        int objectsRange = g_ConfigManager.UpdateRange;

        CGameObject* go = g_World->m_Items;

        while (go != nullptr)
        {
            CGameObject* next = (CGameObject*)go->m_Next;

            if (go->Container == 0xFFFFFFFF && !go->IsPlayer())
            {
                if (go->NPC)
                {
                    if (GetRemoveDistance(g_RemoveRangeXY, go) > objectsRange)
                    {
                        if (g_Party.Contains(go->Serial))
                        {
                            go->RemoveRender();
                            g_GumpManager.UpdateContent(go->Serial, 0, GT_STATUSBAR);
                        }
                        else
                        {
                            g_World->RemoveObject(go);
                        }
                    }
                }
                else if (((CGameItem*)go)->MultiBody)
                {
                    if (!CheckMultiDistance(g_RemoveRangeXY, go, objectsRange))
                    {
                        ((CGameItem*)go)->ClearMultiItems();
                    }
                    //g_World->RemoveObject(go);
                }
                else if (GetRemoveDistance(g_RemoveRangeXY, go) > objectsRange)
                {
                    g_World->RemoveObject(go);
                }
            }

            go = next;
        }
    }

    g_EffectManager.RemoveRangedEffects();
}

void COrion::ClearWorld()
{
    g_CorpseManager.Clear();
    g_Walker.Reset();
    g_ObjectInHand.Clear();
    g_UseItemActions.Clear();

    g_Ping = 0;
    g_ClickObject.Clear();
    g_Weather.Reset();
    g_ConsolePrompt                 = PT_NONE;
    g_MacroPointer                  = nullptr;
    g_Season                        = ST_SUMMER;
    g_OldSeason                     = ST_SUMMER;
    g_GlobalScale                   = 1.0;
    g_PathFinder.BlockMoving        = false;
    g_SkillsManager.SkillsTotal     = 0.0f;
    g_SkillsManager.SkillsRequested = false;

    if (g_World)
    {
        delete g_World;
        g_World = nullptr;
    }
    LOG_INFO("Client", "\tWorld removed?");

    g_PopupMenu = nullptr;

    g_GumpManager.Clear();
    LOG_INFO("Client", "\tGump Manager cleared?");

    g_EffectManager.Clear();
    LOG_INFO("Client", "\tEffect List cleared?");

    g_GameConsole.Clear();

    g_EntryPointer  = nullptr;
    g_GrayMenuCount = 0;

    g_Target.Reset();

    g_SystemChat.Clear();
    LOG_INFO("Client", "\tSystem chat cleared?");

    g_Journal.Clear();
    LOG_INFO("Client", "\tJournal cleared?");

    g_MapManager.Clear();
    LOG_INFO("Client", "\tMap cleared?");

    g_CurrentMap = 0;

    g_Party.Leader  = 0;
    g_Party.Inviter = 0;
    g_Party.Clear();

    g_Ability[0] = AT_DISARM;
    g_Ability[1] = AT_PARALYZING_BLOW;

    g_ResizedGump = nullptr;

    g_DrawStatLockers = false;
}

void COrion::LogOut()
{
    LOG_INFO("Client", "COrion::LogOut->Start");
    SaveLocalConfig(g_PacketManager.ConfigSerial);

    if (g_SendLogoutNotification)
    {
        CPacketLogoutNotification().Send();
    }

    Disconnect();
    LOG_INFO("Client", "\tDisconnected?");

    ClearWorld();

    LOG_INFO("Client", "COrion::LogOut->End");
    InitScreen(GS_MAIN);
}

void COrion::ConsolePromptSend()
{
    size_t len  = g_GameConsole.Length();
    bool cancel = (len < 1);
    if (g_ConsolePrompt == PT_ASCII)
        CPacketASCIIPromptResponse(g_GameConsole.c_str(), len, cancel).Send();
    else if (g_ConsolePrompt == PT_UNICODE)
        CPacketUnicodePromptResponse(g_GameConsole.Data(), len, g_Language, cancel).Send();
    g_ConsolePrompt = PT_NONE;
}

void COrion::ConsolePromptCancel()
{
    if (g_ConsolePrompt == PT_ASCII)
        CPacketASCIIPromptResponse("", 0, true).Send();
    else if (g_ConsolePrompt == PT_UNICODE)
        CPacketUnicodePromptResponse({}, 0, g_Language, true).Send();
    g_ConsolePrompt = PT_NONE;
}

u64 COrion::GetLandFlags(u16 a_id)
{
    return a_id < m_LandData.size() ? m_LandData[a_id].Flags : 0;
}

u64 COrion::GetStaticFlags(u16 a_id)
{
    return a_id < (int)m_StaticData.size() ? m_StaticData[a_id].Flags : 0;
}

Core::Vec2<i32> COrion::GetStaticArtDimension(u16 a_id)
{
    CGLTexture* th = ExecuteStaticArt(a_id);
    return th ? Core::Vec2<i32>(th->Width, th->Height) : Core::Vec2<i32>();
}

Core::Vec2<i32> COrion::GetGumpDimension(u16 a_id)
{
    CGLTexture* th = ExecuteGump(a_id);
    return th ? Core::Vec2<i32>(th->Width, th->Height) : Core::Vec2<i32>();
}

void COrion::OpenStatus(u32 a_serial)
{
    Core::TMousePos pos = g_MouseManager.GetPosition();
    int x               = pos.x - 76;
    int y               = pos.y - 30;
    CPacketStatusRequest(a_serial).Send();
    g_GumpManager.AddGump(new CGumpStatusbar(a_serial, x, y, true));
}

void COrion::DisplayStatusbarGump(int a_serial, int a_x, int a_y)
{
    CPacketStatusRequest(a_serial).Send();

    CGump* gump = g_GumpManager.GetGump(a_serial, 0, GT_STATUSBAR);

    if (gump != nullptr)
    {
        if (gump->Minimized)
        {
            gump->MinimizedX = a_x;
            gump->MinimizedY = a_y;
        }
        else
        {
            gump->SetX(a_x);
            gump->SetY(a_y);
        }
    }
    else
    {
        g_GumpManager.AddGump(new CGumpStatusbar(a_serial, a_x, a_y, true));
    }
}

void COrion::OpenMinimap()
{
    g_GumpManager.AddGump(new CGumpMinimap(0, 0, true));
}

void COrion::OpenWorldMap()
{
    CPluginPacketOpenMap().SendToPlugin();

    /*int x = g_ConfigManager.GameWindowX + (g_ConfigManager.GameWindowWidth / 2) - 200;
	int y = g_ConfigManager.GameWindowY + (g_ConfigManager.GameWindowHeight / 2) - 150;

	CGumpWorldMap *gump = new CGumpWorldMap(x, y);
	gump->Called = true;

	g_GumpManager.AddGump(gump);*/
}

void COrion::OpenJournal()
{
    g_GumpManager.AddGump(new CGumpJournal(0, 0, false, 250));
}

void COrion::OpenSkills()
{
    g_SkillsManager.SkillsRequested = true;
    CPacketSkillsRequest(g_PlayerSerial).Send();
}

void COrion::OpenBackpack()
{
    if (g_Player != nullptr)
    {
        CGameItem* pack = g_Player->FindLayer(OL_BACKPACK);

        if (pack != nullptr)
        {
            DoubleClick(pack->Serial);
        }
    }
}

void COrion::OpenLogOut()
{
    int x = g_ConfigManager.GameWindowX + (g_ConfigManager.GameWindowWidth / 2) - 40;
    int y = g_ConfigManager.GameWindowY + (g_ConfigManager.GameWindowHeight / 2) - 20;

    g_GumpManager.AddGump(new CGumpQuestion(0, x, y, CGumpQuestion::ID_GQ_STATE_QUIT));

    InitScreen(GS_GAME_BLOCKED);
    g_GameBlockedScreen.Code = 3;
}

void COrion::OpenChat()
{
    CPacketOpenChat({}).Send();
}

void COrion::OpenConfiguration()
{
    int x = (g_gameWindow.GetSize().x / 2) - 320;
    int y = (g_gameWindow.GetSize().y / 2) - 240;

    g_OptionsConfig = g_ConfigManager;

    g_GumpManager.AddGump(new CGumpOptions(x, y));
}

void COrion::OpenMail()
{
}

void COrion::OpenPartyManifest()
{
    int x = (g_gameWindow.GetSize().x / 2) - 272;
    int y = (g_gameWindow.GetSize().y / 2) - 240;

    g_GumpManager.AddGump(new CGumpPartyManifest(0, x, y, g_Party.CanLoot));
}

void COrion::OpenProfile(u32 serial)
{
    if (serial == 0u)
    {
        serial = g_PlayerSerial;
    }

    CPacketProfileRequest(serial).Send();
}

void COrion::DisconnectGump()
{
    CServer* server = g_ServerList.GetSelectedServer();
    std::string str = "Disconnected from " + (server != nullptr ? server->Name : "server name...");
    g_Orion.CreateTextMessage(TT_SYSTEM, 0, 3, 0x21, str);

    int x = g_ConfigManager.GameWindowX + (g_ConfigManager.GameWindowWidth / 2) - 100;
    int y = g_ConfigManager.GameWindowY + (g_ConfigManager.GameWindowHeight / 2) - 62;

    CGumpNotify* gump =
        new CGumpNotify(x, y, CGumpNotify::ID_GN_STATE_LOGOUT, 200, 125, "Connection lost");

    g_GumpManager.AddGump(gump);

    g_Orion.InitScreen(GS_GAME_BLOCKED);
    g_GameBlockedScreen.Code = 0;
}

void COrion::OpenCombatBookGump()
{
    int gameWindowCenterX = (g_ConfigManager.GameWindowX - 4) + g_ConfigManager.GameWindowWidth / 2;
    int gameWindowCenterY =
        (g_ConfigManager.GameWindowY - 4) + g_ConfigManager.GameWindowHeight / 2;

    int x = gameWindowCenterX - 200;
    int y = gameWindowCenterY - 100;

    if (x < 0)
    {
        x = 0;
    }

    if (y < 0)
    {
        y = 0;
    }

    g_GumpManager.AddGump(new CGumpCombatBook(x, y));
}

void COrion::OpenRacialAbilitiesBookGump()
{
    int gameWindowCenterX = (g_ConfigManager.GameWindowX - 4) + g_ConfigManager.GameWindowWidth / 2;
    int gameWindowCenterY =
        (g_ConfigManager.GameWindowY - 4) + g_ConfigManager.GameWindowHeight / 2;

    int x = gameWindowCenterX - 200;
    int y = gameWindowCenterY - 100;

    if (x < 0)
    {
        x = 0;
    }

    if (y < 0)
    {
        y = 0;
    }

    g_GumpManager.AddGump(new CGumpRacialAbilitiesBook(x, y));
}

void COrion::StartReconnect()
{
    if (!g_ConnectionManager.Connected() || g_World == nullptr)
    {
        LogOut();
        g_MainScreen.m_AutoLogin->Checked = true;
        InitScreen(GS_MAIN);
    }
}

const std::string& COrion::GetPingString()
{
    return g_pingString;
}
