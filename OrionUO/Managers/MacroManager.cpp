#include "MacroManager.h"
#include "Macro.h"
#include "Core/DataStream.h"
#include "Core/File.h"
#include "Core/MappedFile.h"
#include "Core/StringUtils.h"
#include "Core/TextFileParser.h"
#include "GameObjects/GameItem.h"
#include "GameObjects/GamePlayer.h"
#include "GameObjects/GameWorld.h"
#include "GameObjects/ObjectOnCursor.h"
#include "Gumps/GumpAbility.h"
#include "Gumps/GumpSpellbook.h"
#include "Gumps/GumpTargetSystem.h"
#include "Managers/ConfigManager.h"
#include "Managers/GumpManager.h"
#include "Managers/OptionsMacroManager.h"
#include "Managers/PacketManager.h"
#include "Network/Packets.h"
#include "plugin/enumlist.h"
#include "OrionUO.h"
#include "OrionWindow.h"
#include "Target.h"
#include "TargetGump.h"
#include "TextEngine/EntryText.h"
#include "Walker/PathFinder.h"
#include <SDL_clipboard.h>

MacroManager g_MacroManager;

u8 MacroManager::m_SkillIndexTable[24] = { 1,  2,  35, 4,  6,  12,
                                           14, 15, 16, 19, 21, 0xFF /*imbuing*/,
                                           23, 3,  46, 9,  30, 22,
                                           48, 32, 33, 47, 36, 38 };

MacroManager::MacroManager()
{
}

MacroManager::~MacroManager()
{
}

Keycode MacroManager::ConvertStringToKeyCode(const std::vector<std::string>& a_strings)
{
    std::string str = a_strings[0];

    for (int i = 1; i < (int)a_strings.size() - 3; i++)
        str += " " + a_strings[i];

    Keycode key = 0;

    if (str.length() == 1)
    {
        key = *str.c_str();
    }
    else if (str.length() != 0u)
    {
        str = Core::ToUpperA(str);

        if (str == "ESC")
        {
            key = KEY_ESCAPE;
        }
        else if (str == "BACKSPACE")
        {
            key = KEY_BACK;
        }
        else if (str == "TAB")
        {
            key = KEY_TAB;
        }
        else if (str == "ENTER")
        {
            key = KEY_RETURN;
        }
        else if (str == "CTRL")
        {
            key = KEY_CONTROL;
        }
        else if (str == "ALT")
        {
            key = KEY_MENU;
        }
        else if (str == "SHIFT")
        {
            key = KEY_SHIFT;
        }
        else if (str == "SPACE")
        {
            key = KEY_SPACE;
        }
        else if (str == "CAPS LOCK")
        {
            key = KEY_CAPITAL;
        }
        else if (str == "F1")
        {
            key = KEY_F1;
        }
        else if (str == "F2")
        {
            key = KEY_F2;
        }
        else if (str == "F3")
        {
            key = KEY_F3;
        }
        else if (str == "F4")
        {
            key = KEY_F4;
        }
        else if (str == "F5")
        {
            key = KEY_F5;
        }
        else if (str == "F6")
        {
            key = KEY_F6;
        }
        else if (str == "F7")
        {
            key = KEY_F7;
        }
        else if (str == "F8")
        {
            key = KEY_F8;
        }
        else if (str == "F9")
        {
            key = KEY_F9;
        }
        else if (str == "F10")
        {
            key = KEY_F10;
        }
        else if (str == "F11")
        {
            key = KEY_F11;
        }
        else if (str == "F12")
        {
            key = KEY_F12;
        }
        else if (str == "PAUSE")
        {
            key = KEY_PAUSE;
        }
        else if (str == "SCROLL LOCK")
        {
            key = KEY_SCROLL;
        }
        else if (str == "NUM 0")
        {
            key = KEY_NUMPAD0;
        }
        else if (str == "NUM 1")
        {
            key = KEY_NUMPAD1;
        }
        else if (str == "NUM 2")
        {
            key = KEY_NUMPAD2;
        }
        else if (str == "NUM 3")
        {
            key = KEY_NUMPAD3;
        }
        else if (str == "NUM 4")
        {
            key = KEY_NUMPAD4;
        }
        else if (str == "NUM 5")
        {
            key = KEY_NUMPAD5;
        }
        else if (str == "NUM 6")
        {
            key = KEY_NUMPAD6;
        }
        else if (str == "NUM 7")
        {
            key = KEY_NUMPAD7;
        }
        else if (str == "NUM 8")
        {
            key = KEY_NUMPAD8;
        }
        else if (str == "NUM 9")
        {
            key = KEY_NUMPAD9;
        }
        else if (str == "NUM *")
        {
            key = 42;
        }
        else if (str == "NUM -")
        {
            key = 45;
        }
        else if (str == "NUM +")
        {
            key = 43;
        }
        else if (str == "NUM DEL")
        {
            key = 46;
        }
    }

    return key;
}

