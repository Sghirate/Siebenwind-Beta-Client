#include "Macro.h"
#include "DefinitionMacro.h"

MacroObject *g_MacroPointer = nullptr;

MacroObject::MacroObject(const MACRO_CODE& a_code, const MACRO_SUB_CODE& a_subCode)
    : m_code(a_code)
    , m_subCode(a_subCode)
{
    switch (a_code)
    {
        //With sub menu
        case MC_WALK:
        case MC_OPEN:
        case MC_CLOSE:
        case MC_MINIMIZE:
        case MC_MAXIMIZE:
        case MC_USE_SKILL:
        case MC_ARM_DISARM:
        case MC_INVOKE_VIRTUE:
        case MC_CAST_SPELL:
        case MC_SELECT_NEXT:
        case MC_SELECT_PREVIOUS:
        case MC_SELECT_NEAREST:
        {
            if (a_subCode == MSC_NONE)
            {
                int count = 0;
                int offset = 0;
                Macro::GetBoundByCode(a_code, count, offset);
                m_subCode = (MACRO_SUB_CODE)offset;
            }
            m_hasSubMenu = 1;
            break;
        }
        //With entry text
        case MC_SAY:
        case MC_EMOTE:
        case MC_WHISPER:
        case MC_YELL:
        case MC_DELAY:
        case MC_SET_UPDATE_RANGE:
        case MC_MODIFY_UPDATE_RANGE:
        {
            m_hasSubMenu = 2;
            break;
        }
        default:
        {
            m_hasSubMenu = 0;
            break;
        }
    }
}

MacroObject::~MacroObject()
{
}

MacroObjectString::MacroObjectString(const MACRO_CODE& a_code, const MACRO_SUB_CODE& a_subCode, const std::string& a_str)
    : MacroObject(a_code, a_subCode)
    , m_string(a_str)
{
}

MacroObjectString::~MacroObjectString()
{
}

Macro::Macro(Core::EKey a_key, bool a_alt, bool a_ctrl, bool a_shift)
    : m_key(a_key)
    , m_alt(a_alt)
    , m_ctrl(a_ctrl)
    , m_shift(a_shift)
{
}

Macro::~Macro()
{
}

Macro *Macro::CreateBlankMacro()
{
    auto obj = new Macro(Core::EKey::Key_Unknown, false, false, false);
    obj->Add(new MacroObject(MC_NONE, MSC_NONE));
    return obj;
}

MacroObject *Macro::CreateMacro(const MACRO_CODE& a_code)
{
    MacroObject *obj = nullptr;
    switch (a_code)
    {
        //With entry text
        case MC_SAY:
        case MC_EMOTE:
        case MC_WHISPER:
        case MC_YELL:
        case MC_DELAY:
        case MC_SET_UPDATE_RANGE:
        case MC_MODIFY_UPDATE_RANGE:
        {
            obj = new MacroObjectString(a_code, MSC_NONE, "");
            break;
        }
        default:
        {
            obj = new MacroObject(a_code, MSC_NONE);
            break;
        }
    }

    return obj;
}

void Macro::ChangeObject(MacroObject* a_source, MacroObject* a_obj)
{
    a_obj->m_Prev = a_source->m_Prev;
    a_obj->m_Next = a_source->m_Next;
    if (a_source->m_Prev == nullptr)
        m_Items = a_obj;
    else
        a_source->m_Prev->m_Next = a_obj;

    if (a_source->m_Next != nullptr)
        a_source->m_Next->m_Prev = a_obj;
    a_source->m_Prev = nullptr;
    a_source->m_Next = nullptr;
    delete a_source;
}

