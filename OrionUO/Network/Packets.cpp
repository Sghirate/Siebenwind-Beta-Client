#include "Packets.h"
#include "Core/StringUtils.h"
#include "GameVars.h"
#include "Globals.h"
#include "Config.h"
#include "OrionUO.h"
#include "CityList.h"
#include "ServerList.h"
#include "Profession.h"
#include "CharacterList.h"
#include "ScreenStages/MainScreen.h"
#include "ScreenStages/SelectTownScreen.h"
#include "Managers/PacketManager.h"
#include "Managers/ProfessionManager.h"
#include "Managers/CreateCharacterManager.h"
#include "Managers/ConnectionManager.h"
#include "Managers/SpeechManager.h"
#include "Managers/ConfigManager.h"
#include "GUI/BaseGUI.h"
#include "Gumps/GumpGeneric.h"
#include "Gumps/GumpShop.h"
#include "Gumps/GumpBook.h"
#include "Gumps/GumpTextEntryDialog.h"
#include "Gumps/GumpSecureTrading.h"

CPacket::CPacket(size_t size, bool autoResize)
    : Core::StreamWriter(size, autoResize)
{
}

void CPacket::Send()
{
    if (static_cast<unsigned int>(!m_buffer.empty()) != 0u)
    {
        g_Orion.Send(m_buffer);
    }
}

CPacketFirstLogin::CPacketFirstLogin()
    : CPacket(62)
{
    WriteLE<u8>(0x80);

    if (g_Config.TheAbyss)
    {
        m_buffer[61] = 0xFF;
    }
    else
    {
        WriteString(g_MainScreen.m_Account->c_str(), 30, false);
        WriteString(g_MainScreen.m_Password->c_str(), 30, false);
        WriteLE<u8>(0xFF);
    }
}

CPacketSelectServer::CPacketSelectServer(u8 index)
    : CPacket(3)
{
    WriteLE<u8>(0xA0);
    WriteLE<u8>(0x00);
    WriteLE<u8>(index);
}

CPacketSecondLogin::CPacketSecondLogin()
    : CPacket(65)
{
    WriteLE<u8>(0x91);
    WriteLE(g_GameSeed, 4);
    WriteString(g_MainScreen.m_Account->c_str(), 30, false);

    int passLen = 30;

    if (g_Config.TheAbyss)
    {
        WriteBE<u16>(0xFF07);
        passLen = 28;
    }

    WriteString(g_MainScreen.m_Password->c_str(), passLen, false);
}

CPacketCreateCharacter::CPacketCreateCharacter(const std::string& name)
    : CPacket(104)
{
    int skillsCount = 3;
    u32 packetID    = 0x00;

    if (GameVars::GetClientVersion() >= CV_70160)
    {
        skillsCount++;
        Resize(106, true);
        packetID = 0xF8;
    }

    WriteLE<u8>(packetID);
    WriteBE<u32>(0xEDEDEDED);
    WriteBE<u16>(0xFFFF);
    WriteBE<u16>(0xFFFF);
    WriteLE<u8>(0x00);
    WriteString(name, 30, false);

    //Move(30); //На самом деле, клиент пихает сюда пароль на 30 байт, но по какой-то причине (мб мусор в памяти) - идет то что идет
    WriteBE<u16>(0x0000); //?

    u32 clientFlag = 0;
    for (int i = 0; i < g_Config.ClientFlag; i++)
    {
        clientFlag |= (1 << i);
    }

    WriteBE<u32>(clientFlag); //clientflag
    WriteBE<u32>(0x00000001); //?
    WriteBE<u32>(0x00000000); //logincount

    CProfession* profession = (CProfession*)g_ProfessionManager.Selected;
    u8 val                  = (u8)profession->DescriptionIndex;
    WriteLE<u8>(val); //profession
    Move(15);         //?

    if (GameVars::GetClientVersion() < CV_4011D)
    {
        val = (u8)g_CreateCharacterManager.GetFemale();
    }
    else
    {
        val = (u8)g_CreateCharacterManager.GetRace();

        if (GameVars::GetClientVersion() < CV_7000)
        {
            val--;
        }

        val = (val * 2) + (u8)g_CreateCharacterManager.GetFemale();
    }

    WriteLE<u8>(val);
    val = profession->Str;
    WriteLE<u8>(val);
    val = profession->Dex;
    WriteLE<u8>(val);
    val = profession->Int;
    WriteLE<u8>(val);

    for (int i = 0; i < skillsCount; i++)
    {
        val = profession->GetSkillIndex((int)i);
        if (val == 0xFF)
        {
            //error, skill is not selected
            WriteBE<u16>(0x0000);
        }
        else
        {
            WriteLE<u8>(val);
            WriteLE<u8>(profession->GetSkillValue((int)i));
        }
    }

    WriteBE<u16>(g_CreateCharacterManager.SkinTone);
    WriteBE<u16>(g_CreateCharacterManager.GetHair(g_CreateCharacterManager.HairStyle).GraphicID);
    WriteBE<u16>(g_CreateCharacterManager.HairColor);
    WriteBE<u16>(g_CreateCharacterManager.GetBeard(g_CreateCharacterManager.BeardStyle).GraphicID);
    WriteBE<u16>(g_CreateCharacterManager.BeardColor);

    if (GameVars::GetClientVersion() >= CV_70160)
    {
        u16 location = g_SelectTownScreen.m_City->LocationIndex;

        WriteBE<u16>(location); //location
        WriteBE<u16>(0x0000);   //?

        u16 slot = 0xFFFF;
        for (int i = 0; i < g_CharacterList.Count; i++)
        {
            if (g_CharacterList.GetName(i).length() == 0u)
            {
                slot = (u16)i;
                break;
            }
        }

        WriteBE<u16>(slot);
    }
    else
    {
        CServer* server = g_ServerList.GetSelectedServer();
        u8 serverIndex  = 0;

        if (server != nullptr)
        {
            serverIndex = (u8)server->Index;
        }

        WriteLE<u8>(serverIndex); //server index

        u8 location = g_SelectTownScreen.m_City->LocationIndex;
        if (GameVars::GetClientVersion() < CV_70130)
        {
            location--;
        }
        WriteLE<u8>(location); //location

        u32 slot = 0xFFFFFFFF;
        for (int i = 0; i < g_CharacterList.Count; i++)
        {
            if (g_CharacterList.GetName(i).length() == 0u)
            {
                slot = (u32)i;
                break;
            }
        }

        WriteBE<u32>(slot);
    }
    WriteBE(g_ConnectionManager.GetClientIP(), 4);
    WriteBE<u16>(g_CreateCharacterManager.ShirtColor);
    WriteBE<u16>(g_CreateCharacterManager.PantsColor);
}

