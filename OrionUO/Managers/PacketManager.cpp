#include <miniz.h>
#include "PacketManager.h"
#include "Core/Log.h"
#include "Core/Sockets.h"
#include "Core/StringUtils.h"
#include "Core/TextFileParser.h"
#include "Core/Time.h"
#include "DefinitionMacro.h"
#include "GameVars.h"
#include "GameWindow.h"
#include "Globals.h"
#include "GumpManager.h"
#include "ConfigManager.h"
#include "CustomHousesManager.h"
#include "ClilocManager.h"
#include "MacroManager.h"
#include "ObjectPropertiesManager.h"
#include "ColorManager.h"
#include "FontsManager.h"
#include "AnimationManager.h"
#include "EffectManager.h"
#include "ScreenEffectManager.h"
#include "CorpseManager.h"
#include "SkillsManager.h"
#include "MapManager.h"
#include "ConnectionManager.h"
#include "FileManager.h"
#include "MultiMap.h"
#include "Config.h"
#include "OrionUO.h"
#include "Macro.h"
#include "CityList.h"
#include "ToolTip.h"
#include "Target.h"
#include "Weather.h"
#include "TargetGump.h"
#include "Party.h"
#include "ServerList.h"
#include "QuestArrow.h"
#include "Multi.h"
#include "ContainerStack.h"
#include "Container.h"
#include "CharacterList.h"
#include "TextEngine/GameConsole.h"
#include "GameObjects/GameItem.h"
#include "GameObjects/GameWorld.h"
#include "GameObjects/ObjectOnCursor.h"
#include "GameObjects/GamePlayer.h"
#include "GameObjects/GameEffectMoving.h"
#include "Gumps/Gump.h"
#include "Gumps/GumpAbility.h"
#include "Gumps/GumpSecureTrading.h"
#include "Gumps/GumpStatusbar.h"
#include "Gumps/GumpShop.h"
#include "Gumps/GumpBook.h"
#include "Gumps/GumpMap.h"
#include "Gumps/GumpTip.h"
#include "Gumps/GumpProfile.h"
#include "Gumps/GumpDye.h"
#include "Gumps/GumpGeneric.h"
#include "Gumps/GumpMenu.h"
#include "Gumps/GumpBuff.h"
#include "Gumps/GumpGrayMenu.h"
#include "Gumps/GumpPopupMenu.h"
#include "Gumps/GumpSpellbook.h"
#include "Gumps/GumpPaperdoll.h"
#include "Gumps/GumpTextEntryDialog.h"
#include "Gumps/GumpBulletinBoard.h"
#include "Gumps/GumpBulletinBoardItem.h"
#include "Gumps/GumpCustomHouse.h"
#include "Gumps/GumpContainer.h"
#include "Gumps/GumpSkills.h"
#include "GUI/GUIShopItem.h"
#include "GUI/GUIHTMLGump.h"
#include "Profiler.h"
#include "ScreenStages/MainScreen.h"
#include "ScreenStages/GameScreen.h"
#include "ScreenStages/ConnectionScreen.h"
#include "ScreenStages/CharacterListScreen.h"
#include "Network/Packets.h"
#include "Walker/Walker.h"
#include "Walker/PathFinder.h"
#include "TextEngine/TextData.h"

const int PACKET_VARIABLE_SIZE = 0;

using namespace Core::TimeLiterals;

CPacketManager g_PacketManager;

#define UMSG(save, size)                                                                           \
    {                                                                                              \
        save, "?", size, DIR_BOTH, 0                                                               \
    }
// A message type sent to the server
#define SMSG(save, name, size)                                                                     \
    {                                                                                              \
        save, name, size, DIR_SEND, 0                                                              \
    }
// A message type received from the server
#define RMSG(save, name, size)                                                                     \
    {                                                                                              \
        save, name, size, DIR_RECV, 0                                                              \
    }
// A message type transmitted in both directions
#define BMSG(save, name, size)                                                                     \
    {                                                                                              \
        save, name, size, DIR_BOTH, 0                                                              \
    }
// Message types that have handler methods
#define RMSGH(save, name, size, rmethod)                                                           \
    {                                                                                              \
        save, name, size, DIR_RECV, &CPacketManager::Handle##rmethod                               \
    }
#define BMSGH(save, name, size, rmethod)                                                           \
    {                                                                                              \
        save, name, size, DIR_BOTH, &CPacketManager::Handle##rmethod                               \
    }

CPacketInfo CPacketManager::m_Packets[0x100] = {
    /*0x00*/ SMSG(ORION_SAVE_PACKET, "Create Character", 0x68),
    /*0x01*/ SMSG(ORION_SAVE_PACKET, "Disconnect", 0x05),
    /*0x02*/ SMSG(ORION_IGNORE_PACKET, "Walk Request", 0x07),
    /*0x03*/ BMSGH(ORION_SAVE_PACKET, "Client Talk", PACKET_VARIABLE_SIZE, ClientTalk),
    /*0x04*/ BMSG(ORION_SAVE_PACKET, "Request God mode (God client)", 0x02),
    /*0x05*/ SMSG(ORION_IGNORE_PACKET, "Attack", 0x05),
    /*0x06*/ SMSG(ORION_IGNORE_PACKET, "Double Click", 0x05),
    /*0x07*/ SMSG(ORION_SAVE_PACKET, "Pick Up Item", 0x07),
    /*0x08*/ SMSG(ORION_SAVE_PACKET, "Drop Item", 0x0e),
    /*0x09*/ SMSG(ORION_IGNORE_PACKET, "Single Click", 0x05),
    /*0x0A*/ BMSG(ORION_SAVE_PACKET, "Edit (God client)", 0x0b),
    /*0x0B*/ RMSGH(ORION_IGNORE_PACKET, "Damage Visualization", 0x07, Damage),
    /*0x0C*/ BMSG(ORION_SAVE_PACKET, "Edit tiledata (God client)", PACKET_VARIABLE_SIZE),
    /*0x0D*/ BMSG(ORION_SAVE_PACKET, "Edit NPC data (God client)", 0x03),
    /*0x0E*/ BMSG(ORION_SAVE_PACKET, "Edit template data (God client)", 0x01),
    /*0x0F*/ UMSG(ORION_SAVE_PACKET, 0x3d),
    /*0x10*/ BMSG(ORION_SAVE_PACKET, "Edit hue data (God client)", 0xd7),
    /*0x11*/ RMSGH(ORION_IGNORE_PACKET, "Character Status", PACKET_VARIABLE_SIZE, CharacterStatus),
    /*0x12*/ SMSG(ORION_IGNORE_PACKET, "Perform Action", PACKET_VARIABLE_SIZE),
    /*0x13*/ SMSG(ORION_IGNORE_PACKET, "Client Equip Item", 0x0a),
    /*0x14*/ BMSG(ORION_SAVE_PACKET, "Change tile Z (God client)", 0x06),
    /*0x15*/ BMSG(ORION_SAVE_PACKET, "Follow", 0x09),
    /*0x16*/ RMSGH(ORION_SAVE_PACKET, "Health status bar update (0x16)", 0x01, NewHealthbarUpdate),
    /*0x17*/
    RMSGH(
        ORION_IGNORE_PACKET,
        "Health status bar update (KR)",
        PACKET_VARIABLE_SIZE,
        NewHealthbarUpdate),
    /*0x18*/ BMSG(ORION_SAVE_PACKET, "Add script (God client)", PACKET_VARIABLE_SIZE),
    /*0x19*/ BMSG(ORION_SAVE_PACKET, "Edit NPC speech (God client)", PACKET_VARIABLE_SIZE),
    /*0x1A*/ RMSGH(ORION_SAVE_PACKET, "Update Item", PACKET_VARIABLE_SIZE, UpdateItem),
    /*0x1B*/ RMSGH(ORION_SAVE_PACKET, "Enter World", 0x25, EnterWorld),
    /*0x1C*/ RMSGH(ORION_IGNORE_PACKET, "Server Talk", PACKET_VARIABLE_SIZE, Talk),
    /*0x1D*/ RMSGH(ORION_SAVE_PACKET, "Delete Object", 0x05, DeleteObject),
    /*0x1E*/ BMSG(ORION_SAVE_PACKET, "Animate?", 0x04),
    /*0x1F*/ BMSG(ORION_SAVE_PACKET, "Explode?", 0x08),
    /*0x20*/ RMSGH(ORION_SAVE_PACKET, "Update Player", 0x13, UpdatePlayer),
    /*0x21*/ RMSGH(ORION_IGNORE_PACKET, "Deny Walk", 0x08, DenyWalk),
    /*0x22*/ BMSGH(ORION_IGNORE_PACKET, "Confirm Walk", 0x03, ConfirmWalk),
    /*0x23*/ RMSGH(ORION_SAVE_PACKET, "Drag Animation", 0x1a, DragAnimation),
    /*0x24*/ RMSGH(ORION_SAVE_PACKET, "Open Container", 0x07, OpenContainer),
    /*0x25*/ RMSGH(ORION_SAVE_PACKET, "Update Contained Item", 0x14, UpdateContainedItem),
    /*0x26*/ BMSG(ORION_SAVE_PACKET, "Kick client (God client)", 0x05),
    /*0x27*/ RMSGH(ORION_SAVE_PACKET, "Deny Move Item", 0x02, DenyMoveItem),
    /*0x28*/ RMSGH(ORION_SAVE_PACKET, "End dragging item", 0x05, EndDraggingItem),
    /*0x29*/ RMSGH(ORION_SAVE_PACKET, "Drop Item Accepted", 0x01, DropItemAccepted),
    /*0x2A*/ RMSG(ORION_SAVE_PACKET, "Blood mode", 0x05),
    /*0x2B*/ BMSG(ORION_SAVE_PACKET, "Toggle God mode (God client)", 0x02),
    /*0x2C*/ BMSGH(ORION_IGNORE_PACKET, "Death Screen", 0x02, DeathScreen),
    /*0x2D*/ RMSGH(ORION_SAVE_PACKET, "Mobile Attributes", 0x11, MobileAttributes),
    /*0x2E*/ RMSGH(ORION_SAVE_PACKET, "Server Equip Item", 0x0f, EquipItem),
    /*0x2F*/ RMSG(ORION_SAVE_PACKET, "Combat Notification", 0x0a),
    /*0x30*/ RMSG(ORION_SAVE_PACKET, "Attack ok", 0x05),
    /*0x31*/ RMSG(ORION_SAVE_PACKET, "Attack end", 0x01),
    /*0x32*/ BMSG(ORION_SAVE_PACKET, "Toggle hack mover (God client)", 0x02),
    /*0x33*/ RMSGH(ORION_IGNORE_PACKET, "Pause Control", 0x02, PauseControl),
    /*0x34*/ SMSG(ORION_IGNORE_PACKET, "Status Request", 0x0a),
    /*0x35*/ BMSG(ORION_SAVE_PACKET, "Resource type (God client)", 0x28d),
    /*0x36*/ BMSG(ORION_SAVE_PACKET, "Resource tile data (God client)", PACKET_VARIABLE_SIZE),
    /*0x37*/ BMSG(ORION_SAVE_PACKET, "Move object (God client)", 0x08),
    /*0x38*/ RMSGH(ORION_SAVE_PACKET, "Pathfinding", 0x07, Pathfinding),
    /*0x39*/ BMSG(ORION_SAVE_PACKET, "Remove group (God client)", 0x09),
    /*0x3A*/ BMSGH(ORION_IGNORE_PACKET, "Update Skills", PACKET_VARIABLE_SIZE, UpdateSkills),
    /*0x3B*/ BMSGH(ORION_IGNORE_PACKET, "Vendor Buy Reply", PACKET_VARIABLE_SIZE, BuyReply),
    /*0x3C*/
    RMSGH(ORION_SAVE_PACKET, "Update Contained Items", PACKET_VARIABLE_SIZE, UpdateContainedItems),
    /*0x3D*/ BMSG(ORION_SAVE_PACKET, "Ship (God client)", 0x02),
    /*0x3E*/ BMSG(ORION_SAVE_PACKET, "Versions (God client)", 0x25),
    /*0x3F*/ BMSG(ORION_SAVE_PACKET, "Update Statics (God Client)", PACKET_VARIABLE_SIZE),
    /*0x40*/ BMSG(ORION_SAVE_PACKET, "Update terrains (God client)", 0xc9),
    /*0x41*/ BMSG(ORION_SAVE_PACKET, "Update terrains (God client)", PACKET_VARIABLE_SIZE),
    /*0x42*/ BMSG(ORION_SAVE_PACKET, "Update art (God client)", PACKET_VARIABLE_SIZE),
    /*0x43*/ BMSG(ORION_SAVE_PACKET, "Update animation (God client)", 0x229),
    /*0x44*/ BMSG(ORION_SAVE_PACKET, "Update hues (God client)", 0x2c9),
    /*0x45*/ BMSG(ORION_SAVE_PACKET, "Version OK (God client)", 0x05),
    /*0x46*/ BMSG(ORION_SAVE_PACKET, "New art (God client)", PACKET_VARIABLE_SIZE),
    /*0x47*/ BMSG(ORION_SAVE_PACKET, "New terrain (God client)", 0x0b),
    /*0x48*/ BMSG(ORION_SAVE_PACKET, "New animation (God client)", 0x49),
    /*0x49*/ BMSG(ORION_SAVE_PACKET, "New hues (God client)", 0x5d),
    /*0x4A*/ BMSG(ORION_SAVE_PACKET, "Destroy art (God client)", 0x05),
    /*0x4B*/ BMSG(ORION_SAVE_PACKET, "Check version (God client)", 0x09),
    /*0x4C*/ BMSG(ORION_SAVE_PACKET, "Script names (God client)", PACKET_VARIABLE_SIZE),
    /*0x4D*/ BMSG(ORION_SAVE_PACKET, "Edit script (God client)", PACKET_VARIABLE_SIZE),
    /*0x4E*/ RMSGH(ORION_IGNORE_PACKET, "Personal Light Level", 0x06, PersonalLightLevel),
    /*0x4F*/ RMSGH(ORION_IGNORE_PACKET, "Global Light Level", 0x02, LightLevel),
    /*0x50*/ BMSG(ORION_IGNORE_PACKET, "Board header", PACKET_VARIABLE_SIZE),
    /*0x51*/ BMSG(ORION_IGNORE_PACKET, "Board message", PACKET_VARIABLE_SIZE),
    /*0x52*/ BMSG(ORION_IGNORE_PACKET, "Post board message", PACKET_VARIABLE_SIZE),
    /*0x53*/ RMSGH(ORION_SAVE_PACKET, "Error Code", 0x02, ErrorCode),
    /*0x54*/ RMSGH(ORION_SAVE_PACKET, "Sound Effect", 0x0c, PlaySoundEffect),
    /*0x55*/ RMSGH(ORION_IGNORE_PACKET, "Login Complete", 0x01, LoginComplete),
    /*0x56*/ BMSGH(ORION_IGNORE_PACKET, "Map Data", 0x0b, MapData),
    /*0x57*/ BMSG(ORION_SAVE_PACKET, "Update regions (God client)", 0x6e),
    /*0x58*/ BMSG(ORION_SAVE_PACKET, "New region (God client)", 0x6a),
    /*0x59*/ BMSG(ORION_SAVE_PACKET, "New content FX (God client)", PACKET_VARIABLE_SIZE),
    /*0x5A*/ BMSG(ORION_SAVE_PACKET, "Update content FX (God client)", PACKET_VARIABLE_SIZE),
    /*0x5B*/ RMSGH(ORION_IGNORE_PACKET, "Set Time", 0x04, SetTime),
    /*0x5C*/ BMSG(ORION_SAVE_PACKET, "Restart Version", 0x02),
    /*0x5D*/ SMSG(ORION_IGNORE_PACKET, "Select Character", 0x49),
    /*0x5E*/ BMSG(ORION_SAVE_PACKET, "Server list (God client)", PACKET_VARIABLE_SIZE),
    /*0x5F*/ BMSG(ORION_SAVE_PACKET, "Add server (God client)", 0x31),
    /*0x60*/ BMSG(ORION_SAVE_PACKET, "Remove server (God client)", 0x05),
    /*0x61*/ BMSG(ORION_SAVE_PACKET, "Destroy static (God client)", 0x09),
    /*0x62*/ BMSG(ORION_SAVE_PACKET, "Move static (God client)", 0x0f),
    /*0x63*/ BMSG(ORION_SAVE_PACKET, "Area load (God client)", 0x0d),
    /*0x64*/ BMSG(ORION_SAVE_PACKET, "Area load request (God client)", 0x01),
    /*0x65*/ RMSGH(ORION_IGNORE_PACKET, "Set Weather", 0x04, SetWeather),
    /*0x66*/ BMSGH(ORION_IGNORE_PACKET, "Book Page Data", PACKET_VARIABLE_SIZE, BookData),
    /*0x67*/ BMSG(ORION_SAVE_PACKET, "Simped? (God client)", 0x15),
    /*0x68*/ BMSG(ORION_SAVE_PACKET, "Script attach (God client)", PACKET_VARIABLE_SIZE),
    /*0x69*/ BMSG(ORION_SAVE_PACKET, "Friends (God client)", PACKET_VARIABLE_SIZE),
    /*0x6A*/ BMSG(ORION_SAVE_PACKET, "Notify friend (God client)", 0x03),
    /*0x6B*/ BMSG(ORION_SAVE_PACKET, "Key use (God client)", 0x09),
    /*0x6C*/ BMSGH(ORION_IGNORE_PACKET, "Target Data", 0x13, Target),
    /*0x6D*/ RMSGH(ORION_SAVE_PACKET, "Play Music", 0x03, PlayMusic),
    /*0x6E*/ RMSGH(ORION_IGNORE_PACKET, "Character Animation", 0x0e, CharacterAnimation),
    /*0x6F*/ BMSGH(ORION_IGNORE_PACKET, "Secure Trading", PACKET_VARIABLE_SIZE, SecureTrading),
    /*0x70*/ RMSGH(ORION_IGNORE_PACKET, "Graphic Effect", 0x1c, GraphicEffect),
    /*0x71*/
    BMSGH(ORION_IGNORE_PACKET, "Bulletin Board Data", PACKET_VARIABLE_SIZE, BulletinBoardData),
    /*0x72*/ BMSGH(ORION_IGNORE_PACKET, "War Mode", 0x05, Warmode),
    /*0x73*/ BMSGH(ORION_IGNORE_PACKET, "Ping", 0x02, Ping),
    /*0x74*/ RMSGH(ORION_IGNORE_PACKET, "Vendor Buy List", PACKET_VARIABLE_SIZE, BuyList),
    /*0x75*/ SMSG(ORION_SAVE_PACKET, "Rename Character", 0x23),
    /*0x76*/ RMSG(ORION_SAVE_PACKET, "New Subserver", 0x10),
    /*0x77*/ RMSGH(ORION_SAVE_PACKET, "Update Character", 0x11, UpdateCharacter),
    /*0x78*/ RMSGH(ORION_SAVE_PACKET, "Update Object", PACKET_VARIABLE_SIZE, UpdateObject),
    /*0x79*/ BMSG(ORION_SAVE_PACKET, "Resource query (God client)", 0x09),
    /*0x7A*/ BMSG(ORION_SAVE_PACKET, "Resource data (God client)", PACKET_VARIABLE_SIZE),
    /*0x7B*/ RMSG(ORION_SAVE_PACKET, "Sequence?", 0x02),
    /*0x7C*/ RMSGH(ORION_IGNORE_PACKET, "Open Menu Gump", PACKET_VARIABLE_SIZE, OpenMenu),
    /*0x7D*/ SMSG(ORION_IGNORE_PACKET, "Menu Choice", 0x0d),
    /*0x7E*/ BMSG(ORION_SAVE_PACKET, "God view query (God client)", 0x02),
    /*0x7F*/ BMSG(ORION_SAVE_PACKET, "God view data (God client)", PACKET_VARIABLE_SIZE),
    /*0x80*/ SMSG(ORION_IGNORE_PACKET, "First Login", 0x3e),
    /*0x81*/ RMSG(ORION_SAVE_PACKET, "Change character", PACKET_VARIABLE_SIZE),
    /*0x82*/ RMSGH(ORION_IGNORE_PACKET, "Login Error", 0x02, LoginError),
    /*0x83*/ SMSG(ORION_IGNORE_PACKET, "Delete Character", 0x27),
    /*0x84*/ BMSG(ORION_SAVE_PACKET, "Change password", 0x45),
    /*0x85*/
    RMSGH(ORION_IGNORE_PACKET, "Character List Notification", 0x02, CharacterListNotification),
    /*0x86*/
    RMSGH(ORION_IGNORE_PACKET, "Resend Character List", PACKET_VARIABLE_SIZE, ResendCharacterList),
    /*0x87*/ BMSG(ORION_SAVE_PACKET, "Send resources (God client)", PACKET_VARIABLE_SIZE),
    /*0x88*/ RMSGH(ORION_IGNORE_PACKET, "Open Paperdoll", 0x42, OpenPaperdoll),
    /*0x89*/ RMSGH(ORION_SAVE_PACKET, "Corpse Equipment", PACKET_VARIABLE_SIZE, CorpseEquipment),
    /*0x8A*/ BMSG(ORION_SAVE_PACKET, "Trigger edit (God client)", PACKET_VARIABLE_SIZE),
    /*0x8B*/ RMSG(ORION_SAVE_PACKET, "Display sign gump", PACKET_VARIABLE_SIZE),
    /*0x8C*/ RMSGH(ORION_IGNORE_PACKET, "Relay Server", 0x0b, RelayServer),
    /*0x8D*/ SMSG(ORION_SAVE_PACKET, "UO3D create character", PACKET_VARIABLE_SIZE),
    /*0x8E*/ BMSG(ORION_SAVE_PACKET, "Move character (God client)", PACKET_VARIABLE_SIZE),
    /*0x8F*/ UMSG(ORION_SAVE_PACKET, PACKET_VARIABLE_SIZE),
    /*0x90*/ RMSGH(ORION_IGNORE_PACKET, "Display Map", 0x13, DisplayMap),
    /*0x91*/ SMSG(ORION_IGNORE_PACKET, "Second Login", 0x41),
    /*0x92*/ BMSG(ORION_SAVE_PACKET, "Update multi data (God client)", PACKET_VARIABLE_SIZE),
    /*0x93*/ RMSGH(ORION_IGNORE_PACKET, "Open Book", 0x63, OpenBook),
    /*0x94*/ BMSG(ORION_SAVE_PACKET, "Update skills data (God client)", PACKET_VARIABLE_SIZE),
    /*0x95*/ BMSGH(ORION_IGNORE_PACKET, "Dye Data", 0x09, DyeData),
    /*0x96*/ BMSG(ORION_SAVE_PACKET, "Game central monitor (God client)", PACKET_VARIABLE_SIZE),
    /*0x97*/ RMSGH(ORION_SAVE_PACKET, "Move Player", 0x02, MovePlayer),
    /*0x98*/ BMSG(ORION_SAVE_PACKET, "All Names (3D Client Only)", PACKET_VARIABLE_SIZE),
    /*0x99*/ BMSGH(ORION_SAVE_PACKET, "Multi Placement", 0x1a, MultiPlacement),
    /*0x9A*/ BMSGH(ORION_SAVE_PACKET, "ASCII Prompt", PACKET_VARIABLE_SIZE, ASCIIPrompt),
    /*0x9B*/ SMSG(ORION_IGNORE_PACKET, "Help Request", 0x102),
    /*0x9C*/ BMSG(ORION_SAVE_PACKET, "Assistant request (God client)", 0x135),
    /*0x9D*/ BMSG(ORION_SAVE_PACKET, "GM single (God client)", 0x33),
    /*0x9E*/ RMSGH(ORION_IGNORE_PACKET, "Vendor Sell List", PACKET_VARIABLE_SIZE, SellList),
    /*0x9F*/ SMSG(ORION_IGNORE_PACKET, "Vendor Sell Reply", PACKET_VARIABLE_SIZE),
    /*0xA0*/ SMSG(ORION_IGNORE_PACKET, "Select Server", 0x03),
    /*0xA1*/ RMSGH(ORION_IGNORE_PACKET, "Update Hitpoints", 0x09, UpdateHitpoints),
    /*0xA2*/ RMSGH(ORION_IGNORE_PACKET, "Update Mana", 0x09, UpdateMana),
    /*0xA3*/ RMSGH(ORION_IGNORE_PACKET, "Update Stamina", 0x09, UpdateStamina),
    /*0xA4*/ SMSG(ORION_SAVE_PACKET, "System Information", 0x95),
    /*0xA5*/ RMSGH(ORION_SAVE_PACKET, "Open URL", PACKET_VARIABLE_SIZE, OpenUrl),
    /*0xA6*/ RMSGH(ORION_IGNORE_PACKET, "Tip Window", PACKET_VARIABLE_SIZE, TipWindow),
    /*0xA7*/ SMSG(ORION_SAVE_PACKET, "Request Tip", 0x04),
    /*0xA8*/ RMSGH(ORION_IGNORE_PACKET, "Server List", PACKET_VARIABLE_SIZE, ServerList),
    /*0xA9*/ RMSGH(ORION_IGNORE_PACKET, "Character List", PACKET_VARIABLE_SIZE, CharacterList),
    /*0xAA*/ RMSGH(ORION_IGNORE_PACKET, "Attack Reply", 0x05, AttackCharacter),
    /*0xAB*/ RMSGH(ORION_SAVE_PACKET, "Text Entry Dialog", PACKET_VARIABLE_SIZE, TextEntryDialog),
    /*0xAC*/ SMSG(ORION_SAVE_PACKET, "Text Entry Dialog Reply", PACKET_VARIABLE_SIZE),
    /*0xAD*/ SMSG(ORION_IGNORE_PACKET, "Unicode Client Talk", PACKET_VARIABLE_SIZE),
    /*0xAE*/ RMSGH(ORION_IGNORE_PACKET, "Unicode Server Talk", PACKET_VARIABLE_SIZE, UnicodeTalk),
    /*0xAF*/ RMSGH(ORION_SAVE_PACKET, "Display Death", 0x0d, DisplayDeath),
    /*0xB0*/ RMSGH(ORION_IGNORE_PACKET, "Open Gump", PACKET_VARIABLE_SIZE, OpenGump),
    /*0xB1*/ SMSG(ORION_IGNORE_PACKET, "Gump Choice", PACKET_VARIABLE_SIZE),
    /*0xB2*/ BMSG(ORION_SAVE_PACKET, "Chat Data", PACKET_VARIABLE_SIZE),
    /*0xB3*/ RMSG(ORION_SAVE_PACKET, "Chat Text ?", PACKET_VARIABLE_SIZE),
    /*0xB4*/ BMSG(ORION_SAVE_PACKET, "Target object list (God client)", PACKET_VARIABLE_SIZE),
    /*0xB5*/ BMSGH(ORION_SAVE_PACKET, "Open Chat Window", 0x40, OpenChat),
    /*0xB6*/ SMSG(ORION_SAVE_PACKET, "Popup Help Request", 0x09),
    /*0xB7*/ RMSG(ORION_SAVE_PACKET, "Popup Help Data", PACKET_VARIABLE_SIZE),
    /*0xB8*/
    BMSGH(ORION_IGNORE_PACKET, "Character Profile", PACKET_VARIABLE_SIZE, CharacterProfile),
    /*0xB9*/ RMSGH(ORION_SAVE_PACKET, "Enable locked client features", 0x03, EnableLockedFeatures),
    /*0xBA*/ RMSGH(ORION_IGNORE_PACKET, "Display Quest Arrow", 0x06, DisplayQuestArrow),
    /*0xBB*/ SMSG(ORION_SAVE_PACKET, "Account ID ?", 0x09),
    /*0xBC*/ RMSGH(ORION_IGNORE_PACKET, "Season", 0x03, Season),
    /*0xBD*/ BMSGH(ORION_SAVE_PACKET, "Client Version", PACKET_VARIABLE_SIZE, ClientVersion),
    /*0xBE*/ BMSGH(ORION_SAVE_PACKET, "Assist Version", PACKET_VARIABLE_SIZE, AssistVersion),
    /*0xBF*/ BMSGH(ORION_SAVE_PACKET, "Extended Command", PACKET_VARIABLE_SIZE, ExtendedCommand),
    /*0xC0*/ RMSGH(ORION_IGNORE_PACKET, "Graphical Effect", 0x24, GraphicEffect),
    /*0xC1*/
    RMSGH(ORION_IGNORE_PACKET, "Display cliloc String", PACKET_VARIABLE_SIZE, DisplayClilocString),
    /*0xC2*/ BMSGH(ORION_SAVE_PACKET, "Unicode prompt", PACKET_VARIABLE_SIZE, UnicodePrompt),
    /*0xC3*/ UMSG(ORION_SAVE_PACKET, PACKET_VARIABLE_SIZE),
    /*0xC4*/ UMSG(ORION_SAVE_PACKET, 0x06),
    /*0xC5*/ BMSG(ORION_SAVE_PACKET, "Invalid map (God client)", 0xcb),
    /*0xC6*/ RMSG(ORION_SAVE_PACKET, "Invalid map enable", 0x01),
    /*0xC7*/ RMSGH(ORION_IGNORE_PACKET, "Graphical Effect", 0x31, GraphicEffect),
    /*0xC8*/ BMSGH(ORION_SAVE_PACKET, "Client View Range", 0x02, ClientViewRange),
    /*0xC9*/ BMSG(ORION_SAVE_PACKET, "Trip time", 0x06),
    /*0xCA*/ BMSG(ORION_SAVE_PACKET, "UTrip time", 0x06),
    /*0xCB*/ UMSG(ORION_SAVE_PACKET, 0x07),
    /*0xCC*/
    RMSGH(
        ORION_IGNORE_PACKET,
        "Localized Text Plus String",
        PACKET_VARIABLE_SIZE,
        DisplayClilocString),
    /*0xCD*/ UMSG(ORION_SAVE_PACKET, 0x01),
    /*0xCE*/ UMSG(ORION_SAVE_PACKET, PACKET_VARIABLE_SIZE),
    /*0xCF*/ UMSG(ORION_SAVE_PACKET, 0x4e),
    /*0xD0*/ UMSG(ORION_SAVE_PACKET, PACKET_VARIABLE_SIZE),
    /*0xD1*/ RMSGH(ORION_IGNORE_PACKET, "Logout", 0x02, Logout),
    /*0xD2*/ RMSGH(ORION_SAVE_PACKET, "Update Character (New)", 0x19, UpdateCharacter),
    /*0xD3*/ RMSGH(ORION_SAVE_PACKET, "Update Object (New)", PACKET_VARIABLE_SIZE, UpdateObject),
    /*0xD4*/ BMSGH(ORION_IGNORE_PACKET, "Open Book (New)", PACKET_VARIABLE_SIZE, OpenBookNew),
    /*0xD5*/ UMSG(ORION_SAVE_PACKET, PACKET_VARIABLE_SIZE),
    /*0xD6*/ BMSGH(ORION_IGNORE_PACKET, "Mega cliloc", PACKET_VARIABLE_SIZE, MegaCliloc),
    /*0xD7*/ SMSG(ORION_SAVE_PACKET, "+AoS command", PACKET_VARIABLE_SIZE),
    /*0xD8*/ RMSGH(ORION_IGNORE_PACKET, "Custom house", PACKET_VARIABLE_SIZE, CustomHouse),
    /*0xD9*/ SMSG(ORION_SAVE_PACKET, "+Metrics", 0x10c),
    /*0xDA*/ BMSG(ORION_SAVE_PACKET, "Mahjong game command", PACKET_VARIABLE_SIZE),
    /*0xDB*/ RMSG(ORION_SAVE_PACKET, "Character transfer log", PACKET_VARIABLE_SIZE),
    /*0xDC*/ RMSGH(ORION_IGNORE_PACKET, "OPL Info Packet", 9, OPLInfo),
    /*0xDD*/
    RMSGH(ORION_IGNORE_PACKET, "Compressed Gump", PACKET_VARIABLE_SIZE, OpenCompressedGump),
    /*0xDE*/ RMSG(ORION_SAVE_PACKET, "Update characters combatants", PACKET_VARIABLE_SIZE),
    /*0xDF*/ RMSGH(ORION_SAVE_PACKET, "Buff/Debuff", PACKET_VARIABLE_SIZE, BuffDebuff),
    /*0xE0*/ SMSG(ORION_SAVE_PACKET, "Bug Report KR", PACKET_VARIABLE_SIZE),
    /*0xE1*/ SMSG(ORION_SAVE_PACKET, "Client Type KR/SA", 0x09),
    /*0xE2*/ RMSGH(ORION_IGNORE_PACKET, "New Character Animation", 0xa, NewCharacterAnimation),
    /*0xE3*/ RMSG(ORION_SAVE_PACKET, "KR Encryption Responce", 0x4d),
    /*0xE4*/ UMSG(ORION_SAVE_PACKET, PACKET_VARIABLE_SIZE),
    /*0xE5*/ UMSG(ORION_SAVE_PACKET, PACKET_VARIABLE_SIZE),
    /*0xE6*/ UMSG(ORION_SAVE_PACKET, PACKET_VARIABLE_SIZE),
    /*0xE7*/ UMSG(ORION_SAVE_PACKET, PACKET_VARIABLE_SIZE),
    /*0xE8*/ UMSG(ORION_SAVE_PACKET, PACKET_VARIABLE_SIZE),
    /*0xE9*/ UMSG(ORION_SAVE_PACKET, PACKET_VARIABLE_SIZE),
    /*0xEA*/ UMSG(ORION_SAVE_PACKET, PACKET_VARIABLE_SIZE),
    /*0xEB*/ UMSG(ORION_SAVE_PACKET, PACKET_VARIABLE_SIZE),
    /*0xEC*/ SMSG(ORION_SAVE_PACKET, "Equip Macro", PACKET_VARIABLE_SIZE),
    /*0xED*/ SMSG(ORION_SAVE_PACKET, "Unequip item macro", PACKET_VARIABLE_SIZE),
    /*0xEE*/ UMSG(ORION_SAVE_PACKET, PACKET_VARIABLE_SIZE),
    /*0xEF*/ SMSG(ORION_SAVE_PACKET, "KR/2D Client Login/Seed", 0x15),
    /*0xF0*/
    BMSGH(ORION_SAVE_PACKET, "Krrios client special", PACKET_VARIABLE_SIZE, KrriosClientSpecial),
    /*0xF1*/
    SMSG(ORION_SAVE_PACKET, "Client-Server Time Synchronization Request", PACKET_VARIABLE_SIZE),
    /*0xF2*/
    RMSG(ORION_SAVE_PACKET, "Client-Server Time Synchronization Response", PACKET_VARIABLE_SIZE),
    /*0xF3*/ RMSGH(ORION_SAVE_PACKET, "Update Item (SA)", 0x18, UpdateItemSA),
    /*0xF4*/ UMSG(ORION_SAVE_PACKET, PACKET_VARIABLE_SIZE),
    /*0xF5*/ RMSGH(ORION_IGNORE_PACKET, "Display New Map", 0x15, DisplayMap),
    /*0xF6*/ RMSGH(ORION_SAVE_PACKET, "Boat moving", PACKET_VARIABLE_SIZE, BoatMoving),
    /*0xF7*/ RMSGH(ORION_SAVE_PACKET, "Packets (0xF3) list", PACKET_VARIABLE_SIZE, PacketsList),
    /*0xF8*/ SMSG(ORION_SAVE_PACKET, "Character Creation (7.0.16.0)", 0x6a),
    /*0xF9*/ UMSG(ORION_SAVE_PACKET, PACKET_VARIABLE_SIZE),
    /*0xFA*/ UMSG(ORION_SAVE_PACKET, PACKET_VARIABLE_SIZE),
    /*0xFB*/ UMSG(ORION_SAVE_PACKET, PACKET_VARIABLE_SIZE),
    /*0xFC*/ BMSGH(ORION_SAVE_PACKET, "Orion messages", PACKET_VARIABLE_SIZE, OrionMessages),
    /*0xFD*/ UMSG(ORION_SAVE_PACKET, PACKET_VARIABLE_SIZE),
    /*0xFE*/ RMSG(ORION_SAVE_PACKET, "Razor Handshake", 0x8),
    /*0xFF*/ UMSG(ORION_SAVE_PACKET, PACKET_VARIABLE_SIZE)
};

