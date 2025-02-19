#include "IndexObject.h"
#include "Config.h"
#include "GameVars.h"
#include "plugin/enumlist.h"

CIndexObject::CIndexObject()
{
}

CIndexObject::~CIndexObject()
{
    if (Texture != nullptr)
    {
        delete Texture;
        Texture = nullptr;
    }
}

CIndexObjectLand::CIndexObjectLand()
    : CIndexObject()
{
}

CIndexObjectLand::~CIndexObjectLand()
{
}

CIndexObjectStatic::CIndexObjectStatic()
    : CIndexObject()
{
}

CIndexObjectStatic::~CIndexObjectStatic()
{
}

CIndexSound::CIndexSound()
    : CIndexObject()
{
}

CIndexSound::~CIndexSound()
{
}

CIndexMulti::CIndexMulti()
    : CIndexObject()
{
}

CIndexMulti::~CIndexMulti()
{
}

CIndexLight::CIndexLight()
    : CIndexObject()
{
}

CIndexLight::~CIndexLight()
{
}

CIndexGump::CIndexGump()
    : CIndexObject()
{
}

CIndexGump::~CIndexGump()
{
}

CIndexAnimation::CIndexAnimation()
{
}

CIndexAnimation::~CIndexAnimation()
{
}

CIndexMusic::CIndexMusic()
{
}

CIndexMusic::~CIndexMusic()
{
}

void CIndexObject::ReadIndexFile(size_t address, BASE_IDX_BLOCK *ptr, const u16 id)
{
    Address = ptr->Position;
    DataSize = ptr->Size;

    if (Address == 0xFFFFFFFF || (DataSize == 0) || DataSize == 0xFFFFFFFF)
    {
        Address = 0;
        DataSize = 0;
    }
    else
    {
        Address = Address + address;
    }

    ID = id;
};

void CIndexMulti::ReadIndexFile(size_t address, BASE_IDX_BLOCK *ptr, const u16 id)
{
    CIndexObject::ReadIndexFile(address, ptr, id);
    if (GameVars::GetClientVersion()  >= CV_7090)
    {
        Count = (u16)(DataSize / sizeof(MULTI_BLOCK_NEW));
    }
    else
    {
        Count = (u16)(DataSize / sizeof(MULTI_BLOCK));
    }
};

void CIndexLight::ReadIndexFile(size_t address, BASE_IDX_BLOCK *ptr, const u16 id)
{
    CIndexObject::ReadIndexFile(address, ptr, id);
    LIGHT_IDX_BLOCK *realPtr = (LIGHT_IDX_BLOCK *)ptr;
    Width = realPtr->Width;
    Height = realPtr->Height;
};

void CIndexGump::ReadIndexFile(size_t address, BASE_IDX_BLOCK *ptr, const u16 id)
{
    CIndexObject::ReadIndexFile(address, ptr, id);
    GUMP_IDX_BLOCK *realPtr = (GUMP_IDX_BLOCK *)ptr;
    Width = realPtr->Width;
    Height = realPtr->Height;
};
