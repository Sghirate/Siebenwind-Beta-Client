#pragma once

const int PATHFINDER_MAX_NODES = 10000;

#include "PathNode.h"

enum PATH_STEP_STATE
{
    PSS_NORMAL = 0,
    PSS_DEAD_OR_GM,
    PSS_ON_SEA_HORSE,
    PSS_FLYING
};

enum PATH_OBJECT_FLAGS
{
    POF_IMPASSABLE_OR_SURFACE = 0x00000001,
    POF_SURFACE = 0x00000002,
    POF_BRIDGE = 0x00000004,
    POF_NO_DIAGONAL = 0x00000008
};

class CPathFinder
{
public:
    bool AutoWalking = false;
    bool PathFindidngCanBeCancelled = false;
    bool BlockMoving = false;
    bool FastRotation = false;
    bool IgnoreStaminaCheck = false;

protected:
    bool CalculateNewZ(int x, int y, char &z, int direction);

    int CalculateMinMaxZ(
        int &minZ, int &maxZ, int newX, int newY, int currentZ, int newDirection, int stepState);

    bool CreateItemsList(std::vector<CPathObject> &list, int x, int y, int stepState);

    Core::Vec2<i32> m_StartPoint{ Core::Vec2<i32>() };
    Core::Vec2<i32> m_EndPoint{ Core::Vec2<i32>() };

    int m_GoalNode{ 0 };
    bool m_GoalFound{ 0 };
    int m_ActiveOpenNodes{ 0 };
    int m_ActiveClosedNodes{ 0 };
    int m_PathFindDistance{ 0 };
    CPathNode m_OpenList[PATHFINDER_MAX_NODES];
    CPathNode m_ClosedList[PATHFINDER_MAX_NODES];
    CPathNode *m_Path[PATHFINDER_MAX_NODES];
    int m_PointIndex{ 0 };
    int m_PathSize{ 0 };

    int GetGoalDistCost(const Core::Vec2<i32> &p, int cost);
    bool DoesNotExistOnOpenList(int x, int y, int z);
    bool DoesNotExistOnClosedList(int x, int y, int z);

    int
    AddNodeToList(int list, int direction, int x, int y, int z, CPathNode *parentNode, int cost);

    bool OpenNodes(CPathNode *node);
    int FindCheapestNode();
    bool FindPath(int maxNodes);

public:
    CPathFinder();
    virtual ~CPathFinder();

    void GetNewXY(u8 direction, int &x, int &y);
    bool CanWalk(u8 &direction, int &x, int &y, char &z);
    bool Walk(bool run, u8 direction);
    bool WalkTo(int x, int y, int z, int distance);
    void ProcessAutowalk();
    void StopAutoWalk();
    int GetWalkSpeed(bool run, bool onMount);
};

extern CPathFinder g_PathFinder;