CPacketDeleteCharacter::CPacketDeleteCharacter(int charIndex)
    : CPacket(39)
{
    WriteLE<u8>(0x83);
    Move(30); //character password
    WriteBE<u32>(charIndex);
    WriteBE(g_ConnectionManager.GetClientIP(), 4);
}

CPacketSelectCharacter::CPacketSelectCharacter(int index, const std::string& name)
    : CPacket(73)
{
    int copyLen = (int)name.length();

    if (copyLen > 30)
    {
        copyLen = 30;
    }

    memcpy(&g_SelectedCharName[0], name.c_str(), copyLen);

    WriteLE<u8>(0x5D);
    WriteBE<u32>(0xEDEDEDED);
    WriteString(name, 30, false);
    Move(2);

    u32 clientFlag = 0;

    for (int i = 0; i < g_Config.ClientFlag; i++)
    {
        clientFlag |= (1 << i);
    }

    WriteBE<u32>(clientFlag);

    Move(24);
    WriteBE<u32>(index);
    WriteBE(g_ConnectionManager.GetClientIP(), 4);
}

CPacketPickupRequest::CPacketPickupRequest(u32 serial, u16 count)
    : CPacket(7)
{
    WriteLE<u8>(0x07);
    WriteBE<u32>(serial);
    WriteBE<u16>(count);
}

CPacketDropRequestOld::CPacketDropRequestOld(u32 serial, u16 x, u16 y, char z, u32 container)
    : CPacket(14)
{
    WriteLE<u8>(0x08);
    WriteBE<u32>(serial);
    WriteBE<u16>(x);
    WriteBE<u16>(y);
    WriteLE<u8>(z);
    WriteBE<u32>(container);
}

CPacketDropRequestNew::CPacketDropRequestNew(
    u32 serial, u16 x, u16 y, char z, u8 slot, u32 container)
    : CPacket(15)
{
    WriteLE<u8>(0x08);
    WriteBE<u32>(serial);
    WriteBE<u16>(x);
    WriteBE<u16>(y);
    WriteLE<u8>(z);
    WriteLE<u8>(slot);
    WriteBE<u32>(container);
}

CPacketEquipRequest::CPacketEquipRequest(u32 serial, u8 layer, u32 container)
    : CPacket(10)
{
    WriteLE<u8>(0x13);
    WriteBE<u32>(serial);
    WriteLE<u8>(layer);
    WriteBE<u32>(container);
}

CPacketChangeWarmode::CPacketChangeWarmode(u8 state)
    : CPacket(5)
{
    WriteLE<u8>(0x72);
    WriteLE<u8>(state);
    WriteBE<u16>(0x0032);
}

CPacketHelpRequest::CPacketHelpRequest()
    : CPacket(258)
{
    WriteLE<u8>(0x9B);
}

CPacketStatusRequest::CPacketStatusRequest(u32 serial)
    : CPacket(10)
{
    WriteLE<u8>(0x34);
    WriteBE<u32>(0xEDEDEDED);
    WriteLE<u8>(4);
    WriteBE<u32>(serial);
}

CPacketSkillsRequest::CPacketSkillsRequest(u32 serial)
    : CPacket(10)
{
    WriteLE<u8>(0x34);
    WriteBE<u32>(0xEDEDEDED);
    WriteLE<u8>(5);
    WriteBE<u32>(serial);
}

CPacketSkillsStatusChangeRequest::CPacketSkillsStatusChangeRequest(u8 skill, u8 state)
    : CPacket(6)
{
    WriteLE<u8>(0x3A);
    WriteBE<u16>(0x0006);
    WriteBE<u16>((u16)skill);
    WriteLE<u8>(state);
}

CPacketClickRequest::CPacketClickRequest(u32 serial)
    : CPacket(5)
{
    WriteLE<u8>(0x09);
    WriteBE<u32>(serial);
}

CPacketDoubleClickRequest::CPacketDoubleClickRequest(u32 serial)
    : CPacket(5)
{
    WriteLE<u8>(0x06);
    WriteBE<u32>(serial);
}

CPacketAttackRequest::CPacketAttackRequest(u32 serial)
    : CPacket(5)
{
    WriteLE<u8>(0x05);
    WriteBE<u32>(serial);
}

CPacketClientVersion::CPacketClientVersion(const std::string& version)
    : CPacket(4 + version.length())
{
    WriteLE<u8>(0xBD);
    WriteBE<u16>(4 + (u16)version.length());
    WriteString(version, (int)version.length(), false);
}

CPacketASCIISpeechRequest::CPacketASCIISpeechRequest(
    const char* text, SPEECH_TYPE type, u16 font, u16 color)
    : CPacket(1)
{
    size_t len  = strlen(text);
    size_t size = 8 + len + 1;
    Resize(size, true);

    WriteLE<u8>(0x03);
    WriteBE<u16>((u16)size);
    WriteLE<u8>(type);
    WriteBE<u16>(color);
    WriteBE<u16>(font);
    WriteString(text, len, false);
}

