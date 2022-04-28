#pragma once

#include "BaseGUI.h"

class CGUIShopItem : public CBaseGUI
{
public:
    //!Количество
    u32 Count = 0;

    //!Стоимость
    u32 Price = 0;

    //!Название товара
    string Name = "";

    //!Флаг выбора
    bool Selected = false;

    //!Имя создано из клилока
    bool NameFromCliloc = false;

private:
    //!Текстуры для текста
    CGLTextTexture m_NameText{ CGLTextTexture() };
    CGLTextTexture m_CountText{ CGLTextTexture() };

    //!Смещение картинки
    int m_ImageOffset{ 0 };

    //!Смещение текста
    int m_TextOffset{ 0 };

    //!Максимальное смещение
    int m_MaxOffset{ 0 };

public:
    CGUIShopItem(
        int serial,
        u16 graphic,
        u16 color,
        int count,
        int price,
        const std::string &name,
        int x,
        int y);
    virtual ~CGUIShopItem();

    //!Нажатие на компоненту
    void OnClick();

    //!Создать текстуру названия
    void CreateNameText();

    void UpdateOffsets();

    //!Создать текстуру количества
    void CreateCountText(int lostCount);

    virtual Core::Vec2<i32> GetSize() { return Core::Vec2<i32>(200, m_MaxOffset + 20); }

    virtual void PrepareTextures();

    virtual void SetShaderMode();

    virtual void Draw(bool checktrans = false);
    virtual bool Select();
};