CPacketManager::CPacketManager()
{
}

CPacketManager::~CPacketManager()
{
}

bool CPacketManager::AutoLoginNameExists(const std::string& name)
{
    if (AutoLoginNames.length() == 0u)
    {
        return false;
    }

    std::string search = std::string("|") + name + "|";
    return (AutoLoginNames.find(search) != std::string::npos);
}

#define CV_PRINT 0

#if CV_PRINT != 0
#define CVPRINT(s) LOG(s)
#else //CV_PRINT==0
#define CVPRINT(s)
#endif //CV_PRINT!=0

void CPacketManager::ConfigureClientVersion(u32 newClientVersion)
{
    if (newClientVersion >= CV_500A)
    {
        CVPRINT("Set new length for packet 0x0B (>= 5.0.0a)\n");
        m_Packets[0x0B].Size = 0x07;
        CVPRINT("Set new length for packet 0x16 (>= 5.0.0a)\n");
        m_Packets[0x16].Size = PACKET_VARIABLE_SIZE;
        CVPRINT("Set new length for packet 0x31 (>= 5.0.0a)\n");
        m_Packets[0x31].Size = PACKET_VARIABLE_SIZE;
    }
    else
    {
        CVPRINT("Set standart length for packet 0x0B (< 5.0.0a)\n");
        m_Packets[0x0B].Size = 0x10A;
        CVPRINT("Set standart length for packet 0x16 (< 5.0.0a)\n");
        m_Packets[0x16].Size = 0x01;
        CVPRINT("Set standart length for packet 0x31 (< 5.0.0a)\n");
        m_Packets[0x31].Size = 0x01;
    }

    if (newClientVersion >= CV_5090)
    {
        CVPRINT("Set new length for packet 0xE1 (>= 5.0.9.0)\n");
        m_Packets[0xE1].Size = PACKET_VARIABLE_SIZE;
    }
    else
    {
        CVPRINT("Set standart length for packet 0xE1 (<= 5.0.9.0)\n");
        m_Packets[0xE1].Size = 0x09;
    }

    if (newClientVersion >= CV_6013)
    {
        CVPRINT("Set new length for packet 0xE3 (>= 6.0.1.3)\n");
        m_Packets[0xE3].Size = PACKET_VARIABLE_SIZE;
        CVPRINT("Set new length for packet 0xE6 (>= 6.0.1.3)\n");
        m_Packets[0xE6].Size = 0x05;
        CVPRINT("Set new length for packet 0xE7 (>= 6.0.1.3)\n");
        m_Packets[0xE7].Size = 0x0C;
        CVPRINT("Set new length for packet 0xE8 (>= 6.0.1.3)\n");
        m_Packets[0xE8].Size = 0x0D;
        CVPRINT("Set new length for packet 0xE9 (>= 6.0.1.3)\n");
        m_Packets[0xE9].Size = 0x4B;
        CVPRINT("Set new length for packet 0xEA (>= 6.0.1.3)\n");
        m_Packets[0xEA].Size = 0x03;
    }
    else
    {
        CVPRINT("Set standart length for packet 0xE3 (<= 6.0.1.3)\n");
        m_Packets[0xE3].Size = 0x4D;
        CVPRINT("Set standart length for packet 0xE6 (<= 6.0.1.3)\n");
        m_Packets[0xE6].Size = PACKET_VARIABLE_SIZE;
        CVPRINT("Set standart length for packet 0xE7 (<= 6.0.1.3)\n");
        m_Packets[0xE7].Size = PACKET_VARIABLE_SIZE;
        CVPRINT("Set standart length for packet 0xE8 (<= 6.0.1.3)\n");
        m_Packets[0xE8].Size = PACKET_VARIABLE_SIZE;
        CVPRINT("Set standart length for packet 0xE9 (<= 6.0.1.3)\n");
        m_Packets[0xE9].Size = PACKET_VARIABLE_SIZE;
        CVPRINT("Set standart length for packet 0xEA (<= 6.0.1.3)\n");
        m_Packets[0xEA].Size = PACKET_VARIABLE_SIZE;
    }

    if (newClientVersion >= CV_6017)
    {
        CVPRINT("Set new length for packet 0x08 (>= 6.0.1.7)\n");
        m_Packets[0x08].Size = 0x0F;
        CVPRINT("Set new length for packet 0x25 (>= 6.0.1.7)\n");
        m_Packets[0x25].Size = 0x15;
    }
    else
    {
        CVPRINT("Set standart length for packet 0x08 (<= 6.0.1.7)\n");
        m_Packets[0x08].Size = 0x0E;
        CVPRINT("Set standart length for packet 0x25 (<= 6.0.1.7)\n");
        m_Packets[0x25].Size = 0x14;
    }

    if (newClientVersion == CV_6060)
    {
        CVPRINT("Set new length for packet 0xEE (>= 6.0.6.0)\n");
        m_Packets[0xEE].Size = 0x2000;
        CVPRINT("Set new length for packet 0xEF (>= 6.0.6.0)\n");
        m_Packets[0xEF].Size = 0x2000;
        CVPRINT("Set new length for packet 0xF1 (>= 6.0.6.0)\n");
        m_Packets[0xF1].Size = 0x09;
    }
    else
    {
        CVPRINT("Set standart length for packet 0xEE (<= 6.0.6.0)\n");
        m_Packets[0xEE].Size = PACKET_VARIABLE_SIZE;
        CVPRINT("Set standart length for packet 0xEF (<= 6.0.6.0)\n");
        m_Packets[0xEF].Size = 0x15;
        CVPRINT("Set standart length for packet 0xF1 (<= 6.0.6.0)\n");
        m_Packets[0xF1].Size = PACKET_VARIABLE_SIZE;
    }

    if (newClientVersion >= CV_60142)
    {
        CVPRINT("Set new length for packet 0xB9 (>= 6.0.14.2)\n");
        m_Packets[0xB9].Size = 0x05;
    }
    else
    {
        CVPRINT("Set standart length for packet 0xB9 (<= 6.0.14.2)\n");
        m_Packets[0xB9].Size = 0x03;
    }

    if (newClientVersion >= CV_7000)
    {
        CVPRINT("Set new length for packet 0xEE (>= 7.0.0.0)\n");
        m_Packets[0xEE].Size = 0x2000;
        CVPRINT("Set new length for packet 0xEF (>= 7.0.0.0)\n");
        m_Packets[0xEF].Size = 0x2000;
        /*CVPRINT("Set new length for packet 0xF0 (>= 7.0.0.0)\n");
        m_Packets[0xF0].size = 0x2000;
        CVPRINT("Set new length for packet 0xF1 (>= 7.0.0.0)\n");
        m_Packets[0xF1].size = 0x2000;
        CVPRINT("Set new length for packet 0xF2 (>= 7.0.0.0)\n");
        m_Packets[0xF2].size = 0x2000;*/
    }
    else
    {
        CVPRINT("Set standart length for packet 0xEE (<= 7.0.0.0)\n");
        m_Packets[0xEE].Size = PACKET_VARIABLE_SIZE;
        CVPRINT("Set standart length for packet 0xEF (<= 7.0.0.0)\n");
        m_Packets[0xEF].Size = 0x15;
        /*CVPRINT("Set standart length for packet 0xF0 (<= 7.0.0.0)\n");
        m_Packets[0xF0].size = PACKET_VARIABLE_SIZE;
        CVPRINT("Set standart length for packet 0xF1 (<= 7.0.0.0)\n");
        m_Packets[0xF1].size = PACKET_VARIABLE_SIZE;
        CVPRINT("Set standart length for packet 0xF2 (<= 7.0.0.0)\n");
        m_Packets[0xF2].size = PACKET_VARIABLE_SIZE;*/
    }

    if (newClientVersion >= CV_7090)
    {
        CVPRINT("Set new length for packet 0x24 (>= 7.0.9.0)\n");
        m_Packets[0x24].Size = 0x09;
        CVPRINT("Set new length for packet 0x99 (>= 7.0.9.0)\n");
        m_Packets[0x99].Size = 0x1E;
        CVPRINT("Set new length for packet 0xBA (>= 7.0.9.0)\n");
        m_Packets[0xBA].Size = 0x0A;
        CVPRINT("Set new length for packet 0xF3 (>= 7.0.9.0)\n");
        m_Packets[0xF3].Size = 0x1A;

        // Already changed in client 7.0.8.2
        CVPRINT("Set new length for packet 0xF1 (>= 7.0.9.0)\n");
        m_Packets[0xF1].Size = 0x09;
        CVPRINT("Set new length for packet 0xF2 (>= 7.0.9.0)\n");
        m_Packets[0xF2].Size = 0x19;
    }
    else
    {
        CVPRINT("Set standart length for packet 0x24 (<= 7.0.9.0)\n");
        m_Packets[0x24].Size = 0x07;
        CVPRINT("Set standart length for packet 0x99 (<= 7.0.9.0)\n");
        m_Packets[0x99].Size = 0x1A;
        CVPRINT("Set standart length for packet 0xBA (<= 7.0.9.0)\n");
        m_Packets[0xBA].Size = 0x06;
        CVPRINT("Set standart length for packet 0xF3 (<= 7.0.9.0)\n");
        m_Packets[0xF3].Size = 0x18;

        // Already changed in client 7.0.8.2
        CVPRINT("Set standart length for packet 0xF1 (<= 7.0.9.0)\n");
        m_Packets[0xF1].Size = PACKET_VARIABLE_SIZE;
        CVPRINT("Set standart length for packet 0xF2 (<= 7.0.9.0)\n");
        m_Packets[0xF2].Size = PACKET_VARIABLE_SIZE;
    }

    if (newClientVersion >= CV_70180)
    {
        CVPRINT("Set new length for packet 0x00 (>= 7.0.18.0)\n");
        m_Packets[0x00].Size = 0x6A;
    }
    else
    {
        CVPRINT("Set standart length for packet 0x24 (<= 7.0.18.0)\n");
        m_Packets[0x00].Size = 0x68;
    }
}

int CPacketManager::GetPacketSize(const std::vector<u8>& packet, int& offsetToSize)
{
    if (static_cast<unsigned int>(!packet.empty()) != 0u)
    {
        return m_Packets[packet[0]].Size;
    }

    return 0;
}

void CPacketManager::SendMegaClilocRequests()
{
    PROFILER_EVENT();
    if (g_TooltipsEnabled && !m_MegaClilocRequests.empty())
    {
        if (GameVars::GetClientVersion() >= CV_500A)
        {
            while (!m_MegaClilocRequests.empty())
            {
                CPacketMegaClilocRequest(m_MegaClilocRequests).Send();
            }
        }
        else
        {
            for (int i : m_MegaClilocRequests)
            {
                CPacketMegaClilocRequestOld(i).Send();
            }
            m_MegaClilocRequests.clear();
        }
    }
}

void CPacketManager::AddMegaClilocRequest(int serial)
{
    for (int item : m_MegaClilocRequests)
    {
        if (item == serial)
        {
            return;
        }
    }

    m_MegaClilocRequests.push_back(serial);
}

void CPacketManager::OnReadFailed()
{
    LOG_INFO("PacketManager", "OnReadFailed...Disconnecting...");
    g_Orion.DisconnectGump();
    //g_Orion.Disconnect();
    g_AbyssPacket03First = true;
    g_ConnectionManager.Disconnect();
}

void CPacketManager::OnPacket()
{
    u32 ticks = g_Ticks;
    g_TotalRecvSize += (u32)GetSize();
    CPacketInfo& info = m_Packets[*GetBuffer()];
    if (info.save)
    {
#if defined(ORION_WINDOWS) // FIXME: localtime_s (use C++ if possible)
        time_t rawtime;
        struct tm timeinfo;
        char buffer[80];

        time(&rawtime);
        localtime_s(&timeinfo, &rawtime);
        strftime(buffer, sizeof(buffer), "%d-%m-%Y %H:%M:%S", &timeinfo);
        LOG_INFO(
            "PacketManager",
            "--- ^(%d) r(+%zd => %d) %s Server:: %s",
            ticks - g_LastPacketTime,
            GetSize(),
            g_TotalRecvSize,
            buffer,
            info.Name);
#else
        LOG_INFO(
            "PacketManager",
            "--- ^(%d) r(+%zd => %d) Server:: %s",
            ticks - g_LastPacketTime,
            GetSize(),
            g_TotalRecvSize,
            info.Name);
#endif
        //LOG_DUMP(Start, (int)Size);
    }

    g_LastPacketTime = ticks;
    if (info.Direction != DIR_RECV && info.Direction != DIR_BOTH)
    {
        LOG_INFO("PacketManager", "message direction invalid: 0x%02X", *GetBuffer());
    }
    else if (info.Handler != 0)
    {
        SetPtr(GetBuffer() + 1);
        if (info.Size == 0)
            SetPtr(GetPtr() + 2);
        (this->*(info.Handler))();
    }
}

void CPacketManager::SavePluginReceivePacket(u8* buf, int size)
{
    std::vector<u8> packet(size);
    memcpy(&packet[0], &buf[0], size);

    m_Mutex.Lock();
    m_PluginData.push_front(packet);
    m_Mutex.Unlock();
}

void CPacketManager::ProcessPluginPackets()
{
    PROFILER_EVENT();

    m_Mutex.Lock();
    while (!m_PluginData.empty())
    {
        std::vector<u8>& packet = m_PluginData.back();

        PluginReceiveHandler(&packet[0], (int)packet.size());
        packet.clear();

        m_PluginData.pop_back();
    }
    m_Mutex.Unlock();
}

void CPacketManager::PluginReceiveHandler(u8* buf, int size)
{
    SetData(buf, size);

    u32 ticks = g_Ticks;
    g_TotalRecvSize += (u32)GetSize();
    CPacketInfo& info = m_Packets[*GetBuffer()];
    LOG_INFO(
        "PacketManager",
        "--- ^(%d) r(+%zd => %d) Plugin->Client:: %s",
        ticks - g_LastPacketTime,
        GetSize(),
        g_TotalRecvSize,
        info.Name);
    //LOG_DUMP(Start, (int)Size);

    g_LastPacketTime = ticks;
    if (info.Direction != DIR_RECV && info.Direction != DIR_BOTH)
    {
        LOG_ERROR("PacketManager", "message direction invalid: 0x%02X", *buf);
    }
    else if (info.Handler != 0)
    {
        SetPtr(GetBuffer() + 1);
        if (info.Size == 0)
            SetPtr(GetPtr() + 2);
        (this->*(info.Handler))();
    }
}

#define PACKET_HANDLER(name) void CPacketManager::Handle##name()

PACKET_HANDLER(LoginError)
{
    if (g_GameState == GS_MAIN_CONNECT || g_GameState == GS_SERVER_CONNECT ||
        g_GameState == GS_GAME_CONNECT)
    {
        g_ConnectionScreen.SetConnectionFailed(true);
        g_ConnectionScreen.SetErrorCode(ReadBE<u8>());
        g_ConnectionManager.Disconnect();
    }
}

PACKET_HANDLER(ServerList)
{
    g_ServerList.ParsePacket(*this);
}

PACKET_HANDLER(RelayServer)
{
    memset(&g_SelectedCharName[0], 0, sizeof(g_SelectedCharName));
    u32* paddr = (u32*)GetPtr();
    Move(4);
    char relayIP[30] = { 0 };
    Core::Socket::AddressToString(*paddr, relayIP);
    int relayPort = ReadBE<u16>();
    g_Orion.RelayServer(relayIP, relayPort, GetPtr());
    g_PacketLoginComplete = false;
    g_CurrentMap          = 0;
}

PACKET_HANDLER(CharacterList)
{
    HandleResendCharacterList();
    u8 locCount = ReadBE<u8>();
    g_CityList.Clear();
    if (GameVars::GetClientVersion() >= CV_70130)
    {
        for (int i = 0; i < locCount; i++)
        {
            CCityItemNew* city = new CCityItemNew();

            city->LocationIndex = ReadBE<u8>();

            city->Name = ReadString(32);
            city->Area = ReadString(32);

            city->X        = ReadBE<u32>();
            city->Y        = ReadBE<u32>();
            city->Z        = ReadBE<u32>();
            city->MapIndex = ReadBE<u32>();
            city->Cliloc   = ReadBE<u32>();

            Move(4);

            g_CityList.AddCity(city);
        }
    }
    else
    {
        for (int i = 0; i < locCount; i++)
        {
            CCityItem* city = new CCityItem();

            city->LocationIndex = ReadBE<u8>();

            city->Name = ReadString(31);
            city->Area = ReadString(31);

            city->InitCity();

            g_CityList.AddCity(city);
        }
    }

    g_ClientFlag = ReadBE<u32>();

    g_CharacterList.OnePerson = (bool)(g_ClientFlag & CLF_ONE_CHARACTER_SLOT);
    //g_SendLogoutNotification = (bool)(g_ClientFlag & LFF_RE);
    g_PopupEnabled = (bool)(g_ClientFlag & CLF_CONTEXT_MENU);
    g_TooltipsEnabled =
        (bool)(((g_ClientFlag & CLF_PALADIN_NECROMANCER_TOOLTIPS) != 0u) && (GameVars::GetClientVersion() >= CV_308Z));
    g_PaperdollBooks = (bool)(g_ClientFlag & CLF_PALADIN_NECROMANCER_TOOLTIPS);

    g_CharacterListScreen.UpdateContent();
}