CPacketUnicodeSpeechRequest::CPacketUnicodeSpeechRequest(
    const wchar_t* text, SPEECH_TYPE type, u16 font, u16 color, u8* language)
    : CPacket(1)
{
    size_t len  = lstrlenW(text);
    size_t size = 12;

    u8 typeValue = (u8)type;

    std::vector<u32> codes;
    g_SpeechManager.GetKeywords(text, codes);

    //encoded
    bool encoded = !codes.empty();
    std::string utf8string{};
    std::vector<u8> codeBytes;
    if (encoded)
    {
        typeValue |= ST_ENCODED_COMMAND;
        utf8string = Core::EncodeUTF8(std::wstring(text));
        len        = (int)utf8string.length();
        size += len;
        size += 1; //null terminator

        int length = (int)codes.size();
        codeBytes.push_back(length >> 4);
        int num3  = length & 15;
        bool flag = false;
        int index = 0;

        while (index < length)
        {
            int keywordID = codes[index];

            if (flag)
            {
                codeBytes.push_back(keywordID >> 4);
                num3 = keywordID & 15;
            }
            else
            {
                codeBytes.push_back(((num3 << 4) | ((keywordID >> 8) & 15)));
                codeBytes.push_back(keywordID);
            }

            index++;
            flag = !flag;
        }

        if (!flag)
        {
            codeBytes.push_back(num3 << 4);
        }
        size += codeBytes.size();
    }
    else
    {
        size += len * 2;
        size += 2; //null terminator
    }

    Resize(size, true);

    WriteLE<u8>(0xAD);
    WriteBE<u16>((u16)size);
    WriteLE<u8>(typeValue);
    WriteBE<u16>(color);
    WriteBE<u16>(font);
    WriteLE(language, 4);

    //Sallos aka PlayUO algorithm
    if (encoded)
    {
        for (int i = 0; i < (int)codeBytes.size(); i++)
        {
            WriteLE<u8>(codeBytes[i]);
        }
        WriteString(utf8string, len, true);
        // в данном случае надо посылать как utf8, так читает сервер.
    }
    else
    {
        WriteWString(text, len, true, true);
    }
}

CPacketCastSpell::CPacketCastSpell(int index)
    : CPacket(1)
{
    if (GameVars::GetClientVersion() >= CV_60142)
    {
        Resize(9, true);

        WriteLE<u8>(0xBF);
        WriteBE<u16>(0x0009);
        WriteBE<u16>(0x001C);
        WriteBE<u16>(0x0002);
        WriteBE<u16>(index);
    }
    else
    {
        char spell[10] = { 0 };
        sprintf_s(spell, "%i", index);

        size_t len  = strlen(spell);
        size_t size = 5 + len;
        Resize(size, true);

        WriteLE<u8>(0x12);
        WriteBE<u16>((u16)size);
        WriteLE<u8>(0x56);
        WriteString(spell, len, false);
    }
}

CPacketCastSpellFromBook::CPacketCastSpellFromBook(int index, u32 serial)
    : CPacket(1)
{
    char spell[25] = { 0 };
    sprintf_s(spell, "%i %d", index, (int)serial);

    size_t len  = strlen(spell);
    size_t size = 5 + len;
    Resize(size, true);

    WriteLE<u8>(0x12);
    WriteBE<u16>((u16)size);
    WriteLE<u8>(0x27);
    WriteString(spell, len, false);
}

CPacketUseSkill::CPacketUseSkill(int index)
    : CPacket(10)
{
    char skill[10] = { 0 };
    sprintf_s(skill, "%d 0", index);

    size_t len  = strlen(skill);
    size_t size = 5 + len;
    Resize(size, true);

    WriteLE<u8>(0x12);
    WriteBE<u16>((u16)size);
    WriteLE<u8>(0x24);
    WriteString(skill, len, false);
}

CPacketOpenDoor::CPacketOpenDoor()
    : CPacket(5)
{
    WriteLE<u8>(0x12);
    WriteBE<u16>(0x0005);
    WriteLE<u8>(0x58);
}

CPacketOpenSpellbook::CPacketOpenSpellbook(SPELLBOOK_TYPE type)
    : CPacket(6)
{
    WriteLE<u8>(0x12);
    WriteBE<u16>(0x0006);
    WriteLE<u8>(0x43);
    WriteLE<u8>(type + 30);
}

CPacketEmoteAction::CPacketEmoteAction(const char* action)
    : CPacket(1)
{
    size_t len  = strlen(action);
    size_t size = 5 + len;
    Resize(size, true);

    WriteLE<u8>(0x12);
    WriteBE<u16>((u16)size);
    WriteLE<u8>(0xC7);
    WriteString(action, len, false);
}

CPacketGumpResponse::CPacketGumpResponse(CGumpGeneric* gump, int code)
    : CPacket(1)
{
    int switchesCount   = 0;
    int textLinesCount  = 0;
    int textLinesLength = 0;

    QFOR(item, gump->m_Items, CBaseGUI*)
    {
        switch (item->Type)
        {
            case GOT_CHECKBOX:
            case GOT_RADIO:
            {
                if (((CGUICheckbox*)item)->Checked)
                {
                    switchesCount++;
                }

                break;
            }
            case GOT_TEXTENTRY:
            {
                CGUITextEntry* gte = (CGUITextEntry*)item;

                textLinesCount++;
                textLinesLength += ((int)gte->m_Entry.Length() * 2);

                break;
            }
            default: break;
        }
    }

    size_t size = 19 + (switchesCount * 4) + 4 + ((textLinesCount * 4) + textLinesLength);
    Resize(size, true);

    g_PacketManager.SetCachedGumpCoords(gump->ID, gump->GetX(), gump->GetY());

    WriteLE<u8>(0xB1);
    WriteBE<u16>((u16)size);
    WriteBE<u32>(gump->Serial);
    WriteBE<u32>(gump->MasterGump != 0u ? gump->MasterGump : gump->ID);
    WriteBE<u32>(code);
    WriteBE<u32>(switchesCount);

    QFOR(item, gump->m_Items, CBaseGUI*)
    {
        if (item->Type == GOT_CHECKBOX || item->Type == GOT_RADIO)
        {
            if (((CGUICheckbox*)item)->Checked)
            {
                WriteBE<u32>(((CGUICheckbox*)item)->Serial);
            }
        }
    }

    WriteBE<u32>(textLinesCount);

    QFOR(item, gump->m_Items, CBaseGUI*)
    {
        if (item->Type == GOT_TEXTENTRY)
        {
            CGUITextEntry* entry = (CGUITextEntry*)item;

            WriteBE<u16>(entry->Serial - 1);
            size_t len =
                (((entry->m_Entry.Length()) < (MAX_TEXTENTRY_LENGTH)) ? (entry->m_Entry.Length()) :
                                                                        (MAX_TEXTENTRY_LENGTH));
            WriteBE<u16>((u16)len);
            WriteWString(entry->m_Entry.Data(), len, true, false);
        }
    }
}

