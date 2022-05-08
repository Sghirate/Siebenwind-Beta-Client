#include "GameEffectDrag.h"
#include "Managers/EffectManager.h"
#include "GameObjects/GameEffectMoving.h"

CGameEffectDrag::CGameEffectDrag()
{
}

CGameEffectDrag::~CGameEffectDrag()
{
}

void CGameEffectDrag::Update(CGameObject *parent)
{
    if (!CanMove())
        return;

    OffsetX += 8;
    OffsetY += 8;

    OnMoved();
    if (IsElapsed())
    {
        g_EffectManager.RemoveEffect(this);
    }
}
