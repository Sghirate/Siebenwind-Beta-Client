#include "FileManager.h"
#include "AnimationManager.h"
#include "ClilocManager.h"
#include "Core/Log.h"
#include "GameVars.h"
#include "../OrionUO.h"
#include "../OrionApplication.h"
#include "../Config.h"
#include "../Network/PluginPackets.h"
#include "plugin/enumlist.h"

#define MINIZ_IMPLEMENTATION
#include <miniz.h>
#include <thread>

CFileManager g_FileManager;

CUopMappedFile::CUopMappedFile()
{
}

CUopMappedFile::~CUopMappedFile()
{
}

void CUopMappedFile::Add(u64 hash, const CUopBlockHeader& item)
{
    m_Map[hash] = item;
}

CUopBlockHeader* CUopMappedFile::GetBlock(u64 hash)
{
    auto found = m_Map.find(hash);
    if (found != m_Map.end())
    {
        return &found->second;
    }

    return nullptr;
}

std::vector<u8> CUopMappedFile::GetData(const CUopBlockHeader& block)
{
    ResetPtr();
    Move((int)block.Offset);

    uLongf compressedSize   = block.CompressedSize;
    uLongf decompressedSize = block.DecompressedSize;
    std::vector<u8> result(decompressedSize, 0);
    if ((compressedSize != 0u) && compressedSize != decompressedSize)
    {
        int z_err = mz_uncompress(&result[0], &decompressedSize, m_ptr, compressedSize);
        if (z_err != Z_OK)
        {
            LOG_ERROR("FileManager", "Uncompress error: %i", z_err);
            result.clear();
        }
    }
    else
    {
        memcpy(&result[0], &m_ptr[0], decompressedSize);
    }
    return result;
}

CFileManager::CFileManager()
{
}

CFileManager::~CFileManager()
{
}

bool CFileManager::Load()
{
    if (GameVars::GetClientVersion() >= CV_7000 && LoadUOPFile(m_MainMisc, "MainMisc.uop"))
    {
        return LoadWithUOP();
    }

    struct
    {
        Core::MappedFile& file;
        const char* name;
    } muls[] = {
        { m_ArtIdx, "artidx.mul" },        { m_ArtIdx, "artidx.mul" },
        { m_ArtMul, "art.mul" },           { m_GumpIdx, "gumpidx.mul" },
        { m_GumpMul, "gumpart.mul" },      { m_SoundIdx, "soundidx.mul" },
        { m_SoundMul, "sound.mul" },       { m_AnimIdx[0], "anim.idx" },
        { m_LightIdx, "lightidx.mul" },    { m_MultiIdx, "multi.idx" },
        { m_SkillsIdx, "Skills.idx" },     { m_MultiMap, "Multimap.rle" },
        { m_TextureIdx, "texidx.mul" },    { m_AnimdataMul, "animdata.mul" },
        { m_HuesMul, "hues.mul" },         { m_LightMul, "light.mul" },
        { m_MultiMul, "multi.mul" },       { m_RadarcolMul, "radarcol.mul" },
        { m_SkillsMul, "skills.mul" },     { m_TextureMul, "texmaps.mul" },
        { m_TiledataMul, "tiledata.mul" },
    };
    for (int i = 0; i < sizeof(muls) / sizeof(muls[0]); ++i)
    {
        if (!muls[i].file.Load(g_App.GetGameDir() / muls[i].name))
            return false;
    }
    if (!TryOpenFileStream(m_AnimMul[0], g_App.GetGameDir() / "anim.mul"))
        return false;

    m_SpeechMul.Load(g_App.GetGameDir() / "speech.mul");
    m_LangcodeIff.Load(g_App.GetGameDir() / "Langcode.iff");
    for (int i = 0; i < 6; i++)
    {
        if (i > 1)
        {
            m_AnimIdx[i].Load(g_App.GetGameDir() / ("anim" + std::to_string(i) + ".idx"));
            TryOpenFileStream(
                m_AnimMul[i], g_App.GetGameDir() / ("anim" + std::to_string(i) + ".mul"));
        }

        m_MapMul[i].Load(g_App.GetGameDir() / ("map" + std::to_string(i) + ".mul"));

        m_StaticIdx[i].Load(g_App.GetGameDir() / ("staidx" + std::to_string(i) + ".mul"));
        m_StaticMul[i].Load(g_App.GetGameDir() / ("statics" + std::to_string(i) + ".mul"));
        m_FacetMul[i].Load(g_App.GetGameDir() / ("facet0" + std::to_string(i) + ".mul"));

        m_MapDifl[i].Load(g_App.GetGameDir() / ("mapdifl" + std::to_string(i) + ".mul"));
        m_MapDif[i].Load(g_App.GetGameDir() / ("mapdif" + std::to_string(i) + ".mul"));

        m_StaDifl[i].Load(g_App.GetGameDir() / ("stadifl" + std::to_string(i) + ".mul"));
        m_StaDifi[i].Load(g_App.GetGameDir() / ("stadifi" + std::to_string(i) + ".mul"));
        m_StaDif[i].Load(g_App.GetGameDir() / ("stadif" + std::to_string(i) + ".mul"));
    }

    for (int i = 0; i < 20; i++)
    {
        std::string fileName = i != 0 ? ("unifont" + std::to_string(i) + ".mul") : "unifont.mul";
        if (m_UnifontMul[i].Load(g_App.GetGameDir() / fileName))
            UnicodeFontsCount++;
    }

    if (g_Config.UseVerdata && !m_VerdataMul.Load(g_App.GetGameDir() / "verdata.mul"))
        g_Config.UseVerdata = false;

    return true;
}

