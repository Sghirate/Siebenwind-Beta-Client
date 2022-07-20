#pragma once

#include "Gump.h"

class CGumpProperty : public CGump
{
    u32 Timer = 0;

private:
    CGLTextTexture m_Texture;

public:
    CGumpProperty(const std::wstring &text);
    virtual ~CGumpProperty();

    virtual void PrepareContent();

    virtual class CRenderObject *Select() { return nullptr; }
};
