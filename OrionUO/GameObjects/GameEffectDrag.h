#pragma once

#include "GameEffect.h"
#include "Core/Time.h"

class CGameObject;

class CGameEffectDrag : public CGameEffect
{
public:
    int OffsetX = 0;
    int OffsetY = 0;
    u8 MoveDelay = 20;

    CGameEffectDrag();
    virtual ~CGameEffectDrag();
    virtual void Update(CGameObject *parent);

protected:
    inline bool CanMove() const { return m_nextMove <= Core::FrameTimer::Now(); }
    inline void OnMoved() { m_nextMove = Core::FrameTimer::Now() + Core::TimeDiff::FromMilliseconds(MoveDelay) ; }

private:
    Core::TimeStamp m_nextMove;

};