bool CFileManager::LoadWithUOP()
{
    //Try to use map uop files first, if we can, we will use them.
    if (!LoadUOPFile(m_ArtLegacyMUL, "artLegacyMUL.uop"))
    {
        if (!m_ArtIdx.Load(g_App.GetGameDir() / "artidx.mul") ||
            !m_ArtMul.Load(g_App.GetGameDir() / "art.mul"))
            return false;
    }

    if (!LoadUOPFile(m_GumpartLegacyMUL, "gumpartLegacyMUL.uop"))
    {
        if (!m_GumpIdx.Load(g_App.GetGameDir() / "gumpidx.mul") ||
            !m_GumpMul.Load(g_App.GetGameDir() / "gumpart.mul"))
            return false;
        UseUOPGumps = false;
    }
    else
    {
        UseUOPGumps = true;
    }

    if (!LoadUOPFile(m_SoundLegacyMUL, "soundLegacyMUL.uop"))
    {
        if (!m_SoundIdx.Load(g_App.GetGameDir() / "soundidx.mul") ||
            !m_SoundMul.Load(g_App.GetGameDir() / "sound.mul"))
            return false;
    }

    if (!LoadUOPFile(m_MultiCollection, "MultiCollection.uop"))
    {
        if (!m_MultiIdx.Load(g_App.GetGameDir() / "multi.idx") ||
            !m_MultiMul.Load(g_App.GetGameDir() / "multi.mul"))
            return false;
    }

    LoadUOPFile(m_AnimationSequence, "AnimationSequence.uop");
    LoadUOPFile(m_Tileart, "tileart.uop");

    /* Эти файлы не используются самой последней версией клиента 7.0.52.2
	if (!m_tileart.Load(g_App.UOFilesPath("tileart.uop")))
	return false;
	if (!m_AnimationSequence.Load(g_App.UOFilesPath("AnimationSequence.uop")))
	return false;
	*/

    struct
    {
        Core::MappedFile& file;
        const char* name;
    } muls[] = {
        { m_AnimIdx[0], "anim.idx" },      { m_LightIdx, "lightidx.mul" },
        { m_SkillsIdx, "Skills.idx" },     { m_MultiMap, "Multimap.rle" },
        { m_TextureIdx, "texidx.mul" },    { m_AnimdataMul, "animdata.mul" },
        { m_HuesMul, "hues.mul" },         { m_LightMul, "light.mul" },
        { m_RadarcolMul, "radarcol.mul" }, { m_SkillsMul, "skills.mul" },
        { m_TextureMul, "texmaps.mul" },   { m_TiledataMul, "tiledata.mul" },
    };
    for (int i = 0; i < sizeof(muls) / sizeof(muls[0]); ++i)
    {
        if (!muls[i].file.Load(g_App.GetGameDir() / muls[i].name))
            return false;
    }
    if (!TryOpenFileStream(m_AnimMul[0], g_App.GetGameDir() / "anim.mul"))
        return false;

    m_SpeechMul.Load(g_App.GetGameDir() / "speech.mul");
    m_LangcodeIff.Load(g_App.GetGameDir() / "Langcode.iff");
    for (int i = 0; i < 6; i++)
    {
        if (i > 1)
        {
            m_AnimIdx[i].Load(g_App.GetGameDir() / ("anim" + std::to_string(i) + ".idx"));
            TryOpenFileStream(
                m_AnimMul[i], g_App.GetGameDir() / ("anim" + std::to_string(i) + ".mul"));
        }

        std::string mapName = std::string("map") + std::to_string(i);

        if (!LoadUOPFile(m_MapUOP[i], (mapName + "LegacyMUL.uop").c_str()))
        {
            m_MapMul[i].Load(g_App.GetGameDir() / (mapName + ".mul"));
        }

        m_StaticIdx[i].Load(g_App.GetGameDir() / ("staidx" + std::to_string(i) + ".mul"));
        m_StaticMul[i].Load(g_App.GetGameDir() / ("statics" + std::to_string(i) + ".mul"));
        m_FacetMul[i].Load(g_App.GetGameDir() / ("facet0" + std::to_string(i) + ".mul"));

        m_MapDifl[i].Load(g_App.GetGameDir() / ("mapdifl" + std::to_string(i) + ".mul"));
        m_MapDif[i].Load(g_App.GetGameDir() / ("mapdif" + std::to_string(i) + ".mul"));

        m_StaDifl[i].Load(g_App.GetGameDir() / ("stadifl" + std::to_string(i) + ".mul"));
        m_StaDifi[i].Load(g_App.GetGameDir() / ("stadifi" + std::to_string(i) + ".mul"));
        m_StaDif[i].Load(g_App.GetGameDir() / ("stadif" + std::to_string(i) + ".mul"));
    }

    for (int i = 0; i < 20; i++)
    {
        std::string fileName = i != 0 ? ("unifont" + std::to_string(i) + ".mul") : "unifont.mul";
        if (m_UnifontMul[i].Load(g_App.GetGameDir() / fileName))
            UnicodeFontsCount++;
    }

    if (g_Config.UseVerdata && !m_VerdataMul.Load(g_App.GetGameDir() / "verdata.mul"))
        g_Config.UseVerdata = false;

    return true;
}

