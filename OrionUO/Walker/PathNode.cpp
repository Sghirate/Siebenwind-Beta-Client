#include "PathNode.h"

CPathNode::CPathNode()
{
}

CPathNode::~CPathNode()
{
    m_Parent = nullptr;
}

void CPathNode::Reset()
{
    m_Parent = nullptr;
    Used = false;
    X = Y = Z = Direction = Cost = DistFromStartCost = DistFromGoalCost = 0;
}
