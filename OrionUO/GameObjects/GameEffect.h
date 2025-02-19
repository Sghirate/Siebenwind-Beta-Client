#pragma once

#include "RenderWorldObject.h"
#include "Core/Time.h"

class CGameEffect : public CRenderWorldObject
{
public:
    EFFECT_TYPE EffectType = EF_MOVING;

    u32 DestSerial = 0;
    u16 DestX = 0;
    u16 DestY = 0;
    char DestZ = 0;
    u8 Speed = 0;
    bool FixedDirection = false;
    bool Explode = false;
    u32 RenderMode = 0;
    int AnimIndex = 0;
    int Increment = 0;
    u32 LastChangeFrameTime = 0;

    CGameEffect();
    virtual ~CGameEffect();

    virtual void Draw(int x, int y);
    u16 GetCurrentGraphic();
    u16 CalculateCurrentGraphic();
    void ApplyRenderMode();
    void RemoveRenderMode();
    bool IsEffectObject() { return true; }
    virtual void Update(class CGameObject *parent);
    inline void SetDuration(const Core::TimeDiff& a_duration) { m_end = Core::FrameTimer::Now() + a_duration; }
    inline bool IsElapsed() const { return Core::TimeStamp::Now() > m_end; }

private:
    Core::TimeStamp m_end;
};