bool MacroManager::Convert(const std::filesystem::path& a_path)
{
    Core::TextFileParser file(a_path, "", "", "");
    Core::TextFileParser unicodeParser({}, " ", "", "");

    const int MACRO_POSITION_ALT   = 2;
    const int MACRO_POSITION_CTRL  = 3;
    const int MACRO_POSITION_SHIFT = 1;

    while (!file.IsEOF())
    {
        std::vector<std::string> strings = file.ReadTokens();
        strings     = unicodeParser.GetTokens(file.GetRawLine().c_str(), false);
        size_t size = strings.size();

        if (size == 0u)
        {
            continue;
        }

        if (size < 4 || size > 5)
        {
            LOG("Error! Macros converter: unexpected start args count = %zi", size);
            continue;
        }

        //TPRINT("Key: %s [alt=%i ctrl=%i shift=%i]\n", strings[0].c_str(), atoi(strings[MACRO_ALT_POSITION].c_str()), atoi(strings[MACRO_CTRL_POSITION].c_str()), atoi(strings[MACRO_SHIFT_POSITION].c_str()));
        bool macroAdded = false;

        Macro* macro = new Macro(
            ConvertStringToKeyCode(strings),
            atoi(strings[size - MACRO_POSITION_ALT].c_str()) != 0,
            atoi(strings[size - MACRO_POSITION_CTRL].c_str()) != 0,
            atoi(strings[size - MACRO_POSITION_SHIFT].c_str()) != 0);

        std::string TestLine{};
        while (!file.IsEOF())
        {
            std::vector<std::string> datas = file.ReadTokens();
            TestLine.append(file.GetRawLine());
            if ((*file.GetRawLine().c_str() != '\n') && (*file.GetRawLine().c_str() != '\r') &&
                (!file.GetRawLine().empty()) && (*file.GetRawLine().c_str() != '#'))
            {
                continue;
            }
            std::vector<std::string> data = unicodeParser.GetTokens(TestLine.c_str(), false);
            TestLine                      = "";
            if (data.empty())
            {
                continue;
            }
            if (*data[0].c_str() == '#')
            {
                macroAdded = true;
                Add(macro);
                break;
            }
            if (*data[0].c_str() == '+')
            {
                std::string raw = data[0].c_str() + 1;
                data[0]         = raw;
            }

            std::string upData = ToUpperA(data[0]);
            MACRO_CODE code    = MC_NONE;

            for (int i = 0; i < Macro::kMacroActionNamesCount; i++)
            {
                if (upData == Core::ToUpperA(Macro::GetActionName(i)))
                {
                    code = (MACRO_CODE)i;
                    //LOG_F(INFO, "Action found (%i): %s", i, Macro::m_MacroActionName[i]);
                    break;
                }
            }

            if (code != MC_NONE)
            {
                MacroObject* obj = Macro::CreateMacro(code);
                if (obj->HasString()) //Аргументы - строка
                {
                    if (data.size() > 1)
                    {
                        std::string args = data[1];
                        for (int i = 2; i < (int)data.size(); i++)
                        {
                            args += " " + data[i];
                        }

                        //LOG_F(INFO, "\tSub action std::string is: %s", args.c_str());

                        ((MacroObjectString*)obj)->SetString(args);
                    }
                }
                else if (data.size() > 1) //Аргументы - код (значение), либо просто код макроса
                {
                    upData = data[1];

                    for (int i = 2; i < (int)data.size(); i++)
                    {
                        upData += " " + data[i];
                    }

                    upData = ToUpperA(upData);

                    for (int i = 0; i < Macro::kMacroActionsCount; i++)
                    {
                        if (upData == Core::ToUpperA(Macro::GetAction(i)))
                        {
                            obj->SetSubCode((MACRO_SUB_CODE)i);

                            //LOG_F(INFO, "\tSub action found (%i): %s", i, Macro::m_MacroAction[i]);

                            break;
                        }
                    }
                }
                macro->Add(obj);
            }
        }

        //LOG_F(INFO, "Cycle ends with add: %i", macroAdded);

        if (!macroAdded)
        {
            Add(macro);
        }
    }
    return std::filesystem::exists(a_path);
}

bool MacroManager::Load(
    const std::filesystem::path& a_path, const std::filesystem::path& a_originalPath)
{
    bool result = false;
    Clear();

    Core::MappedFile file;
    if (file.Load(a_path) && (file.GetSize() != 0u))
    {
        u8 version = file.ReadLE<u8>();

        short count = file.ReadLE<i16>();

        for (int i = 0; i < count; i++)
            Add(Macro::Load(file));

        result = true;
    }
    else
    {
        result = Convert(a_originalPath);
    }

    file.Unload();

    return result;
}

void MacroManager::Save(const std::filesystem::path& a_path)
{
    Core::StreamWriter writer;
    writer.WriteLE((u8)0); //verison

    short count = GetItemsCount();
    writer.WriteLE((i16)count);

    QFOR(obj, m_Items, Macro*)
    obj->Save(writer);

    writer.WriteLE((u32)0); //EOF

    Core::File file(a_path, "wb");
    file.Write(writer.GetBuffer(), writer.GetSize(), 1);
}

