#pragma once

#include "Core/Minimal.h"
#include "Core/Time.h"
#include "BaseQueue.h"

class CLandObject;
class CMapObject;
class CRenderWorldObject;

class CMapBlock : public CBaseQueueItem
{
public:
    u32 Index = 0;
    Core::TimeStamp LastAccessed;
    short X = 0;
    short Y = 0;

private:
    //Получить Z координату ландшафта
    char GetLandZ(int x, int y, int map);

    //Проверить объект ландшафта на "растягиваемость" при рендере
    bool TestStretched(int x, int y, char z, int map, bool recurse);

public:
    CMapBlock(int index);
    virtual ~CMapBlock();

    //Данные блока
    CMapObject* Block[8][8];

    //Добавить объект
    CMapObject* AddObject(CMapObject* obj, int x, int y);

    //Получить указатель на объект ландшафта
    CLandObject* GetLand(int x, int y);

    //Добавить объект в очередь рендера
    void AddRender(CRenderWorldObject* item, int x, int y);

    //Получить начало очереди рендера
    CRenderWorldObject* GetRender(int x, int y);

    //Получить цвет точки для радара
    u16 GetRadarColor(int x, int y);

    //Сконструировать вершины текстур ландшафта
    void CreateLandTextureRect();

    bool HasNoExternalData();
};
