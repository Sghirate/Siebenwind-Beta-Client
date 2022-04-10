// MIT License
// Copyright (C) August 2016 Hotride

#include "Walker.h"
#include "../Network/Packets.h"
#include "../GameObjects/GamePlayer.h"

CWalker g_Walker;

CWalker::CWalker()
{
}

void CWalker::Reset()
{
    UnacceptedPacketsCount = 0;
    StepsCount = 0;
    WalkSequence = 0;
    CurrentWalkSequence = 0;
    WalkingFailed = false;
    ResendPacketSended = false;
    LastStepRequestTime = 0;
}

void CWalker::DenyWalk(u8 sequence, int x, int y, char z)
{
    g_Player->m_Steps.clear();

    g_Player->OffsetX = 0;
    g_Player->OffsetY = 0;
    g_Player->OffsetZ = 0;

    Reset();

    /*if (sequence == 0xFF)
	{
	g_Player->GameObject.GameObject.X = g_PlayerX;
	g_Player->GameObject.GameObject.Y = g_PlayerY;
	(*((void(**)(void))g_Player->GameObject.GameObject.VTable + UO_ROFUN_UPDATE_RENDER_AND_DRAW_POS))();
	RenderScene_1();
	(*((void(**)(void))g_Player->GameObject.GameObject.VTable + UO_ROFUN_39))();
	result = UpdateMaxDrawZ();
	}
	else{...}*/
    //UpdatePlayerCoordinates(x, y, z, g_ServerID);

    if (x != -1)
    {
        g_Player->SetX(x);
        g_Player->SetY(y);
        g_Player->SetZ(z);

        g_RemoveRangeXY.x = x;
        g_RemoveRangeXY.y = y;

        UOI_PLAYER_XYZ_DATA xyzData = { g_RemoveRangeXY.x, g_RemoveRangeXY.y, 0 };
        PLUGIN_EVENT(UOMSG_UPDATE_REMOVE_POS, &xyzData);
    }
}

void CWalker::ConfirmWalk(u8 sequence)
{
    if (UnacceptedPacketsCount != 0)
    {
        UnacceptedPacketsCount--;
    }

    int stepIndex = 0;

    for (int i = 0; i < StepsCount; i++)
    {
        if (m_Step[i].Sequence == sequence)
        {
            break;
        }

        stepIndex++;
    }

    bool isBadStep = (stepIndex == StepsCount);

    if (!isBadStep)
    {
        if (stepIndex >= CurrentWalkSequence)
        {
            m_Step[stepIndex].Accepted = true;
            g_RemoveRangeXY.x = m_Step[stepIndex].X;
            g_RemoveRangeXY.y = m_Step[stepIndex].Y;
        }
        else if (stepIndex == 0)
        {
            g_RemoveRangeXY.x = m_Step[0].X;
            g_RemoveRangeXY.y = m_Step[0].Y;

            for (int i = 1; i < StepsCount; i++)
            {
                m_Step[i - 1] = m_Step[i];
            }

            StepsCount--;
            CurrentWalkSequence--;
        }
        else
        { //if (stepIndex)
            isBadStep = true;
        }
    }

    if (isBadStep)
    {
        if (!ResendPacketSended)
        {
            CPacketResend().Send();
            ResendPacketSended = true;
        }

        WalkingFailed = true;
        StepsCount = 0;
        CurrentWalkSequence = 0;
    }
    else
    {
        UOI_PLAYER_XYZ_DATA xyzData = { g_RemoveRangeXY.x, g_RemoveRangeXY.y, 0 };
        PLUGIN_EVENT(UOMSG_UPDATE_REMOVE_POS, &xyzData);
    }
}