Macro* MacroManager::FindMacro(Keycode a_key, bool a_alt, bool a_ctrl, bool a_shift)
{
    Macro* obj = (Macro*)m_Items;
    while (obj != nullptr)
    {
        if (obj->GetKey() == a_key && obj->GetAlt() == a_alt && obj->GetCtrl() == a_ctrl &&
            obj->GetShift() == a_shift)
            break;
        obj = (Macro*)obj->m_Next;
    }
    return obj;
}

void MacroManager::LoadFromOptions()
{
    Clear();
    ChangePointer(nullptr);
    QFOR(obj, g_OptionsMacroManager.m_Items, Macro*) { Add(obj->GetCopy()); }
}

void MacroManager::ChangePointer(MacroObject* macro)
{
    g_MacroPointer = macro;

    if (g_MacroPointer == nullptr && SendNotificationToPlugin)
    {
        SendNotificationToPlugin = false;
        //PLUGIN_EVENT(UOMSG_END_MACRO_PLAYING, nullptr);
    }
}

void MacroManager::Execute()
{
    while (g_MacroPointer != nullptr)
    {
        switch (Process())
        {
            case MRC_STOP:
            {
                ChangePointer(nullptr);
                return;
            }
            case MRC_BREAK_PARSER:
            {
                return;
            }
            case MRC_PARSE_NEXT:
            {
                ChangePointer((MacroObject*)g_MacroPointer->m_Next);
                break;
            }
            default: break;
        }
    }
}