Macro* Macro::Load(Core::MappedFile& a_file)
{
    u8* next = a_file.GetPtr();
    short size = a_file.ReadLE<i16>();
    next += size;

    i32 key = a_file.ReadLE<i32>();
    bool alt = false;
    if ((key & MODKEY_ALT) != 0)
    {
        key -= MODKEY_ALT;
        alt = true;
    }

    bool ctrl = false;
    if ((key & MODKEY_CTRL) != 0)
    {
        key -= MODKEY_CTRL;
        ctrl = true;
    }

    bool shift = false;
    if ((key & MODKEY_SHIFT) != 0)
    {
        key -= MODKEY_SHIFT;
        shift = true;
    }

    int count = a_file.ReadLE<i16>();
    auto macro = new Macro((Core::EKey)key, alt, ctrl, shift);
    for (int i = 0; i < count; i++)
    {
        auto type = a_file.ReadBE<u8>();
        MACRO_CODE code = (MACRO_CODE)a_file.ReadLE<u16>();
        MACRO_SUB_CODE subCode = (MACRO_SUB_CODE)a_file.ReadLE<u16>();
        MacroObject *obj = nullptr;
        switch (type)
        {
            case 0: //original
            {
                obj = new MacroObject(code, subCode);
                break;
            }
            case 2: //with std::string
            {
                short len = a_file.ReadLE<u16>();
                std::string str = a_file.ReadString(len);
                obj = new MacroObjectString(code, subCode, str);
                break;
            }
            default:
                break;
        }
        if (obj != nullptr)
        {
            macro->Add(obj);
        }
    }
    a_file.SetPtr(next);
    return macro;
}

void Macro::Save(Core::StreamWriter& a_writer)
{
    short size = 12;
    short count = 0;
    for (auto obj = (MacroObject *)m_Items; obj != nullptr; obj = (MacroObject *)obj->m_Next)
    {
        size += 5;
        count++;
        if (obj->HasString()) //with std::string
        {
            const std::string& str = ((MacroObjectString *)obj)->GetString();
            size += (short)str.length() + 3;
        }
    }

    a_writer.WriteLE<u16>(size);
    auto key = (i32)m_key;
    if (m_alt)
        key += MODKEY_ALT;

    if (m_ctrl)
        key += MODKEY_CTRL;

    if (m_shift)
        key += MODKEY_SHIFT;

    a_writer.WriteLE<i32>(key);
    a_writer.WriteLE<u16>(count);
    for (auto obj = (MacroObject*)m_Items; obj != nullptr; obj = (MacroObject*)obj->m_Next)
    {
        u8 type = 0;
        if (obj->HasString())
            type = 2;

        a_writer.WriteLE<u8>(type);
        a_writer.WriteLE<u16>(obj->GetCode());
        a_writer.WriteLE<u16>(obj->GetSubCode());
        if (type == 2) //with std::string
        {
            std::string str = ((MacroObjectString *)obj)->GetString();
            int len = (int)str.length();
            a_writer.WriteLE<i16>(len + 1);
            a_writer.WriteString(str);
        }
    }
    a_writer.WriteLE<u32>(0);
}

Macro* Macro::GetCopy()
{
    Macro*macro = new Macro(m_key, m_alt, m_ctrl, m_shift);
    MACRO_CODE oldCode = MC_NONE;
    for (auto obj = (MacroObject*)m_Items; obj != nullptr; obj = (MacroObject*)obj->m_Next)
    {
        if (obj->HasString())
            macro->Add(new MacroObjectString(obj->GetCode(), obj->GetSubCode(), ((MacroObjectString *)obj)->GetString()));
        else
            macro->Add(new MacroObject(obj->GetCode(), obj->GetSubCode()));
        oldCode = obj->GetCode();
    }

    if (oldCode != MC_NONE)
        macro->Add(new MacroObject(MC_NONE, MSC_NONE));
    return macro;
}

