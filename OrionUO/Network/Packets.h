#pragma once

#include "Core/DataStream.h"
#include "plugin/enumlist.h"

class CPacket : public Core::StreamWriter
{
public:
    CPacket(size_t size, bool autoResize = false);
    void Send();
};

class CPacketFirstLogin : public CPacket
{
public:
    CPacketFirstLogin();
};

class CPacketSelectServer : public CPacket
{
public:
    CPacketSelectServer(u8 index);
};

class CPacketSecondLogin : public CPacket
{
public:
    CPacketSecondLogin();
};

class CPacketCreateCharacter : public CPacket
{
public:
    CPacketCreateCharacter(const std::string &name);
};

class CPacketDeleteCharacter : public CPacket
{
public:
    CPacketDeleteCharacter(int charIndex);
};

class CPacketSelectCharacter : public CPacket
{
public:
    CPacketSelectCharacter(int index, const std::string &name);
};

class CPacketPickupRequest : public CPacket
{
public:
    CPacketPickupRequest(u32 serial, u16 count);
};

class CPacketDropRequestOld : public CPacket
{
public:
    CPacketDropRequestOld(u32 serial, u16 x, u16 y, char z, u32 container);
};

class CPacketDropRequestNew : public CPacket
{
public:
    CPacketDropRequestNew(
        u32 serial, u16 x, u16 y, char z, u8 slot, u32 container);
};

class CPacketEquipRequest : public CPacket
{
public:
    CPacketEquipRequest(u32 serial, u8 layer, u32 container);
};

class CPacketChangeWarmode : public CPacket
{
public:
    CPacketChangeWarmode(u8 state);
};

class CPacketHelpRequest : public CPacket
{
public:
    CPacketHelpRequest();
};

class CPacketStatusRequest : public CPacket
{
public:
    CPacketStatusRequest(u32 serial);
};

class CPacketSkillsRequest : public CPacket
{
public:
    CPacketSkillsRequest(u32 serial);
};

class CPacketSkillsStatusChangeRequest : public CPacket
{
public:
    CPacketSkillsStatusChangeRequest(u8 skill, u8 state);
};

class CPacketClickRequest : public CPacket
{
public:
    CPacketClickRequest(u32 serial);
};

class CPacketDoubleClickRequest : public CPacket
{
public:
    CPacketDoubleClickRequest(u32 serial);
};

class CPacketAttackRequest : public CPacket
{
public:
    CPacketAttackRequest(u32 serial);
};

class CPacketClientVersion : public CPacket
{
public:
    CPacketClientVersion(const std::string &version);
};

class CPacketASCIISpeechRequest : public CPacket
{
public:
    CPacketASCIISpeechRequest(const char *text, SPEECH_TYPE type, u16 font, u16 color);
};

class CPacketUnicodeSpeechRequest : public CPacket
{
public:
    CPacketUnicodeSpeechRequest(
        const wchar_t *text, SPEECH_TYPE type, u16 font, u16 color, u8 *language);
};

class CPacketCastSpell : public CPacket
{
public:
    CPacketCastSpell(int index);
};

class CPacketCastSpellFromBook : public CPacket
{
public:
    CPacketCastSpellFromBook(int index, u32 serial);
};

class CPacketUseSkill : public CPacket
{
public:
    CPacketUseSkill(int index);
};

class CPacketOpenDoor : public CPacket
{
public:
    CPacketOpenDoor();
};

class CPacketOpenSpellbook : public CPacket
{
public:
    CPacketOpenSpellbook(SPELLBOOK_TYPE type);
};

class CPacketEmoteAction : public CPacket
{
public:
    CPacketEmoteAction(const char *action);
};

class CPacketGumpResponse : public CPacket
{
public:
    CPacketGumpResponse(class CGumpGeneric *gump, int code);
};

class CPacketVirtueGumpResponse : public CPacket
{
public:
    CPacketVirtueGumpResponse(class CGump *gump, int code);
};

class CPacketMenuResponse : public CPacket
{
public:
    CPacketMenuResponse(class CGump *gump, int code);
};

class CPacketGrayMenuResponse : public CPacket
{
public:
    CPacketGrayMenuResponse(class CGump *gump, int code);
};

class CPacketTradeResponse : public CPacket
{
public:
    CPacketTradeResponse(class CGumpSecureTrading *gump, int code);
};

