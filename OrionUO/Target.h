#pragma once

#include "Core/DataStream.h"

class CMulti;
class CMultiObject;

class CTarget
{
public:
    u8 Type = 0;
    u8 CursorType = 0;
    u16 MultiGraphic = 0;
    u16 MultiX = 0;
    u16 MultiY = 0;
    u32 CursorID = 0;
    bool Targeting = false;

private:
    u8 m_Data[19];
    u8 m_LastData[19];
    CMulti *m_Multi{ nullptr };

    void AddMultiObject(CMultiObject *obj);

public:
    CTarget();
    ~CTarget() {}

    void SetLastTargetObject(int serial);
    void SetData(Core::StreamReader & a_reader);
    void SetMultiData(Core::StreamReader & a_reader);
    bool IsTargeting() const { return Targeting; }
    void Reset();
    void RequestFromCustomHouse();
    void SendTargetObject(int Serial);
    void SendTargetTile(u16 tileID, short x, short Y, char z);
    void SendCancelTarget();
    void Plugin_SendTargetObject(int Serial);
    void Plugin_SendTargetTile(u16 tileID, short x, short Y, char z);
    void Plugin_SendCancelTarget();
    void Plugin_SendTarget();
    void SendLastTarget();
    void SendTarget();
    void LoadMulti(int offsetX, int offsetY, char offsetZ);
    void UnloadMulti();
    CMulti *GetMultiAtXY(short x, short y);
};

extern CTarget g_Target;