CPacketVirtueGumpResponse::CPacketVirtueGumpResponse(CGump* gump, int code)
    : CPacket(15)
{
    g_PacketManager.SetCachedGumpCoords(gump->ID, gump->GetX(), gump->GetY());

    WriteLE<u8>(0xB1);
    WriteBE<u16>(0x000F);
    WriteBE<u32>(gump->Serial);
    WriteBE<u32>(0x000001CD);
    WriteBE<u32>(code);
}

CPacketMenuResponse::CPacketMenuResponse(CGump* gump, int code)
    : CPacket(13)
{
    WriteLE<u8>(0x7D);
    WriteBE<u32>(gump->Serial);
    WriteBE<u16>(gump->ID);

    if (code != 0)
    {
        WriteBE<u16>(code);

        QFOR(item, gump->m_Items, CBaseGUI*)
        {
            if (item->Serial == code && item->Type == GOT_MENUOBJECT)
            {
                WriteBE<u16>(item->Graphic);
                WriteBE<u16>(item->Color);

                break;
            }
        }
    }
}

CPacketGrayMenuResponse::CPacketGrayMenuResponse(CGump* gump, int code)
    : CPacket(13)
{
    WriteLE<u8>(0x7D);
    WriteBE<u32>(gump->Serial);
    WriteBE<u16>(gump->ID);
    WriteBE<u16>(code);
}

CPacketTradeResponse::CPacketTradeResponse(CGumpSecureTrading* gump, int code)
    : CPacket(17)
{
    WriteLE<u8>(0x6F);
    WriteBE<u16>(17);

    if (code == 1) //Закрываем окно
    {
        WriteLE<u8>(0x01);
        WriteBE<u32>(gump->ID);
    }
    else if (code == 2) //Ткнули на чекбокс
    {
        WriteLE<u8>(0x02);
        WriteBE<u32>(gump->ID);
        WriteBE<u32>(static_cast<int>(gump->StateMy));
    }
}

CPacketLogoutNotification::CPacketLogoutNotification()
    : CPacket(5)
{
    WriteLE<u8>(0x01);
    WriteBE<u32>(0xFFFFFFFF);
}

CPacketTextEntryDialogResponse::CPacketTextEntryDialogResponse(
    CGumpTextEntryDialog* gump, CEntryText* entry, bool code)
    : CPacket(1)
{
    size_t len  = entry->Length();
    size_t size = 12 + len + 1;
    Resize(size, true);

    WriteLE<u8>(0xAC);
    WriteBE<u16>((u16)size);
    WriteBE<u32>(gump->Serial);
    WriteLE<u8>(gump->ButtonID);
    WriteLE<u8>(0);
    WriteLE<u8>(code ? 0x01 : 0x00);

    WriteBE<u16>((u16)len + 1);

    WriteString(entry->c_str(), len);
}

CPacketRenameRequest::CPacketRenameRequest(u32 serial, const std::string& newName)
    : CPacket(35)
{
    WriteLE<u8>(0x75);
    WriteBE<u32>(serial);
    WriteString(newName, newName.length(), false);
}

CPacketTipRequest::CPacketTipRequest(u16 id, u8 flag)
    : CPacket(4)
{
    WriteLE<u8>(0xA7);
    WriteBE<u16>(id);
    WriteLE<u8>(flag);
}

CPacketASCIIPromptResponse::CPacketASCIIPromptResponse(const char* text, size_t len, bool cancel)
    : CPacket(1)
{
    size_t size = 15 + len + 1;
    Resize(size, true);

    WriteLE(g_LastASCIIPrompt, 11);
    pack16(&m_buffer[0] + 1, (u16)size);
    WriteBE<u32>((u32)((bool)!cancel));

    WriteString(text, len);
}

CPacketUnicodePromptResponse::CPacketUnicodePromptResponse(
    const wchar_t* text, size_t len, const std::string& lang, bool cancel)
    : CPacket(1)
{
    size_t size = 19 + (len * 2);
    Resize(size, true);

    WriteLE(g_LastUnicodePrompt, 11);
    pack16(&m_buffer[0] + 1, (u16)size);
    WriteBE<u32>((u32)((bool)!cancel));
    WriteString(lang, 4, false);

    WriteWString(text, len, false, false);
}

CPacketDyeDataResponse::CPacketDyeDataResponse(u32 serial, u16 graphic, u16 color)
    : CPacket(9)
{
    WriteLE<u8>(0x95);
    WriteBE<u32>(serial);
    WriteBE<u16>(graphic);
    WriteBE<u16>(color);
}

CPacketProfileRequest::CPacketProfileRequest(u32 serial)
    : CPacket(8)
{
    WriteLE<u8>(0xB8);
    WriteBE<u16>(0x0008);
    WriteLE<u8>(0);
    WriteBE<u32>(serial);
}

CPacketProfileUpdate::CPacketProfileUpdate(u32 serial, const wchar_t* text, size_t len)
    : CPacket(1)
{
    size_t size = 12 + (len * 2);
    Resize(size, true);

    WriteLE<u8>(0xB8);
    WriteBE<u16>((u16)size);
    WriteLE<u8>(1);
    WriteBE<u32>(serial);
    WriteBE<u16>(0x0001);
    WriteBE<u16>((u16)len);
    WriteWString(text, len, true, false);
}

CPacketCloseStatusbarGump::CPacketCloseStatusbarGump(u32 serial)
    : CPacket(9)
{
    WriteLE<u8>(0xBF);
    WriteBE<u16>(0x0009);
    WriteBE<u16>(0x000C);
    WriteBE<u32>(serial);
}

CPacketPartyInviteRequest::CPacketPartyInviteRequest()
    : CPacket(10)
{
    WriteLE<u8>(0xBF);
    WriteBE<u16>(0x000a);
    WriteBE<u16>(0x0006);
    WriteLE<u8>(0x01);
    WriteBE<u32>(0x00000000);
}

CPacketPartyRemoveRequest::CPacketPartyRemoveRequest(u32 serial)
    : CPacket(10)
{
    WriteLE<u8>(0xBF);
    WriteBE<u16>(0x000a);
    WriteBE<u16>(0x0006);
    WriteLE<u8>(0x02);
    WriteBE<u32>(serial);
}

CPacketPartyChangeLootTypeRequest::CPacketPartyChangeLootTypeRequest(u8 type)
    : CPacket(7)
{
    WriteLE<u8>(0xBF);
    WriteBE<u16>(0x0007);
    WriteBE<u16>(0x0006);
    WriteLE<u8>(0x06);
    WriteLE<u8>(type);
}

