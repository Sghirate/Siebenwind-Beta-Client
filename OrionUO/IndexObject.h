// MIT License
// Copyright (C) August 2016 Hotride

#pragma once

#include "TextureObject.h"
#include "plugin/mulstruct.h"

class CUopBlockHeader;

class CIndexObject
{
public:
    size_t Address = 0;
    int DataSize = 0;
    int Width = 0;
    int Height = 0;
    u16 ID = 0;
    u16 Color = 0;
    u32 LastAccessTime = 0;
    CUopBlockHeader *UopBlock = nullptr;
    CIndexObject();
    virtual ~CIndexObject();
    virtual void ReadIndexFile(size_t address, BASE_IDX_BLOCK *ptr, const u16 id);
    CGLTexture *Texture{ nullptr };
};

class CIndexObjectLand : public CIndexObject
{
public:
    bool AllBlack = false;

    CIndexObjectLand();
    virtual ~CIndexObjectLand();
};

class CIndexObjectStatic : public CIndexObject
{
public:
    u16 Index = 0;
    char Offset = 0;
    char AnimIndex = 0;
    u32 ChangeTime = 0;
    u16 LightColor = 0;
    bool IsFiled = false;

    CIndexObjectStatic();
    virtual ~CIndexObjectStatic();
};

class CIndexSound : public CIndexObject
{
public:
    u32 Delay = 0;

    CIndexSound();
    virtual ~CIndexSound();

    u8 *m_WaveFile = nullptr;
    SoundHandle m_Stream = SOUND_NULL;
};

class CIndexMulti : public CIndexObject
{
public:
    u32 Count = 0;

    CIndexMulti();
    virtual ~CIndexMulti();
    virtual void ReadIndexFile(size_t address, BASE_IDX_BLOCK *ptr, const u16 id) override;
};

class CIndexGump : public CIndexObject
{
public:
    CIndexGump();
    virtual ~CIndexGump();
    virtual void ReadIndexFile(size_t address, BASE_IDX_BLOCK *ptr, const u16 id) override;
};

class CIndexLight : public CIndexObject
{
public:
    CIndexLight();
    virtual ~CIndexLight();
    virtual void ReadIndexFile(size_t address, BASE_IDX_BLOCK *ptr, const u16 id) override;
};

class CIndexAnimation
{
public:
    u16 Graphic = 0;
    u16 Color = 0;
    ANIMATION_GROUPS_TYPE Type = AGT_UNKNOWN;
    u32 Flags = 0;
    char MountedHeightOffset = 0;
    bool IsUOP = false;

    CIndexAnimation();
    virtual ~CIndexAnimation();

    CTextureAnimationGroup m_Groups[ANIMATION_GROUPS_COUNT];
};

class CIndexMusic
{
public:
    string FilePath;
    bool Loop = false;

    CIndexMusic();
    virtual ~CIndexMusic();
};