PACKET_HANDLER(ResendCharacterList)
{
    g_Orion.InitScreen(GS_CHARACTER);

    int numSlots = ReadBE<u8>();
    if (*GetBuffer() == 0x86)
    {
        LOG_INFO("PacketManager", "/======Resend chars===");
    }
    else
    {
        LOG_INFO("PacketManager", "/======Chars===");
    }

    g_CharacterList.Clear();
    g_CharacterList.Count = numSlots;

    int autoPos        = -1;
    bool autoLogin     = g_MainScreen.m_AutoLogin->Checked;
    bool haveCharacter = false;

    if (numSlots == 0)
    {
        LOG_WARNING("PacketManager", "No slots in character list");
    }
    else
    {
        int selectedPos = -1;
        for (int i = 0; i < numSlots; i++)
        {
            std::string name = ReadString(30);
            Move(30);
            if (name.length() != 0u)
            {
                haveCharacter = true;
                g_CharacterList.SetName(i, name);

                if (autoLogin && autoPos == -1 && AutoLoginNameExists(name))
                {
                    autoPos = i;
                }

                if (name == g_CharacterList.LastCharacterName)
                {
                    g_CharacterList.Selected = i;
                    if (autoLogin && selectedPos == -1)
                    {
                        selectedPos = i;
                    }
                }
            }

            LOG_INFO("PacketManager", "%d: %s (%zd)", i, name.c_str(), name.length());
        }

        if (autoLogin && autoPos == -1)
        {
            autoPos = selectedPos;
        }
    }

    if (autoLogin && (numSlots != 0))
    {
        if (autoPos == -1)
        {
            autoPos = 0;
        }

        g_CharacterList.Selected = autoPos;
        if (g_CharacterList.GetName(autoPos).length() != 0u)
        {
            g_Orion.CharacterSelection(autoPos);
        }
    }

    if (*GetBuffer() == 0x86)
    {
        g_CharacterListScreen.UpdateContent();
    }

    if (!haveCharacter)
    {
        g_Orion.InitScreen(GS_PROFESSION_SELECT);
    }
}

PACKET_HANDLER(LoginComplete)
{
    g_PacketLoginComplete = true;
    g_Orion.LoginComplete(false);
}

PACKET_HANDLER(SetTime)
{
    g_ServerTimeHour   = ReadBE<u8>();
    g_ServerTimeMinute = ReadBE<u8>();
    g_ServerTimeSecond = ReadBE<u8>();
}

PACKET_HANDLER(EnterWorld)
{
    u32 serial      = ReadBE<u32>();
    ConfigSerial    = serial;
    bool loadConfig = false;

    if (g_World != nullptr)
    {
        LOG_WARNING("PacketManager", "Duplicate enter world message");

        g_Orion.SaveLocalConfig(g_PacketManager.ConfigSerial);
        ConfigSerial   = g_PlayerSerial;
        g_ConfigLoaded = false;
        loadConfig     = true;
    }

    g_Orion.ClearWorld();

    g_World = new CGameWorld(serial);

    Move(4); //unused

    if (strlen(g_SelectedCharName) != 0u)
    {
        g_Player->SetName(g_SelectedCharName);
    }

    g_Player->Graphic = ReadBE<u16>();
    g_Player->OnGraphicChange();

    g_Player->SetX(ReadBE<u16>());
    g_Player->SetY(ReadBE<u16>());
    g_Player->SetZ((char)ReadBE<u16>());
    g_Player->Direction = ReadBE<u8>();
    /*Move(1); //serverID
	Move(4); //unused
	Move(2); //serverBoundaryX
	Move(2); //serverBoundaryY
	Move(2); //serverBoundaryWidth
	Move(2); //serverBoundaryHeight*/

    g_RemoveRangeXY.x = g_Player->GetX();
    g_RemoveRangeXY.y = g_Player->GetY();

    g_Player->OffsetX = 0;
    g_Player->OffsetY = 0;
    g_Player->OffsetZ = 0;

    LOG_INFO("PacketManager", "Player 0x%08lX entered the world.", serial);

    g_MapManager.Init();
    g_MapManager.AddRender(g_Player);

    g_Orion.LoadStartupConfig(ConfigSerial);

    g_LastSpellIndex = 1;
    g_LastSkillIndex = 1;

    CPacketClientVersion(uo_client_version.GetValue()).Send();

    if (GameVars::GetClientVersion() >= CV_200)
    {
        CPacketGameWindowSize().Send();
    }

    if (GameVars::GetClientVersion() >= CV_200)
    {
        CPacketLanguage(g_Language).Send();
    }

    g_Orion.Click(g_PlayerSerial);
    CPacketStatusRequest(g_PlayerSerial).Send();

    if (g_Player->Dead())
    {
        g_Orion.ChangeSeason(ST_DESOLATION, DEATH_MUSIC_INDEX);
    }

    if (loadConfig)
    {
        g_Orion.LoginComplete(true);
    }
    else
    {
        CServer* server = g_ServerList.GetSelectedServer();
        if (server != nullptr)
        {
            g_Orion.CreateTextMessageF(3, 0x0037, "Login confirm to %s", server->Name.c_str());
        }
    }
}

PACKET_HANDLER(UpdateHitpoints)
{
    if (g_World == nullptr)
    {
        return;
    }

    u32 serial          = ReadBE<u32>();
    CGameCharacter* obj = g_World->FindWorldCharacter(serial);
    if (obj == nullptr)
    {
        return;
    }

    obj->MaxHits = ReadBE<i16>();
    obj->Hits    = ReadBE<i16>();

    g_GumpManager.UpdateContent(serial, 0, GT_STATUSBAR);
    g_GumpManager.UpdateContent(serial, 0, GT_TARGET_SYSTEM);
}

PACKET_HANDLER(UpdateMana)
{
    if (g_World == nullptr)
    {
        return;
    }

    u32 serial = ReadBE<u32>();

    CGameCharacter* obj = g_World->FindWorldCharacter(serial);
    if (obj == nullptr)
    {
        return;
    }

    obj->MaxMana = ReadBE<i16>();
    obj->Mana    = ReadBE<i16>();
    g_GumpManager.UpdateContent(serial, 0, GT_STATUSBAR);
}

PACKET_HANDLER(UpdateStamina)
{
    if (g_World == nullptr)
    {
        return;
    }

    u32 serial = ReadBE<u32>();

    CGameCharacter* obj = g_World->FindWorldCharacter(serial);
    if (obj == nullptr)
    {
        return;
    }

    obj->MaxStam = ReadBE<i16>();
    obj->Stam    = ReadBE<i16>();
    g_GumpManager.UpdateContent(serial, 0, GT_STATUSBAR);
}

PACKET_HANDLER(MobileAttributes)
{
    if (g_World == nullptr)
    {
        return;
    }

    u32 serial = ReadBE<u32>();

    CGameCharacter* obj = g_World->FindWorldCharacter(serial);

    if (obj == nullptr)
    {
        return;
    }

    obj->MaxHits = ReadBE<i16>();
    obj->Hits    = ReadBE<i16>();

    obj->MaxMana = ReadBE<i16>();
    obj->Mana    = ReadBE<i16>();

    obj->MaxStam = ReadBE<i16>();
    obj->Stam    = ReadBE<i16>();

    g_GumpManager.UpdateContent(serial, 0, GT_STATUSBAR);
    g_GumpManager.UpdateContent(serial, 0, GT_TARGET_SYSTEM);
}

PACKET_HANDLER(NewHealthbarUpdate)
{
    if (g_World == nullptr)
    {
        return;
    }

    if (*GetBuffer() == 0x16 && GameVars::GetClientVersion() < CV_500A)
    {
        return;
    }

    u32 serial          = ReadBE<u32>();
    CGameCharacter* obj = g_World->FindWorldCharacter(serial);
    if (obj == nullptr)
    {
        return;
    }

    u16 count = ReadBE<u16>();
    for (int i = 0; i < count; i++)
    {
        u16 type  = ReadBE<u16>();
        u8 enable = ReadBE<u8>(); //enable/disable
        u8 flags  = obj->GetFlags();
        if (type == 1) //Poison, enable as poisonlevel + 1
        {
            u8 poisonFlag = 0x04;
            if (enable != 0u)
            {
                if (GameVars::GetClientVersion() >= CV_7000)
                {
                    obj->SA_Poisoned = true;
                }
                else
                {
                    flags |= poisonFlag;
                }
            }
            else
            {
                if (GameVars::GetClientVersion() >= CV_7000)
                {
                    obj->SA_Poisoned = false;
                }
                else
                {
                    flags &= ~poisonFlag;
                }
            }
        }
        else if (type == 2) //Yellow hits
        {
            if (enable != 0u)
            {
                flags |= 0x08;
            }
            else
            {
                flags &= ~0x08;
            }
        }
        else if (type == 3) //Red?
        {
        }

        obj->SetFlags(flags);
    }

    g_GumpManager.UpdateContent(serial, 0, GT_STATUSBAR);
}

PACKET_HANDLER(UpdatePlayer)
{
    if (g_World == nullptr)
    {
        return;
    }

    u32 serial          = ReadBE<u32>();
    u16 graphic         = ReadBE<u16>();
    u8 graphicIncrement = ReadBE<u8>();
    u16 color           = ReadBE<u16>();
    u8 flags            = ReadBE<u8>();
    u16 x               = ReadBE<u16>();
    u16 y               = ReadBE<u16>();
    u16 serverID        = ReadBE<u16>();
    u8 direction        = ReadBE<u8>();
    char z              = ReadBE<u8>();

    // Invert character wakthrough bit.
    flags ^= 0x10;

    LOG_INFO(
        "PacketManager",
        "0x%08X 0x%04X %i 0x%04X 0x%02X %i %i %i %i %i",
        serial,
        graphic,
        graphicIncrement,
        color,
        flags,
        x,
        y,
        serverID,
        direction,
        z);
    g_World->UpdatePlayer(
        serial, graphic, graphicIncrement, color, flags, x, y, serverID, direction, z);
}

PACKET_HANDLER(CharacterStatus)
{
    if (g_World == nullptr)
    {
        return;
    }

    u32 serial = ReadBE<u32>();

    CGameCharacter* obj = g_World->FindWorldCharacter(serial);
    if (obj == nullptr)
    {
        return;
    }

    std::string name = ReadString(30);
    obj->SetName(name);

    obj->Hits    = ReadBE<i16>();
    obj->MaxHits = ReadBE<i16>();

    obj->CanChangeName = (ReadBE<u8>() != 0);

    u8 flag = ReadBE<u8>();

    if (flag > 0)
    {
        obj->Female = (ReadBE<u8>() != 0); //buf[43];

        if (serial == g_PlayerSerial)
        {
            short newStr = ReadBE<i16>();
            short newDex = ReadBE<i16>();
            short newInt = ReadBE<i16>();

            if (g_ConfigManager.StatReport && (g_Player->Str != 0))
            {
                short currentStr = g_Player->Str;
                short currentDex = g_Player->Dex;
                short currentInt = g_Player->Int;

                short deltaStr = newStr - currentStr;
                short deltaDex = newDex - currentDex;
                short deltaInt = newInt - currentInt;

                char str[64] = { 0 };
                if (deltaStr != 0)
                {
                    sprintf_s(
                        str, "Your strength has changed by %d.  It is now %d.", deltaStr, newStr);
                    g_Orion.CreateTextMessage(TT_SYSTEM, 0, 3, 0x0170, str);
                }

                if (deltaDex != 0)
                {
                    sprintf_s(
                        str, "Your dexterity has changed by %d.  It is now %d.", deltaDex, newDex);
                    g_Orion.CreateTextMessage(TT_SYSTEM, 0, 3, 0x0170, str);
                }

                if (deltaInt != 0)
                {
                    sprintf_s(
                        str,
                        "Your intelligence has changed by %d.  It is now %d.",
                        deltaInt,
                        newInt);
                    g_Orion.CreateTextMessage(TT_SYSTEM, 0, 3, 0x0170, str);
                }
            }

            g_Player->Str = newStr;
            g_Player->Dex = newDex;
            g_Player->Int = newInt;

            g_Player->Stam    = ReadBE<i16>();
            g_Player->MaxStam = ReadBE<i16>();
            g_Player->Mana    = ReadBE<i16>();
            g_Player->MaxMana = ReadBE<i16>();
            g_Player->Gold    = ReadBE<u32>();
            g_Player->Armor   = ReadBE<i16>();
            g_Player->Weight  = ReadBE<i16>(); //+64

            if (flag >= 5)
            {
                g_Player->MaxWeight = ReadBE<i16>(); //unpack16(buf + 66);
                u32 race            = ReadBE<u8>();

                if (race == 0u)
                {
                    race = 1;
                }

                g_Player->Race = (RACE_TYPE)race;
            }
            else
            {
                if (GameVars::GetClientVersion() >= CV_500A)
                {
                    g_Player->MaxWeight = 7 * (g_Player->Str / 2) + 40;
                }
                else
                {
                    g_Player->MaxWeight = (g_Player->Str * 4) + 25;
                }
            }

            if (flag >= 3)
            {
                g_Player->StatsCap     = ReadBE<u16>();
                g_Player->Followers    = ReadBE<u8>();
                g_Player->MaxFollowers = ReadBE<u8>();
            }

            if (flag >= 4)
            {
                g_Player->FireResistance   = ReadBE<i16>();
                g_Player->ColdResistance   = ReadBE<i16>();
                g_Player->PoisonResistance = ReadBE<i16>();
                g_Player->EnergyResistance = ReadBE<i16>();
                g_Player->Luck             = ReadBE<i16>();
                g_Player->MinDamage        = ReadBE<i16>();
                g_Player->MaxDamage        = ReadBE<i16>();
                g_Player->TithingPoints    = ReadBE<u32>();
            }

            if (flag >= 6)
            {
                g_Player->MaxPhysicalResistance = ReadBE<i16>();
                g_Player->MaxFireResistance     = ReadBE<i16>();
                g_Player->MaxColdResistance     = ReadBE<i16>();
                g_Player->MaxPoisonResistance   = ReadBE<i16>();
                g_Player->MaxEnergyResistance   = ReadBE<i16>();
                g_Player->DefenceChance         = ReadBE<i16>();
                g_Player->MaxDefenceChance      = ReadBE<i16>();
                g_Player->AttackChance          = ReadBE<i16>();
                g_Player->WeaponSpeed           = ReadBE<i16>();
                g_Player->WeaponDamage          = ReadBE<i16>();
                g_Player->LowerRegCost          = ReadBE<i16>();
                g_Player->SpellDamage           = ReadBE<i16>();
                g_Player->CastRecovery          = ReadBE<i16>();
                g_Player->CastSpeed             = ReadBE<i16>();
                g_Player->LowerManaCost         = ReadBE<i16>();
            }

            if (!g_ConnectionScreen.GetCompleted() && g_PacketLoginComplete)
            {
                g_Orion.LoginComplete(false);
            }
        }
    }

    g_GumpManager.UpdateContent(serial, 0, GT_STATUSBAR);
    g_GumpManager.UpdateContent(serial, 0, GT_TARGET_SYSTEM);
}

PACKET_HANDLER(UpdateItem)
{
    if (g_World == nullptr)
    {
        return;
    }

    u32 serial = ReadBE<u32>();

    if (serial == g_PlayerSerial)
    {
        return;
    }

    UPDATE_GAME_OBJECT_TYPE updateType = UGOT_ITEM;
    u16 count                          = 0;
    u8 graphicIncrement                = 0;
    u8 direction                       = 0;
    u16 color                          = 0;
    u8 flags                           = 0;

    if ((serial & 0x80000000) != 0u)
    {
        serial &= 0x7FFFFFFF;
        count = 1;
    }

    u16 graphic = ReadBE<u16>();

    if (g_Config.TheAbyss && (graphic & 0x7FFF) == 0x0E5C)
    {
        return;
    }

    if ((graphic & 0x8000) != 0)
    {
        graphic &= 0x7FFF;
        graphicIncrement = ReadBE<u8>();
    }

    if (count != 0u)
    {
        count = ReadBE<u16>();
    }
    else
    {
        count++;
    }

    u16 x = ReadBE<u16>();

    if ((x & 0x8000) != 0)
    {
        x &= 0x7FFF;
        direction = 1;
    }

    u16 y = ReadBE<u16>();

    if ((y & 0x8000) != 0)
    {
        y &= 0x7FFF;
        color = 1;
    }

    if ((y & 0x4000) != 0)
    {
        y &= 0x3FFF;
        flags = 1;
    }

    if (direction != 0u)
    {
        direction = ReadBE<u8>();
    }

    char z = ReadBE<u8>();

    if (color != 0u)
    {
        color = ReadBE<u16>();
    }

    if (flags != 0u)
    {
        flags = ReadBE<u8>();
    }

    if (graphic >= 0x4000)
    {
        //graphic += 0xC000;
        //updateType = UGOT_NEW_ITEM;
        updateType = UGOT_MULTI;
    }

    g_World->UpdateGameObject(
        serial,
        graphic,
        graphicIncrement,
        count,
        x,
        y,
        z,
        direction,
        color,
        flags,
        count,
        updateType,
        1);
}

PACKET_HANDLER(UpdateItemSA)
{
    if (g_World == nullptr)
    {
        return;
    }

    Move(2);
    UPDATE_GAME_OBJECT_TYPE updateType = (UPDATE_GAME_OBJECT_TYPE)ReadBE<u8>();
    u32 serial                         = ReadBE<u32>();
    u16 graphic                        = ReadBE<u16>();
    u8 graphicIncrement                = ReadBE<u8>();
    u16 count                          = ReadBE<u16>();
    u16 unknown                        = ReadBE<u16>();
    u16 x                              = ReadBE<u16>();
    u16 y                              = ReadBE<u16>();
    u8 z                               = ReadBE<u8>();
    u8 direction                       = ReadBE<u8>();
    u16 color                          = ReadBE<u16>();
    u8 flags                           = ReadBE<u8>();
    u16 unknown2                       = ReadBE<u16>();

    if (serial != g_PlayerSerial)
    {
        g_World->UpdateGameObject(
            serial,
            graphic,
            graphicIncrement,
            count,
            x,
            y,
            z,
            direction,
            color,
            flags,
            unknown,
            updateType,
            unknown2);
    }
    else if (*GetBuffer() == 0xF7)
    { //из пакета 0xF7 для игрока определенная обработка
        g_World->UpdatePlayer(
            serial, graphic, graphicIncrement, color, flags, x, y, 0 /*serverID*/, direction, z);
    }
}

PACKET_HANDLER(UpdateObject)
{
    if (g_World == nullptr)
    {
        return;
    }

    u32 serial   = ReadBE<u32>();
    u16 graphic  = ReadBE<u16>();
    u16 x        = ReadBE<u16>();
    u16 y        = ReadBE<u16>();
    u8 z         = ReadBE<u8>();
    u8 direction = ReadBE<u8>();
    u16 color    = ReadBE<u16>();
    u8 flags     = ReadBE<u8>();
    u8 notoriety = ReadBE<u8>();
    bool oldDead = false;

    bool isAlreadyExists = (g_World->FindWorldObject(serial) != nullptr);

    if (serial == g_PlayerSerial)
    {
        if (g_Player != nullptr)
        {
            bool updateStatusbar = (g_Player->GetFlags() != flags);

            oldDead           = g_Player->Dead();
            g_Player->Graphic = graphic;
            g_Player->OnGraphicChange(1000);
            g_Player->Color = g_ColorManager.FixColor(color);
            g_Player->SetFlags(flags);

            if (updateStatusbar)
            {
                g_GumpManager.UpdateContent(serial, 0, GT_STATUSBAR);
            }
        }
    }
    else
    {
        g_World->UpdateGameObject(
            serial, graphic, 0, 0, x, y, z, direction, color, flags, 0, UGOT_ITEM, 1);
    }

    CGameObject* obj = g_World->FindWorldObject(serial);

    if (obj == nullptr)
    {
        return;
    }

    obj->ClearNotOpenedItems();

    if (obj->NPC)
    {
        ((CGameCharacter*)obj)->Notoriety = notoriety;
        g_GumpManager.UpdateContent(serial, 0, GT_PAPERDOLL);
    }

    if (*GetBuffer() != 0x78)
    {
        Move(6);
    }

    u32 itemSerial = ReadBE<u32>();

    while (itemSerial != 0 && !IsEOF())
    {
        u16 itemGraphic = ReadBE<u16>();
        u8 layer        = ReadBE<u8>();
        u16 itemColor   = 0;

        if (GameVars::GetClientVersion() >= CV_70331)
        {
            itemColor = ReadBE<u16>();
        }
        else if ((itemGraphic & 0x8000) != 0)
        {
            itemGraphic &= 0x7FFF;
            itemColor = ReadBE<u16>();
        }

        CGameItem* item = g_World->GetWorldItem(itemSerial);

        item->MapIndex = g_CurrentMap;

        item->Graphic = itemGraphic;
        item->Color   = g_ColorManager.FixColor(itemColor);

        g_World->PutEquipment(item, obj, layer);
        item->OnGraphicChange();

        LOG_INFO(
            "PacketManager",
            "\t0x%08X:%04X [%d] %04X",
            item->Serial,
            item->Graphic,
            layer,
            item->Color);

        g_World->MoveToTop(item);

        itemSerial = ReadBE<u32>();
    }

    if (obj->IsPlayer())
    {
        if (oldDead != g_Player->Dead())
        {
            if (g_Player->Dead())
            {
                g_Orion.ChangeSeason(ST_DESOLATION, DEATH_MUSIC_INDEX);
            }
            else
            {
                g_Orion.ChangeSeason(g_OldSeason, g_OldSeasonMusic);
            }
        }

        g_GumpManager.UpdateContent(serial, 0, GT_PAPERDOLL);

        g_Player->UpdateAbilities();
    }
}

PACKET_HANDLER(EquipItem)
{
    if (g_World == nullptr)
    {
        return;
    }

    u32 serial = ReadBE<u32>();

    CGameItem* obj = g_World->GetWorldItem(serial);
    obj->MapIndex  = g_CurrentMap;

    if ((obj->Graphic != 0u) && obj->Layer != OL_BACKPACK)
    {
        obj->Clear();
    }

    obj->Graphic = ReadBE<u16>();
    Move(1);
    int layer   = ReadBE<u8>();
    u32 cserial = ReadBE<u32>();
    obj->Color  = g_ColorManager.FixColor(ReadBE<u16>());

    if (obj->Container != 0xFFFFFFFF)
    {
        g_GumpManager.UpdateContent(obj->Container, 0, GT_CONTAINER);
        g_GumpManager.UpdateContent(obj->Container, 0, GT_PAPERDOLL);
    }

    g_World->PutEquipment(obj, cserial, layer);
    obj->OnGraphicChange();

    if (g_NewTargetSystem.Serial == serial)
    {
        g_NewTargetSystem.Serial = 0;
    }

    if (layer >= OL_BUY_RESTOCK && layer <= OL_SELL)
    {
        obj->Clear();
    }
    else if (layer < OL_MOUNT)
    {
        g_GumpManager.UpdateContent(cserial, 0, GT_PAPERDOLL);
    }

    if (cserial == g_PlayerSerial && (layer == OL_1_HAND || layer == OL_2_HAND))
    {
        g_Player->UpdateAbilities();
    }
}

PACKET_HANDLER(UpdateContainedItem)
{
    if (g_World == nullptr)
    {
        return;
    }

    u32 serial          = ReadBE<u32>();
    u16 graphic         = ReadBE<u16>();
    u8 graphicIncrement = ReadBE<u8>();
    u16 count           = ReadBE<u16>();
    u16 x               = ReadBE<u16>();
    u16 y               = ReadBE<u16>();

    if (GameVars::GetClientVersion() >= CV_6017)
    {
        Move(1);
    }

    u32 containerSerial = ReadBE<u32>();
    u16 color           = ReadBE<u16>();

    g_World->UpdateContainedItem(
        serial, graphic, graphicIncrement, count, x, y, containerSerial, color);
}

PACKET_HANDLER(UpdateContainedItems)
{
    if (g_World == nullptr)
    {
        return;
    }

    u16 itemsCount = ReadBE<u16>();

    for (int i = 0; i < itemsCount; i++)
    {
        u32 serial          = ReadBE<u32>();
        u16 graphic         = ReadBE<u16>();
        u8 graphicIncrement = ReadBE<u8>();
        u16 count           = ReadBE<u16>();
        u16 x               = ReadBE<u16>();
        u16 y               = ReadBE<u16>();

        if (GameVars::GetClientVersion() >= CV_6017)
        {
            Move(1);
        }

        u32 containerSerial = ReadBE<u32>();
        u16 color           = ReadBE<u16>();

        if (i == 0)
        {
            CGameObject* container = g_World->FindWorldObject(containerSerial);

            if (container != nullptr)
            {
                LOG_INFO("PacketManager", "Making %08X empty...", containerSerial);

                if (container->IsCorpse())
                {
                    container->ClearUnequipped();
                }
                else
                {
                    container->Clear();
                }

                /*if ((*(int(__thiscall **)(CGameContainer *))((int(__thiscall **)(_DWORD))containerObj1->GameObject.VTable
					+ 12))(containerObj1))
				{
					if (containerObj1->GameObject.Parent)
					{
						if ((*((int(**)(void))containerObj1->GameObject.Parent->VTable + 9))())
						{
							v3 = containerObj1->GameObject.Parent;
							if (v3->field_B8)
							{
								if ((*(int(**)(void))(*(_DWORD *)v3->field_B8 + 380))())
								{
									v4 = containerObj1->GameObject.Parent->field_B8;
									if (v4)
										(**(void(__stdcall ***)(_DWORD))v4)(1);
								}
							}
						}
					}
					sub_5A6FE0(containerObj1);
				}*/
            }
        }

        g_World->UpdateContainedItem(
            serial, graphic, graphicIncrement, count, x, y, containerSerial, color);
    }
}

