// MIT License
// Copyright (C) August 2016 Hotride

#include "ObjectOnCursor.h"
#include "GameObject.h"
#include "../plugin/mulstruct.h"

CObjectOnCursor g_ObjectInHand;

void CObjectOnCursor::Clear()
{
    Enabled = false;
    Dropped = false;
    Serial = 0;
    Graphic = 0;
    Color = 0;
    Count = 0;
    TotalCount = 0;
    Layer = 0;
    Flags = 0;
    Container = 0;
    IsGameFigure = false;
    TiledataPtr = nullptr;
    UpdatedInWorld = false;
}

u16 CObjectOnCursor::GetDrawGraphic(bool &doubleDraw)
{
    int index = CGameObject::IsGold(Graphic);
    u16 result = Graphic;

    const u16 graphicAssociateTable[3][3] = { { 0x0EED, 0x0EEE, 0x0EEF },
                                                   { 0x0EEA, 0x0EEB, 0x0EEC },
                                                   { 0x0EF0, 0x0EF1, 0x0EF2 } };

    if (index != 0)
    {
        int graphicIndex = (int)(Count > 1) + (int)(Count > 5);
        result = graphicAssociateTable[index - 1][graphicIndex];
    }
    else
    {
        doubleDraw = IsStackable(TiledataPtr->Flags) && (Count > 1);
    }

    return result;
}
