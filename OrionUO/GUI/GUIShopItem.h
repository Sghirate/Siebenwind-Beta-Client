#pragma once

#include "Core/Minimal.h"
#include "BaseGUI.h"
#include "GLEngine/GLTextTexture.h"
#include <string>

class CGUIShopItem : public CBaseGUI
{
public:
    u32 Count           = 0;
    u32 Price           = 0;
    std::string Name    = "";
    bool Selected       = false;
    bool NameFromCliloc = false;

private:
    CGLTextTexture m_NameText{ CGLTextTexture() };
    CGLTextTexture m_CountText{ CGLTextTexture() };
    int m_ImageOffset{ 0 };
    int m_TextOffset{ 0 };
    int m_MaxOffset{ 0 };

public:
    CGUIShopItem(
        int serial,
        u16 graphic,
        u16 color,
        int count,
        int price,
        const std::string& name,
        int x,
        int y);
    virtual ~CGUIShopItem();

    void OnClick();
    void CreateNameText();
    void UpdateOffsets();
    void CreateCountText(int lostCount);
    virtual Core::Vec2<i32> GetSize() { return Core::Vec2<i32>(200, m_MaxOffset + 20); }
    virtual void PrepareTextures();
    virtual void SetShaderMode();
    virtual void Draw(bool checktrans = false);
    virtual bool Select();
};