PACKET_HANDLER(DenyMoveItem)
{
    if (g_World == nullptr)
    {
        return;
    }

    if (g_ObjectInHand.Enabled ||
        (g_ObjectInHand.Dropped && g_World->FindWorldItem(g_ObjectInHand.Serial) == nullptr))
    {
        if (g_World->ObjectToRemove == g_ObjectInHand.Serial)
        {
            g_World->ObjectToRemove = 0;
        }

        if (!g_ObjectInHand.UpdatedInWorld)
        {
            if ((g_ObjectInHand.Layer == 0u) && (g_ObjectInHand.Container != 0u) &&
                g_ObjectInHand.Container != 0xFFFFFFFF)
            {
                g_World->UpdateContainedItem(
                    g_ObjectInHand.Serial,
                    g_ObjectInHand.Graphic,
                    0,
                    g_ObjectInHand.TotalCount,
                    g_ObjectInHand.X,
                    g_ObjectInHand.Y,
                    g_ObjectInHand.Container,
                    g_ObjectInHand.Color);

                g_GumpManager.UpdateContent(g_ObjectInHand.Container, 0, GT_CONTAINER);
            }
            else
            {
                CGameItem* obj = g_World->GetWorldItem(g_ObjectInHand.Serial);

                if (obj != nullptr)
                {
                    obj->Graphic = g_ObjectInHand.Graphic;
                    obj->Color   = g_ObjectInHand.Color;
                    obj->Count   = g_ObjectInHand.TotalCount;
                    obj->SetFlags(g_ObjectInHand.Flags);
                    obj->SetX(g_ObjectInHand.X);
                    obj->SetY(g_ObjectInHand.Y);
                    obj->SetZ(g_ObjectInHand.Z);
                    obj->OnGraphicChange();

                    CGameObject* container = g_World->FindWorldObject(g_ObjectInHand.Container);

                    if (container != nullptr)
                    {
                        if (container->NPC)
                        {
                            g_World->PutEquipment(obj, container, g_ObjectInHand.Layer);

                            g_GumpManager.UpdateContent(obj->Container, 0, GT_PAPERDOLL);
                        }
                        else
                        {
                            g_World->RemoveObject(obj);
                            obj = nullptr;
                        }
                    }
                    else
                    {
                        g_World->RemoveFromContainer(obj);
                    }

                    if (obj != nullptr)
                    {
                        g_World->MoveToTop(obj);
                    }
                }
            }
        }

        g_GumpManager.CloseGump(g_ObjectInHand.Serial, 0, GT_DRAG);

        g_ObjectInHand.Clear();
    }

    u8 code = ReadBE<u8>();

    if (code < 5)
    {
        const std::string errorMessages[] = {
            "You can not pick that up.",
            "That is too far away.",
            "That is out of sight.",
            "That item does not belong to you.  You'll have to steal it.",
            "You are already holding an item."
        };

        g_Orion.CreateTextMessage(TT_SYSTEM, 0xFFFFFFFF, 3, 0, errorMessages[code]);
    }
}

PACKET_HANDLER(EndDraggingItem)
{
    if (g_World == nullptr)
    {
        return;
    }

    //Unused
    //Move(2);
    //Move(2);

    g_ObjectInHand.Enabled = false;
    g_ObjectInHand.Dropped = false;
}

PACKET_HANDLER(DropItemAccepted)
{
    if (g_World == nullptr)
    {
        return;
    }

    g_ObjectInHand.Enabled = false;
    g_ObjectInHand.Dropped = false;
}

PACKET_HANDLER(DeleteObject)
{
    if (g_World == nullptr)
    {
        return;
    }

    u32 serial = ReadBE<u32>();

    if (serial == g_PlayerSerial)
    {
        return;
    }

    CGameObject* obj = g_World->FindWorldObject(serial);

    if (obj != nullptr)
    {
        bool updateAbilities = false;
        u32 cont             = obj->Container & 0x7FFFFFFF;

        if (obj->Container != 0xFFFFFFFF)
        {
            CGameObject* top = obj->GetTopObject();

            if (top != nullptr)
            {
                if (top->IsPlayer())
                {
                    CGameItem* item = (CGameItem*)obj;
                    updateAbilities = (item->Layer == OL_1_HAND || item->Layer == OL_2_HAND);
                }

                CGameObject* tradeBox = top->FindSecureTradeBox();

                if (tradeBox != nullptr)
                {
                    g_GumpManager.UpdateContent(0, tradeBox->Serial, GT_TRADE);
                }
            }

            if (cont == g_PlayerSerial && ((CGameItem*)obj)->Layer == OL_NONE)
            {
                g_ObjectInHand.Enabled = false;
            }

            if (!obj->NPC && ((CGameItem*)obj)->Layer != OL_NONE)
            {
                g_GumpManager.UpdateContent(cont, 0, GT_PAPERDOLL);
            }

            CGump* gump = g_GumpManager.UpdateContent(cont, 0, GT_CONTAINER);

            if (obj->Graphic == 0x0EB0)
            {
                g_GumpManager.CloseGump(serial, cont, GT_BULLETIN_BOARD_ITEM);

                CGumpBulletinBoard* bbGump =
                    (CGumpBulletinBoard*)g_GumpManager.UpdateGump(cont, 0, GT_BULLETIN_BOARD);

                if (bbGump != nullptr && bbGump->m_HTMLGump != nullptr)
                {
                    QFOR(go, bbGump->m_HTMLGump->m_Items, CBaseGUI*)
                    {
                        if (go->Serial == serial)
                        {
                            bbGump->m_HTMLGump->Delete(go);

                            int posY = 0;

                            QFOR(go1, bbGump->m_HTMLGump->m_Items, CBaseGUI*)
                            {
                                if (go1->Type == GOT_BB_OBJECT)
                                {
                                    go1->SetY(posY);
                                    posY += 18;
                                }
                            }

                            bbGump->m_HTMLGump->CalculateDataSize();

                            break;
                        }
                    }
                }
            }
        }

        if (!g_CorpseManager.InList(0, serial))
        {
            if (obj->NPC)
            {
                if (g_Party.Contains(obj->Serial))
                {
                    g_GumpManager.UpdateContent(obj->Serial, 0, GT_STATUSBAR);

                    obj->RemoveRender();
                }
                else
                {
                    g_World->RemoveObject(obj);
                }
            }
            else
            {
                g_World->RemoveObject(obj);

                if (updateAbilities)
                {
                    g_Player->UpdateAbilities();
                }
            }
        }
    }
}

PACKET_HANDLER(UpdateCharacter)
{
    if (g_World == nullptr)
    {
        return;
    }

    u32 serial          = ReadBE<u32>();
    CGameCharacter* obj = g_World->FindWorldCharacter(serial);

    if (obj == nullptr)
    {
        return;
    }

    u16 graphic  = ReadBE<u16>();
    u16 x        = ReadBE<u16>();
    u16 y        = ReadBE<u16>();
    char z       = ReadBE<u8>();
    u8 direction = ReadBE<u8>();
    u16 color    = ReadBE<u16>();
    u8 flags     = ReadBE<u8>();
    u8 notoriety = ReadBE<u8>();

    // Bug #78
    // Outlands server somewhat sends back what seems to be invalid direction data
    // Looking at RunUO source, mobile directions flag is valid to be at most 0x87
    // https://github.com/runuo/runuo/blob/d715573172fc432a673825b0136444bdab7863b5/Server/Mobile.cs#L390-L405
    // But in Outlands when a Mobile has low HP and start running away, if the player
    // forces it to change direction by circling it, eventually a bad packet with a
    // direction of 0x08 will come in.
    if ((direction & 0x87) != direction)
    {
        LOG_WARNING("PacketManager", "Clamping invalid/unknown direction: %d", direction);
        direction &= 0x87;
    }

    obj->Notoriety = notoriety;

    if (obj->IsPlayer())
    {
        obj->SetFlags(flags);
    }
    else
    {
        if (!obj->m_Steps.empty() && obj->Direction == obj->m_Steps.back().Direction)
        {
            CWalkData& wd = obj->m_Steps.back();

            obj->SetX(wd.X);
            obj->SetY(wd.Y);
            obj->SetZ(wd.Z);
            obj->Direction = wd.Direction;

            obj->m_Steps.clear();
        }

        g_World->UpdateGameObject(
            serial, graphic, 0, 0, x, y, z, direction, color, flags, 0, UGOT_ITEM, 1);
    }

    g_World->MoveToTop(obj);
}

PACKET_HANDLER(Warmode)
{
    if (g_World == nullptr)
    {
        return;
    }

    g_Player->Warmode = (ReadBE<u8>() != 0u);

    g_GumpManager.UpdateContent(g_PlayerSerial, 0, GT_STATUSBAR);

    CGumpPaperdoll* gump = (CGumpPaperdoll*)g_GumpManager.GetGump(g_PlayerSerial, 0, GT_PAPERDOLL);

    if (gump != nullptr && gump->m_ButtonWarmode != nullptr)
    {
        u16 graphic = 0x07E5;

        if (g_Player->Warmode)
        {
            graphic += 3;
        }

        gump->m_ButtonWarmode->Graphic         = graphic;
        gump->m_ButtonWarmode->GraphicSelected = graphic + 2;
        gump->m_ButtonWarmode->GraphicPressed  = graphic + 1;

        gump->WantRedraw = true;
    }

    g_World->MoveToTop(g_Player);
}

PACKET_HANDLER(PauseControl)
{
    /*g_ClientPaused = ReadBE<u8>();

	if (!g_ClientPaused)
	UO->ResumeClient();*/
}

PACKET_HANDLER(OpenPaperdoll)
{
    if (g_World == nullptr)
    {
        return;
    }

    u32 serial = ReadBE<u32>();

    CGameCharacter* obj = g_World->FindWorldCharacter(serial);

    std::string text = ReadString(60);

    u8 flags = ReadBE<u8>();

    if (obj != nullptr)
    {
        obj->Title = text;
    }

    CGumpPaperdoll* gump = (CGumpPaperdoll*)g_GumpManager.UpdateGump(serial, 0, GT_PAPERDOLL);

    if (gump == nullptr)
    {
        gump = new CGumpPaperdoll(serial, 0, 0, false);
        g_GumpManager.AddGump(gump);
    }
    else 
    {
        gump->SetX(10);
        gump->SetY(10);
    }

    gump->CanLift = ((flags & 0x02) != 0);
    gump->UpdateDescription(text);
}

PACKET_HANDLER(ClientVersion)
{
    CPacketClientVersion(uo_client_version.GetValue()).Send();
}

PACKET_HANDLER(Ping)
{
    g_Ping = 0;
}

PACKET_HANDLER(SetWeather)
{
    g_Weather.Reset();

    u8 type         = ReadBE<u8>();
    g_Weather.Type  = type;
    g_Weather.Count = ReadBE<u8>();

    bool showMessage = (g_Weather.Count > 0);

    if (g_Weather.Count > 70)
    {
        g_Weather.Count = 70;
    }

    g_Weather.Temperature = ReadBE<u8>();
    g_Weather.Timer       = g_Ticks + WEATHER_TIMER;
    g_Weather.Generate();

    switch (type)
    {
        case 0:
        {
            if (showMessage)
            {
                g_Orion.CreateTextMessage(TT_SYSTEM, 0xFFFFFFFF, 3, 0, "It begins to rain.");
            }

            break;
        }
        case 1:
        {
            if (showMessage)
            {
                g_Orion.CreateTextMessage(
                    TT_SYSTEM, 0xFFFFFFFF, 3, 0, "A fierce storm approaches.");
            }

            break;
        }
        case 2:
        {
            if (showMessage)
            {
                g_Orion.CreateTextMessage(TT_SYSTEM, 0xFFFFFFFF, 3, 0, "It begins to snow.");
            }

            break;
        }
        case 3:
        {
            if (showMessage)
            {
                g_Orion.CreateTextMessage(TT_SYSTEM, 0xFFFFFFFF, 3, 0, "A storm is brewing.");
            }

            break;
        }
        case 0xFE:
        case 0xFF:
        {
            g_Weather.Timer = 0;
            break;
        }
        default: break;
    }
}

PACKET_HANDLER(PersonalLightLevel)
{
    u32 serial = ReadBE<u32>();

    if (serial == g_PlayerSerial)
    {
        u8 level = ReadBE<u8>();

        if (level > 0x1F)
        {
            level = 0x1F;
        }

        g_PersonalLightLevel = level;
    }
}

PACKET_HANDLER(LightLevel)
{
    u8 level = ReadBE<u8>();

    if (level > 0x1F)
    {
        level = 0x1F;
    }

    g_LightLevel = level;
}

PACKET_HANDLER(EnableLockedFeatures)
{
    if (GameVars::GetClientVersion() >= CV_60142)
    {
        g_LockedClientFeatures = ReadBE<u32>();
    }
    else
    {
        g_LockedClientFeatures = ReadBE<u16>();
    }

    g_ChatEnabled = (bool)(g_LockedClientFeatures & LFF_T2A);

    g_AnimationManager.UpdateAnimationAddressTable();
}

PACKET_HANDLER(OpenContainer)
{
    if (g_World == nullptr)
    {
        return;
    }

    u32 serial = ReadBE<u32>();
    u16 gumpid = ReadBE<u16>();

    CGump* gump = nullptr;

    if (gumpid == 0xFFFF) //Spellbook
    {
        int gameWindowCenterX =
            (g_ConfigManager.GameWindowX - 4) + g_ConfigManager.GameWindowWidth / 2;
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

        gump = new CGumpSpellbook(serial, x, y);
        g_Orion.PlaySoundEffect(0x0055);
    }
    else if (gumpid == 0x0030) //Buylist
    {
        g_GumpManager.CloseGump(serial, 0, GT_SHOP);

        CGameCharacter* vendor = g_World->FindWorldCharacter(serial);

        if (vendor != nullptr)
        {
            CGumpShop* buyGump = new CGumpShop(serial, true, 150, 5);
            gump               = buyGump;
            buyGump->Visible   = true;

            for (int layer = OL_BUY_RESTOCK; layer < OL_BUY + 1; layer++)
            {
                CGameItem* item = vendor->FindLayer((int)layer);

                if (item == nullptr)
                {
                    LOG_WARNING("PacketManager", "Buy layer %i not found!", layer);
                    continue;
                }

                item = (CGameItem*)item->m_Items;

                if (item == nullptr)
                {
                    LOG_WARNING("PacketManager", "Buy items not found!");
                    continue;
                }

                bool reverse = (item->GetX() > 1);

                if (reverse)
                {
                    while (item != nullptr && item->m_Next != nullptr)
                    {
                        item = (CGameItem*)item->m_Next;
                    }
                }

                CGUIHTMLGump* htmlGump = buyGump->m_ItemList[0];

                int currentY = 0;

                QFOR(shopItem, htmlGump->m_Items, CBaseGUI*)
                {
                    if (shopItem->Type == GOT_SHOPITEM)
                    {
                        currentY += shopItem->GetSize().y;
                    }
                }

                while (item != nullptr)
                {
                    CGUIShopItem* shopItem   = (CGUIShopItem*)htmlGump->Add(new CGUIShopItem(
                        item->Serial,
                        item->Graphic,
                        item->Color,
                        item->Count,
                        item->Price,
                        item->GetName(),
                        0,
                        currentY));
                    shopItem->NameFromCliloc = item->NameFromCliloc;

                    if (currentY == 0)
                    {
                        shopItem->Selected = true;
                        shopItem->CreateNameText();
                        shopItem->UpdateOffsets();
                    }

                    currentY += shopItem->GetSize().y;

                    if (reverse)
                    {
                        item = (CGameItem*)item->m_Prev;
                    }
                    else
                    {
                        item = (CGameItem*)item->m_Next;
                    }
                }

                htmlGump->CalculateDataSize();
            }
        }
        else
        {
            LOG_WARNING("PacketManager", "Buy vendor not found!");
        }
    }
    else //Container
    {
        u16 graphic = 0xFFFF;

        for (int i = 0; i < (int)g_ContainerOffset.size(); i++)
        {
            if (gumpid == g_ContainerOffset[i].Gump)
            {
                graphic = (u16)i;
                break;
            }
        }

        if (graphic == 0xFFFF)
        {
            return;
        }

        g_ContainerRect.Calculate(gumpid);

        gump          = new CGumpContainer(serial, gumpid, g_ContainerRect.X, g_ContainerRect.Y);
        gump->Graphic = graphic;
        g_Orion.ExecuteGump(gumpid);
    }

    if (gump == nullptr)
    {
        return;
    }

    gump->ID = gumpid;

    if (gumpid != 0x0030)
    {
        if (static_cast<unsigned int>(!g_ContainerStack.empty()) != 0u)
        {
            for (std::deque<CContainerStackItem>::iterator cont = g_ContainerStack.begin();
                 cont != g_ContainerStack.end();
                 ++cont)
            {
                if (cont->Serial == serial)
                {
                    gump->SetX(cont->X);
                    gump->SetY(cont->Y);
                    gump->Minimized  = cont->Minimized;
                    gump->MinimizedX = cont->MinimizedX;
                    gump->MinimizedY = cont->MinimizedY;
                    gump->LockMoving = cont->LockMoving;

                    if (gumpid != 0xFFFF)
                    {
                        if (cont->Minimized)
                        {
                            gump->Page = 1;
                        }
                        else
                        {
                            gump->Page = 2;
                        }
                    }

                    g_ContainerStack.erase(cont);

                    break;
                }
            }

            if (g_CheckContainerStackTimer < g_Ticks)
            {
                g_ContainerStack.clear();
            }
        }

        CGameItem* obj = g_World->FindWorldItem(serial);

        if (obj != nullptr)
        {
            /*if (gumpid != 0xFFFF)*/ obj->Opened = true;
            if (!obj->IsCorpse() && gumpid != 0xFFFF)
            {
                g_World->ClearContainer(obj);
            }

            if (gumpid == 0xFFFF)
            {
                ((CGumpSpellbook*)gump)->UpdateGraphic(obj->Graphic);
            }
        }
    }

    g_GumpManager.AddGump(gump);
}

PACKET_HANDLER(UpdateSkills)
{
    if (g_World == nullptr)
    {
        return;
    }

    u8 type             = ReadBE<u8>();
    bool haveCap        = (((type != 0u) && type <= 0x03) || type == 0xDF);
    bool isSingleUpdate = (type == 0xFF || type == 0xDF);
    LOG_INFO("PacketManager", "Skill update type %i (cap=%d)", type, haveCap);

    if (type == 0xFE)
    {
        int count = ReadBE<i16>();
        g_SkillsManager.Clear();

        for (int i = 0; i < count; i++)
        {
            bool haveButton = (ReadBE<u8>() != 0);
            int nameLength  = ReadBE<u8>();
            g_SkillsManager.Add(CSkill(haveButton, ReadString(nameLength)));
        }

        g_SkillsManager.Sort();

        return;
    }

    CGumpSkills* gump = (CGumpSkills*)g_GumpManager.UpdateGump(0, 0, GT_SKILLS);

    if (!isSingleUpdate && (type == 1 || type == 3 || g_SkillsManager.SkillsRequested))
    {
        g_SkillsManager.SkillsRequested = false;

        if (gump == nullptr)
        {
            gump = new CGumpSkills(0, 0, false, 250);
            g_GumpManager.AddGump(gump);
        }

        gump->Visible = true;
    }

    while (!IsEOF())
    {
        u16 id = ReadBE<u16>();

        if (IsEOF())
        {
            break;
        }

        if ((id == 0u) && (type == 0u))
        {
            break;
        }
        if ((type == 0u) || type == 0x02)
        {
            id--;
        }

        u16 baseVal = ReadBE<u16>();
        u16 realVal = ReadBE<u16>();
        u8 lock     = ReadBE<u8>();
        u16 cap     = 1000;

        if (haveCap)
        {
            cap = ReadBE<u16>();
        }

        CSkill* skill = g_SkillsManager.Get(id);

        if (id < g_SkillsManager.Count && skill != nullptr)
        {
            if (isSingleUpdate)
            {
                float change = (float)(baseVal / 10.0f) - skill->BaseValue;

                if (change != 0.0f)
                {
                    char str[128] = { 0 };
                    sprintf_s(
                        str,
                        "Your skill in %s has %s by %.1f%%.  It is now %.1f%%.",
                        skill->Name.c_str(),
                        ((change < 0) ? "decreased" : "increased"),
                        std::abs(change),
                        skill->BaseValue + change);
                    //else if (change > 0) sprintf(str, "Your skill in %s has increased by %.1f%%.  It is now %.1f%%.", UO->m_Skills[id].m_Name.c_str(), change, obj->GetSkillBaseValue(id) + change);
                    g_Orion.CreateTextMessage(TT_SYSTEM, 0, 3, 0x58, str);
                }
            }

            skill->BaseValue = (float)(baseVal / 10.0f);
            skill->Value     = (float)(realVal / 10.0f);
            skill->Cap       = (float)(cap / 10.0f);
            skill->Status    = lock;

            if (gump != nullptr)
            {
                gump->UpdateSkillValue(id);
            }

            /*if (haveCap)
				LOG("Skill %i is %i|%i|%i\n", id, baseVal, realVal, cap);
			else
				LOG("Skill %i is %i|%i\n", id, baseVal, realVal);*/
        }
        //else
        //	LOG("Unknown skill update %d\n", id);

        if (isSingleUpdate)
        {
            break;
        }
    }

    g_SkillsManager.UpdateSkillsSum();

    if (gump != nullptr)
    {
        gump->UpdateSkillsSum();
    }

    LOG_INFO("PacketManager", "Skill(s) updated!");
}

