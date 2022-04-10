#pragma once

#include "Constants.h"
#include "Backend.h"
#include <filesystem>

struct UOPAnimationData
{
    std::filesystem::path path;
    unsigned int offset;
    unsigned int compressedLength;
    unsigned int decompressedLength;
    std::fstream* fileStream;
};

class CTextureAnimationFrame : public CGLTexture
{
public:
    short CenterX = 0;
    short CenterY = 0;

    CTextureAnimationFrame();
    virtual ~CTextureAnimationFrame();
};

class CTextureAnimationDirection
{
public:
    u8 FrameCount         = 0;
    size_t BaseAddress    = 0;
    u32 BaseSize          = 0;
    size_t PatchedAddress = 0;
    u32 PatchedSize       = 0;
    int FileIndex         = 0;
    size_t Address        = 0;
    u32 Size              = 0;
    u32 LastAccessTime    = 0;
    bool IsUOP            = false;
    bool IsVerdata        = false;

    CTextureAnimationDirection();
    virtual ~CTextureAnimationDirection();

    CTextureAnimationFrame* m_Frames;
};

class CTextureAnimationGroup
{
public:
    CTextureAnimationGroup();
    virtual ~CTextureAnimationGroup();

    CTextureAnimationDirection m_Direction[MAX_MOBILE_DIRECTIONS];
    UOPAnimationData m_UOPAnimData;
};