void CFileManager::Unload()
{
    //DEBUG_TRACE_FUNCTION;
    m_ArtIdx.Unload();
    m_GumpIdx.Unload();
    m_SoundIdx.Unload();
    m_ArtMul.Unload();
    m_GumpMul.Unload();
    m_SoundMul.Unload();
    m_ArtLegacyMUL.Unload();
    m_GumpartLegacyMUL.Unload();
    m_SoundLegacyMUL.Unload();
    m_Tileart.Unload();
    m_MultiCollection.Unload();
    m_AnimationSequence.Unload();
    m_MainMisc.Unload();

    m_LightIdx.Unload();
    m_MultiIdx.Unload();
    m_SkillsIdx.Unload();

    m_MultiMap.Unload();
    m_TextureIdx.Unload();
    m_SpeechMul.Unload();
    m_AnimdataMul.Unload();

    m_HuesMul.Unload();

    m_LightMul.Unload();
    m_MultiMul.Unload();
    m_RadarcolMul.Unload();
    m_SkillsMul.Unload();

    m_TextureMul.Unload();
    m_TiledataMul.Unload();

    m_LangcodeIff.Unload();

    for (int i = 0; i < 6; i++)
    {
        m_AnimIdx[i].Unload();
        m_AnimMul[i].close();
        m_MapUOP[i].Unload();
        m_MapXUOP[i].Unload();
        m_MapMul[i].Unload();
        m_StaticIdx[i].Unload();
        m_StaticMul[i].Unload();
        m_FacetMul[i].Unload();

        m_MapDifl[i].Unload();
        m_MapDif[i].Unload();

        m_StaDifl[i].Unload();
        m_StaDifi[i].Unload();
        m_StaDif[i].Unload();
    }

    for (int i = 0; i < 20; i++)
    {
        m_UnifontMul[i].Unload();
    }

    m_VerdataMul.Unload();
}

