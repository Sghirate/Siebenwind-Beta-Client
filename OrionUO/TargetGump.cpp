#include "TargetGump.h"
#include "OrionUO.h"
#include "Managers/ConfigManager.h"

CTargetGump g_TargetGump;
CTargetGump g_AttackTargetGump;
CNewTargetSystem g_NewTargetSystem;

CTargetGump::CTargetGump()
{
}

CTargetGump::~CTargetGump()
{
}

void CTargetGump::Draw()
{
    if (Color != 0)
    {
        g_Orion.DrawGump(0x1068, Color, X, Y);

        if (Hits > 0)
        {
            g_Orion.DrawGump(0x1069, HealthColor, X, Y, Hits, 0);
        }
    }
}

CNewTargetSystem::CNewTargetSystem()
{
}

CNewTargetSystem::~CNewTargetSystem()
{
}

void CNewTargetSystem::Draw()
{
    if (!g_ConfigManager.DisableNewTargetSystem && ColorGump != 0)
    {
        CIndexObject& top = g_Orion.m_GumpDataIndex[GumpTop];

        int x = X - (top.Width / 2);

        g_Orion.DrawGump(GumpTop, 0, x, TopY - top.Height);
        g_Orion.DrawGump(ColorGump, 0, x, TopY - top.Height);
        g_Orion.DrawGump(GumpBottom, 0, x, BottomY);

        if (Hits > 0)
        {
            g_Orion.DrawGump(0x1069, HealthColor, X - 16, BottomY + 15, Hits, 0);
        }
    }
}