PACKET_HANDLER(ExtendedCommand)
{
    u16 cmd = ReadBE<u16>();

    switch (cmd)
    {
        case 0: break;
        case 1: //Initialize Fast Walk Prevention
        {
            for (int i = 0; i < 6; i++)
            {
                g_Player->m_FastWalkStack.SetValue((int)i, ReadBE<u32>());
            }

            break;
        }
        case 2: //Add key to Fast Walk Stack
        {
            g_Player->m_FastWalkStack.AddValue(ReadBE<u32>());

            break;
        }
        case 4: //Close generic gump
        {
            u32 id     = ReadBE<u32>();
            u32 button = ReadBE<u32>();

            for (CGump* gump = (CGump*)g_GumpManager.m_Items; gump != nullptr;)
            {
                CGump* next = (CGump*)gump->m_Next;

                if (gump->GumpType == GT_GENERIC && gump->ID == id)
                {
                    ((CGumpGeneric*)gump)->SendGumpResponse(button);
                }

                gump = next;
            }

            break;
        }
        case 5: //Screen size
        {
            //g_GameWindowSizeX = unpack16(buf + 5);
            //g_GameWindowSizeY = unpack16(buf + 9);
            break;
        }
        case 6: //Party commands
        {
            g_Party.ParsePacketData(*this);

            break;
        }
        case 8: //Set cursor / map
        {
            g_Orion.ChangeMap(ReadBE<u8>());

            break;
        }
        case 0xC: //Close statusbar gump
        {
            u32 serial = ReadBE<u32>();
            g_GumpManager.CloseGump(serial, 0, GT_STATUSBAR);

            break;
        }
        case 0x10: //DisplayEquipmentInfo
        {
            u32 serial      = ReadBE<u32>();
            CGameItem* item = g_World->FindWorldItem(serial);
            if (item == nullptr)
            {
                return;
            }

            item->JournalPrefix = "";
            std::wstring str    = {};
            int clilocNum       = ReadBE<i32>();
            if (clilocNum != 0)
            {
                str = g_ClilocManager.GetCliloc(g_Language)->GetW(clilocNum, true);
                if (str.length() > 0)
                {
                    item->SetName(Core::ToString(str));
                }

                g_Orion.CreateUnicodeTextMessage(TT_OBJECT, serial, 0x03, 0x3B2, str);
            }

            str                = {};
            u16 crafterNameLen = 0;
            u32 next           = ReadBE<u32>();
            if (next == 0xFFFFFFFD)
            {
                crafterNameLen = ReadBE<u16>();
                if (crafterNameLen != 0u)
                {
                    str = L"Crafted by " + Core::DecodeUTF8(ReadString(crafterNameLen));
                }
            }

            if (crafterNameLen != 0)
            {
                next = ReadBE<u32>();
            }
            if (next == 0xFFFFFFFC)
            {
                str += L"[Unidentified";
            }

            // -4 потому что последние 4 байта в пакете 0xFFFFFFFF
            u8* end  = GetBuffer() + GetSize() - 4;
            u8 count = 0;
            while (GetPtr() < end)
            {
                if (count != 0 || next == 0xFFFFFFFD || next == 0xFFFFFFFC)
                {
                    next = ReadBE<i32>();
                }
                short charges            = ReadBE<i16>();
                std::wstring attrsString = g_ClilocManager.GetCliloc(g_Language)->GetW(next, true);
                if (charges == -1)
                {
                    if (count > 0)
                    {
                        str += L"/";
                        str += attrsString;
                    }
                    else
                    {
                        str += L" [";
                        str += attrsString;
                    }
                }
                else
                {
                    str += L"\n[";
                    str += attrsString;
                    str += L" : ";
                    str += std::to_wstring(charges);
                    str += L"]";
                    count += 20;
                }
                count++;
            }
            if ((count < 20 && count > 0) || (next == 0xFFFFFFFC && count == 0))
            {
                str += L"]";
            }

            if (str.length() != 0u)
            {
                g_Orion.CreateUnicodeTextMessage(TT_OBJECT, serial, 0x03, 0x3B2, str);
            }
            CPacketMegaClilocRequestOld(serial).Send();
            break;
        }
        case 0x14: //Display Popup/context menu (2D and KR)
        {
            CGumpPopupMenu::Parse(*this);

            break;
        }
        case 0x16: //Close User Interface Windows
        {
            //ID:
            //0x01: Paperdoll
            //0x02: Status
            //0x08: Character Profile
            //0x0C: Container
            u32 id     = ReadBE<u32>();
            u32 serial = ReadBE<u32>();

            switch (id)
            {
                case 1: //Paperdoll
                {
                    g_GumpManager.CloseGump(serial, 0, GT_PAPERDOLL);
                    break;
                }
                case 2: //Statusbar
                {
                    g_GumpManager.CloseGump(serial, 0, GT_STATUSBAR);
                    break;
                }
                case 8: //Character Profile
                {
                    g_GumpManager.CloseGump(serial, 0, GT_PROFILE);
                    break;
                }
                case 0xC: //Container
                {
                    g_GumpManager.CloseGump(serial, 0, GT_CONTAINER);
                    break;
                }
                default: break;
            }

            break;
        }
        case 0x18: //Enable map (diff) patches
        {
            g_MapManager.ApplyPatches(*this);

            break;
        }
        case 0x19: //Extended stats
        {
            u8 version = ReadBE<u8>();
            u32 serial = ReadBE<u32>();

            switch (version)
            {
                case 0:
                {
                    CGameCharacter* bonded = g_World->FindWorldCharacter(serial);
                    if (bonded == nullptr)
                    {
                        break;
                    }

                    bool dead = ReadBE<u8>() != 0u;
                    bonded->SetDead(dead);
                    break;
                }
                case 2:
                {
                    if (serial == g_PlayerSerial)
                    {
                        u8 updateGump     = ReadBE<u8>();
                        u8 state          = ReadBE<u8>();
                        g_DrawStatLockers = true;

                        g_Player->LockStr = (state >> 4) & 3;
                        g_Player->LockDex = (state >> 2) & 3;
                        g_Player->LockInt = state & 3;

                        CGump* statusbar = g_GumpManager.GetGump(g_PlayerSerial, 0, GT_STATUSBAR);

                        if (statusbar != nullptr && !statusbar->Minimized)
                        {
                            statusbar->WantUpdateContent = true;
                        }
                    }
                    break;
                }
                case 5:
                {
                    if (g_World == nullptr)
                    {
                        return;
                    }

                    CGameCharacter* character = g_World->FindWorldCharacter(serial);
                    if (character == nullptr)
                    {
                        break;
                    }

                    if (GetSize() == 19)
                    {
                        bool dead = ReadBE<u8>() != 0u;
                        character->SetDead(dead);
                    }
                    break;
                }
            }
            break;
        }
        case 0x1B: //New spellbook content
        {
            Move(2);
            u32 serial = ReadBE<u32>();

            CGameItem* spellbook = g_World->FindWorldItem(serial);

            if (spellbook == nullptr)
            {
                LOG_WARNING("PacketManager", "Where is a spellbook?!?");
                return;
            }

            g_World->ClearContainer(spellbook);

            u16 graphic             = ReadBE<u16>();
            SPELLBOOK_TYPE bookType = (SPELLBOOK_TYPE)ReadBE<u16>();

            for (int j = 0; j < 2; j++)
            {
                u32 spells = 0;

                for (int i = 0; i < 4; i++)
                {
                    spells |= (ReadBE<u8>() << (i * 8));
                }

                for (int i = 0; i < 32; i++)
                {
                    if ((spells & (1 << i)) != 0u)
                    {
                        CGameItem* spellItem = new CGameItem();
                        spellItem->Graphic   = 0x1F2E;
                        spellItem->Count     = ((int)j * 32) + (int)i + 1;

                        spellbook->AddItem(spellItem);
                    }
                }
            }

            break;
        }
        case 0x1D:
        {
            //house revision state, server sends this when player comes in range of a custom house
            u32 serial   = ReadBE<u32>();
            u32 revision = ReadBE<u32>();

            CCustomHouse* house = g_CustomHousesManager.Get(serial);
            LOG_INFO("PacketManager", "Seek house: 0x%08X 0x%08X", serial, revision);

            if (house != nullptr)
            {
                LOG_INFO(
                    "PacketManager", "House found: 0x%08X 0x%08X", house->Serial, house->Revision);
            }

            if (house == nullptr || house->Revision != revision)
            {
                CPacketCustomHouseDataReq(serial).Send();
            }
            else
            {
                house->Paste(g_World->FindWorldItem(serial));
            }

            break;
        }
        case 0x20:
        {
            u32 serial  = ReadBE<u32>();
            u8 type     = ReadBE<u8>();
            u16 graphic = ReadBE<u16>();
            u16 x       = ReadBE<u16>();
            u16 y       = ReadBE<u16>();
            u8 z        = ReadBE<u8>();

            switch (type)
            {
                case CHUT_UPDATE:
                {
                    break;
                }
                case CHUT_REMOVE:
                {
                    break;
                }
                case CHUT_UPDATE_MULTI_POS:
                {
                    break;
                }
                case CHUT_CONSTRUCT_BEGIN:
                {
                    if (g_GumpManager.GetGump(0, 0, GT_CUSTOM_HOUSE) != nullptr)
                    {
                        break;
                    }

                    CGumpCustomHouse* gump = new CGumpCustomHouse(serial, 50, 50);

                    g_GumpManager.AddGump(gump);

                    break;
                }
                case CHUT_CONSTRUCT_END:
                {
                    g_GumpManager.CloseGump(serial, 0, GT_CUSTOM_HOUSE);
                    break;
                }
                default: break;
            }

            break;
        }
        case 0x21:
        {
            for (int i = 0; i < 2; i++)
            {
                g_Ability[i] &= 0x7F;
                g_GumpManager.UpdateContent((int)i, 0, GT_ABILITY);
            }

            break;
        }
        case 0x22:
        {
            if (g_World == nullptr)
            {
                return;
            }

            Move(1);
            u32 serial                = ReadBE<u32>();
            CGameCharacter* character = g_World->FindWorldCharacter(serial);

            if (character != nullptr)
            {
                int damage = ReadBE<u8>();

                CTextData* text = new CTextData();
                text->Unicode   = false;
                text->Font      = 3;
                text->Serial    = serial;
                text->Color     = (serial == g_PlayerSerial ? 0x0034 : 0x0021);
                text->Type      = TT_OBJECT;
                text->Text      = std::to_string(damage);
                text->GenerateTexture(0);
                text->SetX(text->m_Texture.Width / 2);
                int height = text->m_Texture.Height;

                CTextData* head = (CTextData*)character->m_DamageTextControl.Last();

                if (head != nullptr)
                {
                    height += head->GetY();

                    if (height > 0)
                    {
                        if (height > 100)
                        {
                            height = 0;
                        }

                        text->SetY(height);
                    }
                }

                character->m_DamageTextControl.Add(text);
                text->Timer = g_Ticks + DAMAGE_TEXT_NORMAL_DELAY;
            }

            break;
        }
        case 0x26:
        {
            u8 val = ReadBE<u8>();

            if (val > CST_FAST_UNMOUNT_AND_CANT_RUN)
            {
                val = 0;
            }

            g_SpeedMode = (CHARACTER_SPEED_TYPE)val;

            break;
        }
        default: break;
    }
}

PACKET_HANDLER(DenyWalk)
{
    if (g_Player == nullptr)
    {
        return;
    }

    g_Ping = 0;

    u8 sequence  = ReadBE<u8>();
    u16 x        = ReadBE<u16>();
    u16 y        = ReadBE<u16>();
    u8 direction = ReadBE<u8>();
    char z       = ReadBE<u8>();

    g_Walker.DenyWalk(sequence, x, y, z);

    g_Player->Direction = direction;

    g_World->MoveToTop(g_Player);
}

PACKET_HANDLER(ConfirmWalk)
{
    if (g_Player == nullptr)
    {
        return;
    }

    u8 sequence = ReadBE<u8>();

    //player->SetDirection(newdir);

    u8 newnoto = ReadBE<u8>() & (~0x40);

    if ((newnoto == 0u) || newnoto >= 8)
    {
        newnoto = 0x01;
    }

    g_Player->Notoriety = newnoto;

    g_Walker.ConfirmWalk(sequence);

    g_World->MoveToTop(g_Player);
}

PACKET_HANDLER(OpenUrl)
{
    g_Orion.GoToWebLink(ReadString());
}

PACKET_HANDLER(Target)
{
    g_Target.SetData(*this);

    if (g_PartyHelperTimer > g_Ticks && (g_PartyHelperTarget != 0u))
    {
        g_Target.SendTargetObject(g_PartyHelperTarget);
        g_PartyHelperTimer  = 0;
        g_PartyHelperTarget = 0;
    }
}

PACKET_HANDLER(Talk)
{
    if (g_World == nullptr)
    {
        if (g_GameState == GS_GAME_CONNECT)
        {
            u32 serial       = ReadBE<u32>();
            u16 graphic      = ReadBE<u16>();
            SPEECH_TYPE type = (SPEECH_TYPE)ReadBE<u8>();
            u16 textColor    = ReadBE<u16>();
            u16 font         = ReadBE<u16>();

            std::string name(ReadString());
            if (GetSize() > 44)
            {
                SetPtr(GetBuffer() + 44);
                g_ConnectionScreen.SetConnectionFailed(true);
                g_ConnectionScreen.SetTextA(ReadString());
            }
        }

        return;
    }

    u32 serial       = ReadBE<u32>();
    u16 graphic      = ReadBE<u16>();
    SPEECH_TYPE type = (SPEECH_TYPE)ReadBE<u8>();
    u16 textColor    = ReadBE<u16>();
    u16 font         = ReadBE<u16>();
    std::string name(ReadString());

    if ((serial == 0u) && (graphic == 0u) && type == ST_NORMAL && font == 0xFFFF &&
        textColor == 0xFFFF && Core::ToLowerA(name) == "system")
    {
        u8 sbuffer[0x28] = { 0x03, 0x00, 0x28, 0x20, 0x00, 0x34, 0x00, 0x03, 0xdb, 0x13,
                             0x14, 0x3f, 0x45, 0x2c, 0x58, 0x0f, 0x5d, 0x44, 0x2e, 0x50,
                             0x11, 0xdf, 0x75, 0x5c, 0xe0, 0x3e, 0x71, 0x4f, 0x31, 0x34,
                             0x05, 0x4e, 0x18, 0x1e, 0x72, 0x0f, 0x59, 0xad, 0xf5, 0x00 };

        g_Orion.Send(sbuffer, 0x28);

        return;
    }

    if (font >= g_FontManager.FontCount)
    {
        return;
    }

    std::string str{};
    if (GetSize() > 44)
    {
        SetPtr(GetBuffer() + 44);
        str = ReadString();
    }

    LOG_INFO("PacketManager", "%s: %s", name.c_str(), str.c_str());

    CGameObject* obj = g_World->FindWorldObject(serial);

    if (type == ST_BROADCAST || /*type == ST_SYSTEM ||*/ serial == 0xFFFFFFFF || (serial == 0u) ||
        (Core::ToLowerA(name) == "system" && obj == nullptr))
    {
        g_Orion.CreateTextMessage(TT_SYSTEM, serial, (u8)font, textColor, str);
    }
    else
    {
        if (type == ST_EMOTE)
        {
            textColor = g_ConfigManager.EmoteColor;
            str       = "*" + str + "*";
        }

        if (obj != nullptr)
        {
            //reset
            obj->JournalPrefix = "";
            if (obj->GetName().length() == 0u)
            {
                obj->SetName(name);

                if (obj->NPC)
                {
                    g_GumpManager.UpdateContent(serial, 0, GT_STATUSBAR);
                }
            }

            if (type == ST_SYSTEM)
            {
                obj->JournalPrefix = "You see: ";
            }
            else if (obj->GetName().length() != 0u)
            {
                obj->JournalPrefix = obj->GetName() + ": ";
            }
        }

        g_Orion.CreateTextMessage(TT_OBJECT, serial, (u8)font, textColor, str);
    }
}

PACKET_HANDLER(UnicodeTalk)
{
    if (g_World == nullptr)
    {
        if (g_GameState == GS_GAME_CONNECT)
        {
            u32 serial       = ReadBE<u32>();
            u16 graphic      = ReadBE<u16>();
            SPEECH_TYPE type = (SPEECH_TYPE)ReadBE<u8>();
            u16 textColor    = ReadBE<u16>();
            u16 font         = ReadBE<u16>();
            u32 language     = ReadBE<u32>();

            std::string name(ReadString());

            if (GetSize() > 48)
            {
                SetPtr(GetBuffer() + 48);
                g_ConnectionScreen.SetConnectionFailed(true);
                g_ConnectionScreen.SetTextA(Core::ToString(ReadWStringBE((GetSize() - 48) / 2)));
            }
        }

        return;
    }

    u32 serial       = ReadBE<u32>();
    u16 graphic      = ReadBE<u16>();
    SPEECH_TYPE type = (SPEECH_TYPE)ReadBE<u8>();
    u16 textColor    = ReadBE<u16>();
    u16 font         = ReadBE<u16>();
    u32 language     = ReadBE<u32>();
    std::string name(ReadString());

    if ((serial == 0u) && (graphic == 0u) && type == ST_NORMAL && font == 0xFFFF &&
        textColor == 0xFFFF && Core::ToLowerA(name) == "system")
    {
        u8 sbuffer[0x28] = { 0x03, 0x00, 0x28, 0x20, 0x00, 0x34, 0x00, 0x03, 0xdb, 0x13,
                             0x14, 0x3f, 0x45, 0x2c, 0x58, 0x0f, 0x5d, 0x44, 0x2e, 0x50,
                             0x11, 0xdf, 0x75, 0x5c, 0xe0, 0x3e, 0x71, 0x4f, 0x31, 0x34,
                             0x05, 0x4e, 0x18, 0x1e, 0x72, 0x0f, 0x59, 0xad, 0xf5, 0x00 };

        g_Orion.Send(sbuffer, 0x28);

        return;
    }

    std::wstring str = {};
    if (GetSize() > 48)
    {
        SetPtr(GetBuffer() + 48);
        str = ReadWStringBE((GetSize() - 48) / 2);
    }

    LOG_INFO("PacketManager", "%s: %s", name.c_str(), Core::ToString(str).c_str());

    CGameObject* obj = g_World->FindWorldObject(serial);
    if (type == ST_GUILD_CHAT)
    {
        type      = ST_BROADCAST;
        textColor = g_ConfigManager.GuildMessageColor;
        str       = L"[Guild][" + Core::ToWString(name) + L"]: " + str;
    }
    else if (type == ST_ALLIANCE_CHAT)
    {
        type      = ST_BROADCAST;
        textColor = g_ConfigManager.AllianceMessageColor;
        str       = L"[Alliance][" + Core::ToWString(name) + L"]: " + str;
    }

    if (type == ST_BROADCAST /*|| type == ST_SYSTEM*/ || serial == 0xFFFFFFFF || (serial == 0u) ||
        (Core::ToLowerA(name) == "system" && obj == nullptr))
    {
        g_Orion.CreateUnicodeTextMessage(
            TT_SYSTEM, serial, (u8)g_ConfigManager.SpeechFont, textColor, str);
    }
    else
    {
        if (type == ST_EMOTE && (textColor == 0u))
        {
            textColor = g_ConfigManager.EmoteColor;
        }

        if (obj != nullptr)
        {
            //reset
            obj->JournalPrefix = "";

            if (obj->GetName().length() == 0u)
            {
                obj->SetName(name);

                if (obj->NPC)
                {
                    g_GumpManager.UpdateContent(serial, 0, GT_STATUSBAR);
                }
            }

            if (type == ST_SYSTEM)
            {
                obj->JournalPrefix = "You see: ";
            }
            else if (obj->GetName().length() != 0u)
            {
                // Use the name that was sent by the Siebenwind Server.
                obj->JournalPrefix = name + ": ";
            }
        }

        g_Orion.CreateUnicodeTextMessage(
            TT_OBJECT, serial, (u8)g_ConfigManager.SpeechFont, textColor, str);
    }
}

PACKET_HANDLER(ClientTalk)
{
    if (!g_AbyssPacket03First)
    {
        bool parse = true;

        if (m_buffer[4] == 0x78)
        {
            m_size = ReadBE<i16>(2);
            m_buffer += 4;
        }
        else if (m_buffer[4] == 0x3C)
        {
            m_size = ReadBE<i16>(2);
            m_buffer += 4;
        }
        else if (m_buffer[4] == 0x25)
        {
            m_size = 0x14;
            m_buffer += 4;
        }
        else if (m_buffer[4] == 0x2E)
        {
            m_size = 0x0F;
            m_buffer += 4;
        }
        else
        {
            parse = false;
        }

        if (parse)
        {
            OnPacket();
        }
    }

    g_AbyssPacket03First = false;
}

PACKET_HANDLER(MultiPlacement)
{
    if (g_World == nullptr)
    {
        return;
    }

    //u32 serial = unpack32(buf + 2);
    //u16 graphic = unpack16(buf + 18);

    g_Target.SetMultiData(*this);
}

PACKET_HANDLER(GraphicEffect)
{
    if (g_World == nullptr)
    {
        return;
    }

    u8 type = ReadBE<u8>();

    if (type > 3)
    {
        if (type == 4 && *GetBuffer() == 0x70)
        {
            Move(8);
            u16 val = ReadBE<i16>();

            if (val > SET_TO_BLACK_VERY_FAST)
            {
                val = SET_TO_BLACK_VERY_FAST;
            }

            g_ScreenEffectManager.Use(SEM_SUNSET, (SCREEN_EFFECT_TYPE)val, true);
        }

        return;
    }

    u32 sourceSerial = ReadBE<u32>();
    u32 destSerial   = ReadBE<u32>();
    u16 graphic      = ReadBE<u16>();
    short sourceX    = ReadBE<i16>();
    short sourceY    = ReadBE<i16>();
    char sourceZ     = ReadBE<i8>();
    short destX      = ReadBE<i16>();
    short destY      = ReadBE<i16>();
    char destZ       = ReadBE<i8>();
    u8 speed         = ReadBE<u8>();
    short duration   = (short)ReadBE<u8>() * 50;
    //what is in 24-25 bytes?
    Move(2);
    u8 fixedDirection = ReadBE<u8>();
    u8 explode        = ReadBE<u8>();

    u32 color      = 0;
    u32 renderMode = 0;

    if (*GetBuffer() != 0x70)
    {
        //0xC0
        color      = ReadBE<u32>();
        renderMode = ReadBE<u32>() % 7;

        if (*m_buffer == 0xC7)
        {
            /*
			0000: c7 03 00 13 82 2f 00 00 00 00 37 6a 05 d6 06 47 : ...../....7j...G
			0010: 15 05 d6 06 47 15 09 20 00 00 01 00 00 00 00 00 : ....G.. ........
			0020: 00 00 00 00 13 8d 00 01 00 00 00 13 82 2f 03 00 : ............./..
			0030: 00 -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- : .
			*/
        }
    }

    CGameEffect* effect = nullptr;
    if (type == 0u) //Moving
    {
        effect = new CGameEffectMoving();

        if (speed == 0u)
        {
            speed++;
        }

        ((CGameEffectMoving*)effect)->MoveDelay = 20 / speed;
    }
    else
    {
        effect = new CGameEffect();
    }

    effect->EffectType = (EFFECT_TYPE)type;
    effect->Serial     = sourceSerial;
    effect->DestSerial = destSerial;
    effect->Graphic    = graphic;

    CGameCharacter* sourceObject = g_World->FindWorldCharacter(sourceSerial);

    if (sourceObject != nullptr)
    {
        effect->SetX(sourceObject->GetX());
        effect->SetY(sourceObject->GetY());
        effect->SetZ(sourceObject->GetZ());
    }
    else
    {
        effect->SetX(sourceX);
        effect->SetY(sourceY);
        effect->SetZ(sourceZ);
    }

    CGameCharacter* destObject = g_World->FindWorldCharacter(destSerial);

    if (destObject != nullptr)
    {
        effect->DestX = destObject->GetX();
        effect->DestY = destObject->GetY();
        effect->DestZ = destObject->GetZ();
    }
    else
    {
        effect->DestX = destX;
        effect->DestY = destY;
        effect->DestZ = destZ;
    }

    uintptr_t addressAnimData = (uintptr_t)g_FileManager.m_AnimdataMul.GetBuffer();

    if (addressAnimData != 0u)
    {
        ANIM_DATA* pad = (ANIM_DATA*)(addressAnimData + ((graphic * 68) + 4 * ((graphic / 8) + 1)));

        effect->Speed = pad->FrameInterval * 45;
        //effect->Speed = (pad->FrameInterval - effect->Speed) * 45;
    }
    else
    {
        effect->Speed = speed + 6;
    }

    effect->SetDuration(Core::TimeDiff::FromMilliseconds(duration));
    effect->FixedDirection = (fixedDirection != 0);
    effect->Explode        = (explode != 0);

    effect->Color      = (u16)color;
    effect->RenderMode = renderMode;

    g_EffectManager.AddEffect(effect);
}

PACKET_HANDLER(DeathScreen)
{
    u8 action = ReadBE<u8>();

    if (action != 1)
    {
        g_GameScreen.SmoothScreenAction = 0;
        g_ScreenEffectManager.Use(SEM_SUNSET, SET_TO_BLACK, true);

        g_Weather.Reset();
        g_Target.Reset();

        g_GumpManager.CloseGump(0, 0, GT_DRAG);

        if (g_ConfigManager.GetMusic())
        {
            g_Orion.PlayMusic(42, true);
        }

        g_Orion.ChangeWarmode(0);

        g_DeathScreenTimer = g_Ticks + DEATH_SCREEN_DELAY;
    }
}

PACKET_HANDLER(PlaySoundEffect)
{
    Move(1);
    u16 index  = ReadBE<u16>();
    u16 volume = ReadBE<u16>();
    u16 xCoord = ReadBE<u16>(); // CHECK: unsigned for position?
    u16 yCoord = ReadBE<u16>();
    g_Orion.PlaySoundEffectAtPosition(index, xCoord, yCoord);
}

PACKET_HANDLER(PlayMusic)
{
    u16 index = ReadBE<u16>();

    //LOG("Play midi music 0x%04X\n", index);
    if (!g_ConfigManager.GetMusic() || !g_gameWindow.IsActive() ||
        g_ConfigManager.GetMusicVolume() < 1)
    {
        return;
    }

    g_Orion.PlayMusic(index);
}

PACKET_HANDLER(DragAnimation)
{
    if (g_World == nullptr)
    {
        return;
    }

    u16 graphic = ReadBE<u16>();
    graphic += ReadBE<u8>(); //graphic increment

    u16 color = ReadBE<u16>();
    u16 count = ReadBE<u16>();

    u32 sourceSerial = ReadBE<u32>();
    short sourceX    = ReadBE<i16>();
    short sourceY    = ReadBE<i16>();
    char sourceZ     = ReadBE<i8>();
    u32 destSerial   = ReadBE<u32>();
    short destX      = ReadBE<i16>();
    short destY      = ReadBE<i16>();
    char destZ       = ReadBE<i8>();

    if (graphic == 0x0EED)
    {
        graphic = 0x0EEF;
    }
    else if (graphic == 0x0EEA)
    {
        graphic = 0x0EEC;
    }
    else if (graphic == 0x0EF0)
    {
        graphic = 0x0EF2;
    }

    CGameCharacter* sourceObj = g_World->FindWorldCharacter(sourceSerial);

    if (sourceObj == nullptr)
    {
        sourceSerial = 0;
    }
    else
    {
        sourceX = sourceObj->GetX();
        sourceY = sourceObj->GetY();
        sourceZ = sourceObj->GetZ();
    }

    CGameCharacter* destObj = g_World->FindWorldCharacter(destSerial);

    if (destObj == nullptr)
    {
        destSerial = 0;
    }
    else
    {
        destX = destObj->GetX();
        destY = destObj->GetY();
        destZ = destObj->GetZ();
    }

    CGameEffect* effect = nullptr;

    u8 speed = 5;

    if ((sourceSerial == 0u) || (destSerial == 0u)) //Игрок/НПС кладет предмет в контейнер
    {
        effect                 = new CGameEffectMoving();
        effect->EffectType     = EF_MOVING;
        effect->FixedDirection = true;

        ((CGameEffectMoving*)effect)->MoveDelay = 20 / speed;
    }
    else //Предмет взяли из контейнера
    {
        effect             = new CGameEffectDrag();
        effect->EffectType = EF_DRAG;
    }

    effect->Graphic    = graphic;
    effect->Color      = color;
    effect->Serial     = sourceSerial;
    effect->DestSerial = destSerial;
    effect->SetX(sourceX);
    effect->SetY(sourceY);
    effect->SetZ(sourceZ);
    effect->DestX    = destX;
    effect->DestY    = destY;
    effect->DestZ    = destZ;
    effect->SetDuration(5_s);

    uintptr_t addressAnimData = (uintptr_t)g_FileManager.m_AnimdataMul.GetBuffer();

    if (addressAnimData != 0u)
    {
        ANIM_DATA* pad = (ANIM_DATA*)(addressAnimData + ((graphic * 68) + 4 * ((graphic / 8) + 1)));

        effect->Speed = pad->FrameInterval * 45;
    }
    else
    {
        effect->Speed = speed + 6;
    }

    g_EffectManager.AddEffect(effect);
}

PACKET_HANDLER(CorpseEquipment)
{
    if (g_World == nullptr)
    {
        return;
    }

    u32 cserial = ReadBE<u32>();

    u8* end = GetBuffer() + GetSize();

    int layer = ReadBE<u8>();

    while ((layer != 0) && GetPtr() < end)
    {
        u32 serial = ReadBE<u32>();

        CGameItem* obj = g_World->FindWorldItem(serial);

        if (obj != nullptr && obj->Container == cserial)
        {
            g_World->PutEquipment(obj, cserial, layer);
        }

        layer = ReadBE<u8>();
    }
}

PACKET_HANDLER(ASCIIPrompt)
{
    if (g_World == nullptr)
    {
        return;
    }

    g_ConsolePrompt = PT_ASCII;
    memcpy(&g_LastASCIIPrompt[0], &m_buffer[0], 11);
}

PACKET_HANDLER(UnicodePrompt)
{
    if (g_World == nullptr)
    {
        return;
    }

    g_ConsolePrompt = PT_UNICODE;
    memcpy(&g_LastUnicodePrompt[0], &m_buffer[0], 11);
}

PACKET_HANDLER(CharacterAnimation)
{
    if (g_World == nullptr)
    {
        return;
    }

    u32 serial          = ReadBE<u32>();
    CGameCharacter* obj = g_World->FindWorldCharacter(serial);

    if (obj != nullptr)
    {
        u16 action          = ReadBE<u16>();
        u16 frameCount      = ReadBE<u16>();
        frameCount          = 0;
        u16 repeatMode      = ReadBE<u16>();
        bool frameDirection = (ReadBE<u8>() == 0); //true - forward, false - backward
        bool repeat         = (ReadBE<u8>() != 0);
        u8 delay            = ReadBE<u8>();
        obj->SetAnimation(
            g_AnimationManager.GetReplacedObjectAnimation(obj, action),
            delay,
            (u8)frameCount,
            (u8)repeatMode,
            repeat,
            frameDirection);
        obj->AnimationFromServer = true;
    }
}