void MacroManager::ProcessSubMenu()
{
    switch (g_MacroPointer->GetCode())
    {
        case MC_OPEN:
        {
            switch (g_MacroPointer->GetSubCode())
            {
                case MSC_G2_CONFIGURATION:
                {
                    g_Orion.OpenConfiguration();
                    break;
                }
                case MSC_G2_PAPERDOLL:
                {
                    g_Orion.PaperdollReq(g_PlayerSerial);
                    break;
                }
                case MSC_G2_STATUS:
                {
                    g_Orion.OpenStatus(g_PlayerSerial);
                    break;
                }
                case MSC_G2_JOURNAL:
                {
                    g_Orion.OpenJournal();
                    break;
                }
                case MSC_G2_SKILLS:
                {
                    g_Orion.OpenSkills();
                    break;
                }
                case MSC_G2_MAGE_SPELLBOOK:
                case MSC_G2_NECRO_SPELLBOOK:
                case MSC_G2_PALADIN_SPELLBOOK:
                case MSC_G2_BUSHIDO_SPELLBOOK:
                case MSC_G2_NINJITSU_SPELLBOOK:
                case MSC_G2_SPELL_WEAVING_SPELLBOOK:
                case MSC_G2_MYSTICISM_SPELLBOOK:
                {
                    SPELLBOOK_TYPE type = ST_MAGE;

                    switch (g_MacroPointer->GetSubCode())
                    {
                        case MSC_G2_NECRO_SPELLBOOK:
                        {
                            type = ST_NECRO;
                            break;
                        }
                        case MSC_G2_PALADIN_SPELLBOOK:
                        {
                            type = ST_PALADIN;
                            break;
                        }
                        case MSC_G2_BUSHIDO_SPELLBOOK:
                        {
                            type = ST_BUSHIDO;
                            break;
                        }
                        case MSC_G2_NINJITSU_SPELLBOOK:
                        {
                            type = ST_NINJITSU;
                            break;
                        }
                        case MSC_G2_SPELL_WEAVING_SPELLBOOK:
                        {
                            type = ST_SPELL_WEAVING;
                            break;
                        }
                        case MSC_G2_MYSTICISM_SPELLBOOK:
                        {
                            type = ST_MYSTICISM;
                            break;
                        }
                        default: break;
                    }

                    CPacketOpenSpellbook(type).Send();

                    break;
                }
                case MSC_G2_CHAT:
                {
                    break;
                }
                case MSC_G2_BACKPACK:
                {
                    CGameItem* backpack = g_Player->FindLayer(OL_BACKPACK);
                    if (backpack != nullptr)
                    {
                        g_Orion.DoubleClick(backpack->Serial);
                    }
                    break;
                }
                case MSC_G2_OWERVIEW:
                {
                    g_Orion.OpenMinimap();
                    break;
                }
                case MSC_G2_WORLD_MAP:
                {
                    g_Orion.OpenWorldMap();
                    break;
                }
                case MSC_G2_MAIL:
                case MSC_G2_PARTY_MANIFEST:
                {
                    g_Orion.OpenPartyManifest();
                    break;
                }
                case MSC_G2_GUILD:
                {
                    CPacketGuildMenuRequest().Send();
                    break;
                }
                case MSC_G2_QUEST_LOG:
                {
                    CPacketQuestMenuRequest().Send();
                    break;
                }
                case MSC_G2_PARTY_CHAT:
                case MSC_G2_COMBAT_BOOK:
                case MSC_G2_RACIAL_ABILITIES_BOOK:
                case MSC_G2_BARD_SPELLBOOK:
                {
                    break;
                }
                default: break;
            }

            break;
        }
        case MC_CLOSE:
        case MC_MINIMIZE:
        case MC_MAXIMIZE:
        {
            CGump* gump = nullptr;

            switch (g_MacroPointer->GetSubCode())
            {
                case MSC_G2_CONFIGURATION:
                {
                    gump = g_GumpManager.GetGump(0, 0, GT_OPTIONS);
                    g_OptionsMacroManager.Clear();
                    break;
                }
                case MSC_G2_PAPERDOLL:
                {
                    gump = g_GumpManager.GetGump(g_PlayerSerial, 0, GT_PAPERDOLL);
                    break;
                }
                case MSC_G2_STATUS:
                {
                    gump = g_GumpManager.GetGump(g_PlayerSerial, 0, GT_STATUSBAR);
                    break;
                }
                case MSC_G2_JOURNAL:
                {
                    gump = g_GumpManager.GetGump(0, 0, GT_JOURNAL);
                    break;
                }
                case MSC_G2_SKILLS:
                {
                    gump = g_GumpManager.GetGump(0, 0, GT_SKILLS);
                    break;
                }
                case MSC_G2_MAGE_SPELLBOOK:
                case MSC_G2_NECRO_SPELLBOOK:
                case MSC_G2_PALADIN_SPELLBOOK:
                case MSC_G2_BUSHIDO_SPELLBOOK:
                case MSC_G2_NINJITSU_SPELLBOOK:
                case MSC_G2_SPELL_WEAVING_SPELLBOOK:
                case MSC_G2_MYSTICISM_SPELLBOOK:
                {
                    //gump = g_GumpManager.GetGump(0, 0, GT_SPELLBOOK);

                    QFOR(item, g_GumpManager.m_Items, CGump*)
                    {
                        if (item->GumpType == GT_SPELLBOOK)
                        {
                            CGameItem* gi = g_World->FindWorldItem(item->Serial);
                            if (gi != nullptr)
                            {
                                switch (g_MacroPointer->GetSubCode())
                                {
                                    case MSC_G2_MAGE_SPELLBOOK:
                                    {
                                        if (gi->Graphic == 0x0EFA)
                                        {
                                            gump = item;
                                        }
                                        break;
                                    }
                                    case MSC_G2_NECRO_SPELLBOOK:
                                    {
                                        if (gi->Graphic == 0x2253)
                                        {
                                            gump = item;
                                        }
                                        break;
                                    }
                                    case MSC_G2_PALADIN_SPELLBOOK:
                                    {
                                        if (gi->Graphic == 0x2252)
                                        {
                                            gump = item;
                                        }
                                        break;
                                    }
                                    case MSC_G2_BUSHIDO_SPELLBOOK:
                                    {
                                        if (gi->Graphic == 0x238C)
                                        {
                                            gump = item;
                                        }
                                        break;
                                    }
                                    case MSC_G2_NINJITSU_SPELLBOOK:
                                    {
                                        if (gi->Graphic == 0x23A0)
                                        {
                                            gump = item;
                                        }
                                        break;
                                    }
                                    case MSC_G2_SPELL_WEAVING_SPELLBOOK:
                                    {
                                        if (gi->Graphic == 0x2D50)
                                        {
                                            gump = item;
                                        }
                                        break;
                                    }
                                    case MSC_G2_MYSTICISM_SPELLBOOK:
                                    {
                                        if (gi->Graphic == 0)
                                        { //??????????
                                            gump = item;
                                        }
                                        break;
                                    }
                                    default: break;
                                }
                            }

                            if (gump != nullptr)
                            {
                                break;
                            }
                        }
                    }
                    break;
                }
                case MSC_G2_CHAT:
                {
                    gump = g_GumpManager.GetGump(0, 0, GT_CHAT);
                    break;
                }
                case MSC_G2_BACKPACK:
                {
                    CGameItem* backpack = g_Player->FindLayer(OL_BACKPACK);
                    if (backpack != nullptr)
                    {
                        g_GumpManager.CloseGump(backpack->Serial, 0, GT_SPELLBOOK);
                    }
                    break;
                }
                case MSC_G2_OWERVIEW:
                {
                    gump = g_GumpManager.GetGump(0, 0, GT_MINIMAP);
                    break;
                }
                case MSC_G2_MAIL:
                {
                    gump = g_GumpManager.GetGump(0, 0, GT_MAIL);
                    break;
                }
                case MSC_G2_PARTY_MANIFEST:
                {
                    gump = g_GumpManager.GetGump(0, 0, GT_PARTY_MANIFEST);
                    break;
                }
                case MSC_G2_PARTY_CHAT:
                case MSC_G2_COMBAT_BOOK:
                case MSC_G2_GUILD:
                case MSC_G2_QUEST_LOG:
                case MSC_G2_RACIAL_ABILITIES_BOOK:
                case MSC_G2_BARD_SPELLBOOK:
                {
                    break;
                }
                default: break;
            }

            if (gump != nullptr)
            {
                if (g_MacroPointer->GetCode() == MC_CLOSE)
                {
                    g_GumpManager.RemoveGump(gump);
                }
                else
                {
                    gump->Minimized  = (g_MacroPointer->GetCode() == MC_MINIMIZE);
                    gump->WantRedraw = true;
                }
            }

            break;
        }
        default: break;
    }
}