CPacketPartyAccept::CPacketPartyAccept(u32 serial)
    : CPacket(10)
{
    WriteLE<u8>(0xBF);
    WriteBE<u16>(0x000a);
    WriteBE<u16>(0x0006);
    WriteLE<u8>(0x08);
    WriteBE<u32>(serial);
}

CPacketPartyDecline::CPacketPartyDecline(u32 serial)
    : CPacket(10)
{
    WriteLE<u8>(0xBF);
    WriteBE<u16>(0x000a);
    WriteBE<u16>(0x0006);
    WriteLE<u8>(0x09);
    WriteBE<u32>(serial);
}

CPacketPartyMessage::CPacketPartyMessage(const wchar_t* text, size_t len, u32 serial)
    : CPacket(1)
{
    size_t size = 10 + (len * 2) + 2;
    Resize(size, true);

    WriteLE<u8>(0xBF);
    WriteBE<u16>((u16)size);
    WriteBE<u16>(0x0006);

    if (serial != 0u) //Private message to member
    {
        WriteLE<u8>(0x03);
        WriteBE<u32>(serial);
    }
    else
    { //Message to full party
        WriteLE<u8>(0x04);
    }

    WriteWString(text, len, true, false);
}

CPacketGameWindowSize::CPacketGameWindowSize()
    : CPacket(13)
{
    WriteLE<u8>(0xBF);
    WriteBE<u16>(0x000D);
    WriteBE<u16>(0x0005);
    WriteBE<u32>(g_ConfigManager.GameWindowWidth);
    WriteBE<u32>(g_ConfigManager.GameWindowHeight);
}

CPacketClientViewRange::CPacketClientViewRange(u8 range)
    : CPacket(2)
{
    WriteLE<u8>(0xC8);

    if (range < MIN_VIEW_RANGE)
    {
        range = MIN_VIEW_RANGE;
    }
    else if (range > g_MaxViewRange)
    {
        range = g_MaxViewRange;
    }

    WriteLE<u8>(range);
}

CPacketBulletinBoardRequestMessage::CPacketBulletinBoardRequestMessage(u32 serial, u32 msgSerial)
    : CPacket(12)
{
    WriteLE<u8>(0x71);
    WriteBE<u16>(0x000C);
    WriteLE<u8>(0x03);
    WriteBE<u32>(serial);
    WriteBE<u32>(msgSerial);
}

CPacketBulletinBoardRequestMessageSummary::CPacketBulletinBoardRequestMessageSummary(
    u32 serial, u32 msgSerial)
    : CPacket(12)
{
    WriteLE<u8>(0x71);
    WriteBE<u16>(0x000C);
    WriteLE<u8>(0x04);
    WriteBE<u32>(serial);
    WriteBE<u32>(msgSerial);
}

CPacketBulletinBoardPostMessage::CPacketBulletinBoardPostMessage(
    u32 serial, u32 replySerial, const char* subject, const char* message)
    : CPacket(1)
{
    size_t subjectLen = strlen(subject);
    size_t size       = 14 + subjectLen + 1;

    int lines = 1;

    const auto msgLen = (int)strlen(message);
    int len           = 0;

    for (int i = 0; i < msgLen; i++)
    {
        if (message[i] == '\n')
        {
            len++;

            size += len + 1;
            len = 0;
            lines++;
        }
        else
        {
            len++;
        }
    }

    size += len + 2;

    Resize(size, true);

    WriteLE<u8>(0x71);
    WriteBE<u16>((u16)size);
    WriteLE<u8>(0x05);
    WriteBE<u32>(serial);
    WriteBE<u32>(replySerial);

    WriteLE<u8>((u8)subjectLen + 1);
    WriteString(subject, subjectLen, false);
    WriteLE<u8>(0);

    WriteLE<u8>(lines);
    len          = 0;
    char* msgPtr = (char*)message;

    for (int i = 0; i < msgLen; i++)
    {
        if (msgPtr[len] == '\n')
        {
            len++;

            WriteLE<u8>(len);

            if (len > 1)
            {
                WriteString(msgPtr, len - 1, false);
            }

            WriteLE<u8>(0);

            msgPtr += len;
            len = 0;
        }
        else
        {
            len++;
        }
    }

    WriteLE<u8>(len + 1);
    WriteString(msgPtr, len, false);
    WriteLE<u8>(0);
}

CPacketBulletinBoardRemoveMessage::CPacketBulletinBoardRemoveMessage(u32 serial, u32 msgSerial)
    : CPacket(12)
{
    WriteLE<u8>(0x71);
    WriteBE<u16>(0x000C);
    WriteLE<u8>(0x06);
    WriteBE<u32>(serial);
    WriteBE<u32>(msgSerial);
}

CPacketAssistVersion::CPacketAssistVersion(u32 version, const std::string& clientVersion)
    : CPacket(1)
{
    size_t size = 7 + clientVersion.length() + 1;
    Resize(size, true);

    WriteLE<u8>(0xBE);
    WriteBE<u16>((u16)size);
    WriteBE<u32>(version);
    WriteString(clientVersion, clientVersion.length());
}

CPacketRazorAnswer::CPacketRazorAnswer()
    : CPacket(4)
{
    WriteLE<u8>(0xF0);
    WriteBE<u16>(0x0004);
    WriteLE<u8>(0xFF);
}

CPacketLanguage::CPacketLanguage(const std::string& lang)
    : CPacket(1)
{
    size_t size = 5 + lang.length() + 1;
    Resize(size, true);

    WriteLE<u8>(0xBF);
    WriteBE<u16>(0x0009);
    WriteBE<u16>(0x000B);
    WriteString(lang, lang.length(), false);
}

CPacketClientType::CPacketClientType()
    : CPacket(10)
{
    WriteLE<u8>(0xBF);
    WriteBE<u16>(0x000A);
    WriteBE<u16>(0x000F);
    WriteLE<u8>(0x0A);

    u32 clientFlag = 0;

    for (int i = 0; i < g_Config.ClientFlag; i++)
    {
        clientFlag |= (1 << i);
    }

    WriteBE<u32>(clientFlag);
}