PACKET_HANDLER(NewCharacterAnimation)
{
    if (g_World == nullptr)
    {
        return;
    }

    u32 serial          = ReadBE<u32>();
    CGameCharacter* obj = g_World->FindWorldCharacter(serial);

    if (obj != nullptr)
    {
        u16 type   = ReadBE<u16>();
        u16 action = ReadBE<u16>();
        u8 mode    = ReadBE<u8>();

        u8 group = g_AnimationManager.GetObjectNewAnimation(obj, type, action, mode);

        obj->SetAnimation(group);

        obj->AnimationRepeatMode = 1;
        obj->AnimationDirection  = true;

        if ((type == 1 || type == 2) && obj->Graphic == 0x0015)
        {
            obj->AnimationRepeat = true;
        }

        obj->AnimationFromServer = true;
    }
}

PACKET_HANDLER(DisplayQuestArrow)
{
    g_QuestArrow.Timer   = g_Ticks + 1000;
    g_QuestArrow.Enabled = (ReadBE<u8>() != 0);
    g_QuestArrow.X       = ReadBE<u16>();
    g_QuestArrow.Y       = ReadBE<u16>();
}

PACKET_HANDLER(ClientViewRange)
{
    g_ConfigManager.UpdateRange = ReadBE<u8>();
}

PACKET_HANDLER(KrriosClientSpecial)
{
    u8 type = ReadBE<u8>();
    if (type == 0xFE)
    {
        CPacketRazorAnswer().Send();
    }
}

PACKET_HANDLER(AssistVersion)
{
    u32 version = ReadBE<u32>();
    CPacketAssistVersion(version, uo_client_version.GetValue()).Send();
}

PACKET_HANDLER(CharacterListNotification)
{
    g_Orion.InitScreen(GS_DELETE);
    g_ConnectionScreen.SetType(CST_CHARACTER_LIST);
    g_ConnectionScreen.SetConnectionFailed(true);
    g_ConnectionScreen.SetErrorCode(ReadBE<u8>());
}

PACKET_HANDLER(ErrorCode)
{
    u8 code = ReadBE<u8>();

    g_Orion.InitScreen(GS_DELETE);
    g_ConnectionScreen.SetType(CST_GAME_LOGIN);
    g_ConnectionScreen.SetErrorCode(code);

    if (code > 7)
    {
        g_ConnectionScreen.SetErrorCode(3);
    }
}

PACKET_HANDLER(AttackCharacter)
{
    g_LastAttackObject = ReadBE<u32>();

    if (g_LastAttackObject != 0 && g_World != nullptr)
    {
        CGameCharacter* obj = g_World->FindWorldCharacter(g_LastAttackObject);

        if (obj != nullptr && (obj->MaxHits == 0))
        {
            CPacketStatusRequest(g_LastAttackObject).Send();
        }
    }
}

PACKET_HANDLER(Season)
{
    if (g_World == nullptr)
    {
        return;
    }

    u8 season = ReadBE<u8>();

    if (season > ST_DESOLATION)
    {
        season = 0;
    }

    if ((g_Player->Dead() /*|| g_Season == ST_DESOLATION*/) && (SEASON_TYPE)season != ST_DESOLATION)
    {
        return;
    }

    g_OldSeason      = (SEASON_TYPE)season;
    g_OldSeasonMusic = ReadBE<u8>();

    if (g_Season == ST_DESOLATION)
    {
        g_OldSeasonMusic = DEATH_MUSIC_INDEX;
    }

    g_Orion.ChangeSeason((SEASON_TYPE)season, g_OldSeasonMusic);
}

PACKET_HANDLER(DisplayDeath)
{
    if (g_World == nullptr)
    {
        return;
    }

    u32 serial       = ReadBE<u32>();
    u32 corpseSerial = ReadBE<u32>();
    u32 running      = ReadBE<u32>();

    CGameCharacter* owner = g_World->FindWorldCharacter(serial);

    if (owner == nullptr)
    {
        return;
    }

    serial |= 0x80000000;

    g_World->ReplaceObject(owner, serial);

    if (corpseSerial != 0u)
    {
        g_CorpseManager.Add(CCorpse(corpseSerial, serial, owner->Direction, running != 0));
    }

    u8 group = g_AnimationManager.GetDieGroupIndex(owner->Graphic, running != 0);

    owner->SetAnimation(group, 0, 5, 1, false, false);
}

PACKET_HANDLER(OpenChat)
{
    u8 newbuf[4] = { 0xf0, 0x00, 0x04, 0xff };
    g_Orion.Send(newbuf, 4);
}

PACKET_HANDLER(DisplayClilocString)
{
    if (g_World == nullptr)
    {
        return;
    }

    u32 serial  = ReadBE<u32>();
    u16 graphic = ReadBE<u16>();
    u8 type     = ReadBE<u8>();
    u16 color   = ReadBE<u16>();
    u8 font     = (u8)ReadBE<u16>();
    u32 cliloc  = ReadBE<u32>();
    if (!g_FontManager.UnicodeFontExists(font))
    {
        font = 0;
    }

    u8 flags = 0;
    if (*m_buffer == 0xCC)
    {
        flags = ReadBE<u8>();
    }

    std::string name = ReadString(30);
    std::wstring affix{};
    if (*m_buffer == 0xCC)
    {
        affix = Core::DecodeUTF8(ReadString());
    }

    std::wstring wc = ReadWStringLE();
    auto message    = g_ClilocManager.ParseArgumentsToClilocString(cliloc, false, wc);
    message += affix;

    CGameObject* obj = g_World->FindWorldObject(serial);
    if (/*type == ST_BROADCAST || type == ST_SYSTEM ||*/ serial == 0xFFFFFFFF || (serial == 0u) ||
        (Core::ToLowerA(name) == "system" && obj == nullptr))
    {
        g_Orion.CreateUnicodeTextMessage(TT_SYSTEM, serial, font, color, message);
    }
    else
    {
        /*if (type == ST_EMOTE)
		{
			color = ConfigManager.EmoteColor;
			str = L"*" + str + L"*";
		}*/
        if (obj != nullptr)
        {
            if (name.length() == 0u)
            {
                obj->JournalPrefix = "You see: ";
            }
            else
            {
                obj->JournalPrefix = name + ": ";
                obj->SetName(name);
                if (obj->NPC)
                {
                    g_GumpManager.UpdateContent(serial, 0, GT_STATUSBAR);
                }
            }
        }
        g_Orion.CreateUnicodeTextMessage(TT_OBJECT, serial, font, color, message);
    }
}

PACKET_HANDLER(MegaCliloc)
{
    if (g_World == nullptr)
    {
        return;
    }

    u16 unknown = ReadBE<u16>();
    if (unknown > 1)
    {
        return;
    }

    u32 serial = ReadBE<u32>();
    Move(2);
    u32 clilocRevision = ReadBE<u32>();

    u8* end = m_buffer + m_size;
    std::vector<std::wstring> list;
    while (m_ptr < end)
    {
        u32 cliloc = ReadBE<u32>();
        if (cliloc == 0u)
        {
            break;
        }

        const int len         = ReadBE<i16>();
        std::wstring argument = {};
        if (len > 0)
        {
            argument = ReadWStringLE(len / 2);
        }

        std::wstring str = g_ClilocManager.ParseArgumentsToClilocString(cliloc, true, argument);
        //LOG("Cliloc: argstr=%s\n", ToString(str).c_str());
        //LOG("Cliloc: 0x%08X len=%i arg=%s\n", cliloc, len, ToString(argument).c_str());

        bool canAdd = true;
        for (const std::wstring& tempStr : list)
        {
            if (tempStr == str)
            {
                canAdd = false;
                break;
            }
        }

        if (canAdd)
        {
            list.push_back(str);
        }
    }

    CGameItem* container = nullptr;
    CGameObject* obj     = g_World->FindWorldObject(serial);
    if (obj != nullptr)
    {
        container = g_World->FindWorldItem(obj->Container);
    }

    bool inBuyList = false;
    if (container != nullptr)
    {
        inBuyList =
            (container->Layer == OL_BUY || container->Layer == OL_BUY_RESTOCK ||
             container->Layer == OL_SELL);
    }

    bool first        = true;
    std::wstring name = {};
    std::wstring data = {};
    if (!list.empty())
    {
        for (const std::wstring& str : list)
        {
            if (first)
            {
                name = str;
                if (obj != nullptr && !obj->NPC)
                {
                    obj->SetName(Core::ToString(str));
                    obj->GenerateObjectHandlesTexture(str);
                }

                first = false;
            }
            else
            {
                if (data.length() != 0u)
                {
                    data += L"\n";
                }

                data += str;
            }
        }
    }

    //LOG_DUMP((u8 *)message.c_str(), message.length() * 2);
    g_ObjectPropertiesManager.Add(serial, CObjectProperty(serial, clilocRevision, name, data));
    if (obj != nullptr && g_ToolTip.m_Object == obj)
    {
        g_ObjectPropertiesManager.Reset();
    }

    //LOG("message=%s\n", ToString(message).c_str());
    if (inBuyList && (container->Serial != 0u))
    {
        CGumpShop* gump = (CGumpShop*)g_GumpManager.GetGump(container->Serial, 0, GT_SHOP);
        if (gump != nullptr)
        {
            CGUIHTMLGump* htmlGump = gump->m_ItemList[0];
            QFOR(shopItem, htmlGump->m_Items, CBaseGUI*)
            {
                if (shopItem->Type == GOT_SHOPITEM && shopItem->Serial == serial &&
                    ((CGUIShopItem*)shopItem)->NameFromCliloc)
                {
                    CGUIShopItem* si = (CGUIShopItem*)shopItem;
                    int oldHeight    = si->GetSize().y;

                    si->Name = Core::ToString(name);
                    si->CreateNameText();
                    si->UpdateOffsets();

                    int delta = si->GetSize().y - oldHeight;
                    if ((delta != 0) && shopItem->m_Next != nullptr)
                    {
                        QFOR(shopItem2, shopItem->m_Next, CBaseGUI*)
                        {
                            if (shopItem2->Type == GOT_SHOPITEM)
                            {
                                ((CGUIShopItem*)shopItem2)
                                    ->SetY(((CGUIShopItem*)shopItem2)->GetY() + delta);
                            }
                        }
                    }

                    break;
                }
            }

            htmlGump->CalculateDataSize();
        }
    }
}

PACKET_HANDLER(Damage)
{
    if (g_World == nullptr)
    {
        return;
    }

    u32 serial                = ReadBE<u32>();
    CGameCharacter* character = g_World->FindWorldCharacter(serial);

    if (character != nullptr)
    {
        int damage = ReadBE<u16>();

        CTextData* text = new CTextData();
        text->Unicode   = false;
        text->Font      = 3;
        text->Serial    = serial;
        text->Color     = (serial == g_PlayerSerial ? 0x0034 : 0x0021);
        text->Type      = TT_OBJECT;
        text->Text      = std::to_string(damage);
        text->GenerateTexture(0);
        text->SetX(text->m_Texture.Width / 2);
        int height = text->m_Texture.Height;

        CTextData* head = (CTextData*)character->m_DamageTextControl.Last();

        if (head != nullptr)
        {
            height += head->GetY();

            if (height > 0)
            {
                if (height > 100)
                {
                    height = 0;
                }

                text->SetY(height);
            }
        }

        character->m_DamageTextControl.Add(text);
        text->Timer = g_Ticks + DAMAGE_TEXT_NORMAL_DELAY;
    }
}

PACKET_HANDLER(BuffDebuff)
{
    if (g_World == nullptr)
    {
        return;
    }

    /*
	df
	00 2e
	00 00 2b b5
	04 04
	00 01
	00 00 00 00 04 04 00 01 00 00 00 00
	01 ca
	00 00 00
	00 10 6a 6b
	00 10 6a 6c
	00 00 00 00
	00 00
	00 00
	00 00



	df
	00 46
	00 00 2b b5
	04 05
	00 01
	00 00 00 00 04 05 00 01 00 00 00 00
	00 85
	00 00 00
	00 10 6a 66
	00 10 56 82
	00 00 00 00
	00 01
	00 00
	09 00 2b 00 20 00 39 00 20 00 41 00 72 00 6d 00 6f 00 72 00 00 00
	00 01
	00 00




	Buffs And Attributes Packet.
	from server
	byte    ID (DF)
	word    Packet Size
	dword    Player Serial
	word    Buff Type
	(
	BonusStr = 0x01, BonusDex = 0x02, BonusInt = 0x03, BonusHits = 0x07, BonusStamina = 0x08, BonusMana = 0x09,
	RegenHits = 0x0A, RegenStam = 0x0B, RegenMana = 0x0C, NightSight = 0x0D, Luck = 0x0E, ReflectPhysical = 0x10,
	EnhancePotions = 0x11, AttackChance = 0x12, DefendChance = 0x13, SpellDamage = 0x14, CastRecovery = 0x15,
	CastSpeed = 0x16, ManaCost = 0x17, ReagentCost = 0x18, WeaponSpeed = 0x19, WeaponDamage = 0x1A,
	PhysicalResistance = 0x1B, FireResistance = 0x1C, ColdResistance = 0x1D, PoisonResistance = 0x1E,
	EnergyResistance = 0x1F, MaxPhysicalResistance = 0x20, MaxFireResistance = 0x21, MaxColdResistance = 0x22,
	MaxPoisonResistance = 0x23, MaxEnergyResistance = 0x24, AmmoCost = 0x26, KarmaLoss = 0x28, 0x3EA+ = buff icons
	)

	word    Buffs Count

	loop    Buffs >>>
	word    Source Type
	(
	0 = Character, 50 = two-handed weapon, 53 = one-handed weapon or spellbook, 54 = shield or ranged weapon,
	55 = shoes, 56 = pants or legs, 58 = helm or hat, 59 = gloves, 60 = ring, 61 = talisman, 62 = necklace or gorget,
	64 = waist, 65 = inner torso, 66 = bracelet, 69 = middle torso, 70 = earring, 71 = arms, 72 = cloak or quiver,
	74 = outer torso, 1000 = spells
	)

	word    0x00
	word    Buff Icon ID (0 for attributes)
	word    Buff Queue Index (Delta Value for attributes)
	dword    0x00
	word    Buff Duration in seconds (0 for attributes)
	byte[3]    0x00
	dword    Buff Title Cliloc
	dword    Buff Secondary Cliloc (0 for attributes)
	dword    Buff Third Cliloc (0 for attributes)
	word    Primary Cliloc Arguments Length (0 for attributes)
	u8[*]    Primary Cliloc Arguments
	word    Secondary Cliloc Arguments Length (0 for attributes)
	u8[*]    Secondary Cliloc Arguments
	word    Third Cliloc Arguments Length (0 for attributes)
	u8[*]    Third Cliloc Arguments
	endloop    Buffs <<<<
	*/

    const int tableCount = 126;

    static const u16 table[tableCount] = {
        0x754C, 0x754A, 0x0000, 0x0000, 0x755E, 0x7549, 0x7551, 0x7556, 0x753A, 0x754D, 0x754E,
        0x7565, 0x753B, 0x7543, 0x7544, 0x7546, 0x755C, 0x755F, 0x7566, 0x7554, 0x7540, 0x7568,
        0x754F, 0x7550, 0x7553, 0x753E, 0x755D, 0x7563, 0x7562, 0x753F, 0x7559, 0x7557, 0x754B,
        0x753D, 0x7561, 0x7558, 0x755B, 0x7560, 0x7541, 0x7545, 0x7552, 0x7569, 0x7548, 0x755A,
        0x753C, 0x7547, 0x7567, 0x7542, 0x758A, 0x758B, 0x758C, 0x758D, 0x0000, 0x758E, 0x094B,
        0x094C, 0x094D, 0x094E, 0x094F, 0x0950, 0x753E, 0x5011, 0x7590, 0x7591, 0x7592, 0x7593,
        0x7594, 0x7595, 0x7596, 0x7598, 0x7599, 0x759B, 0x759C, 0x759E, 0x759F, 0x75A0, 0x75A1,
        0x75A3, 0x75A4, 0x75A5, 0x75A6, 0x75A7, 0x75C0, 0x75C1, 0x75C2, 0x75C3, 0x75C4, 0x75F2,
        0x75F3, 0x75F4, 0x75F5, 0x75F6, 0x75F7, 0x75F8, 0x75F9, 0x75FA, 0x75FB, 0x75FC, 0x75FD,
        0x75FE, 0x75FF, 0x7600, 0x7601, 0x7602, 0x7603, 0x7604, 0x7605, 0x7606, 0x7607, 0x7608,
        0x7609, 0x760A, 0x760B, 0x760C, 0x760D, 0x760E, 0x760F, 0x7610, 0x7611, 0x7612, 0x7613,
        0x7614, 0x7615, 0x75C5, 0x75F6, 0x761B
    };

    const u16 buffIconStart = 0x03E9; //0x03EA ???

    u32 serial = ReadBE<u32>();
    u16 iconID = ReadBE<u16>() - buffIconStart;
    if (iconID < tableCount) //buff
    {
        CGumpBuff* gump = (CGumpBuff*)g_GumpManager.UpdateGump(0, 0, GT_BUFF);

        if (gump != nullptr)
        {
            u16 mode = ReadBE<u16>();
            if (mode != 0u)
            {
                Move(12);

                u16 timer = ReadBE<u16>();
                Move(3);

                u32 titleCliloc       = ReadBE<u32>();
                u32 descriptionCliloc = ReadBE<u32>();
                u32 wtfCliloc         = ReadBE<u32>();
                Move(4);

                std::wstring title = g_ClilocManager.GetCliloc(g_Language)->GetW(titleCliloc, true);
                std::wstring description = {};
                std::wstring wtf         = {};

                if (descriptionCliloc != 0u)
                {
                    std::wstring arguments = ReadWStringLE();
                    description            = L'\n' + g_ClilocManager.ParseArgumentsToClilocString(
                                              descriptionCliloc, true, arguments);

                    if (description.length() < 2)
                    {
                        description = {};
                    }
                }

                if (wtfCliloc != 0u)
                {
                    wtf = L'\n' + g_ClilocManager.GetCliloc(g_Language)->GetW(wtfCliloc, true);
                }

                std::wstring text = L"<left>" + title + description + wtf + L"</left>";
                gump->AddBuff(table[iconID], timer, text);
            }
            else
            {
                gump->DeleteBuff(table[iconID]);
            }
        }
    }
}

PACKET_HANDLER(SecureTrading)
{
    if (g_World == nullptr)
    {
        return;
    }

    u8 type    = ReadBE<u8>();
    u32 serial = ReadBE<u32>();

    if (type == 0) //Новое трэйд окно
    {
        u32 id1    = ReadBE<u32>();
        u32 id2    = ReadBE<u32>();
        u8 hasName = ReadBE<u8>();

        CGumpSecureTrading* gump = new CGumpSecureTrading(id1, 0, 0, id1, id2);

        CGameObject* obj = g_World->FindWorldObject(id1);

        if (obj != nullptr)
        {
            obj = obj->GetTopObject()->FindSecureTradeBox();

            if (obj != nullptr)
            {
                obj->Clear();
            }
        }

        obj = g_World->FindWorldObject(id2);

        if (obj != nullptr)
        {
            obj = obj->GetTopObject()->FindSecureTradeBox();

            if (obj != nullptr)
            {
                obj->Clear();
            }
        }

        if ((hasName != 0u) && (*m_ptr != 0u))
        {
            gump->Text = ReadString();
        }

        g_GumpManager.AddGump(gump);
    }
    else if (type == 1)
    { //Отмена
        g_GumpManager.CloseGump(serial, 0, GT_TRADE);
    }
    else if (type == 2) //Обновление
    {
        CGumpSecureTrading* gump =
            (CGumpSecureTrading*)g_GumpManager.UpdateGump(serial, 0, GT_TRADE);

        if (gump != nullptr)
        {
            u32 id1 = ReadBE<u32>();
            u32 id2 = ReadBE<u32>();

            gump->StateMy       = (id1 != 0);
            gump->StateOpponent = (id2 != 0);
        }
    }
}

PACKET_HANDLER(TextEntryDialog)
{
    if (g_World == nullptr)
    {
        return;
    }

    u32 serial  = ReadBE<u32>();
    u8 parentID = ReadBE<u8>();
    u8 buttonID = ReadBE<u8>();

    short textLen    = ReadBE<i16>();
    std::string text = ReadString(textLen);

    bool haveCancel = ReadBE<u8>() == 0u;
    u8 variant      = ReadBE<u8>();
    int maxLength   = ReadBE<u32>();

    short descLen    = ReadBE<i16>();
    std::string desc = ReadString(descLen);

    CGumpTextEntryDialog* gump =
        new CGumpTextEntryDialog(serial, 143, 172, variant, maxLength, text, desc);
    gump->NoClose  = haveCancel;
    gump->ParentID = parentID;
    gump->ButtonID = buttonID;

    g_GumpManager.AddGump(gump);
}

PACKET_HANDLER(OpenMenu)
{
    if (g_World == nullptr)
    {
        return;
    }

    u32 serial = ReadBE<u32>();
    u32 id     = ReadBE<u16>();

    u8 nameLen       = ReadBE<u8>();
    std::string name = ReadString(nameLen);

    u8 count = ReadBE<u8>();

    if (unpack16(m_ptr) != 0u) //menu
    {
        CGumpMenu* gump = new CGumpMenu(serial, id, 0, 0);

        gump->Add(new CGUIGumppic(0x0910, 0, 0));

        gump->Add(new CGUIColoredPolygone(0, 0, 40, 42, 217, 49, 0xFF000001));

        CGUIText* text = (CGUIText*)gump->Add(new CGUIText(0x0386, 39, 18));
        text->CreateTextureA(1, name, 200, TS_LEFT, UOFONT_FIXED);

        CGUIHTMLGump* htmlGump = (CGUIHTMLGump*)gump->Add(
            new CGUIHTMLGump(CGumpMenu::ID_GM_HTMLGUMP, 0, 40, 42, 217, 49, false, true));
        htmlGump->Initalize(true);

        htmlGump->Add(new CGUIShader(&g_ColorizerShader, true));

        int posX = 0;

        for (int i = 0; i < count; i++)
        {
            u16 graphic = ReadBE<u16>();
            u16 color   = ReadBE<u16>();

            nameLen = ReadBE<u8>();
            name    = ReadString(nameLen);

            Core::Vec2<i32> size = g_Orion.GetStaticArtDimension(graphic);

            if ((size.x != 0) && (size.y != 0))
            {
                int posY = size.y;

                if (posY >= 47)
                {
                    posY = 0;
                }
                else
                {
                    posY = ((47 - posY) / 2);
                }

                CGUIMenuObject* menuObject = (CGUIMenuObject*)htmlGump->Add(
                    new CGUIMenuObject((int)i + 1, graphic, color, posX, posY, name));

                posX += size.x;
            }
        }

        htmlGump->Add(new CGUIShader(&g_ColorizerShader, false));

        htmlGump->CalculateDataSize();

        gump->m_TextObject = (CGUIText*)gump->Add(new CGUIText(0x0386, 42, 105));
        //gump->m_TextObject->CreateTextureA(1, name, 200, TS_LEFT, UOFONT_FIXED); //На данный момнт создавать нечего

        g_GumpManager.AddGump(gump);
    }
    else //gray menu
    {
        int x = (g_gameWindow.GetSize().x / 2) - 200;
        int y = (g_gameWindow.GetSize().y / 2) - ((121 + (count * 21)) / 2);

        CGumpGrayMenu* gump = new CGumpGrayMenu(serial, id, x, y);

        CGUIResizepic* background =
            (CGUIResizepic*)gump->Add(new CGUIResizepic(0, 0x13EC, 0, 0, 400, 11111));

        CGUIText* text = (CGUIText*)gump->Add(new CGUIText(0x0386, 20, 16));
        text->CreateTextureA(1, name, 370);

        int offsetY    = 35 + text->m_Texture.Height;
        int gumpHeight = 70 + offsetY;

        for (int i = 0; i < count; i++)
        {
            Move(4);

            nameLen = ReadBE<u8>();
            name    = ReadString(nameLen);

            gump->Add(new CGUIRadio((int)i + 1, 0x138A, 0x138B, 0x138A, 20, offsetY)); //Button

            offsetY += 2;

            text = (CGUIText*)gump->Add(new CGUIText(0x0386, 50, offsetY));
            text->CreateTextureA(1, name, 340);

            int addHeight = text->m_Texture.Height;

            if (addHeight < 21)
            {
                addHeight = 21;
            }

            offsetY += addHeight - 1;
            gumpHeight += addHeight;
        }

        offsetY += 5;

        gump->Add(new CGUIButton(
            CGumpGrayMenu::ID_GGM_CANCEL, 0x1450, 0x1450, 0x1451, 70, offsetY)); //CANCEL
        gump->Add(new CGUIButton(
            CGumpGrayMenu::ID_GGM_CONTINUE, 0x13B2, 0x13B2, 0x13B3, 200, offsetY)); //CONTINUE

        background->Height = gumpHeight;

        g_GumpManager.AddGump(gump);
    }
}