MACRO_RETURN_CODE MacroManager::Process()
{
    MACRO_RETURN_CODE result = MRC_PARSE_NEXT;
    if (g_MacroPointer == nullptr)
    {
        result = MRC_STOP;
    }
    else if (m_NextTimer <= g_Ticks)
    {
        result = Process(g_MacroPointer);
    }
    else
    {
        result = MRC_BREAK_PARSER;
    }
    return result;
}

MACRO_RETURN_CODE MacroManager::Process(MacroObject* macro)
{
    MACRO_RETURN_CODE result = MRC_PARSE_NEXT;
    static int itemInHand[2] = { 0, 0 };

    switch (macro->GetCode())
    {
        case MC_SAY:
        case MC_EMOTE:
        case MC_WHISPER:
        case MC_YELL:
        {
            MacroObjectString* mos = (MacroObjectString*)macro;

            if (mos->GetString().length() != 0u)
            {
                SPEECH_TYPE st = ST_NORMAL;

                switch (macro->GetCode())
                {
                    case MC_EMOTE:
                    {
                        st = ST_EMOTE;
                        break;
                    }
                    case MC_WHISPER:
                    {
                        st = ST_WHISPER;
                        break;
                    }
                    case MC_YELL:
                    {
                        st = ST_YELL;
                        break;
                    }
                    default: break;
                }

                // Always send unicode speech requests to the Siebenwind Server.
                CPacketUnicodeSpeechRequest(
                    ToWString(mos->GetString()).c_str(),
                    st,
                    3,
                    g_ConfigManager.SpeechColor,
                    (u8*)g_Language.c_str())
                    .Send();
            }

            break;
        }
        case MC_WALK:
        {
            u8 dt = (u8)DT_NW;

            if (macro->GetSubCode() != MSC_G1_NW)
            {
                dt = (u8)(macro->GetCode() - 2);

                if (dt > 7)
                {
                    dt = 0;
                }
            }

            if (!g_PathFinder.AutoWalking)
            {
                g_PathFinder.Walk(false, dt);
            }

            break;
        }
        case MC_WAR_PEACE:
        {
            g_Orion.ChangeWarmode();

            break;
        }
        case MC_PASTE:
        {
            if (g_EntryPointer != nullptr)
            {
#if defined(ORION_WINDOWS)
                // FIXME: move clipboard access to wisp window
                if (OpenClipboard(g_OrionWindow.Handle))
                {
                    HANDLE cb = GetClipboardData(CF_TEXT);

                    if (cb != nullptr)
                    {
                        char* chBuffer = (char*)GlobalLock(cb);

                        if (chBuffer != nullptr && (strlen(chBuffer) != 0u))
                        {
                            std::wstring str = g_EntryPointer->Data() + ToWString(chBuffer);
                            g_EntryPointer->SetTextW(str);
                        }

                        GlobalUnlock(cb);
                    }

                    CloseClipboard();
                }
#else
                auto chBuffer = SDL_GetClipboardText();
                if (chBuffer != nullptr && (strlen(chBuffer) != 0u))
                {
                    std::wstring str = g_EntryPointer->Data() + ToWString(chBuffer);
                    g_EntryPointer->SetTextW(str);
                }
#endif
            }

            break;
        }
        case MC_OPEN:
        case MC_CLOSE:
        case MC_MINIMIZE:
        case MC_MAXIMIZE:
        {
            ProcessSubMenu();
            break;
        }
        case MC_OPEN_DOOR:
        {
            g_Orion.OpenDoor();
            break;
        }
        case MC_USE_SKILL:
        {
            int skill = (macro->GetSubCode() - MSC_G3_ANATOMY);
            if (skill >= 0 && skill < 24)
            {
                skill = m_SkillIndexTable[skill];
                if (skill != 0xFF)
                {
                    g_Orion.UseSkill(skill);
                }
            }
            break;
        }
        case MC_LAST_SKILL:
        {
            g_Orion.UseSkill(g_LastSkillIndex);
            break;
        }
        case MC_CAST_SPELL:
        {
            int spell = (macro->GetSubCode() - MSC_G6_CLUMSY + 1);
            if (spell > 0 && spell <= 151)
            {
                const int spellsCountTable[7] = { CGumpSpellbook::SPELLBOOK_1_SPELLS_COUNT,
                                                  CGumpSpellbook::SPELLBOOK_2_SPELLS_COUNT,
                                                  CGumpSpellbook::SPELLBOOK_3_SPELLS_COUNT,
                                                  CGumpSpellbook::SPELLBOOK_4_SPELLS_COUNT,
                                                  CGumpSpellbook::SPELLBOOK_5_SPELLS_COUNT,
                                                  CGumpSpellbook::SPELLBOOK_6_SPELLS_COUNT,
                                                  CGumpSpellbook::SPELLBOOK_7_SPELLS_COUNT };

                int totalCount = 0;
                int spellType  = 0;
                for (spellType = 0; spellType < 7; spellType++)
                {
                    totalCount += spellsCountTable[spellType];
                    if (spell < totalCount)
                    {
                        break;
                    }
                }

                if (spellType < 7)
                {
                    spell += spellType * 100;
                    if (spellType > 2)
                    {
                        spell += 100;
                    }
                    g_Orion.CastSpell(spell);
                }
            }
            break;
        }
        case MC_LAST_SPELL:
        {
            g_Orion.CastSpell(g_LastSpellIndex);
            break;
        }
        case MC_LAST_OBJECT:
        {
            if (g_World->FindWorldObject(g_LastUseObject) != nullptr)
            {
                g_Orion.DoubleClick(g_LastUseObject);
            }
            break;
        }
        case MC_BOW:
        case MC_SALUTE:
        {
            const char* emote[2] = { "bow", "salute" };
            int index            = macro->GetCode() - MC_BOW;
            g_Orion.EmoteAction(emote[index]);
            break;
        }
        case MC_QUIT_GAME:
        {
            g_Orion.OpenLogOut();
            break;
        }
        case MC_ALL_NAMES:
        {
            g_Orion.AllNames();
            break;
        }
        case MC_LAST_TARGET:
        {
            if (WaitForTargetTimer == 0)
            {
                WaitForTargetTimer = g_Ticks + WAIT_FOR_TARGET_DELAY;
            }

            if (g_Target.IsTargeting())
            {
                if (g_Target.Type != 0u)
                {
                    g_Target.SendLastTarget();
                }
                else
                {
                    g_Target.SendTargetObject(g_LastTargetObject);
                }

                WaitForTargetTimer = 0;
            }
            else if (WaitForTargetTimer < g_Ticks)
            {
                WaitForTargetTimer = 0;
            }
            else
            {
                result = MRC_BREAK_PARSER;
            }

            break;
        }
        case MC_TARGET_SELF:
        {
            if (WaitForTargetTimer == 0)
            {
                WaitForTargetTimer = g_Ticks + WAIT_FOR_TARGET_DELAY;
            }

            if (g_Target.IsTargeting())
            {
                g_Target.SendTargetObject(g_PlayerSerial);
                WaitForTargetTimer = 0;
            }
            else if (WaitForTargetTimer < g_Ticks)
            {
                WaitForTargetTimer = 0;
            }
            else
            {
                result = MRC_BREAK_PARSER;
            }

            break;
        }
        case MC_ARM_DISARM:
        {
            int handIndex = 1 - (macro->GetSubCode() - MSC_G4_LEFT_HAND);
            if (handIndex < 0 || handIndex > 1 || g_ObjectInHand.Enabled)
            {
                break;
            }

            if (itemInHand[handIndex] != 0u)
            {
                CGameItem* objHand = g_World->FindWorldItem(itemInHand[handIndex]);
                if (objHand != nullptr)
                {
                    g_Orion.PickupItem(objHand, 1, false);
                    g_Orion.EquipItem(g_PlayerSerial);
                }
                itemInHand[handIndex] = 0;
            }
            else
            {
                u32 backpack           = 0;
                CGameItem* objBackpack = g_Player->FindLayer(OL_BACKPACK);
                if (objBackpack != nullptr)
                {
                    backpack = objBackpack->Serial;
                }

                if (backpack == 0u)
                {
                    break;
                }

                CGameItem* objHand = g_Player->FindLayer(OL_1_HAND + handIndex);
                if (objHand != nullptr)
                {
                    itemInHand[handIndex] = objHand->Serial;
                    g_Orion.PickupItem(objHand, 1, false);
                    g_Orion.DropItem(backpack, 0xFFFF, 0xFFFF, 0);
                    g_GumpManager.UpdateGump(g_PlayerSerial, 0, GT_PAPERDOLL);
                }
            }
            break;
        }
        case MC_WAIT_FOR_TARGET:
        {
            if (WaitForTargetTimer == 0)
            {
                WaitForTargetTimer = g_Ticks + WAIT_FOR_TARGET_DELAY;
            }

            if (g_Target.IsTargeting() || WaitForTargetTimer < g_Ticks)
            {
                WaitForTargetTimer = 0;
            }
            else
            {
                result = MRC_BREAK_PARSER;
            }
            break;
        }
        case MC_TARGET_NEXT:
        {
            CGameObject* obj =
                g_World->SearchWorldObject(g_LastTargetObject, 30, STO_MOBILES, SMO_NEXT);

            if (obj != nullptr)
            {
                if (obj->NPC && (((CGameCharacter*)obj)->MaxHits == 0))
                {
                    CPacketStatusRequest(obj->Serial).Send();
                }

                g_LastTargetObject = obj->Serial;
                g_LastAttackObject = obj->Serial;
                //PLUGIN_EVENT(UOMSG_STATUS_REQUEST, obj->Serial);
            }
            break;
        }
        case MC_ATTACK_LAST:
        {
            g_Orion.Attack(g_LastAttackObject);
            break;
        }
        case MC_DELAY:
        {
            MacroObjectString* mos = (MacroObjectString*)macro;
            std::string str        = mos->GetString();
            if (str.length() != 0u)
            {
                m_NextTimer = g_Ticks + std::atoi(str.c_str());
            }
            break;
        }
        case MC_CIRCLE_TRANS:
        {
            g_ConfigManager.UseCircleTrans = !g_ConfigManager.UseCircleTrans;
            break;
        }
        case MC_CLOSE_GUMPS:
        {
            CGump* gump = (CGump*)g_GumpManager.m_Items;
            while (gump != nullptr)
            {
                CGump* next = (CGump*)gump->m_Next;
                if (gump->GumpType == GT_OPTIONS)
                {
                    g_OptionsMacroManager.Clear();
                }

                if (gump->GumpType != GT_MENUBAR && gump->GumpType != GT_BUFF &&
                    (gump->GumpType != GT_GENERIC && !gump->NoClose))
                {
                    g_GumpManager.RemoveGump(gump);
                }
                gump = next;
            }
            break;
        }
        case MC_ALWAYS_RUN:
        {
            g_ConfigManager.AlwaysRun = !g_ConfigManager.AlwaysRun;
            break;
        }
        case MC_SAVE_DESKTOP:
        {
            g_Orion.SaveLocalConfig(g_PacketManager.ConfigSerial);
            break;
        }
        case MC_ENABLE_RANGE_COLOR:
        {
            g_ConfigManager.GrayOutOfRangeObjects = true;
            break;
        }
        case MC_DISABLE_RANGE_COLOR:
        {
            g_ConfigManager.GrayOutOfRangeObjects = false;
            break;
        }
        case MC_TOGGLE_RANGE_COLOR:
        {
            g_ConfigManager.GrayOutOfRangeObjects = !g_ConfigManager.GrayOutOfRangeObjects;
            break;
        }
        case MC_ATTACK_SELECTED_TARGET:
        {
            if (!g_ConfigManager.DisableNewTargetSystem && (g_NewTargetSystem.Serial != 0u) &&
                g_NewTargetSystem.Serial < 0x40000000)
            {
                g_Orion.Attack(g_NewTargetSystem.Serial);
            }
            break;
        }
        case MC_USE_SELECTED_TARGET:
        {
            if (!g_ConfigManager.DisableNewTargetSystem && (g_NewTargetSystem.Serial != 0u))
            {
                g_Orion.DoubleClick(g_NewTargetSystem.Serial);
            }
            break;
        }
        case MC_CURRENT_TARGET:
        {
            if (!g_ConfigManager.DisableNewTargetSystem && (g_NewTargetSystem.Serial != 0u))
            {
                if (WaitForTargetTimer == 0)
                {
                    WaitForTargetTimer = g_Ticks + WAIT_FOR_TARGET_DELAY;
                }

                if (g_Target.IsTargeting())
                {
                    g_Target.SendTargetObject(g_NewTargetSystem.Serial);

                    WaitForTargetTimer = 0;
                }
                else if (WaitForTargetTimer < g_Ticks)
                {
                    WaitForTargetTimer = 0;
                }
                else
                {
                    result = MRC_BREAK_PARSER;
                }
            }
            break;
        }
        case MC_TARGET_SYSTEM_ON_OFF:
        {
            g_ConfigManager.DisableNewTargetSystem = !g_ConfigManager.DisableNewTargetSystem;
            break;
        }
        case MC_BANDAGE_SELF:
        case MC_BANDAGE_TARGET:
        {
            //На самом деле с 5.0.4a
            if (g_PacketManager.GetClientVersion() < CV_5020)
            {
                if (WaitingBandageTarget)
                {
                    if (WaitForTargetTimer == 0)
                    {
                        WaitForTargetTimer = g_Ticks + 500;
                    }

                    if (g_Target.IsTargeting())
                    {
                        if (macro->GetCode() == MC_BANDAGE_SELF)
                        {
                            g_Target.SendTargetObject(g_PlayerSerial);
                        }
                        else if (
                            !g_ConfigManager.DisableNewTargetSystem &&
                            (g_NewTargetSystem.Serial != 0u))
                        {
                            g_Target.SendTargetObject(g_NewTargetSystem.Serial);
                        }

                        WaitingBandageTarget = false;
                        WaitForTargetTimer   = 0;
                    }
                    else if (WaitForTargetTimer < g_Ticks)
                    {
                        WaitingBandageTarget = false;
                        WaitForTargetTimer   = 0;
                    }
                    else
                    {
                        result = MRC_BREAK_PARSER;
                    }
                }
                else
                {
                    CGameItem* bandage = g_Player->FindBandage();
                    if (bandage != nullptr)
                    {
                        WaitingBandageTarget = true;
                        g_Orion.DoubleClick(bandage->Serial);
                        result = MRC_BREAK_PARSER;
                    }
                }
            }
            else
            {
                CGameItem* bandage = g_Player->FindBandage();
                if (bandage != nullptr)
                {
                    if (macro->GetCode() == MC_BANDAGE_SELF)
                    {
                        CPacketTargetSelectedObject(bandage->Serial, g_PlayerSerial).Send();
                    }
                    else if (
                        !g_ConfigManager.DisableNewTargetSystem &&
                        (g_NewTargetSystem.Serial != 0u) && g_NewTargetSystem.Serial < 0x40000000)
                    {
                        CPacketTargetSelectedObject(bandage->Serial, g_NewTargetSystem.Serial)
                            .Send();
                    }
                }
            }
            break;
        }
        case MC_SET_UPDATE_RANGE:
        case MC_MODIFY_UPDATE_RANGE:
        {
            MacroObjectString* mos = (MacroObjectString*)macro;
            std::string str        = mos->GetString();
            if (str.length() != 0u)
            {
                g_ConfigManager.UpdateRange = std::atoi(str.c_str());
                if (g_ConfigManager.UpdateRange < MIN_VIEW_RANGE)
                {
                    g_ConfigManager.UpdateRange = MIN_VIEW_RANGE;
                }
                else if (g_ConfigManager.UpdateRange > g_MaxViewRange)
                {
                    g_ConfigManager.UpdateRange = g_MaxViewRange;
                }
            }
            break;
        }
        case MC_INCREASE_UPDATE_RANGE:
        {
            g_ConfigManager.UpdateRange += 1;
            if (g_ConfigManager.UpdateRange > g_MaxViewRange)
            {
                g_ConfigManager.UpdateRange = g_MaxViewRange;
            }
            break;
        }
        case MC_DECREASE_UPDATE_RANGE:
        {
            g_ConfigManager.UpdateRange -= 1;
            if (g_ConfigManager.UpdateRange < MIN_VIEW_RANGE)
            {
                g_ConfigManager.UpdateRange = MIN_VIEW_RANGE;
            }
            break;
        }
        case MC_MAX_UPDATE_RANGE:
        {
            g_ConfigManager.UpdateRange = g_MaxViewRange;
            break;
        }
        case MC_MIN_UPDATE_RANGE:
        {
            g_ConfigManager.UpdateRange = MIN_VIEW_RANGE;
            break;
        }
        case MC_DEFAULT_UPDATE_RANGE:
        {
            g_ConfigManager.UpdateRange = g_MaxViewRange;
            break;
        }
        case MC_UPDATE_RANGE_INFO:
        {
            g_Orion.CreateTextMessageF(
                3, 0, "Current update range is %i", g_ConfigManager.UpdateRange);
            break;
        }
        case MC_SELECT_NEXT:
        case MC_SELECT_PREVIOUS:
        case MC_SELECT_NEAREST:
        {
            if (g_ConfigManager.DisableNewTargetSystem)
            {
                break;
            }

            SCAN_TYPE_OBJECT scanType = (SCAN_TYPE_OBJECT)(macro->GetSubCode() - MSC_G7_HOSTILE);
            CGameObject* obj          = g_World->SearchWorldObject(
                g_NewTargetSystem.Serial,
                10,
                scanType,
                (SCAN_MODE_OBJECT)(macro->GetCode() - MC_SELECT_NEXT));

            if (obj != nullptr)
            {
                g_GumpManager.CloseGump(g_NewTargetSystem.Serial, 0, GT_TARGET_SYSTEM);
                g_NewTargetSystem.Serial = obj->Serial;
                if (g_GumpManager.UpdateContent(g_NewTargetSystem.Serial, 0, GT_TARGET_SYSTEM) ==
                    nullptr)
                {
                    if (g_NewTargetSystem.Serial < 0x40000000)
                    {
                        CPacketStatusRequest(g_NewTargetSystem.Serial).Send();
                    }
                    g_GumpManager.AddGump(new CGumpTargetSystem(
                        g_NewTargetSystem.Serial,
                        g_NewTargetSystem.GumpX,
                        g_NewTargetSystem.GumpY));
                }
            }
            else
            {
                const char* resultNames[5] = {
                    "Hostiles", "Party Members", "Followers", "Objects", "Mobiles"
                };

                g_Orion.CreateUnicodeTextMessageF(
                    0, 0x038A, "There are no %s on the screen to select.", resultNames[scanType]);
            }
            break;
        }
        case MC_TOGGLE_BUICON_WINDOW:
        {
            g_ConfigManager.ToggleBufficonWindow = !g_ConfigManager.ToggleBufficonWindow;
            break;
        }
        case MC_INVOKE_VIRTUE:
        {
            u8 id = macro->GetSubCode() - MSC_G5_HONOR + 31;
            CPacketInvokeVirtueRequest(id).Send();
            break;
        }
        case MC_PRIMARY_ABILITY:
        {
            CGumpAbility::OnAbilityUse(0);
            break;
        }
        case MC_SECONDARY_ABILITY:
        {
            CGumpAbility::OnAbilityUse(1);
            break;
        }
        case MC_TOGGLE_GARGOYLE_FLYING:
        {
            if (g_Player->Race == RT_GARGOYLE)
            {
                CPacketToggleGargoyleFlying().Send();
            }
            break;
        }
        case MC_EQUIP_LAST_WEAPON:
        {
            CPacketEquipLastWeapon().Send();
            break;
        }
        case MC_KILL_GUMP_OPEN:
        {
            g_Orion.CreateTextMessage(TT_SYSTEM, 0xFFFFFFFF, 3, 0x77, "That macro is not work now");
            break;
        }
        default: break;
    }

    return result;
}
