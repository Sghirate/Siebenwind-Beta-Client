#pragma once

#include "Core/MappedFile.h"
#include "../Utility/AutoResetEvent.h"
#include <fstream>
#include <unordered_map>
#include <vector>

struct UOPAnimationData;
class CTextureAnimationDirection;

class CUopBlockHeader
{
public:
    u64 Offset = 0;
    //u32 HeaderSize = 0;
    u32 CompressedSize   = 0;
    u32 DecompressedSize = 0;
    //u64 Hash = 0;
    //u32 Unknown = 0;
    //u16 Flags = 0;

    CUopBlockHeader() {}
    CUopBlockHeader(
        u64 offset,
        int headerSize,
        int compresseSize,
        int decompressedSize,
        u64 Hash,
        int unknown,
        u16 flags)
        : Offset(offset)
        , CompressedSize(compresseSize)
        , DecompressedSize(decompressedSize)
    {
    }
    ~CUopBlockHeader() {}
};

class CUopMappedFile : public Core::MappedFile
{
public:
    std::unordered_map<u64, CUopBlockHeader> m_Map;

public:
    CUopMappedFile();
    virtual ~CUopMappedFile();

    void Add(u64 hash, const CUopBlockHeader& item);

    CUopBlockHeader* GetBlock(u64 hash);

    std::vector<u8> GetData(const CUopBlockHeader& block);
};

class CFileManager : public Core::StreamReader
{
public:
    bool UseUOPGumps      = false;
    int UnicodeFontsCount = 0;

    CFileManager();
    virtual ~CFileManager();

    AutoResetEvent m_AutoResetEvent;

    //!Адреса файлов в памяти
    Core::MappedFile m_AnimIdx[6];
    Core::MappedFile m_ArtIdx;
    Core::MappedFile m_GumpIdx;
    Core::MappedFile m_LightIdx;
    Core::MappedFile m_MultiIdx;
    Core::MappedFile m_SkillsIdx;
    Core::MappedFile m_SoundIdx;
    Core::MappedFile m_StaticIdx[6];
    Core::MappedFile m_TextureIdx;

    std::fstream m_AnimMul[6];
    Core::MappedFile m_AnimdataMul;
    Core::MappedFile m_ArtMul;
    Core::MappedFile m_HuesMul;
    Core::MappedFile m_GumpMul;
    Core::MappedFile m_LightMul;
    Core::MappedFile m_MapMul[6];
    Core::MappedFile m_MultiMul;
    Core::MappedFile m_RadarcolMul;
    Core::MappedFile m_SkillsMul;
    Core::MappedFile m_SoundMul;
    Core::MappedFile m_StaticMul[6];
    Core::MappedFile m_TextureMul;
    Core::MappedFile m_TiledataMul;
    Core::MappedFile m_UnifontMul[20];
    Core::MappedFile m_VerdataMul;
    Core::MappedFile m_FacetMul[6];

    Core::MappedFile m_MultiMap;
    Core::MappedFile m_SpeechMul;
    Core::MappedFile m_LangcodeIff;

    //UOP
    CUopMappedFile m_ArtLegacyMUL;
    CUopMappedFile m_GumpartLegacyMUL;
    CUopMappedFile m_SoundLegacyMUL;
    CUopMappedFile m_Tileart;
    CUopMappedFile m_MainMisc;
    CUopMappedFile m_MapUOP[6];
    CUopMappedFile m_MapXUOP[6];
    CUopMappedFile m_AnimationSequence;
    CUopMappedFile m_MultiCollection;

    //Map patches
    Core::MappedFile m_MapDifl[6];
    Core::MappedFile m_MapDif[6];

    Core::MappedFile m_StaDifl[6];
    Core::MappedFile m_StaDifi[6];
    Core::MappedFile m_StaDif[6];

    bool Load();
    bool LoadWithUOP();
    void Unload();
    void TryReadUOPAnimations();

    //Чтение сжатых данных с кадрами из УОП аним файла.
    static char* ReadUOPDataFromFileStream(UOPAnimationData& animData);

    void ReadAnimMulDataFromFileStream(
        std::vector<char>& animData, CTextureAnimationDirection& direction);

    //Разжатие данных с кадрами из УОП аним файла.
    static bool
    DecompressUOPFileData(UOPAnimationData& animData, std::vector<u8>& decLayoutData, char* buf);

    void SendFilesInfo();

    bool IsMulFileOpen(int idx) const;

private:
    void ReadTask();
    static bool TryOpenFileStream(std::fstream& a_stream, const std::filesystem::path& a_path);
    bool LoadUOPFile(CUopMappedFile& a_file, const char* a_fileName);
};

extern CFileManager g_FileManager;