void CPacketManager::AddHTMLGumps(CGump* gump, std::vector<HTMLGumpDataInfo>& list)
{
    for (int i = 0; i < (int)list.size(); i++)
    {
        HTMLGumpDataInfo& data = list[i];

        CGUIHTMLGump* htmlGump = (CGUIHTMLGump*)gump->Add(new CGUIHTMLGump(
            data.TextID + 1,
            0x0BB8,
            data.sGumpCoords->X,
            data.sGumpCoords->Y,
            data.Width,
            data.Height,
            data.HaveBackground != 0,
            data.HaveScrollbar != 0));
        htmlGump->DrawOnly     = (data.HaveScrollbar == 0);

        int width = htmlGump->Width;

        if (data.HaveScrollbar != 0)
        {
            width -= 16;
        }

        u32 htmlColor = 0xFFFFFFFF;
        u16 color     = 0;

        if (data.Color != 0)
        {
            if (data.Color == 0x00FFFFFF)
            {
                htmlColor = 0xFFFFFFFE;
            }
            else
            {
                htmlColor = (g_ColorManager.Color16To32((u16)data.Color) << 8) | 0xFF;
            }
        }
        else if (data.HaveBackground == 0)
        {
            color = 0xFFFF;

            if (data.HaveScrollbar == 0)
            {
                htmlColor = 0x010101FF;
            }
        }
        else
        {
            width -= 9;
            htmlColor = 0x010101FF;
        }

        CGUIHTMLText* htmlText = (CGUIHTMLText*)htmlGump->Add(new CGUIHTMLText(
            data.TextID,
            (u8)(GameVars::GetClientVersion() >= CV_305D),
            color,
            0,
            0,
            width,
            TS_LEFT,
            /*UOFONT_BLACK_BORDER*/ 0,
            htmlColor));

        if (data.IsXMF)
        {
            htmlText->Text = g_ClilocManager.GetCliloc(g_Language)->GetW(data.TextID);
            htmlText->CreateTexture(data.HaveBackground == 0);
            htmlGump->CalculateDataSize();
        }
    }

    list.clear();
}

PACKET_HANDLER(OpenGump)
{
    if (g_World == nullptr)
    {
        return;
    }

    std::vector<HTMLGumpDataInfo> htmlGumlList;

    //TPRINT("Gump dump::\n");
    //TDUMP(buf, size);

    u32 serial = ReadBE<u32>();
    u32 id     = ReadBE<u32>();
    int x      = ReadBE<i32>();
    int y      = ReadBE<i32>();

    auto found = m_GumpsCoordsCache.find(id);
    if (found != m_GumpsCoordsCache.end())
    {
        x = found->second.X;
        y = found->second.Y;
    }
    else
    {
        SetCachedGumpCoords(id, x, y);
    }

    CGumpGeneric* gump = new CGumpGeneric(serial, x, y, id);

    u16 commandsLength   = ReadBE<u16>();
    std::string commands = ReadString(commandsLength);

    Core::TextFileParser parser({}, " ", "", "{}");
    Core::TextFileParser cmdParser({}, " ", "", "");
    Core::TextFileParser tilepicGraphicParser({}, ",", "", "");

    std::vector<std::string> commandList = parser.GetTokens(commands.c_str());
    CBaseGUI* lastGumpObject             = nullptr;

    bool EntryChanged = false;
    int FirstPage     = 0;
    int CurrentPage   = 0;
    CEntryText* ChangeEntry;

    for (const std::string& str : commandList)
    {
        std::vector<std::string> list = cmdParser.GetTokens(str.c_str());

        int listSize = (int)list.size();

        if (listSize == 0)
        {
            continue;
        }

        std::string cmd = Core::ToLowerA(list[0]);

        CBaseGUI* go = nullptr;

        if (cmd == "nodispose")
        {
        }
        else if (cmd == "nomove")
        {
            gump->NoMove = true;
        }
        else if (cmd == "noclose")
        {
            gump->NoClose = true;
        }
        else if (cmd == "page")
        {
            if (listSize >= 2)
            {
                AddHTMLGumps(gump, htmlGumlList);

                int page = Core::ToInt(list[1]);
                go       = new CGUIPage(page);
                if (FirstPage == 0)
                {
                    FirstPage = page;
                }
                CurrentPage = page;
            }
        }
        else if (cmd == "group")
        {
            if (listSize >= 2)
            {
                int group = Core::ToInt(list[1]);
                go        = new CGUIGroup(group);
            }
        }
        else if (cmd == "endgroup")
        {
            go = new CGUIGroup(0);
        }
        else if (cmd == "resizepic")
        {
            if (listSize >= 6)
            {
                int x       = Core::ToInt(list[1]);
                int y       = Core::ToInt(list[2]);
                int graphic = Core::ToInt(list[3]);
                int width   = Core::ToInt(list[4]);
                int height  = Core::ToInt(list[5]);

                go           = new CGUIResizepic(0, graphic, x, y, width, height);
                go->DrawOnly = true;
            }
        }
        else if (cmd == "checkertrans")
        {
            if (listSize >= 5)
            {
                int x      = Core::ToInt(list[1]);
                int y      = Core::ToInt(list[2]);
                int width  = Core::ToInt(list[3]);
                int height = Core::ToInt(list[4]);

                go = new CGUIChecktrans(x, y, width, height);
            }
        }
        else if (cmd == "button")
        {
            if (listSize >= 5)
            {
                int x      = Core::ToInt(list[1]);
                int y      = Core::ToInt(list[2]);
                int up     = Core::ToInt(list[3]);
                int down   = Core::ToInt(list[4]);
                int action = 0;
                int toPage = 0;
                int index  = 0;

                if (listSize >= 6)
                {
                    action = Core::ToInt(list[5]);
                }
                if (listSize >= 7)
                {
                    toPage = Core::ToInt(list[6]);
                }
                if (listSize >= 8)
                {
                    index = Core::ToInt(list[7]);
                }

                if (action != 0)
                {
                    toPage = -1;
                }

                go                = new CGUIButton(index, up, up, down, x, y);
                go->CheckPolygone = true;

                ((CGUIButton*)go)->ToPage = toPage;
            }
        }
        else if (cmd == "buttontileart")
        {
            if (listSize >= 12)
            {
                int x           = Core::ToInt(list[1]);
                int y           = Core::ToInt(list[2]);
                int up          = Core::ToInt(list[3]);
                int down        = Core::ToInt(list[4]);
                int action      = Core::ToInt(list[5]);
                int toPage      = Core::ToInt(list[6]);
                int index       = Core::ToInt(list[7]);
                int tileGraphic = Core::ToInt(list[8]);
                int tileColor   = Core::ToInt(list[9]);
                int tileX       = Core::ToInt(list[10]);
                int tileY       = Core::ToInt(list[11]);

                if (action != 0)
                {
                    toPage = -1;
                }

                go = new CGUIButtonTileart(
                    index, up, up, down, x, y, tileGraphic, tileColor, tileX, tileY);

                ((CGUIButton*)go)->ToPage = toPage;
            }
        }
        else if (cmd == "checkbox")
        {
            if (listSize >= 5)
            {
                int x     = Core::ToInt(list[1]);
                int y     = Core::ToInt(list[2]);
                int up    = Core::ToInt(list[3]);
                int down  = Core::ToInt(list[4]);
                int state = 0;
                int index = 0;

                if (listSize >= 6)
                {
                    state = Core::ToInt(list[5]);
                }
                if (listSize >= 7)
                {
                    index = Core::ToInt(list[6]);
                }

                go = new CGUICheckbox(index, up, down, up, x, y);

                ((CGUICheckbox*)go)->Checked = (state != 0);
            }
        }
        else if (cmd == "radio")
        {
            if (listSize >= 5)
            {
                int x     = Core::ToInt(list[1]);
                int y     = Core::ToInt(list[2]);
                int up    = Core::ToInt(list[3]);
                int down  = Core::ToInt(list[4]);
                int state = 0;
                int index = 0;

                if (listSize >= 6)
                {
                    state = Core::ToInt(list[5]);
                }
                if (listSize >= 7)
                {
                    index = Core::ToInt(list[6]);
                }

                go = new CGUIRadio(index, up, down, up, x, y);

                ((CGUIRadio*)go)->Checked = (state != 0);
            }
        }
        else if (cmd == "text")
        {
            if (listSize >= 5)
            {
                int x     = Core::ToInt(list[1]);
                int y     = Core::ToInt(list[2]);
                int color = Core::ToInt(list[3]);
                int index = Core::ToInt(list[4]);

                if (color != 0)
                {
                    color++;
                }

                go           = new CGUIGenericText(index, color, x, y);
                go->DrawOnly = true;
            }
        }
        else if (cmd == "croppedtext")
        {
            if (listSize >= 7)
            {
                int x      = Core::ToInt(list[1]);
                int y      = Core::ToInt(list[2]);
                int width  = Core::ToInt(list[3]);
                int height = Core::ToInt(list[4]);
                int color  = Core::ToInt(list[5]);
                int index  = Core::ToInt(list[6]);

                if (color != 0)
                {
                    color++;
                }

                go           = new CGUIGenericText(index, color, x, y, width);
                go->DrawOnly = true;
            }
        }
        else if (cmd == "textentry")
        {
            if (listSize >= 8)
            {
                int x         = Core::ToInt(list[1]);
                int y         = Core::ToInt(list[2]);
                int width     = Core::ToInt(list[3]);
                int height    = Core::ToInt(list[4]);
                int color     = Core::ToInt(list[5]);
                int index     = Core::ToInt(list[6]);
                int textIndex = Core::ToInt(list[7]);

                //if (color)
                //	color++;

                gump->Add(new CGUIHitBox(index + 1, x, y, width, height));
                gump->Add(new CGUIScissor(true, x, y, 0, 0, width, height));
                go = new CGUIGenericTextEntry(index + 1, textIndex, color, x, y);
                ((CGUIGenericTextEntry*)go)->CheckOnSerial = true;
                ((CGUITextEntry*)go)->m_Entry.Width        = width;
                if (!EntryChanged)
                {
                    if (CurrentPage == 0 || CurrentPage == FirstPage)
                    {
                        ChangeEntry  = &((CGUITextEntry*)go)->m_Entry;
                        EntryChanged = true;
                    }
                }
            }
        }
        else if (cmd == "textentrylimited")
        {
            if (listSize >= 9)
            {
                int x         = Core::ToInt(list[1]);
                int y         = Core::ToInt(list[2]);
                int width     = Core::ToInt(list[3]);
                int height    = Core::ToInt(list[4]);
                int color     = Core::ToInt(list[5]);
                int index     = Core::ToInt(list[6]);
                int textIndex = Core::ToInt(list[7]);
                int length    = Core::ToInt(list[8]);

                //if (color)
                //	color++;

                gump->Add(new CGUIHitBox(index + 1, x, y, width, height));
                gump->Add(new CGUIScissor(true, x, y, 0, 0, width, height));
                go = new CGUIGenericTextEntry(index + 1, textIndex, color, x, y, width, length);
                ((CGUIGenericTextEntry*)go)->CheckOnSerial = true;
                ((CGUITextEntry*)go)->m_Entry.Width        = width;
                if (!EntryChanged)
                {
                    if (CurrentPage == 0 || CurrentPage == FirstPage)
                    {
                        ChangeEntry  = &((CGUITextEntry*)go)->m_Entry;
                        EntryChanged = true;
                    }
                }
            }
        }
        else if (cmd == "tilepic" || cmd == "tilepichue")
        {
            if (listSize >= 4)
            {
                int x       = Core::ToInt(list[1]);
                int y       = Core::ToInt(list[2]);
                int color   = 0;
                int graphic = 0;

                if (cmd == "tilepic")
                {
                    std::vector<std::string> graphicList =
                        tilepicGraphicParser.GetTokens(list[3].c_str());

                    if (!graphicList.empty())
                    {
                        graphic = Core::ToInt(graphicList[0]);

                        if (graphicList.size() >= 2)
                        {
                            color = Core::ToInt(graphicList[1]);
                        }
                    }
                }
                else
                {
                    graphic = Core::ToInt(list[3]);
                }

                if (listSize >= 5)
                {
                    color = Core::ToInt(list[4]);
                }

                if (color != 0)
                {
                    gump->Add(new CGUIShader(&g_ColorizerShader, true));
                }

                go           = new CGUITilepic(graphic, color, x, y);
                go->DrawOnly = true;
            }
        }
        else if (cmd == "gumppic")
        {
            if (listSize >= 4)
            {
                int x       = Core::ToInt(list[1]);
                int y       = Core::ToInt(list[2]);
                int graphic = Core::ToInt(list[3]);
                int color   = 0;

                if (listSize >= 5 && GameVars::GetClientVersion() >= CV_305D)
                {
                    Core::TextFileParser gumppicParser({}, "=", "", "");
                    std::vector<std::string> hueList = gumppicParser.GetTokens(list[4].c_str());

                    if (hueList.size() > 1)
                    {
                        color = Core::ToInt(hueList[1]);
                    }
                    else
                    {
                        color = Core::ToInt(hueList[0]);
                    }

                    if (listSize >= 6)
                    {
                        std::vector<std::string> classList =
                            gumppicParser.GetTokens(list[5].c_str());

                        if (hueList.size() > 1)
                        {
                            if (Core::ToLowerA(classList[0]) == "class" &&
                                Core::ToLowerA(Core::Trim(classList[1])) == "virtuegumpitem")
                            {
                                go = new CGUIVirtueGump(graphic, x, y);
                            }
                        }
                    }
                }

                if (go == nullptr)
                {
                    if (color != 0)
                    {
                        gump->Add(new CGUIShader(&g_ColorizerShader, true));
                    }

                    go           = new CGUIGumppic(graphic, x, y);
                    go->Color    = color;
                    go->DrawOnly = true;
                }
            }
        }
        else if (cmd == "gumppictiled")
        {
            if (listSize >= 6)
            {
                int x       = Core::ToInt(list[1]);
                int y       = Core::ToInt(list[2]);
                int width   = Core::ToInt(list[3]);
                int height  = Core::ToInt(list[4]);
                int graphic = Core::ToInt(list[5]);

                go           = new CGUIGumppicTiled(graphic, x, y, width, height);
                go->DrawOnly = true;
            }
        }
        else if (cmd == "htmlgump" || cmd == "xmfhtmlgump" || cmd == "xmfhtmlgumpcolor")
        {
            if (listSize >= 8)
            {
                HTMLGumpDataInfo htmlInfo = {};
                htmlInfo.IsXMF            = (cmd != "htmlgump");
                GumpCoords* gumpCoords =
                    new GumpCoords{ Core::ToInt(list[1]), Core::ToInt(list[2]) };
                htmlInfo.sGumpCoords    = gumpCoords;
                htmlInfo.Width          = Core::ToInt(list[3]);
                htmlInfo.Height         = Core::ToInt(list[4]);
                htmlInfo.TextID         = Core::ToInt(list[5]);
                htmlInfo.HaveBackground = Core::ToInt(list[6]);
                htmlInfo.HaveScrollbar  = Core::ToInt(list[7]);
                htmlInfo.Color          = 0;

                if (cmd == "xmfhtmlgumpcolor" && listSize >= 9)
                {
                    htmlInfo.Color = Core::ToInt(list[8]);

                    if (htmlInfo.Color == 0x7FFF)
                    {
                        htmlInfo.Color = 0x00FFFFFF;
                    }
                }

                htmlGumlList.push_back(htmlInfo);
            }
        }
        else if (cmd == "xmfhtmltok")
        {
            if (listSize >= 9)
            {
                HTMLGumpDataInfo htmlInfo = {};
                htmlInfo.IsXMF            = true;
                GumpCoords* gumpCoords =
                    new GumpCoords{ Core::ToInt(list[1]), Core::ToInt(list[2]) };
                htmlInfo.sGumpCoords    = gumpCoords;
                htmlInfo.Width          = Core::ToInt(list[3]);
                htmlInfo.Height         = Core::ToInt(list[4]);
                htmlInfo.HaveBackground = Core::ToInt(list[5]);
                htmlInfo.HaveScrollbar  = Core::ToInt(list[6]);
                htmlInfo.Color          = Core::ToInt(list[7]);

                if (htmlInfo.Color == 0x7FFF)
                {
                    htmlInfo.Color = 0x00FFFFFF;
                }

                htmlInfo.TextID = Core::ToInt(list[8]);

                if (listSize >= 10)
                {
                }

                htmlGumlList.push_back(htmlInfo);
            }
        }
        else if (cmd == "tooltip")
        {
            if (listSize >= 2 && lastGumpObject != nullptr)
            {
                lastGumpObject->ClilocID = Core::ToInt(list[1]);
            }
        }
        else if (cmd == "mastergump")
        {
            if (listSize >= 2)
            {
                gump->MasterGump = Core::ToInt(list[1]);
            }
        }

        if (go != nullptr)
        {
            lastGumpObject = go;
            gump->Add(go);

            if ((go->Type == GOT_TILEPIC || go->Type == GOT_GUMPPIC) && (go->Color != 0u))
            {
                gump->Add(new CGUIShader(&g_ColorizerShader, false));
            }
            else if (go->Type == GOT_TEXTENTRY)
            {
                gump->Add(new CGUIScissor(false));
            }
        }
    }

    AddHTMLGumps(gump, htmlGumlList);

    int textLinesCount = ReadBE<i16>();

    for (int i = 0; i < textLinesCount; i++)
    {
        int linelen = ReadBE<i16>();

        if (linelen != 0)
        {
            gump->AddText((int)i, ReadWStringBE(linelen));
        }
        else
        {
            gump->AddText((int)i, {});
        }
    }

    if (EntryChanged)
    {
        g_EntryPointer = ChangeEntry;
    }

    g_GumpManager.AddGump(gump);
}

PACKET_HANDLER(OpenCompressedGump)
{
    if (g_World == nullptr)
    {
        return;
    }

    u32 senderID = ReadBE<u32>();
    u32 gumpID   = ReadBE<u32>();
    u32 x        = ReadBE<u32>();
    u32 y        = ReadBE<u32>();
    uLongf cLen  = ReadBE<u32>() - 4; //Compressed Length (4 == sizeof(DecompressedLen)
    uLongf dLen  = ReadBE<u32>();     //Decompressed Length

    if (cLen < 1)
    {
        LOG_ERROR("PacketManager", "CLen=%d\nServer Sends bad Compressed Gumpdata!", cLen);

        return;
    }
    if ((int)(28 + cLen) > m_size)
    {
        LOG_ERROR("PacketManager", "Server Sends bad Compressed Gumpdata!");

        return;
    }

    // Layout data.....
    std::vector<u8> decLayoutData(dLen);
    LOG_INFO(
        "PacketManager",
        "Gump layout:\n\tSenderID=0x%08X\n\tGumpID=0x%08X\n\tCLen=%d\n\tDLen=%d\nDecompressing layout gump data...",
        senderID,
        gumpID,
        cLen,
        dLen);

    int z_err = mz_uncompress(&decLayoutData[0], &dLen, m_ptr, cLen);
    if (z_err != Z_OK)
    {
        LOG_ERROR("PacketManager", "Decompress layout gump error %d", z_err);
        return;
    }

    LOG_INFO("PacketManager", "Layout gump data decompressed!");
    // Text data.....

    Move(cLen);

    u32 linesCount = ReadBE<u32>(); //Text lines count
    u32 cTLen      = 0;
    uLongf dTLen   = 0;
    std::vector<u8> gumpDecText;

    if (linesCount > 0)
    {
        cTLen = ReadBE<u32>(); //Compressed lines length
        dTLen = ReadBE<u32>(); //Decompressed lines length

        gumpDecText.resize(dTLen);
        LOG_INFO("PacketManager", "Decompressing text gump data...");

        z_err = mz_uncompress(&gumpDecText[0], &dTLen, m_ptr, cTLen);
        if (z_err != Z_OK)
        {
            LOG_ERROR("PacketManager", "Decompress text gump error %d", z_err);
            return;
        }

        LOG_INFO(
            "PacketManager",
            "Text gump data decompressed!\nGump text lines:\n\tLinesCount=%d\n\tCTLen=%d\n\tDTLen=%d",
            linesCount,
            cTLen,
            dTLen);
    }

    int newsize = 21 + dLen + 2 + dTLen;

    std::vector<u8> newbufData(newsize);
    u8* newbuf = &newbufData[0];
    newbuf[0]  = 0xb0;
    pack16(newbuf + 1, newsize);
    pack32(newbuf + 3, senderID);
    pack32(newbuf + 7, gumpID);
    pack32(newbuf + 11, x);
    pack32(newbuf + 15, y);
    pack16(newbuf + 19, (u16)dLen);
    memcpy(newbuf + 21, &decLayoutData[0], dLen);
    pack16(newbuf + 21 + dLen, (u16)linesCount);

    if (linesCount > 0)
    {
        memcpy(newbuf + 23 + dLen, &gumpDecText[0], dTLen);
    }
    else
    {
        newbuf[newsize - 1] = 0x00;
    }

    LOG_INFO("PacketManager", "Gump decompressed! newsize=%d", newsize);

    m_size   = newsize;
    m_buffer = newbuf;
    m_end    = m_buffer + m_size;

    OnPacket();
}

PACKET_HANDLER(DyeData)
{
    u32 serial = ReadBE<u32>();
    Move(2);
    u16 graphic = ReadBE<u16>();

    Core::Vec2<i32> gumpSize = g_Orion.GetGumpDimension(0x0906);

    auto x         = i16((g_gameWindow.GetSize().x / 2) - (gumpSize.x / 2));
    auto y         = i16((g_gameWindow.GetSize().y / 2) - (gumpSize.y / 2));
    CGumpDye* gump = new CGumpDye(serial, x, y, graphic);

    g_GumpManager.AddGump(gump);
}

PACKET_HANDLER(DisplayMap)
{
    if (g_World == nullptr)
    {
        return;
    }

    u32 serial = ReadBE<u32>();
    u16 gumpid = ReadBE<u16>();

    //TPRINT("gumpid = 0x%04X\n", gumpid);

    u16 startX = ReadBE<u16>();
    u16 startY = ReadBE<u16>();
    u16 endX   = ReadBE<u16>();
    u16 endY   = ReadBE<u16>();
    u16 width  = ReadBE<u16>();
    u16 height = ReadBE<u16>();

    CGumpMap* gump = new CGumpMap(serial, gumpid, startX, startY, endX, endY, width, height);

    if (*m_buffer == 0xF5 || GameVars::GetClientVersion() >= CV_308Z) //308z или выше?
    {
        u16 facet = 0;

        if (*m_buffer == 0xF5)
        {
            facet = ReadBE<u16>();
        }

        g_MultiMap.LoadFacet(gump, gump->m_Texture, facet);
    }
    else
    {
        g_MultiMap.LoadMap(gump, gump->m_Texture);
    }

    //TPRINT("GumpX=%d GumpY=%d\n", startX, startY);
    //TPRINT("GumpTX=%d GumpTY=%d\n", endX, endY);
    //TPRINT("GumpW=%d GumpH=%d\n", width, height);

    g_GumpManager.AddGump(gump);

    CGameItem* obj = g_World->FindWorldItem(serial);

    if (obj != nullptr)
    {
        obj->Opened = true;
    }
}

PACKET_HANDLER(MapData)
{
    if (g_World == nullptr)
    {
        return;
    }

    u32 serial     = ReadBE<u32>();
    CGumpMap* gump = (CGumpMap*)g_GumpManager.UpdateGump(serial, 0, GT_MAP);

    if (gump != nullptr && gump->m_DataBox != nullptr)
    {
        switch ((MAP_MESSAGE)ReadBE<u8>()) //Action
        {
            case MM_ADD: //Add Pin
            {
                Move(1);

                short x = ReadBE<u16>();
                short y = ReadBE<u16>();

                gump->m_DataBox->Add(new CGUIGumppic(0x139B, x, y));
                gump->WantRedraw = true;

                break;
            }
            case MM_INSERT: //Insert New Pin
            {
                break;
            }
            case MM_MOVE: //Change Pin
            {
                break;
            }
            case MM_REMOVE: //Remove Pin
            {
                break;
            }
            case MM_CLEAR: //Clear Pins
            {
                gump->m_DataBox->Clear();
                gump->WantRedraw = true;

                break;
            }
            case MM_EDIT_RESPONSE: //Reply From Server to Action 6 (Plotting request)
            {
                gump->SetPlotState(ReadBE<u8>());

                break;
            }
            default: break;
        }
    }
}

PACKET_HANDLER(TipWindow)
{
    u8 flag = ReadBE<u8>();

    if (flag != 1) //1 - ignore
    {
        u32 serial = ReadBE<u32>();
        short len  = ReadBE<i16>();

        std::string str = ReadString(len);

        int x = 20;
        int y = 20;

        if (flag == 0u)
        {
            x = 200;
            y = 100;
        }

        CGumpTip* gump = new CGumpTip(serial, x, y, str, flag != 0);

        g_GumpManager.AddGump(gump);
    }
}

PACKET_HANDLER(CharacterProfile)
{
    if (g_World == nullptr)
    {
        return;
    }

    u32 serial              = ReadBE<u32>();
    std::wstring topText    = Core::ToWString(ReadString());
    std::wstring bottomText = ReadWStringBE();
    std::wstring dataText   = ReadWStringBE();
    CGumpProfile* gump      = new CGumpProfile(serial, 170, 90, topText, bottomText, dataText);
    g_GumpManager.AddGump(gump);
}