CPacketRequestPopupMenu::CPacketRequestPopupMenu(u32 serial)
    : CPacket(9)
{
    WriteLE<u8>(0xBF);
    WriteBE<u16>(0x0009);
    WriteBE<u16>(0x0013);
    WriteBE<u32>(serial);
}

CPacketPopupMenuSelection::CPacketPopupMenuSelection(u32 serial, u16 menuID)
    : CPacket(11)
{
    WriteLE<u8>(0xBF);
    WriteBE<u16>(0x000B);
    WriteBE<u16>(0x0015);
    WriteBE<u32>(serial);
    WriteBE<u16>(menuID);
}

CPacketOpenChat::CPacketOpenChat(const std::wstring& name)
    : CPacket(64)
{
    WriteLE<u8>(0xB5);

    size_t len = name.length();

    if (len > 0)
    {
        if (len > 30)
        {
            len = 30;
        }

        WriteWString(name, len, false, false);
    }
}

CPacketMapMessage::CPacketMapMessage(u32 serial, MAP_MESSAGE action, u8 pin, short x, short y)
    : CPacket(11)
{
    WriteLE<u8>(0x56);
    WriteBE<u32>(serial);
    WriteLE<u8>(action);
    WriteLE<u8>(pin);
    WriteBE<u16>(x);
    WriteBE<u16>(y);
}

CPacketGuildMenuRequest::CPacketGuildMenuRequest()
    : CPacket(10)
{
    WriteLE<u8>(0xD7);
    WriteBE<u16>(0x000A);
    WriteBE<u32>(g_PlayerSerial);
    WriteBE<u16>(0x0028);
    WriteLE<u8>(0x0A);
}

CPacketQuestMenuRequest::CPacketQuestMenuRequest()
    : CPacket(10)
{
    WriteLE<u8>(0xD7);
    WriteBE<u16>(0x000A);
    WriteBE<u32>(g_PlayerSerial);
    WriteBE<u16>(0x0032);
    WriteLE<u8>(0x0A);
}

CPacketEquipLastWeapon::CPacketEquipLastWeapon()
    : CPacket(10)
{
    WriteLE<u8>(0xD7);
    WriteBE<u16>(0x000A);
    WriteBE<u32>(g_PlayerSerial);
    WriteBE<u16>(0x001E);
    WriteLE<u8>(0x0A);
}

CPacketVirtueRequest::CPacketVirtueRequest(int buttonID)
    : CPacket(23)
{
    WriteLE<u8>(0xB1);
    WriteBE<u16>(0x0017);
    WriteBE<u32>(g_PlayerSerial);
    WriteBE<u32>(0x000001CD);
    WriteBE<u32>(buttonID);
    WriteBE<u32>(0x00000001);
    WriteBE<u32>(g_PlayerSerial);
}

CPacketInvokeVirtueRequest::CPacketInvokeVirtueRequest(u8 id)
    : CPacket(6)
{
    WriteLE<u8>(0x12);
    WriteBE<u16>(0x0006);
    WriteLE<u8>(0xF4);
    WriteLE<u8>(id);
    WriteLE<u8>(0x00);
}

CPacketMegaClilocRequestOld::CPacketMegaClilocRequestOld(int serial)
    : CPacket(9)
{
    WriteLE<u8>(0xBF);
    WriteBE<u16>(9); //size
    WriteBE<u16>(0x0010);
    WriteBE<u32>(serial);
}

CPacketMegaClilocRequest::CPacketMegaClilocRequest(std::vector<u32>& list)
    : CPacket(1)
{
    size_t len = list.size();

    if (len > 50)
    {
        len = 50;
    }

    size_t size = 3 + (len * 4);
    Resize(size, true);

    WriteLE<u8>(0xD6);
    WriteBE<u16>((u16)size);

    for (int i = 0; i < (int)len; i++)
    {
        WriteBE<u32>(list[i]);
    }

    if ((int)list.size() > 50)
    {
        list.erase(list.begin(), list.begin() + 50);
    }
    else
    {
        list.clear();
    }
}

CPacketChangeStatLockStateRequest::CPacketChangeStatLockStateRequest(u8 stat, u8 state)
    : CPacket(7)
{
    WriteLE<u8>(0xBF);
    WriteBE<u16>(0x0007);
    WriteBE<u16>(0x001A);
    WriteLE<u8>(stat);
    WriteLE<u8>(state);
}

CPacketBookHeaderChangeOld::CPacketBookHeaderChangeOld(CGumpBook* gump)
    : CPacket(99)
{
    std::string title  = Core::EncodeUTF8(gump->m_EntryTitle->m_Entry.Data());
    std::string author = Core::EncodeUTF8(gump->m_EntryAuthor->m_Entry.Data());

    WriteLE<u8>(0xD4);
    WriteBE<u32>(gump->Serial);
    WriteBE<u16>((u16)0x0000); //flags
    WriteBE<u16>((u16)gump->PageCount);
    WriteString(gump->m_EntryTitle->m_Entry.GetTextA(), 60);
    WriteString(gump->m_EntryAuthor->m_Entry.GetTextA(), 30);
}

CPacketBookHeaderChange::CPacketBookHeaderChange(CGumpBook* gump)
    : CPacket(1)
{
    std::string title  = Core::EncodeUTF8(gump->m_EntryTitle->m_Entry.Data());
    std::string author = Core::EncodeUTF8(gump->m_EntryAuthor->m_Entry.Data());
    auto titlelen      = (u16)title.length();
    auto authorlen     = (u16)author.length();
    size_t size        = 16 + title.length() + author.length();
    Resize(size, true);

    WriteLE<u8>(0xD4);
    WriteBE<u16>((u16)size);
    WriteBE<u32>(gump->Serial);
    WriteBE<u16>((u16)0x0000); //flags
    WriteBE<u16>((u16)gump->PageCount);
    WriteBE<u16>(titlelen);
    if (titlelen != 0u)
    {
        const char* str = title.c_str();

        for (int i = 0; i < titlelen; i++)
        {
            char ch  = *(str + i);
            *m_ptr++ = ch;
        }
        *m_ptr = 0;
    }
    WriteBE<u16>(authorlen);
    if (authorlen != 0u)
    {
        const char* str = author.c_str();

        for (int i = 0; i < authorlen; i++)
        {
            char ch  = *(str + i);
            *m_ptr++ = ch;
        }
        *m_ptr = 0;
    }
}