void CFileManager::SendFilesInfo()
{
    if (m_TiledataMul.GetBuffer() != nullptr)
    {
        CPluginPacketFileInfo(
            OFI_TILEDATA_MUL,
            (u64)m_TiledataMul.GetBuffer(),
            (u64)m_TiledataMul.GetSize())
            .SendToPlugin();
    }

    if (m_MultiIdx.GetBuffer() != nullptr)
    {
        CPluginPacketFileInfo(
            OFI_MULTI_IDX, (u64)m_MultiIdx.GetBuffer(), (u64)m_MultiIdx.GetSize())
            .SendToPlugin();
    }

    if (m_MultiMul.GetBuffer() != nullptr)
    {
        CPluginPacketFileInfo(
            OFI_MULTI_MUL, (u64)m_MultiMul.GetBuffer(), (u64)m_MultiMul.GetSize())
            .SendToPlugin();
    }

    if (m_MultiCollection.GetBuffer() != nullptr)
    {
        CPluginPacketFileInfo(
            OFI_MULTI_UOP,
            (u64)m_MultiCollection.GetBuffer(),
            (u64)m_MultiCollection.GetSize())
            .SendToPlugin();
    }

    if (m_HuesMul.GetBuffer() != nullptr)
    {
        CPluginPacketFileInfo(
            OFI_HUES_MUL, (u64)m_HuesMul.GetBuffer(), (u64)m_HuesMul.GetSize())
            .SendToPlugin();
    }

    for (int i = 0; i < 6; i++)
    {
        if (m_MapMul[i].GetBuffer() != nullptr)
        {
            CPluginPacketFileInfo(
                OFI_MAP_0_MUL + i,
                (u64)m_MapMul[i].GetBuffer(),
                (u64)m_MapMul[i].GetSize())
                .SendToPlugin();
        }

        if (m_MapUOP[i].GetBuffer() != nullptr)
        {
            CPluginPacketFileInfo(
                OFI_MAP_0_UOP + i,
                (u64)m_MapUOP[i].GetBuffer(),
                (u64)m_MapUOP[i].GetSize())
                .SendToPlugin();
        }

        if (m_MapXUOP[i].GetBuffer() != nullptr)
        {
            CPluginPacketFileInfo(
                OFI_MAPX_0_UOP + i,
                (u64)m_MapXUOP[i].GetBuffer(),
                (u64)m_MapXUOP[i].GetSize())
                .SendToPlugin();
        }

        if (m_StaticIdx[i].GetBuffer() != nullptr)
        {
            CPluginPacketFileInfo(
                OFI_STAIDX_0_MUL + i,
                (u64)m_StaticIdx[i].GetBuffer(),
                (u64)m_StaticIdx[i].GetSize())
                .SendToPlugin();
        }

        if (m_StaticMul[i].GetBuffer() != nullptr)
        {
            CPluginPacketFileInfo(
                OFI_STATICS_0_MUL + i,
                (u64)m_StaticMul[i].GetBuffer(),
                (u64)m_StaticMul[i].GetSize())
                .SendToPlugin();
        }

        if (m_MapDif[i].GetBuffer() != nullptr)
        {
            CPluginPacketFileInfo(
                OFI_MAP_DIF_0_MUL + i,
                (u64)m_MapDif[i].GetBuffer(),
                (u64)m_MapDif[i].GetSize())
                .SendToPlugin();
        }

        if (m_MapDifl[i].GetBuffer() != nullptr)
        {
            CPluginPacketFileInfo(
                OFI_MAP_DIFL_0_MUL + i,
                (u64)m_MapDifl[i].GetBuffer(),
                (u64)m_MapDifl[i].GetSize())
                .SendToPlugin();
        }

        if (m_StaDif[i].GetBuffer() != nullptr)
        {
            CPluginPacketFileInfo(
                OFI_STA_DIF_0_MUL + i,
                (u64)m_StaDif[i].GetBuffer(),
                (u64)m_StaDif[i].GetSize())
                .SendToPlugin();
        }

        if (m_StaDifi[i].GetBuffer() != nullptr)
        {
            CPluginPacketFileInfo(
                OFI_STA_DIFI_0_MUL + i,
                (u64)m_StaDifi[i].GetBuffer(),
                (u64)m_StaDifi[i].GetSize())
                .SendToPlugin();
        }

        if (m_StaDifl[i].GetBuffer() != nullptr)
        {
            CPluginPacketFileInfo(
                OFI_STA_DIFL_0_MUL + i,
                (u64)m_StaDifl[i].GetBuffer(),
                (u64)m_StaDifl[i].GetSize())
                .SendToPlugin();
        }
    }

    if (m_VerdataMul.GetBuffer() != nullptr)
    {
        CPluginPacketFileInfo(
            OFI_VERDATA_MUL, (u64)m_VerdataMul.GetBuffer(), (u64)m_VerdataMul.GetSize())
            .SendToPlugin();
    }

    if (m_RadarcolMul.GetBuffer() != nullptr)
    {
        CPluginPacketFileInfo(
            OFI_RADARCOL_MUL,
            (u64)m_RadarcolMul.GetBuffer(),
            (u64)m_RadarcolMul.GetSize())
            .SendToPlugin();
    }

    QFOR(item, g_ClilocManager.m_Items, Cliloc*)
    {
        if (item->IsLoaded() && item->GetFile().GetBuffer() != nullptr)
        {
            CPluginPacketFileInfoLocalized(
                OFI_CLILOC_MUL,
                (u64)item->GetFile().GetBuffer(),
                (u64)item->GetFile().GetSize(),
                item->GetLanguage())
                .SendToPlugin();
        }
    }
}