class CPacketLogoutNotification : public CPacket
{
public:
    CPacketLogoutNotification();
};

class CPacketTextEntryDialogResponse : public CPacket
{
public:
    CPacketTextEntryDialogResponse(
        class CGumpTextEntryDialog *gump, class CEntryText *entry, bool code);
};

class CPacketRenameRequest : public CPacket
{
public:
    CPacketRenameRequest(u32 serial, const std::string &newName);
};

class CPacketTipRequest : public CPacket
{
public:
    CPacketTipRequest(u16 id, u8 flag);
};

class CPacketASCIIPromptResponse : public CPacket
{
public:
    CPacketASCIIPromptResponse(const char *text, size_t len, bool cancel);
};

class CPacketUnicodePromptResponse : public CPacket
{
public:
    CPacketUnicodePromptResponse(const wchar_t *text, size_t len, const std::string &lang, bool cancel);
};

class CPacketDyeDataResponse : public CPacket
{
public:
    CPacketDyeDataResponse(u32 serial, u16 graphic, u16 color);
};

class CPacketProfileRequest : public CPacket
{
public:
    CPacketProfileRequest(u32 serial);
};

class CPacketProfileUpdate : public CPacket
{
public:
    CPacketProfileUpdate(u32 serial, const wchar_t *text, size_t len);
};

class CPacketCloseStatusbarGump : public CPacket
{
public:
    CPacketCloseStatusbarGump(u32 serial);
};

class CPacketPartyInviteRequest : public CPacket
{
public:
    CPacketPartyInviteRequest();
};

class CPacketPartyRemoveRequest : public CPacket
{
public:
    CPacketPartyRemoveRequest(u32 serial);
};

class CPacketPartyChangeLootTypeRequest : public CPacket
{
public:
    CPacketPartyChangeLootTypeRequest(u8 type);
};

class CPacketPartyAccept : public CPacket
{
public:
    CPacketPartyAccept(u32 serial);
};

class CPacketPartyDecline : public CPacket
{
public:
    CPacketPartyDecline(u32 serial);
};

class CPacketPartyMessage : public CPacket
{
public:
    CPacketPartyMessage(const wchar_t *text, size_t len, u32 serial = 0x00000000);
};

class CPacketGameWindowSize : public CPacket
{
public:
    CPacketGameWindowSize();
};

class CPacketClientViewRange : public CPacket
{
public:
    CPacketClientViewRange(u8 range);
};

class CPacketBulletinBoardRequestMessage : public CPacket
{
public:
    CPacketBulletinBoardRequestMessage(u32 serial, u32 msgSerial);
};

class CPacketBulletinBoardRequestMessageSummary : public CPacket
{
public:
    CPacketBulletinBoardRequestMessageSummary(u32 serial, u32 msgSerial);
};

class CPacketBulletinBoardPostMessage : public CPacket
{
public:
    CPacketBulletinBoardPostMessage(
        u32 serial, u32 replySerial, const char *subject, const char *message);
};

class CPacketBulletinBoardRemoveMessage : public CPacket
{
public:
    CPacketBulletinBoardRemoveMessage(u32 serial, u32 msgSerial);
};

class CPacketAssistVersion : public CPacket
{
public:
    CPacketAssistVersion(u32 version, const std::string &clientVersion);
};

class CPacketRazorAnswer : public CPacket
{
public:
    CPacketRazorAnswer();
};

class CPacketLanguage : public CPacket
{
public:
    CPacketLanguage(const std::string &lang);
};

class CPacketClientType : public CPacket
{
public:
    CPacketClientType();
};

class CPacketRequestPopupMenu : public CPacket
{
public:
    CPacketRequestPopupMenu(u32 serial);
};

class CPacketPopupMenuSelection : public CPacket
{
public:
    CPacketPopupMenuSelection(u32 serial, u16 menuID);
};

class CPacketOpenChat : public CPacket
{
public:
    CPacketOpenChat(const std::wstring &name);
};

class CPacketMapMessage : public CPacket
{
public:
    CPacketMapMessage(
        u32 serial, MAP_MESSAGE action, u8 pin = 0, short x = -24, short y = -31);
};

class CPacketGuildMenuRequest : public CPacket
{
public:
    CPacketGuildMenuRequest();
};

class CPacketQuestMenuRequest : public CPacket
{
public:
    CPacketQuestMenuRequest();
};