CPacketBookPageData::CPacketBookPageData(CGumpBook* gump, int page)
    : CPacket(1)
{
    int lineCount = 0;

    CGUITextEntry* entry = gump->GetEntry(page);

    if (entry != nullptr)
    {
        CEntryText& textEntry = entry->m_Entry;
        std::string data      = Core::EncodeUTF8(textEntry.Data());
        size_t len            = data.length();
        size_t size           = 9 + 4 + 1;

        if (len != 0u)
        {
            size += len;
            const char* str = data.c_str();

            for (int i = 0; i < (int)len; i++)
            {
                if (*(str + i) == '\n')
                {
                    lineCount++;
                }
            }

            if (str[len - 1] != '\n')
            {
                lineCount++;
            }
        }

        Resize(size, true);

        WriteLE<u8>(0x66);
        WriteBE<u16>((u16)size);
        WriteBE<u32>(gump->Serial);
        WriteBE<u16>(0x0001);

        WriteBE<u16>(page);
        WriteBE<u16>(lineCount);

        if (len != 0u)
        {
            const char* str = data.c_str();

            for (int i = 0; i < (int)len; i++)
            {
                char ch = *(str + i);

                if (ch == '\n')
                {
                    ch = 0;
                }

                *m_ptr++ = ch;
            }

            *m_ptr = 0;
        }
    }
}

CPacketBookPageDataRequest::CPacketBookPageDataRequest(int serial, int page)
    : CPacket(13)
{
    WriteLE<u8>(0x66);
    WriteBE<u16>(0x000D);
    WriteBE<u32>(serial);
    WriteBE<u16>(0x0001);
    WriteBE<u16>(page);
    WriteBE<u16>(0xFFFF);
}

CPacketBuyRequest::CPacketBuyRequest(CGumpShop* gump)
    : CPacket(1)
{
    size_t size = 8;
    int count   = 0;

    QFOR(item, gump->m_ItemList[1]->m_Items, CBaseGUI*)
    {
        if (item->Type == GOT_SHOPRESULT)
        {
            size += 7;
            count++;
        }
    }

    Resize(size, true);

    WriteLE<u8>(0x3B);
    WriteBE<u16>((u16)size);
    WriteBE<u32>(gump->Serial);

    if (count != 0)
    {
        WriteLE<u8>(0x02);

        QFOR(item, gump->m_ItemList[1]->m_Items, CBaseGUI*)
        {
            if (item->Type == GOT_SHOPRESULT)
            {
                WriteLE<u8>(0x1A);
                WriteBE<u32>(item->Serial);
                WriteBE<u16>(((CGUIShopResult*)item)->m_MinMaxButtons->Value);
            }
        }
    }
    else
    {
        WriteLE<u8>(0x00);
    }
}

CPacketSellRequest::CPacketSellRequest(CGumpShop* gump)
    : CPacket(1)
{
    size_t size = 9;
    int count   = 0;

    QFOR(item, gump->m_ItemList[1]->m_Items, CBaseGUI*)
    {
        if (item->Type == GOT_SHOPRESULT)
        {
            size += 6;
            count++;
        }
    }

    Resize(size, true);

    WriteLE<u8>(0x9F);
    WriteBE<u16>((u16)size);
    WriteBE<u32>(gump->Serial);
    WriteBE<u16>(count);

    QFOR(item, gump->m_ItemList[1]->m_Items, CBaseGUI*)
    {
        if (item->Type == GOT_SHOPRESULT)
        {
            WriteBE<u32>(item->Serial);
            WriteBE<u16>(((CGUIShopResult*)item)->m_MinMaxButtons->Value);
        }
    }
}

CPacketUseCombatAbility::CPacketUseCombatAbility(u8 index)
    : CPacket(15)
{
    WriteLE<u8>(0xD7);
    WriteBE<u16>(0x000F);
    WriteBE<u32>(g_PlayerSerial);
    WriteBE<u16>(0x0019);
    WriteBE<u32>(0x00000000);
    WriteLE<u8>(index);
    WriteLE<u8>(0x0A);
}

CPacketTargetSelectedObject::CPacketTargetSelectedObject(
    int useObjectSerial, int targetObjectSerial)
    : CPacket(13)
{
    WriteLE<u8>(0xBF);
    WriteBE<u16>(0x000D);
    WriteBE<u16>(0x002C);
    WriteBE<u32>(useObjectSerial);
    WriteBE<u32>(targetObjectSerial);
}

CPacketToggleGargoyleFlying::CPacketToggleGargoyleFlying()
    : CPacket(11)
{
    WriteLE<u8>(0xBF);
    WriteBE<u16>(0x000B);
    WriteBE<u16>(0x0032);
    WriteBE<u16>(0x0001);
    WriteBE<u32>(0x00000000);
}

CPacketCustomHouseDataReq::CPacketCustomHouseDataReq(int serial)
    : CPacket(9)
{
    WriteLE<u8>(0xBF);
    WriteBE<u16>(0x09);
    WriteBE<u16>(0x1E);
    WriteBE<u32>(serial);
}

CPacketStunReq::CPacketStunReq()
    : CPacket(5)
{
    WriteLE<u8>(0xBF);
    WriteBE<u16>(0x05);
    WriteBE<u16>(0x09);
}

CPacketDisarmReq::CPacketDisarmReq()
    : CPacket(5)
{
    WriteLE<u8>(0xBF);
    WriteBE<u16>(0x05);
    WriteBE<u16>(0x0A);
}

CPacketResend::CPacketResend()
    : CPacket(3)
{
    WriteLE<u8>(0x22);
}

CPacketWalkRequest::CPacketWalkRequest(u8 direction, u8 sequence, int fastWalkKey)
    : CPacket(7)
{
    WriteLE<u8>(0x02);
    WriteLE<u8>(direction);
    WriteLE<u8>(sequence);
    WriteBE<u32>(fastWalkKey);
}

CPacketCustomHouseBackup::CPacketCustomHouseBackup()
    : CPacket(10)
{
    WriteLE<u8>(0xD7);
    WriteBE<u16>(0x000A);
    WriteBE<u32>(g_PlayerSerial);
    WriteBE<u16>(0x0002);
    WriteLE<u8>(0x0A);
}