void CFileManager::TryReadUOPAnimations()
{
    std::thread readThread(&CFileManager::ReadTask, this);
    readThread.detach();
}

void CFileManager::ReadTask()
{
    std::unordered_map<u64, UOPAnimationData> hashes;
    for (int i = 1; i < 5; i++)
    {
        char magic[4];
        char version[4];
        char signature[4];
        char nextBlock[8];

        std::fstream* animFile = new std::fstream();
        if (animFile == nullptr)
        {
            continue;
        }

        std::filesystem::path path =
            g_App.GetGameDir() / ("AnimationFrame" + std::to_string(i) + ".uop");
        if (!std::filesystem::exists(path))
            continue;

        animFile->open(path, std::ios::binary | std::ios::in);
        animFile->read(magic, 4);
        animFile->read(version, 4);
        animFile->read(signature, 4);
        animFile->read(nextBlock, 8);
        animFile->seekg(*reinterpret_cast<u64*>(nextBlock), std::ios::beg);

        do
        {
            char fileCount[4];
            char offset[8];
            char headerlength[4];
            char compressedlength[4];
            char hash[8];
            char decompressedlength[4];
            char skip1[4];
            char skip2[2];

            animFile->read(fileCount, 4);
            animFile->read(nextBlock, 8);
            int count = *reinterpret_cast<unsigned int*>(fileCount);
            for (int i = 0; i < count; i++)
            {
                animFile->read(offset, 8);
                animFile->read(headerlength, 4);
                animFile->read(compressedlength, 4);
                animFile->read(decompressedlength, 4);
                animFile->read(hash, 8);
                animFile->read(skip1, 4);
                animFile->read(skip2, 2);

                auto hashVal   = *reinterpret_cast<u64*>(hash);
                auto offsetVal = *reinterpret_cast<u64*>(offset);
                if (offsetVal == 0)
                {
                    continue;
                }

                UOPAnimationData dataStruct;
                dataStruct.offset = static_cast<u32>(
                    offsetVal + *reinterpret_cast<unsigned int*>(headerlength));
                dataStruct.compressedLength = *reinterpret_cast<unsigned int*>(compressedlength);
                dataStruct.decompressedLength =
                    *reinterpret_cast<unsigned int*>(decompressedlength);

                dataStruct.fileStream = animFile;
                dataStruct.path       = path;
                hashes[hashVal]       = dataStruct;
            }

            animFile->seekg(*reinterpret_cast<u64*>(nextBlock), std::ios::beg);
        } while (*reinterpret_cast<u64*>(nextBlock) != 0);
    }

    int maxGroup = 0;

    for (int animId = 0; animId < MAX_ANIMATIONS_DATA_INDEX_COUNT; animId++)
    {
        CIndexAnimation* indexAnim = &g_AnimationManager.m_DataIndex[animId];

        for (int grpId = 0; grpId < ANIMATION_GROUPS_COUNT; grpId++)
        {
            CTextureAnimationGroup* group = &(*indexAnim).m_Groups[grpId];
            char hashString[100];
            sprintf_s(hashString, "build/animationlegacyframe/%06i/%02i.bin", animId, grpId);
            auto hash = COrion::CreateHash(hashString);
            if (hashes.find(hash) != hashes.end())
            {
                if (grpId > maxGroup)
                {
                    maxGroup = (int)grpId;
                }

                UOPAnimationData dataStruct = hashes.at(hash);
                indexAnim->IsUOP            = true;
                group->m_UOPAnimData        = dataStruct;
                for (int dirId = 0; dirId < 5; dirId++)
                {
                    CTextureAnimationDirection* dir = &group->m_Direction[dirId];
                    dir->IsUOP                      = true;
                    dir->BaseAddress                = 0;
                    dir->Address                    = 0;
                }
            }
        }
    }

    if (g_AnimationManager.AnimGroupCount < maxGroup)
    {
        g_AnimationManager.AnimGroupCount = maxGroup;
    }

    m_AutoResetEvent.Set();
}

