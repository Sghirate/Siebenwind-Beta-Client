// MIT License
// Copyright (C) August 2016 Hotride

#pragma once

#include "GameObject.h"

class CMulti;
class CMultiObject;

class CGameItem : public CGameObject
{
public:
    u8 Layer = 0;
    u16 AnimID = 0;
    u8 UsedLayer = 0;
    bool Opened = false;
    bool Dragged = false;
    bool MultiBody = false;
    bool WantUpdateMulti = true;
    u16 FieldColor = 0;
    short MultiDistanceBonus = 0;
    u32 Price = 0;
    bool NameFromCliloc = false;
    u16 MultiTileGraphic = 0;
    u8 LightID = 0;

protected:
    void AddMultiObject(CMultiObject *obj);

public:
    CGameItem(int serial = 0);
    virtual ~CGameItem();

    void CalculateFieldColor();
    CMulti *GetMulti();
    void ClearMultiItems();
    virtual void Draw(int x, int y);
    virtual void Select(int x, int y);
    void ClearCustomHouseMultis(int state);

    CMultiObject *
    AddMulti(u16 graphic, u16 color, char x, char y, char z, bool isCustomHouseMulti);

    void OnGraphicChange(int direction = 0);
    u16 GetMountAnimation();
    virtual bool IsInternal() { return ((m_TiledataPtr->Flags & 0x00010000) && !IsCorpse()); }

    bool IsHuman()
    {
        return (
            IsCorpse() && (IN_RANGE(Count, 0x0192, 0x0193) || IN_RANGE(Count, 0x025F, 0x0260) ||
                           IN_RANGE(Count, 0x02B6, 0x02B7)));
    }

    void LoadMulti(bool dropAlpha);
    CMulti *GetMultiAtXY(short x, short y);
    CGameItem *FindItem(u16 graphic, u16 color = 0xFFFF);
};