class CPacketEquipLastWeapon : public CPacket
{
public:
    CPacketEquipLastWeapon();
};

class CPacketVirtueRequest : public CPacket
{
public:
    CPacketVirtueRequest(int buttonID);
};

class CPacketInvokeVirtueRequest : public CPacket
{
public:
    CPacketInvokeVirtueRequest(u8 id);
};

class CPacketMegaClilocRequestOld : public CPacket
{
public:
    CPacketMegaClilocRequestOld(int serial);
};

class CPacketMegaClilocRequest : public CPacket
{
public:
    CPacketMegaClilocRequest(std::vector<u32> &list);
};

class CPacketChangeStatLockStateRequest : public CPacket
{
public:
    CPacketChangeStatLockStateRequest(u8 stat, u8 state);
};

class CPacketBookHeaderChangeOld : public CPacket
{
public:
    CPacketBookHeaderChangeOld(class CGumpBook *gump);
};

class CPacketBookHeaderChange : public CPacket
{
public:
    CPacketBookHeaderChange(class CGumpBook *gump);
};

class CPacketBookPageData : public CPacket
{
public:
    CPacketBookPageData(class CGumpBook *gump, int page);
};

class CPacketBookPageDataRequest : public CPacket
{
public:
    CPacketBookPageDataRequest(int serial, int page);
};

class CPacketBuyRequest : public CPacket
{
public:
    CPacketBuyRequest(class CGumpShop *gump);
};

class CPacketSellRequest : public CPacket
{
public:
    CPacketSellRequest(class CGumpShop *gump);
};

class CPacketUseCombatAbility : public CPacket
{
public:
    CPacketUseCombatAbility(u8 index);
};

class CPacketTargetSelectedObject : public CPacket
{
public:
    CPacketTargetSelectedObject(int useObjectSerial, int targetObjectSerial);
};

class CPacketToggleGargoyleFlying : public CPacket
{
public:
    CPacketToggleGargoyleFlying();
};

class CPacketCustomHouseDataReq : public CPacket
{
public:
    CPacketCustomHouseDataReq(int serial);
};

class CPacketStunReq : public CPacket
{
public:
    CPacketStunReq();
};

class CPacketDisarmReq : public CPacket
{
public:
    CPacketDisarmReq();
};

class CPacketResend : public CPacket
{
public:
    CPacketResend();
};

class CPacketWalkRequest : public CPacket
{
public:
    CPacketWalkRequest(u8 direction, u8 sequence, int fastWalkKey);
};

class CPacketCustomHouseBackup : public CPacket
{
public:
    CPacketCustomHouseBackup();
};

class CPacketCustomHouseRestore : public CPacket
{
public:
    CPacketCustomHouseRestore();
};

class CPacketCustomHouseCommit : public CPacket
{
public:
    CPacketCustomHouseCommit();
};

class CPacketCustomHouseBuildingExit : public CPacket
{
public:
    CPacketCustomHouseBuildingExit();
};

class CPacketCustomHouseGoToFloor : public CPacket
{
public:
    CPacketCustomHouseGoToFloor(u8 floor);
};

class CPacketCustomHouseSync : public CPacket
{
public:
    CPacketCustomHouseSync();
};

class CPacketCustomHouseClear : public CPacket
{
public:
    CPacketCustomHouseClear();
};

class CPacketCustomHouseRevert : public CPacket
{
public:
    CPacketCustomHouseRevert();
};

class CPacketCustomHouseResponse : public CPacket
{
public:
    CPacketCustomHouseResponse();
};

class CPacketCustomHouseAddItem : public CPacket
{
public:
    CPacketCustomHouseAddItem(u16 graphic, int x, int y);
};

class CPacketCustomHouseDeleteItem : public CPacket
{
public:
    CPacketCustomHouseDeleteItem(u16 graphic, int x, int y, int z);
};

class CPacketCustomHouseAddRoof : public CPacket
{
public:
    CPacketCustomHouseAddRoof(u16 graphic, int x, int y, int z);
};

class CPacketCustomHouseDeleteRoof : public CPacket
{
public:
    CPacketCustomHouseDeleteRoof(u16 graphic, int x, int y, int z);
};

class CPacketCustomHouseAddStair : public CPacket
{
public:
    CPacketCustomHouseAddStair(u16 graphic, int x, int y);
};

class CPacketOrionVersion : public CPacket
{
public:
    CPacketOrionVersion(int version);
};
