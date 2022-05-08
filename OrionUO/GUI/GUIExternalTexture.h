#pragma once

#include "Core/Minimal.h"
#include "BaseGUI.h"

class CGLTexture;

class CGUIExternalTexture : public CBaseGUI
{
    bool DeleteTextureOnDestroy = false;
    short DrawWidth             = 0;
    short DrawHeight            = 0;

public:
    CGUIExternalTexture(
        CGLTexture* texture,
        bool deleteTextureOnDestroy,
        int x,
        int y,
        int drawWidth  = 0,
        int drawHeight = 0);
    virtual ~CGUIExternalTexture();

    CGLTexture* m_Texture;

    virtual Core::Vec2<i32> GetSize();

    virtual void SetShaderMode();
    virtual void Draw(bool checktrans = false);
    virtual bool Select();
};
