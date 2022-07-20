#include "UseItemsList.h"
#include "Globals.h"
#include "OrionUO.h"
#include "GameObjects/GameWorld.h"

CUseItemActions g_UseItemActions;

void CUseItemActions::Add(int serial)
{
    for (std::deque<u32>::iterator i = m_List.begin(); i != m_List.end(); ++i)
    {
        if (*i == serial)
        {
            return;
        }
    }

    m_List.push_back(serial);
}

void CUseItemActions::Process()
{
    if (Timer <= g_Ticks)
    {
        Timer = g_Ticks + 1000;

        if (m_List.empty())
        {
            return;
        }

        u32 serial = m_List.front();
        m_List.pop_front();

        if (g_World->FindWorldObject(serial) != nullptr)
        {
            if (serial < 0x40000000)
            { //NPC
                g_Orion.PaperdollReq(serial);
            }
            else
            { //item
                g_Orion.DoubleClick(serial);
            }
        }
    }
}
