#pragma once

#if defined(__GNUC__) && __GNUC__ <= 5 && !__clang__
#define USE_PCH 0
#else
#define USE_PCH 1
#endif

#include <math.h> // M_PI

#include <SDL.h>

#include "Definitions.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <string.h>
#include <wchar.h>
#include <algorithm>
#include <stdint.h>

#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <deque>
#include <map>
#include <condition_variable>
#include <unordered_map>
#include <locale>
#include <sstream>
#include <istream>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <functional>
#include <cassert>

using std::deque;
using std::map;
using std::pair;
using std::string;
using std::unordered_map;
using std::vector;
using std::wstring;

#define UNUSED(x) (void)x
#define ToColorR(x) ((x)&0xff)
#define ToColorG(x) ((x >> 8) & 0xff)
#define ToColorB(x) ((x >> 16) & 0xff)
#define ToColorA(x) ((x >> 24) & 0xff)

#if defined(ORION_WINDOWS)

#define HAVE_M_PI // SDL conflicting with winsdk10

#if !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif

#if !defined(_CRT_NON_CONFORMING_SWPRINTFS)
#define _CRT_NON_CONFORMING_SWPRINTFS
#endif

#pragma comment(lib, "Winmm.lib")
#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "Glu32.lib")

#if !defined(ORION_CMAKE)
#if defined(_M_IX86)
#pragma comment(lib, "../external/GLEW/x86/lib/glew32.lib")
#pragma comment(lib, "../external/psapi/x86/lib/Psapi.lib")
#pragma comment(lib, "../external/SDL2/x86/lib/SDL2.lib")
#pragma comment(lib, "../external/SDL2/x86/lib/SDL2main.lib")
//#pragma comment(lib, "../external/SDL2/x86/lib/SDL2test.lib")

#else
#pragma comment(lib, "../external/GLEW/x64/lib/glew32.lib")
#pragma comment(lib, "../external/psapi/x64/lib/Psapi.lib")
#pragma comment(lib, "../external/SDL2/x64/lib/SDL2.lib")
#pragma comment(lib, "../external/SDL2/x64/lib/SDL2main.lib")
//#pragma comment(lib, "../external/SDL2/x64/lib/SDL2test.lib")
#endif
#endif

#include "targetver.h"
#include "Resource.h"

#else // ORION_WINDOWS

#include <unistd.h>
#include <chrono>
#include <thread>

#endif // ORION_WINDOWS

#include "Globals.h"

#if USE_PCH

#include "Backend.h"

#include "Platform.h"
#include "plugin/mulstruct.h"
#include "BaseQueue.h"
#include "Utility/AutoResetEvent.h"

#include "CharacterList.h"
#include "ClickObject.h"
#include "Container.h"
#include "ContainerStack.h"
#include "UseItemsList.h"
#include "ImageBounds.h"
#include "TextureObject.h"
#include "Managers/FileManager.h"
#include "IndexObject.h"
#include "Macro.h"
#include "Multi.h"
#include "MultiMap.h"
#include "OrionApplication.h"
#include "OrionUO.h"
#include "PartyObject.h"
#include "Party.h"
#include "SelectedObject.h"
#include "PressedObject.h"
#include "Profession.h"
#include "QuestArrow.h"
#include "RenderObject.h"
#include "RenderWorldObject.h"
#include "ScreenshotBuilder.h"
#include "ServerList.h"
#include "SkillGroup.h"
#include "StumpsData.h"
#include "TargetGump.h"
#include "WeatherEffect.h"
#include "Weather.h"

#if defined(ORION_WINDOWS)
#include "VMQuery.h"
#endif

#include "Network/UOHuffman.h"
#include "Network/Packets.h"
#include "Network/PluginPackets.h"
#include "Network/Connection.h"

#include "ToolTip.h"

#include "TextEngine/EntryText.h"
#include "TextEngine/GameConsole.h"
#include "TextEngine/Journal.h"
#include "TextEngine/RenderTextObject.h"
#include "TextEngine/TextData.h"
#include "TextEngine/TextContainer.h"
#include "TextEngine/TextRenderer.h"

#include "Walker/FastWalk.h"
#include "Walker/PathNode.h"
#include "Walker/PathFinder.h"
#include "Walker/WalkData.h"
#include "Walker/Walker.h"

#include "GUI/GUI.h"

#include "Gumps/Gump.h"
#include "GameObjects/CustomHouseObjects.h"
#include "GameObjects/MapObject.h"
#include "GameObjects/RenderStaticObject.h"
#include "GameObjects/LandObject.h"
#include "GameObjects/StaticObject.h"
#include "GameObjects/MultiObject.h"
#include "GameObjects/CustomHouseMultiObject.h"
#include "GameObjects/GameObject.h"
#include "GameObjects/GameItem.h"
#include "GameObjects/GameCharacter.h"
#include "GameObjects/GameEffect.h"
#include "GameObjects/GameEffectDrag.h"
#include "GameObjects/GameEffectMoving.h"
#include "GameObjects/GamePlayer.h"
#include "GameObjects/GameWorld.h"
#include "GameObjects/MapBlock.h"
#include "GameObjects/ObjectOnCursor.h"

#include "Target.h"

#include "Managers/CustomHousesManager.h"
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

#include "Managers/AnimationManager.h"
#include "Managers/CityManager.h"
#include "CityList.h"
#include "Managers/ClilocManager.h"
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

#include "SiebenwindClient.h"
#endif // USE_PCH