void Macro::GetBoundByCode(const MACRO_CODE& a_code, int& a_count, int& a_offset)
{
    switch (a_code)
    {
        case MC_WALK:
        {
            a_offset = MSC_G1_NW;
            a_count = MSC_G2_CONFIGURATION - MSC_G1_NW;
            break;
        }
        case MC_OPEN:
        case MC_CLOSE:
        case MC_MINIMIZE:
        case MC_MAXIMIZE:
        {
            a_offset = MSC_G2_CONFIGURATION;
            a_count = MSC_G3_ANATOMY - MSC_G2_CONFIGURATION;
            break;
        }
        case MC_USE_SKILL:
        {
            a_offset = MSC_G3_ANATOMY;
            a_count = MSC_G4_LEFT_HAND - MSC_G3_ANATOMY;
            break;
        }
        case MC_ARM_DISARM:
        {
            a_offset = MSC_G4_LEFT_HAND;
            a_count = MSC_G5_HONOR - MSC_G4_LEFT_HAND;
            break;
        }
        case MC_INVOKE_VIRTUE:
        {
            a_offset = MSC_G5_HONOR;
            a_count = MSC_G6_CLUMSY - MSC_G5_HONOR;
            break;
        }
        case MC_CAST_SPELL:
        {
            a_offset = MSC_G6_CLUMSY;
            a_count = MSC_G7_HOSTILE - MSC_G6_CLUMSY;
            break;
        }
        case MC_SELECT_NEXT:
        case MC_SELECT_PREVIOUS:
        case MC_SELECT_NEAREST:
        {
            a_offset = MSC_G7_HOSTILE;
            a_count = MSC_TOTAL_COUNT - MSC_G7_HOSTILE;
            break;
        }
        default:
            break;
    }
}

