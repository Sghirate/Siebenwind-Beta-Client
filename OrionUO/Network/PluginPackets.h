#pragma once

#include "Core/DataStream.h"

class CPluginPacket : public Core::StreamWriter
{
public:
    CPluginPacket();
    CPluginPacket(int size, bool autoResize = false);

    void SendToPlugin();
};

class CPluginPacketSkillsList : public CPluginPacket
{
public:
    CPluginPacketSkillsList();
};

class CPluginPacketSpellsList : public CPluginPacket
{
public:
    CPluginPacketSpellsList();
};

class CPluginPacketMacrosList : public CPluginPacket
{
public:
    CPluginPacketMacrosList();
};

class CPluginPacketFileInfo : public CPluginPacket
{
public:
    CPluginPacketFileInfo(int index, u64 address, u64 size);
};

class CPluginPacketFileInfoLocalized : public CPluginPacket
{
public:
    CPluginPacketFileInfoLocalized(
        int index, u64 address, u64 size, const std::string &language);
};

class CPluginPacketStaticArtGraphicDataInfo : public CPluginPacket
{
public:
    CPluginPacketStaticArtGraphicDataInfo(
        u16 graphic, u64 address, u64 size, u64 compressedSize);
};

class CPluginPacketGumpArtGraphicDataInfo : public CPluginPacket
{
public:
    CPluginPacketGumpArtGraphicDataInfo(
        u16 graphic,
        u64 address,
        u64 size,
        u64 compressedSize,
        u16 width,
        u16 height);
};

class CPluginPacketFilesTransfered : public CPluginPacket
{
public:
    CPluginPacketFilesTransfered();
};

class CPluginPacketOpenMap : public CPluginPacket
{
public:
    CPluginPacketOpenMap();
};
