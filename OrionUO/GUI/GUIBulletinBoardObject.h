#pragma once

#include "Core/Minimal.h"
#include "BaseGUI.h"
#include "GLEngine/GLTextTexture.h"
#include <string>

class CGUIBulletinBoardObject : public CBaseGUI
{
    std::wstring Text = {};

private:
    CGLTextTexture m_Texture;

public:
    CGUIBulletinBoardObject(int serial, int x, int y, const std::wstring& text);
    virtual ~CGUIBulletinBoardObject();

    virtual void PrepareTextures();
    virtual void Draw(bool checktrans = false);
    virtual bool Select();
};
