#pragma once

#include "Core/Minimal.h"
#include "BaseGUI.h"
#include "GLEngine/GLTextTexture.h"
#include <string>

class CGUIShopResult : public CBaseGUI
{
public:
    u32 Price        = 0;
    std::string Name = "";

private:
    CGLTextTexture m_NameText{ CGLTextTexture() };

public:
    CGUIShopResult(class CGUIShopItem* shopItem, int x, int y);
    virtual ~CGUIShopResult();

    class CGUIMinMaxButtons* m_MinMaxButtons{ nullptr };
    CBaseGUI* SelectedItem();
    virtual Core::Vec2<i32> GetSize() { return Core::Vec2<i32>(200, m_NameText.Height); }
    virtual void PrepareTextures();
    virtual void Draw(bool checktrans = false);
    virtual bool Select();
};
