#pragma once

#include "Core/Minimal.h"
#include "GameObject.h"
#include "GLEngine/GLTextTexture.h"
#include "Globals.h"
#include "Walker/WalkData.h"
#include "TextEngine/TextContainer.h"
#include "plugin/enumlist.h"
#include <deque>
#include <string>

class CGameCharacter : public CGameObject
{
public:
    //!Hitpoints
    short Hits = 0;

    //!Max hitpoints
    short MaxHits = 0;

    //!Mana
    short Mana = 0;

    //!Max mana
    short MaxMana = 0;

    //!Stamina
    short Stam = 0;

    //!Max stamina
    short MaxStam = 0;

    //!Is female
    bool Female = false;

    //!Character's race
    RACE_TYPE Race = RT_HUMAN;

    //!Current direction
    u8 Direction = 0;

    //!Character's notoriety
    u8 Notoriety = 0;

    //!Player can change the name for this character
    bool CanChangeName = false;

    //!Interval between animation playback
    u8 AnimationInterval = 0;

    //!Current animation frame count
    u8 AnimationFrameCount = 0;

    //!Current animation repeat mode
    u8 AnimationRepeatMode = 1;

    //!Current animation group
    u8 AnimationGroup = 0xFF;

    //!Current animation is repeat
    bool AnimationRepeat = false;

    //!Current animation direction (front or back)
    bool AnimationDirection = false;

    //!Current animation called from the server
    bool AnimationFromServer = false;

    //!Last step sound time stamp
    u32 LastStepSoundTime = 0;

    //!Time stamp to fidget animation
    u32 TimeToRandomFidget = 0;

    //!Offset to step sound
    u8 StepSoundOffset = 0;

    //!Sprite offset by X coordinate on the tile
    int OffsetX = 0;

    //!Sprite offset by Y coordinate on the tile
    int OffsetY = 0;

    //!Sprite offset by Z coordinate on the tile
    int OffsetZ = 0;

    //!Last step time stamp
    u32 LastStepTime = 0;

    //!Character's title
    std::string Title = "";

    //!Percent of hits
    u8 HitsPercent = 0;

protected:
    void CorrectAnimationGroup(u16 graphic, ANIMATION_GROUPS group, u8 &animation);
    bool m_Dead = false;

public:
    CGameCharacter(int serial);
    virtual ~CGameCharacter();

    CTextContainer m_DamageTextControl{ CTextContainer(10) };
    std::deque<CWalkData> m_Steps;
    CGLTextTexture m_HitsTexture{ CGLTextTexture() };

    virtual void UpdateTextCoordinates();
    void UpdateHitsTexture(u8 hits);
    void ProcessGargoyleAnims(int &animGroup);
    int IsSitting();
    virtual void Draw(int x, int y);
    virtual void Select(int x, int y);
    void OnGraphicChange(int direction = 0);
    void ResetAnimationGroup(u8 val);
    void SetRandomFidgetAnimation();
    void SetAnimation(
        u8 id,
        u8 interval = 0,
        u8 frameCount = 0,
        u8 repeatCount = 0,
        bool repeat = false,
        bool frameDirection = false);

    u16 GetMountAnimation();
    u8 GetAnimationGroup(u16 checkGraphic = 0);
    void GetAnimationGroup(ANIMATION_GROUPS group, u8 &animation);
    bool Staying() { return AnimationGroup == 0xFF && m_Steps.empty(); }
    bool TestStepNoChangeDirection(u8 group);
    virtual bool Walking() { return (LastStepTime > (u32)(g_Ticks - WALKING_DELAY)); }
    virtual bool NoIterateAnimIndex()
    {
        return ((LastStepTime > (u32)(g_Ticks - WALKING_DELAY)) && m_Steps.empty());
    }

    void UpdateAnimationInfo(u8 &dir, bool canChange = false);

    bool IsHuman()
    {
        return (
            IN_RANGE(Graphic, 0x0190, 0x0193) || IN_RANGE(Graphic, 0x00B7, 0x00BA) ||
            IN_RANGE(Graphic, 0x025D, 0x0260) || IN_RANGE(Graphic, 0x029A, 0x029B) ||
            IN_RANGE(Graphic, 0x02B6, 0x02B7) || (Graphic == 0x03DB) || (Graphic == 0x03DF) ||
            (Graphic == 0x03E2));
    }

    bool Dead()
    {
        return (IN_RANGE(Graphic, 0x0192, 0x0193) || IN_RANGE(Graphic, 0x025F, 0x0260) ||
                IN_RANGE(Graphic, 0x02B6, 0x02B7)) ||
               m_Dead;
    }

    virtual CGameCharacter *GameCharacterPtr() { return this; }
    virtual CGameItem *FindSecureTradeBox();
    void SetDead(bool &dead);
};