CPacketCustomHouseRestore::CPacketCustomHouseRestore()
    : CPacket(10)
{
    WriteLE<u8>(0xD7);
    WriteBE<u16>(0x000A);
    WriteBE<u32>(g_PlayerSerial);
    WriteBE<u16>(0x0003);
    WriteLE<u8>(0x0A);
}

CPacketCustomHouseCommit::CPacketCustomHouseCommit()
    : CPacket(10)
{
    WriteLE<u8>(0xD7);
    WriteBE<u16>(0x000A);
    WriteBE<u32>(g_PlayerSerial);
    WriteBE<u16>(0x0004);
    WriteLE<u8>(0x0A);
}

CPacketCustomHouseBuildingExit::CPacketCustomHouseBuildingExit()
    : CPacket(10)
{
    WriteLE<u8>(0xD7);
    WriteBE<u16>(0x000A);
    WriteBE<u32>(g_PlayerSerial);
    WriteBE<u16>(0x000C);
    WriteLE<u8>(0x0A);
}

CPacketCustomHouseGoToFloor::CPacketCustomHouseGoToFloor(u8 floor)
    : CPacket(15)
{
    WriteLE<u8>(0xD7);
    WriteBE<u16>(0x000F);
    WriteBE<u32>(g_PlayerSerial);
    WriteBE<u16>(0x0012);
    WriteBE<u32>(0);
    WriteLE<u8>(floor);
    WriteLE<u8>(0x0A);
}

CPacketCustomHouseSync::CPacketCustomHouseSync()
    : CPacket(10)
{
    WriteLE<u8>(0xD7);
    WriteBE<u16>(0x000A);
    WriteBE<u32>(g_PlayerSerial);
    WriteBE<u16>(0x000E);
    WriteLE<u8>(0x0A);
}

CPacketCustomHouseClear::CPacketCustomHouseClear()
    : CPacket(10)
{
    WriteLE<u8>(0xD7);
    WriteBE<u16>(0x000A);
    WriteBE<u32>(g_PlayerSerial);
    WriteBE<u16>(0x0010);
    WriteLE<u8>(0x0A);
}

CPacketCustomHouseRevert::CPacketCustomHouseRevert()
    : CPacket(10)
{
    WriteLE<u8>(0xD7);
    WriteBE<u16>(0x000A);
    WriteBE<u32>(g_PlayerSerial);
    WriteBE<u16>(0x001A);
    WriteLE<u8>(0x0A);
}

CPacketCustomHouseResponse::CPacketCustomHouseResponse()
    : CPacket(10)
{
    WriteLE<u8>(0xD7);
    WriteBE<u16>(0x000A);
    WriteBE<u32>(g_PlayerSerial);
    WriteBE<u16>(0x000A);
    WriteLE<u8>(0x0A);
}

CPacketCustomHouseAddItem::CPacketCustomHouseAddItem(u16 graphic, int x, int y)
    : CPacket(25)
{
    WriteLE<u8>(0xD7);
    WriteBE<u16>(0x0019);
    WriteBE<u32>(g_PlayerSerial);
    WriteBE<u16>(0x0006);
    WriteLE<u8>(0x00);
    WriteBE<u32>(graphic);
    WriteLE<u8>(0x00);
    WriteBE<u32>(x);
    WriteLE<u8>(0x00);
    WriteBE<u32>(y);
    WriteLE<u8>(0x0A);
}

CPacketCustomHouseDeleteItem::CPacketCustomHouseDeleteItem(u16 graphic, int x, int y, int z)
    : CPacket(30)
{
    WriteLE<u8>(0xD7);
    WriteBE<u16>(0x001E);
    WriteBE<u32>(g_PlayerSerial);
    WriteBE<u16>(0x0005);
    WriteLE<u8>(0x00);
    WriteBE<u32>(graphic);
    WriteLE<u8>(0x00);
    WriteBE<u32>(x);
    WriteLE<u8>(0x00);
    WriteBE<u32>(y);
    WriteLE<u8>(0x00);
    WriteBE<u32>(z);
    WriteLE<u8>(0x0A);
}

CPacketCustomHouseAddRoof::CPacketCustomHouseAddRoof(u16 graphic, int x, int y, int z)
    : CPacket(30)
{
    WriteLE<u8>(0xD7);
    WriteBE<u16>(0x001E);
    WriteBE<u32>(g_PlayerSerial);
    WriteBE<u16>(0x0013);
    WriteLE<u8>(0x00);
    WriteBE<u32>(graphic);
    WriteLE<u8>(0x00);
    WriteBE<u32>(x);
    WriteLE<u8>(0x00);
    WriteBE<u32>(y);
    WriteLE<u8>(0x00);
    WriteBE<u32>(z);
    WriteLE<u8>(0x0A);
}

CPacketCustomHouseDeleteRoof::CPacketCustomHouseDeleteRoof(u16 graphic, int x, int y, int z)
    : CPacket(30)
{
    WriteLE<u8>(0xD7);
    WriteBE<u16>(0x001E);
    WriteBE<u32>(g_PlayerSerial);
    WriteBE<u16>(0x0014);
    WriteLE<u8>(0x00);
    WriteBE<u32>(graphic);
    WriteLE<u8>(0x00);
    WriteBE<u32>(x);
    WriteLE<u8>(0x00);
    WriteBE<u32>(y);
    WriteLE<u8>(0x00);
    WriteBE<u32>(z);
    WriteLE<u8>(0x0A);
}

CPacketCustomHouseAddStair::CPacketCustomHouseAddStair(u16 graphic, int x, int y)
    : CPacket(25)
{
    WriteLE<u8>(0xD7);
    WriteBE<u16>(0x0019);
    WriteBE<u32>(g_PlayerSerial);
    WriteBE<u16>(0x000D);
    WriteLE<u8>(0x00);
    WriteBE<u32>(graphic);
    WriteLE<u8>(0x00);
    WriteBE<u32>(x);
    WriteLE<u8>(0x00);
    WriteBE<u32>(y);
    WriteLE<u8>(0x0A);
}

CPacketOrionVersion::CPacketOrionVersion(int version)
    : CPacket(9)
{
    WriteLE<u8>(0xFC);
    WriteBE<u16>(9);
    WriteBE<u16>(OCT_ORION_VERSION);
    WriteBE<u32>(version);
}