PACKET_HANDLER(BulletinBoardData)
{
    if (g_World == nullptr)
    {
        return;
    }

    switch (ReadBE<u8>())
    {
        case 0: //Open board
        {
            u32 serial = ReadBE<u32>();

            CGameItem* item = g_World->FindWorldItem(serial);

            if (item != nullptr)
            {
                CGumpBulletinBoard* bbGump =
                    (CGumpBulletinBoard*)g_GumpManager.UpdateGump(serial, 0, GT_BULLETIN_BOARD);

                if (bbGump != nullptr)
                {
                    CBaseGUI* bbItem = (CBaseGUI*)bbGump->m_HTMLGump->m_Items;

                    while (bbItem != nullptr)
                    {
                        CBaseGUI* bbNext = (CBaseGUI*)bbItem->m_Next;

                        if (bbItem->Type == GOT_BB_OBJECT)
                        {
                            bbGump->m_HTMLGump->Delete(bbItem);
                        }

                        bbItem = bbNext;
                    }

                    bbGump->m_HTMLGump->CalculateDataSize();
                }

                item->Opened = true;
            }

            std::string str((char*)m_ptr);

            int x = (g_gameWindow.GetSize().x / 2) - 245;
            int y = (g_gameWindow.GetSize().y / 2) - 205;

            CGumpBulletinBoard* gump = new CGumpBulletinBoard(serial, x, y, str);

            g_GumpManager.AddGump(gump);

            break;
        }
        case 1: //Summary message
        {
            u32 boardSerial = ReadBE<u32>();

            CGumpBulletinBoard* gump =
                (CGumpBulletinBoard*)g_GumpManager.GetGump(boardSerial, 0, GT_BULLETIN_BOARD);

            if (gump != nullptr)
            {
                u32 serial   = ReadBE<u32>();
                u32 parentID = ReadBE<u32>();

                //poster
                int len           = ReadBE<u8>();
                std::wstring text = (len > 0 ? Core::DecodeUTF8(ReadString(len)) : L"") + L" - ";

                //subject
                len = ReadBE<u8>();
                text += (len > 0 ? Core::DecodeUTF8(ReadString(len)) : L"") + L" - ";

                //data time
                len = ReadBE<u8>();
                text += (len > 0 ? Core::DecodeUTF8(ReadString(len)) : L"");

                int posY = (gump->m_HTMLGump->GetItemsCount() - 5) * 18;

                if (posY < 0)
                {
                    posY = 0;
                }

                gump->m_HTMLGump->Add(new CGUIBulletinBoardObject(serial, 0, posY, text));
                gump->m_HTMLGump->CalculateDataSize();
            }

            break;
        }
        case 2: //Message
        {
            u32 boardSerial = ReadBE<u32>();

            CGumpBulletinBoard* gump =
                (CGumpBulletinBoard*)g_GumpManager.GetGump(boardSerial, 0, GT_BULLETIN_BOARD);

            if (gump != nullptr)
            {
                u32 serial = ReadBE<u32>();

                //poster
                int len             = ReadBE<u8>();
                std::wstring poster = (len > 0 ? Core::DecodeUTF8(ReadString(len)) : L"");

                //subject
                len                  = ReadBE<u8>();
                std::wstring subject = (len > 0 ? Core::DecodeUTF8(ReadString(len)) : L"");

                //data time
                len                   = ReadBE<u8>();
                std::wstring dataTime = (len > 0 ? Core::DecodeUTF8(ReadString(len)) : L"");

                //unused, in old clients: user's graphic, color
                Move(4);

                u8 unknown = ReadBE<u8>();

                if (unknown > 0)
                {
                    //unused data
                    Move(unknown * 4);
                }

                u8 lines          = ReadBE<u8>();
                std::wstring data = {};

                for (int i = 0; i < lines; i++)
                {
                    u8 linelen = ReadBE<u8>();

                    if (data.length() != 0u)
                    {
                        data += L"\n";
                    }

                    if (linelen > 0)
                    {
                        data += Core::DecodeUTF8(ReadString(linelen));
                    }
                }

                u8 variant = 1 + (int)(poster == Core::ToWString(g_Player->GetName()));
                g_GumpManager.AddGump(new CGumpBulletinBoardItem(
                    serial, 0, 0, variant, boardSerial, poster, subject, dataTime, data));
            }

            break;
        }
        default: break;
    }
}

PACKET_HANDLER(OpenBook) // 0x93
{
    if (g_World == nullptr)
    {
        return;
    }

    u32 serial = ReadBE<u32>();
    u8 flags   = ReadBE<u8>();
    Move(1);
    auto pageCount  = ReadBE<u16>();
    CGumpBook* gump = new CGumpBook(
        serial, 0, 0, pageCount, flags != 0, (GameVars::GetClientVersion() >= CV_308Z));

    gump->m_EntryTitle->m_Entry.SetTextW(Core::DecodeUTF8(ReadString(60)));
    gump->m_EntryAuthor->m_Entry.SetTextW(Core::DecodeUTF8(ReadString(30)));

    g_GumpManager.AddGump(gump);
}

PACKET_HANDLER(OpenBookNew) // 0xD4
{
    if (g_World == nullptr)
    {
        return;
    }

    u32 serial    = ReadBE<u32>();
    u8 flag1      = ReadBE<u8>();
    u8 flag2      = ReadBE<u8>();
    u16 pageCount = ReadBE<u16>();

    CGumpBook* gump = new CGumpBook(serial, 0, 0, pageCount, (flag1 + flag2) != 0, true);

    int titleLen = ReadBE<u16>();

    if (titleLen > 0)
    {
        gump->m_EntryTitle->m_Entry.SetTextW(Core::DecodeUTF8(ReadString(titleLen)));
    }

    int authorLen = ReadBE<u16>();

    if (authorLen > 0)
    {
        gump->m_EntryAuthor->m_Entry.SetTextW(Core::DecodeUTF8(ReadString(authorLen)));
    }

    g_GumpManager.AddGump(gump);
}

PACKET_HANDLER(BookData)
{
    if (g_World == nullptr)
    {
        return;
    }

    u32 serial = ReadBE<u32>();

    CGumpBook* gump = (CGumpBook*)g_GumpManager.GetGump(serial, 0, GT_BOOK);

    if (gump != nullptr)
    {
        u16 pageCount = ReadBE<u16>();

        for (int i = 0; i < pageCount; i++)
        {
            u16 page = ReadBE<u16>();

            if (page > gump->PageCount)
            {
                continue;
            }

            u16 lineCount = ReadBE<u16>();

            std::wstring str = {};

            for (int j = 0; j < lineCount; j++)
            {
                if (j != 0)
                {
                    str += L'\n';
                }

                std::wstring temp = Core::DecodeUTF8(ReadString());

                while ((temp.length() != 0u) && (temp.back() == L'\n' || temp.back() == L'\r'))
                {
                    temp.resize(temp.length() - 1);
                }

                str += temp;
            }

            gump->SetPageData(page, str);
        }
    }
}

PACKET_HANDLER(BuyList)
{
    if (g_World == nullptr)
    {
        return;
    }

    u32 serial = ReadBE<u32>();

    CGameItem* container = g_World->FindWorldItem(serial);

    if (container == nullptr)
    {
        LOG_ERROR("PacketManager", "Error!!! Buy container is not found!!!");
        return;
    }

    u32 vendorSerial = container->Container;

    CGameCharacter* vendor = g_World->FindWorldCharacter(vendorSerial);

    if (vendor == nullptr)
    {
        LOG_ERROR("PacketManager", "Error!!! Buy vendor is not found!!!");
        return;
    }

    CGumpShop* gump = (CGumpShop*)g_GumpManager.GetGump(vendorSerial, 0, GT_SHOP);

    if (gump != nullptr && (gump->Serial != vendorSerial || !gump->IsBuyGump))
    {
        g_GumpManager.RemoveGump(gump);
        gump = nullptr;
    }

    if (gump == nullptr)
    {
        gump = new CGumpShop(vendorSerial, true, 150, 5);
        g_GumpManager.AddGump(gump);
    }

    gump->WantRedraw = true;

    if (container->Layer == OL_BUY_RESTOCK || container->Layer == OL_BUY)
    {
        u8 count = ReadBE<u8>();

        CGameItem* item = (CGameItem*)container->m_Items;
        //oldp spams this packet twice in a row on NPC verndors. nullptr check is needed t
        if (item == nullptr)
        {
            return;
        }

        bool reverse = (item->GetX() > 1);

        if (reverse)
        {
            while (item != nullptr && item->m_Next != nullptr)
            {
                item = (CGameItem*)item->m_Next;
            }
        }

        CGUIHTMLGump* htmlGump = gump->m_ItemList[0];

        int currentY = 0;

        QFOR(shopItem, htmlGump->m_Items, CBaseGUI*)
        {
            if (shopItem->Type == GOT_SHOPITEM)
            {
                currentY += shopItem->GetSize().y;
            }
        }

        for (int i = 0; i < count; i++)
        {
            if (item == nullptr)
            {
                LOG_ERROR("PacketManager", "Error!!! Buy item is not found!!!");
                break;
            }

            item->Price = ReadBE<u32>();

            u8 nameLen       = ReadBE<u8>();
            std::string name = ReadString(nameLen);

            //try int.parse and read cliloc.
            int clilocNum = 0;

            if (Int32TryParse(name, clilocNum))
            {
                item->SetName(g_ClilocManager.GetCliloc(g_Language)->GetA(clilocNum, true));
                item->NameFromCliloc = true;
            }
            else
            {
                item->SetName(name);
                item->NameFromCliloc = false;
            }

            if (reverse)
            {
                item = (CGameItem*)item->m_Prev;
            }
            else
            {
                item = (CGameItem*)item->m_Next;
            }
        }

        htmlGump->CalculateDataSize();
    }
    else
    {
        LOG_ERROR("PacketManager", "Unknown layer for buy container!!!");
    }
}

PACKET_HANDLER(SellList)
{
    if (g_World == nullptr)
    {
        return;
    }

    u32 serial = ReadBE<u32>();

    CGameCharacter* vendor = g_World->FindWorldCharacter(serial);

    if (vendor == nullptr)
    {
        LOG_ERROR("PacketManager", "Error!!! Sell vendor is not found!!!");
        return;
    }

    u16 itemsCount = ReadBE<u16>();

    if (itemsCount == 0u)
    {
        LOG_ERROR("PacketManager", "Error!!! Sell list is empty.");
        return;
    }

    g_GumpManager.CloseGump(0, 0, GT_SHOP);

    CGumpShop* gump        = new CGumpShop(serial, false, 100, 0);
    CGUIHTMLGump* htmlGump = gump->m_ItemList[0];

    int currentY = 0;

    for (int i = 0; i < itemsCount; i++)
    {
        u32 itemSerial   = ReadBE<u32>();
        u16 graphic      = ReadBE<u16>();
        u16 color        = ReadBE<u16>();
        u16 count        = ReadBE<u16>();
        u16 price        = ReadBE<u16>();
        int nameLen      = ReadBE<i16>();
        std::string name = ReadString(nameLen);

        int clilocNum       = 0;
        bool nameFromCliloc = false;

        if (Int32TryParse(name, clilocNum))
        {
            name           = g_ClilocManager.GetCliloc(g_Language)->GetA(clilocNum, true);
            nameFromCliloc = true;
        }

        CGUIShopItem* shopItem = (CGUIShopItem*)htmlGump->Add(
            new CGUIShopItem(itemSerial, graphic, color, count, price, name, 0, currentY));
        shopItem->NameFromCliloc = nameFromCliloc;

        if (i == 0)
        {
            shopItem->Selected = true;
            shopItem->CreateNameText();
            shopItem->UpdateOffsets();
        }

        currentY += shopItem->GetSize().y;
    }

    htmlGump->CalculateDataSize();

    g_GumpManager.AddGump(gump);
}

PACKET_HANDLER(BuyReply)
{
    u32 serial = ReadBE<u32>();
    u8 flag    = ReadBE<u8>();

    if (flag == 0u)
    { //Close shop gump
        g_GumpManager.CloseGump(serial, 0, GT_SHOP);
    }
}

PACKET_HANDLER(Logout)
{
    g_Orion.LogOut();
}

PACKET_HANDLER(OPLInfo)
{
    if (g_TooltipsEnabled)
    {
        u32 serial   = ReadBE<u32>();
        u32 revision = ReadBE<u32>();

        if (!g_ObjectPropertiesManager.RevisionCheck(serial, revision))
        {
            AddMegaClilocRequest(serial);
        }
    }
}

PACKET_HANDLER(CustomHouse)
{
    bool compressed           = ReadBE<u8>() == 0x03;
    bool enableResponse       = ReadBE<u8>() == 0x01;
    u32 serial                = ReadBE<u32>();
    u32 revision              = ReadBE<u32>();
    CGameItem* foundationItem = g_World->FindWorldItem(serial);

    if (foundationItem == nullptr)
    {
        return;
    }

    CMulti* multi = foundationItem->GetMulti();

    if (multi == nullptr)
    {
        return;
    }

    ReadBE<u16>();
    ReadBE<u16>();

    CCustomHouse* house = g_CustomHousesManager.Get(serial);

    if (house == nullptr)
    {
        house = new CCustomHouse(serial, revision);
        g_CustomHousesManager.Add(house);
    }
    else
    {
        house->Revision = revision;
    }

    LOG_INFO("PacketManager", "House update in cache: 0x%08X 0x%08X", serial, revision);

    house->m_Items.clear();

    short minX = multi->MinX;
    short minY = multi->MinY;
    short maxY = multi->MaxY;

    u8 planes = ReadBE<u8>();

    for (int plane = 0; plane < planes; plane++)
    {
        u32 header    = ReadBE<u32>();
        uLongf dLen   = ((header & 0xFF0000) >> 16) | ((header & 0xF0) << 4);
        int cLen      = ((header & 0xFF00) >> 8) | ((header & 0x0F) << 8);
        int planeZ    = (header & 0x0F000000) >> 24;
        int planeMode = (header & 0xF0000000) >> 28;

        if (cLen <= 0)
        {
            continue;
        }

        std::vector<u8> decompressedBytes(dLen);
        int z_err = mz_uncompress(&decompressedBytes[0], &dLen, m_ptr, cLen);
        if (z_err != Z_OK)
        {
            LOG_ERROR(
                "PacketManager",
                "Bad CustomHouseStruct compressed data received from server, house serial:%i",
                serial);
            //LOG("House plane idx:%i\n", idx);
            continue;
        }

        Core::StreamReader tempReader(static_cast<u8*>(&decompressedBytes[0]), dLen);
        Move(cLen);

        u16 id = 0;
        char x = 0;
        char y = 0;
        char z = 0;

        switch (planeMode)
        {
            case 0:
            {
                for (u32 i = 0; i < decompressedBytes.size() / 5; i++)
                {
                    id = tempReader.ReadBE<u16>();
                    x  = tempReader.ReadLE<u8>();
                    y  = tempReader.ReadLE<u8>();
                    z  = tempReader.ReadLE<u8>();

                    if (id != 0)
                    {
                        house->m_Items.push_back(CBuildObject(id, x, y, z));
                    }
                }

                break;
            }
            case 1:
            {
                if (planeZ > 0)
                {
                    z = ((planeZ - 1) % 4) * 20 + 7; // Z=7,27,47,67
                }
                else
                {
                    z = 0;
                }

                for (u32 i = 0; i < decompressedBytes.size() / 4; i++)
                {
                    id = tempReader.ReadBE<u16>();
                    x  = tempReader.ReadLE<u8>();
                    y  = tempReader.ReadLE<u8>();

                    if (id != 0)
                    {
                        house->m_Items.push_back(CBuildObject(id, x, y, z));
                    }
                }

                break;
            }
            case 2:
            {
                short xOffs       = 0;
                short yOffs       = 0;
                short multiHeight = 0;

                if (planeZ > 0)
                {
                    z = ((planeZ - 1) % 4) * 20 + 7; // Z=7,27,47,67
                }
                else
                {
                    z = 0;
                }

                if (planeZ <= 0)
                {
                    xOffs       = minX;
                    yOffs       = minY;
                    multiHeight = (maxY - minY) + 2;
                }
                else if (planeZ <= 4)
                {
                    xOffs       = minX + 1;
                    yOffs       = minY + 1;
                    multiHeight = (maxY - minY);
                }
                else
                {
                    xOffs       = minX;
                    yOffs       = minY;
                    multiHeight = (maxY - minY) + 1;
                }

                for (u32 i = 0; i < decompressedBytes.size() / 2; i++)
                {
                    id = tempReader.ReadBE<u16>();
                    x  = i / multiHeight + xOffs;
                    y  = i % multiHeight + yOffs;

                    if (id != 0)
                    {
                        house->m_Items.push_back(CBuildObject(id, x, y, z));
                    }
                }

                break;
            }
            default: break;
        }
    }

    house->Paste(foundationItem);

    if (enableResponse)
    {
        CPacketCustomHouseResponse().Send();
    }
}

PACKET_HANDLER(OrionMessages)
{
    u16 command = ReadBE<u16>();
    u8 type     = command >> 12;
    command &= 0x0FFF;

    if (type != 0u)
    {
        return;
    }

    switch (command)
    {
        case OCT_ORION_FEATURES:
        {
            g_OrionFeaturesFlags = ReadBE<u32>();
            g_ConfigManager.UpdateFeatures();
            break;
        }
        case OCT_ORION_IGNORE_TILES_IN_FILTER:
        {
            g_Orion.m_IgnoreInFilterTiles.clear();
            u16 count = ReadBE<u16>();
            for (int i = 0; i < count; i++)
            {
                g_Orion.m_IgnoreInFilterTiles.push_back(std::pair<u16, u16>(ReadBE<u16>(), 0));
            }

            u16 countRange = ReadBE<u16>();
            for (int i = 0; i < countRange; i++)
            {
                u16 rangeStart = ReadBE<u16>();
                u16 rangeEnd   = ReadBE<u16>();
                g_Orion.m_IgnoreInFilterTiles.push_back(std::pair<u16, u16>(rangeStart, rangeEnd));
            }
            break;
        }
        case OCT_ORION_VERSION:
        {
            // FIXME: get numeric version from GitRevision.h
            CPacketOrionVersion(0).Send();
            break;
        }
        case OCT_CLOSE_GENERIC_GUMP_WITHOUT_RESPONSE:
        {
            u32 serial = ReadBE<u32>();
            u32 id     = ReadBE<u32>();
            u8 all     = ReadBE<u8>();
            QFOR(gump, g_GumpManager.m_Items, CGump*)
            {
                if (gump->GumpType == GT_GENERIC && gump->Serial == serial && gump->ID == id)
                {
                    gump->RemoveMark = true;
                    if (all == 0u)
                    {
                        break;
                    }
                }
            }

            break;
        }
        case OCT_SELECT_MENU:
        {
            u32 serial = ReadBE<u32>();
            u32 id     = ReadBE<u32>();
            u32 code   = ReadBE<u32>();
            if ((serial == 0u) && (id == 0u))
            {
                for (CGump* gump = (CGump*)g_GumpManager.m_Items; gump != nullptr;)
                {
                    CGump* next = (CGump*)gump->m_Next;
                    if (gump->GumpType == GT_MENU || gump->GumpType == GT_GRAY_MENU)
                    {
                        CPacketMenuResponse(gump, code).Send();
                        g_GumpManager.RemoveGump(gump);
                    }
                    gump = next;
                }

                break;
            }

            CGump* gump = g_GumpManager.GetGump(serial, id, GT_MENU);
            if (gump == nullptr)
            {
                gump = g_GumpManager.GetGump(serial, id, GT_GRAY_MENU);
                if (gump != nullptr)
                {
                    CPacketGrayMenuResponse(gump, code).Send();
                    g_GumpManager.RemoveGump(gump);
                }
            }
            else
            {
                CPacketMenuResponse(gump, code).Send();
                g_GumpManager.RemoveGump(gump);
            }
            break;
        }
        case OCT_CAST_SPELL_REQUEST:
        {
            int id = ReadBE<u32>();
            if (id >= 0)
            {
                g_LastSpellIndex = id;
                CPacketCastSpell(id).Send();
            }
            break;
        }
        case OCT_USE_SKILL_REQUEST:
        {
            int id = ReadBE<u32>();
            if (id >= 0)
            {
                g_LastSkillIndex = id;
                CPacketUseSkill(id).Send();
            }
            break;
        }
        case OCT_DRAW_STATUSBAR:
        {
            u32 serial     = ReadBE<u32>();
            int x          = ReadBE<i32>();
            int y          = ReadBE<i32>();
            bool minimized = (ReadBE<u8>() != 0);
            if (serial != g_PlayerSerial)
            {
                minimized = true;
            }

            CGump* gump = g_GumpManager.UpdateContent(serial, 0, GT_STATUSBAR);
            if (gump != nullptr)
            {
                gump->Minimized = minimized;

                if (gump->Minimized)
                {
                    gump->MinimizedX = x;
                    gump->MinimizedY = y;
                }
                else
                {
                    gump->SetX(x);
                    gump->SetY(y);
                }
            }
            else
            {
                CPacketStatusRequest(serial).Send();
                g_GumpManager.AddGump(new CGumpStatusbar(serial, x, y, minimized));
            }
            break;
        }
        case OCT_CLOSE_STATUSBAR:
        {
            u32 serial = ReadBE<u32>();
            g_GumpManager.CloseGump(serial, 0, GT_STATUSBAR);
            break;
        }
        case OCT_SECURE_TRADE_CHECK:
        {
            u32 id1 = ReadBE<u32>();
            CGumpSecureTrading* gump =
                (CGumpSecureTrading*)g_GumpManager.UpdateGump(id1, 0, GT_TRADE);
            if (gump != nullptr)
            {
                gump->StateMy = (ReadBE<u8>() != 0);
                CPacketTradeResponse(gump, 2).Send();
            }
            break;
        }
        case OCT_SECURE_TRADE_CLOSE:
        {
            u32 id1   = ReadBE<u32>();
            auto gump = (CGumpSecureTrading*)g_GumpManager.GetGump(id1, 0, GT_TRADE);
            if (gump != nullptr)
            {
                gump->RemoveMark = true;
                CPacketTradeResponse(gump, 1).Send();
            }
            break;
        }
        case OCT_UNICODE_SPEECH_REQUEST:
        {
            u16 color         = ReadBE<u16>();
            std::wstring text = ReadWStringBE();
            CGameConsole::Send(text, color);
            break;
        }
        case OCT_RENAME_MOUNT_REQUEST:
        {
            u32 serial       = ReadBE<u32>();
            std::string text = ReadString();
            CPacketRenameRequest(serial, text).Send();
            break;
        }
        case OCT_RECONNECT:
        {
            g_Orion.StartReconnect();
            break;
        }
        case OCT_PLAY_MACRO:
        {
            int count = ReadBE<u16>();
            static Macro existsMacros(Core::EKey::Key_Unknown, false, false, false);
            existsMacros.Clear();

            g_MacroPointer                          = nullptr;
            g_MacroManager.SendNotificationToPlugin = true;
            for (int m = 0; m < count; m++)
            {
                std::string name     = ReadString();
                std::string param    = ReadString();
                MACRO_CODE macroCode = MC_NONE;
                for (int i = 0; i < Macro::kMacroActionNamesCount; i++)
                {
                    std::string macroName = Macro::GetActionName(i);
                    if (strcmp(name.c_str(), macroName.c_str()) == 0)
                    {
                        macroCode = (MACRO_CODE)i;
                        break;
                    }
                }

                if (macroCode == MC_NONE)
                {
                    LOG_ERROR("PacketManager", "Invalid macro name: %s", name.c_str());
                    continue;
                }

                MacroObject* macro = Macro::CreateMacro(macroCode);
                if (param.length() != 0u)
                {
                    if (macro->HasString())
                    {
                        ((MacroObjectString*)macro)->SetString(param);
                    }
                    else
                    {
                        for (int i = 0; i < Macro::kMacroActionsCount; i++)
                        {
                            if (param == Macro::GetAction(i))
                            {
                                macro->SetSubCode((MACRO_SUB_CODE)i);
                                break;
                            }
                        }
                    }
                }
                existsMacros.Add(macro);
            }

            g_MacroPointer                      = (MacroObject*)existsMacros.m_Items;
            g_MacroManager.WaitingBandageTarget = false;
            g_MacroManager.WaitForTargetTimer   = 0;
            g_MacroManager.Execute();
            break;
        }
        case OCT_MOVE_PAPERDOLL:
        {
            u32 serial  = ReadBE<u32>();
            int x       = ReadBE<i32>();
            int y       = ReadBE<i32>();
            CGump* gump = g_GumpManager.UpdateContent(serial, 0, GT_PAPERDOLL);
            if (gump != nullptr)
            {
                if (gump->Minimized)
                {
                    gump->MinimizedX = x;
                    gump->MinimizedY = y;
                }
                else
                {
                    gump->SetX(x);
                    gump->SetY(y);
                }
            }
            break;
        }
        case OCT_USE_ABILITY:
        {
            CGumpAbility::OnAbilityUse(ReadBE<u8>() % 2);
            break;
        }
        case OCT_OPEN_DOOR:
        {
            g_Orion.OpenDoor();
            break;
        }
        default: break;
    }
}

PACKET_HANDLER(PacketsList)
{
    if (g_World == nullptr)
    {
        return;
    }

    int count = ReadBE<u16>();
    for (int i = 0; i < count; i++)
    {
        u8 id = ReadBE<u8>();
        if (id == 0xF3)
        {
            HandleUpdateItemSA();
        }
        else
        {
            LOG_ERROR("PacketManager", "Unknown packet ID=0x%02X in packet 0xF7!!!", id);
            break;
        }
    }
}

PACKET_HANDLER(MovePlayer)
{
    if (g_World == nullptr)
    {
        return;
    }

    u8 direction = ReadBE<u8>();
    g_PathFinder.Walk((direction & 0x80) != 0, direction & 7);
}

PACKET_HANDLER(Pathfinding)
{
    if (g_World == nullptr)
    {
        return;
    }

    u16 x = ReadBE<i16>();
    u16 y = ReadBE<i16>();
    u16 z = ReadBE<i16>();
    g_PathFinder.WalkTo(x, y, z, 0);
}

void CPacketManager::SetCachedGumpCoords(u32 id, int x, int y)
{
    auto found = m_GumpsCoordsCache.find(id);
    if (found != m_GumpsCoordsCache.end())
    {
        found->second.X = x;
        found->second.Y = y;
    }
    else
    {
        m_GumpsCoordsCache[id] = GumpCoords{ x, y };
    }
}

PACKET_HANDLER(BoatMoving)
{
    // FIXME: disable BoatMoving for the 0.1.9.6 patch
    return;
    u32 boatSerial = ReadBE<u32>();

    CGameObject* boat = g_World->FindWorldObject(boatSerial);
    if (boat == nullptr)
    {
        return;
    }

    u8 boatSpeed         = ReadBE<u8>();
    u8 movingDirection   = ReadBE<u8>();
    u8 facingDirection   = ReadBE<u8>();
    u16 boatX            = ReadBE<u16>();
    u16 boatY            = ReadBE<u16>();
    u16 boatZ            = ReadBE<u16>();
    u16 boatObjectsCount = ReadBE<u16>();

    g_World->UpdateGameObject(
        boatSerial,
        boat->Graphic,
        0,
        boat->Count,
        boatX,
        boatY,
        (char)boatZ,
        facingDirection,
        boat->Color,
        boat->GetFlags(),
        0,
        UGOT_MULTI,
        (u16)1);

    for (u16 i = 0; i < boatObjectsCount; i++)
    {
        u32 boatObjectSerial = ReadBE<u32>();
        u16 boatObjectX      = ReadBE<u16>();
        u16 boatObjectY      = ReadBE<u16>();
        u16 boatObjectZ      = ReadBE<u16>();

        CGameObject* boatObject = g_World->FindWorldObject(boatObjectSerial);
        if (boatObject == nullptr)
        {
            continue;
        }

        u8 direction = boatObject->NPC ? ((CGameCharacter*)boatObject)->Direction : 0;
        g_World->UpdateGameObject(
            boatObjectSerial,
            boatObject->Graphic,
            0,
            0,
            boatObjectX,
            boatObjectY,
            (char)boatObjectZ,
            direction,
            boatObject->Color,
            boatObject->GetFlags(),
            0,
            UGOT_ITEM,
            (u16)1);
    }
}