char* CFileManager::ReadUOPDataFromFileStream(UOPAnimationData& animData)
{
    animData.fileStream->clear();
    animData.fileStream->seekg(animData.offset, std::ios::beg);
    //reading into buffer on the heap
    char* buf = new char[animData.compressedLength];
    animData.fileStream->read(buf, animData.compressedLength);
    return buf;
}

bool CFileManager::DecompressUOPFileData(
    UOPAnimationData& animData, std::vector<u8>& decLayoutData, char* buf)
{
    uLongf cLen = animData.compressedLength;
    uLongf dLen = animData.decompressedLength;

    int z_err =
        mz_uncompress(&decLayoutData[0], &dLen, reinterpret_cast<unsigned char const*>(buf), cLen);
    delete[] buf;
    if (z_err != Z_OK)
    {
        LOG_ERROR("FileManager", "UOP anim decompression failed %d", z_err);
        LOG_ERROR("FileManager", "Anim file: %s", animData.path.string().c_str());
        LOG_ERROR("FileManager", "Anim offset: %d", animData.offset);
        return false;
    }
    return true;
}

bool CFileManager::LoadUOPFile(CUopMappedFile& a_file, const char* a_fileName)
{
    LOG_INFO("FileManager", "Loading UOP fileName: %s", a_fileName);
    if (!a_file.Load(g_App.GetGameDir() / a_fileName))
    {
        return false;
    }

    u32 formatID = a_file.ReadLE<u32>();
    if (formatID != 0x0050594D)
    {
        LOG_WARNING("FileManager", "WARNING!!! UOP file '%s' formatID is %i!", a_fileName, formatID);
        return false;
    }

    u32 formatVersion = a_file.ReadLE<u32>();
    if (formatVersion > 5)
    {
        LOG_WARNING("FileManager", "WARNING!!! UOP file '%s' version is %i!", a_fileName, formatVersion);
    }

    a_file.Move(4); //Signature?
    u64 next = a_file.ReadLE<u64>();

    a_file.Move(4); //Block capacity?
    u32 filesCount = a_file.ReadLE<u32>();

    a_file.ResetPtr();
    a_file.Move((int)next);

    do
    {
        int count = a_file.ReadLE<i32>();
        next      = a_file.ReadLE<i64>();

        for (int i = 0; i < count; i++)
        {
            u64 offset = a_file.ReadLE<u64>();

            u32 headerSize       = a_file.ReadLE<u32>();
            u32 compressedSize   = a_file.ReadLE<u32>();
            u32 decompressedSize = a_file.ReadLE<u32>();

            u64 hash    = a_file.ReadLE<u64>();
            u32 unknown = a_file.ReadLE<u32>();
            u16 flag    = a_file.ReadLE<u16>();

            if ((offset == 0u) || (decompressedSize == 0u))
                continue;

            if (flag == 0u)
                compressedSize = 0;

            CUopBlockHeader item;
            item.Offset           = offset + headerSize;
            item.CompressedSize   = compressedSize;
            item.DecompressedSize = decompressedSize;
            a_file.Add(hash, item);
        }
        a_file.ResetPtr();
        a_file.Move((int)next);
    } while (next != 0);

    a_file.ResetPtr();

    //if (string("MainMisc.uop") != fileName)
    //if (string("AnimationSequence.uop") != fileName)
    //if (string("tileart.uop") != fileName)
    return true;

    for (std::unordered_map<u64, CUopBlockHeader>::iterator i = a_file.m_Map.begin();
         i != a_file.m_Map.end();
         ++i)
    {
        LOG_INFO("FileManager", "item dump start: %016llX, %i\n", i->first, i->second.CompressedSize);

        std::vector<u8> data = a_file.GetData(i->second);

        if (data.empty())
        {
            continue;
        }

        Core::StreamReader reader(&data[0], data.size());

        //LOG("%s\n", reader.ReadString(decompressedSize));

        //LOG_DUMP(reader.GetBuffer(), (int)reader.GetSize());

        LOG_INFO("FileManager", "item dump end:\n");
    }

    a_file.ResetPtr();

    return true;
}

bool CFileManager::TryOpenFileStream(std::fstream& a_stream, const std::filesystem::path& a_path)
{
    LOG_INFO("FileManager", "Trying to open file stream for %s", a_path.string().c_str());
    if (!std::filesystem::exists(a_path))
    {
        LOG_ERROR("FileManager", "File doesnt exist");
        return false;
    }
    a_stream.open(a_path, std::ios::binary | std::ios::in);
    LOG_INFO("FileManager", "File stream opened");
    return true;
}

bool CFileManager::IsMulFileOpen(int idx) const
{
    //we only have 5 anim mul files atm
    if (idx > 5)
    {
        return false;
    }
    return m_AnimMul[idx].is_open();
}

void CFileManager::ReadAnimMulDataFromFileStream(
    std::vector<char>& animData, CTextureAnimationDirection& direction)
{
    std::fstream* fileStream = &m_AnimMul[direction.FileIndex];
    fileStream->clear();
    fileStream->seekg(direction.Address, std::ios::beg);
    fileStream->read(static_cast<char*>(animData.data()), direction.Size);
}
