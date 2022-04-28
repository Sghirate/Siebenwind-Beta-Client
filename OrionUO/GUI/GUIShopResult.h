#pragma once

#include "BaseGUI.h"

class CGUIShopResult : public CBaseGUI
{
public:
    //!Стоимость
    u32 Price = 0;

    //!Название
    string Name = "";

private:
    //!Текстура названия
    CGLTextTexture m_NameText{ CGLTextTexture() };

public:
    CGUIShopResult(class CGUIShopItem *shopItem, int x, int y);
    virtual ~CGUIShopResult();

    //!Компонента кнопок MinMax
    class CGUIMinMaxButtons *m_MinMaxButtons{ nullptr };

    //!Получить ссылку на выбранную компоненту или ссылку на себя
    CBaseGUI *SelectedItem();

    virtual Core::Vec2<i32> GetSize() { return Core::Vec2<i32>(200, m_NameText.Height); }

    virtual void PrepareTextures();

    virtual void Draw(bool checktrans = false);
    virtual bool Select();
};