const char* Macro::s_macroActionName[kMacroActionNamesCount] = { 
    "(NONE)",
    "Say",
    "Emote",
    "Whisper",
    "Yell",
    "Walk",
    "War/Peace",
    "Paste",
    "Open",
    "Close",
    "Minimize",
    "Maximize",
    "OpenDoor",
    "UseSkill",
    "LastSkill",
    "CastSpell",
    "LastSpell",
    "LastObject",
    "Bow",
    "Salute",
    "QuitGame",
    "AllNames",
    "LastTarget",
    "TargetSelf",
    "Arm/Disarm",
    "WaitForTarget",
    "TargetNext",
    "AttackLast",
    "Delay",
    "CircleTrans",
    "CloseGumps",
    "AlwaysRun",
    "SaveDesktop",
    "KillGumpOpen",
    "PrimaryAbility",
    "SecondaryAbility",
    "EquipLastWeapon",
    "SetUpdateRange",
    "ModifyUpdateRange",
    "IncreaseUpdateRange",
    "DecreaseUpdateRange",
    "MaxUpdateRange",
    "MinUpdateRange",
    "DefaultUpdateRange",
    "UpdateRangeInfo",
    "EnableRangeColor",
    "DisableRangeColor",
    "ToggleRangeColor",
    "InvokeVirtue",
    "SelectNext",
    "SelectPrevious",
    "SelectNearest",
    "AttackSelectedTarget",
    "UseSelectedTarget",
    "CurrentTarget",
    "TargetSystemOn/Off",
    "ToggleBuficonWindow",
    "BandageSelf",
    "BandageTarget",
    "ToggleGargoyleFlying",
};
const char* Macro::s_macroAction[kMacroActionsCount] = {
    "?",
    "NW (top)", //Walk group
    "N (topright)",
    "NE (right)",
    "E (bottonright)",
    "SE (bottom)",
    "S (bottomleft)",
    "SW (left)",
    "W (topleft)",
    "Configuration", //Open/Close/Minimize/Maximize group
    "Paperdoll",
    "Status",
    "Journal",
    "Skills",
    "Mage Spellbook",
    "Chat",
    "Backpack",
    "Overview",
    "World Map",
    "Mail",
    "Party Manifest",
    "Party Chat",
    "Necro Spellbook",
    "Paladin Spellbook",
    "Combat Book",
    "Bushido Spellbook",
    "Ninjitsu Spellbook",
    "Guild",
    "Spell Weaving Spellbook",
    "Quest Log",
    "Mysticism Spellbook",
    "Racial Abilities Book",
    "Bard Spellbook",
    "Anatomy", //Skills group
    "Animal Lore",
    "Animal Taming",
    "Arms Lore",
    "Begging",
    "Cartograpy",
    "Detecting Hidden",
    "Enticement",
    "Evaluating Intelligence",
    "Forensic Evaluation",
    "Hiding",
    "Imbuing",
    "Inscription",
    "Item Identification",
    "Meditation",
    "Peacemaking",
    "Poisoning",
    "Provocation",
    "Remove Trap",
    "Spirit Speak",
    "Stealing",
    "Stealth",
    "Taste Identification",
    "Tracking",
    "Left Hand", ///Arm/Disarm group
    "Right Hand",
    "Honor", //Invoke Virtue group
    "Sacrifice",
    "Valor",
    "Clumsy", //Cast Spell group
    "Create Food",
    "Feeblemind",
    "Heal",
    "Magic Arrow",
    "Night Sight",
    "Reactive Armor",
    "Weaken",
    "Agility",
    "Cunning",
    "Cure",
    "Harm",
    "Magic Trap",
    "Magic Untrap",
    "Protection",
    "Strength",
    "Bless",
    "Fireball",
    "Magic Lock",
    "Poison",
    "Telekinesis",
    "Teleport",
    "Unlock",
    "Wall Of Stone",
    "Arch Cure",
    "Arch Protection",
    "Curse",
    "Fire Field",
    "Greater Heal",
    "Lightning",
    "Mana Drain",
    "Recall",
    "Blade Spirits",
    "Dispell Field",
    "Incognito",
    "Magic Reflection",
    "Mind Blast",
    "Paralyze",
    "Poison Field",
    "Summon Creature",
    "Dispel",
    "Energy Bolt",
    "Explosion",
    "Invisibility",
    "Mark",
    "Mass Curse",
    "Paralyze Field",
    "Reveal",
    "Chain Lightning",
    "Energy Field",
    "Flame Strike",
    "Gate Travel",
    "Mana Vampire",
    "Mass Dispel",
    "Meteor Swarm",
    "Polymorph",
    "Earthquake",
    "Energy Vortex",
    "Resurrection",
    "Air Elemental",
    "Summon Daemon",
    "Earth Elemental",
    "Fire Elemental",
    "Water Elemental",
    "Animate Dead",
    "Blood Oath",
    "Corpse Skin",
    "Curse Weapon",
    "Evil Omen",
    "Horrific Beast",
    "Lich Form",
    "Mind Rot",
    "Pain Spike",
    "Poison Strike",
    "Strangle",
    "Summon Familar",
    "Vampiric Embrace",
    "Vengeful Spirit",
    "Wither",
    "Wraith Form",
    "Exorcism",
    "Cleanse By Fire",
    "Close Wounds",
    "Concentrate Weapon",
    "Dispel Evil",
    "Divine Fury",
    "Enemy Of One",
    "Holy Light",
    "Noble Sacrifice",
    "Remove Curse",
    "Sacred Journey",
    "Honorable Execution",
    "Confidence",
    "Evasion",
    "Counter Attack",
    "Lightning Strike",
    "Momentum Strike",
    "Focus Attack",
    "Death Strike",
    "Animal Form",
    "Ki Attack",
    "Surprice Attack",
    "Backstab",
    "Shadowjump",
    "Mirror Image",
    "Arcane Circle",
    "Gift Of Reneval",
    "Immolating Weapon",
    "Attunement",
    "Thunderstorm",
    "Natures Fury",
    "Summon Fey",
    "Summon Fiend",
    "Reaper Form",
    "Wildfire",
    "Essence Of Wind",
    "Dryad Allure",
    "Ethereal Voyage",
    "Word Of Death",
    "Gift Of Life",
    "Arcane Empowerment",
    "Nether Bolt",
    "Healing Stone",
    "Purge Magic",
    "Enchant",
    "Sleep",
    "Eagle Strike",
    "Animated Weapon",
    "Stone Form",
    "Spell Trigger",
    "Mass Sleep",
    "Cleansing Winds",
    "Bombard",
    "Spell Plague",
    "Hail Storm",
    "Nether Cyclone",
    "Rising Colossus",
    "Inspire",
    "Invigorate",
    "Resilience",
    "Perseverance",
    "Tribulation",
    "Despair",
    "Hostile", //Select Next/Previous/Nearest group
    "Party",
    "Follower",
    "Object",
    "Mobile",
};
