#include "PluginPackets.h"
#include "Macro.h"
#include "Managers/SkillsManager.h"
#include "Gumps/GumpSpellbook.h"

CPluginPacket::CPluginPacket()
{
    WriteBE<u8>(0xFC);
    WriteBE<u16>(0); //size reserved
}

CPluginPacket::CPluginPacket(int size, bool autoResize)
    : Core::StreamWriter(size, autoResize)
{
}

void CPluginPacket::SendToPlugin()
{
    if (m_buffer.size() >= 5)
    {
        pack16(&m_buffer[1], (u16)m_buffer.size());
        //g_PluginManager.PacketRecv(&m_buffer[0], (int)m_buffer.size());
    }
}

CPluginPacketSkillsList::CPluginPacketSkillsList()
    : CPluginPacket()
{
    int count = g_SkillsManager.Count;

    WriteBE<u16>(OIPMT_SKILL_LIST);
    WriteBE<u16>(count);

    for (int i = 0; i < count; i++)
    {
        CSkill *skill = g_SkillsManager.Get((u32)i);
        if (skill == nullptr)
            continue;

        WriteBE<u8>(skill->Button);
        WriteString(skill->Name);
    }
}

CPluginPacketSpellsList::CPluginPacketSpellsList()
    : CPluginPacket()
{
    WriteBE<u16>(OIPMT_SPELL_LIST);
    WriteBE<u16>(7);

    WriteBE<u16>(CGumpSpellbook::SPELLBOOK_1_SPELLS_COUNT);
    for (int i = 0; i < CGumpSpellbook::SPELLBOOK_1_SPELLS_COUNT; i++)
        WriteString(CGumpSpellbook::m_SpellName1[i][0]);

    WriteBE<u16>(CGumpSpellbook::SPELLBOOK_2_SPELLS_COUNT);
    for (int i = 0; i < CGumpSpellbook::SPELLBOOK_2_SPELLS_COUNT; i++)
        WriteString(CGumpSpellbook::m_SpellName2[i][0]);

    WriteBE<u16>(CGumpSpellbook::SPELLBOOK_3_SPELLS_COUNT);
    for (int i = 0; i < CGumpSpellbook::SPELLBOOK_3_SPELLS_COUNT; i++)
        WriteString(CGumpSpellbook::m_SpellName3[i][0]);

    WriteBE<u16>(CGumpSpellbook::SPELLBOOK_4_SPELLS_COUNT);
    for (int i = 0; i < CGumpSpellbook::SPELLBOOK_4_SPELLS_COUNT; i++)
        WriteString(CGumpSpellbook::m_SpellName4[i]);

    WriteBE<u16>(CGumpSpellbook::SPELLBOOK_5_SPELLS_COUNT);
    for (int i = 0; i < CGumpSpellbook::SPELLBOOK_5_SPELLS_COUNT; i++)
        WriteString(CGumpSpellbook::m_SpellName5[i]);

    WriteBE<u16>(CGumpSpellbook::SPELLBOOK_6_SPELLS_COUNT);
    for (int i = 0; i < CGumpSpellbook::SPELLBOOK_6_SPELLS_COUNT; i++)
        WriteString(CGumpSpellbook::m_SpellName6[i][0]);

    WriteBE<u16>(CGumpSpellbook::SPELLBOOK_7_SPELLS_COUNT);
    for (int i = 0; i < CGumpSpellbook::SPELLBOOK_7_SPELLS_COUNT; i++)
        WriteString(CGumpSpellbook::m_SpellName7[i][0]);
}

CPluginPacketMacrosList::CPluginPacketMacrosList()
    : CPluginPacket()
{
    WriteBE<u16>(OIPMT_MACRO_LIST);
    WriteBE<u16>(Macro::kMacroActionNamesCount);

    for (int i = 0; i < Macro::kMacroActionNamesCount; i++)
    {
        WriteString(Macro::GetActionName(i));

        int count = 0;
        int offset = 0;
        Macro::GetBoundByCode((MACRO_CODE)i, count, offset);

        WriteBE<u16>(count);
        for (int j = 0; j < count; j++)
            WriteString(Macro::GetAction(j + offset));
    }
}

CPluginPacketFileInfo::CPluginPacketFileInfo(int index, u64 address, u64 size)
    : CPluginPacket()
{
    WriteBE<u16>(OIPMT_FILE_INFO);
    WriteBE<u16>(index);
    WriteBE<u64>(address);
    WriteBE<u64>(size);
}

CPluginPacketFileInfoLocalized::CPluginPacketFileInfoLocalized(
    int index, u64 address, u64 size, const std::string &language)
    : CPluginPacket()
{
    WriteBE<u16>(OIPMT_FILE_INFO_LOCALIZED);
    WriteBE<u16>(index);
    WriteBE<u64>(address);
    WriteBE<u64>(size);
    WriteString(language);
}

CPluginPacketStaticArtGraphicDataInfo::CPluginPacketStaticArtGraphicDataInfo(
    u16 graphic, u64 address, u64 size, u64 compressedSize)
    : CPluginPacket()
{
    WriteBE<u16>(OIPMT_GRAPHIC_DATA_INFO);
    WriteBE<u8>(OGDT_STATIC_ART);
    WriteBE<u16>(graphic);
    WriteBE<u64>(address);
    WriteBE<u64>(size);
    WriteBE<u64>(compressedSize);
}

CPluginPacketGumpArtGraphicDataInfo::CPluginPacketGumpArtGraphicDataInfo(
    u16 graphic,
    u64 address,
    u64 size,
    u64 compressedSize,
    u16 width,
    u16 height)
    : CPluginPacket()
{
    WriteBE<u16>(OIPMT_GRAPHIC_DATA_INFO);
    WriteBE<u8>(OGDT_GUMP_ART);
    WriteBE<u16>(graphic);
    WriteBE<u64>(address);
    WriteBE<u64>(size);
    WriteBE<u64>(compressedSize);
    WriteBE<u16>(width);
    WriteBE<u16>(height);
}

CPluginPacketFilesTransfered::CPluginPacketFilesTransfered()
    : CPluginPacket()
{
    WriteBE<u16>(OIPMT_FILES_TRANSFERED);
}

CPluginPacketOpenMap::CPluginPacketOpenMap()
    : CPluginPacket()
{
    WriteBE<u16>(OIPMT_OPEN_MAP);
}
