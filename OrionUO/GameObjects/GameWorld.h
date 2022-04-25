#pragma once

#include "GameItem.h"

typedef std::map<u32, CGameObject *> WORLD_MAP;

class CGameWorld
{
public:
    u32 ObjectToRemove = 0;

private:
    void CreatePlayer(int serial);
    void RemovePlayer();

public:
    CGameWorld(int serial);
    ~CGameWorld();

    WORLD_MAP m_Map;
    CGameObject *m_Items{ nullptr };

    void ResetObjectHandlesState();
    void ProcessAnimation();
    void ProcessSound(int ticks, CGameCharacter *gc);
    void SetPlayer(int serial);
    CGameItem *GetWorldItem(int serial);
    CGameCharacter *GetWorldCharacter(int serial);
    CGameObject *FindWorldObject(int serial);
    CGameItem *FindWorldItem(int serial);
    CGameCharacter *FindWorldCharacter(int serial);
    void ReplaceObject(CGameObject *obj, int newSerial);
    void RemoveObject(CGameObject *obj);
    void RemoveFromContainer(CGameObject *obj);
    void ClearContainer(CGameObject *obj);

    void PutContainer(CGameObject *obj, int containerSerial)
    {
        CGameObject *cnt = FindWorldObject(containerSerial);
        if (cnt != nullptr)
            PutContainer(obj, cnt);
    }

    void PutContainer(CGameObject *obj, CGameObject *container);

    void PutEquipment(CGameItem *obj, int containerSerial, int layer)
    {
        CGameObject *cnt = FindWorldObject(containerSerial);
        if (cnt != nullptr)
            PutEquipment(obj, cnt, layer);
    }

    void PutEquipment(CGameItem *obj, CGameObject *container, int layer)
    {
        PutContainer(obj, container);
        obj->Layer = layer;
    }

    void MoveToTop(CGameObject *obj);
    void Dump(u8 tCount = 0, u32 serial = 0xFFFFFFFF);
    CGameObject *SearchWorldObject(
        int serialStart, int scanDistance, SCAN_TYPE_OBJECT scanType, SCAN_MODE_OBJECT scanMode);

    void UpdateContainedItem(
        int serial,
        u16 graphic,
        u8 graphicIncrement,
        u16 count,
        int x,
        int y,
        int containerSerial,
        u16 color);

    void UpdateItemInContainer(CGameObject *obj, CGameObject *container, int x, int y);

    void UpdateGameObject(
        int serial,
        u16 graphic,
        u8 graphicIncrement,
        int count,
        int x,
        int y,
        char z,
        u8 direction,
        u16 color,
        u8 flags,
        int a11,
        UPDATE_GAME_OBJECT_TYPE updateType,
        u16 a13);

    void UpdatePlayer(
        int serial,
        u16 graphic,
        u8 graphicIncrement,
        u16 color,
        u8 flags,
        int x,
        int y,
        u16 serverID,
        u8 direction,
        char z);
};

extern CGameWorld *g_World;
