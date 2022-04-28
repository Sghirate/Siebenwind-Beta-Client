#include "Party.h"
#include "Core/StringUtils.h"
#include "Globals.h"
#include "OrionUO.h"
#include "Network/Packets.h"
#include "Managers/ConfigManager.h"
#include "Managers/GumpManager.h"
#include "Managers/MouseManager.h"
#include "GameObjects/GameWorld.h"
#include "GameObjects/GameCharacter.h"
#include "Gumps/GumpStatusbar.h"

CParty g_Party;

CParty::CParty()
{
}

CParty::~CParty()
{
}

bool CParty::Contains(int a_serial)
{
    bool result = false;
    if (Leader != 0)
    {
        for (int i = 0; i < 10; i++)
        {
            if (Member[i].Serial == a_serial)
            {
                result = true;
                break;
            }
        }
    }
    return result;
}

void CParty::Clear()
{
    for (int i = 0; i < 10; i++)
    {
        Member[i].Serial    = 0;
        Member[i].Character = nullptr;
    }
}

void CParty::ParsePacketData(Core::StreamReader& a_reader)
{
    u8 code = a_reader.ReadLE<u8>();
    switch (code)
    {
        case 1: //Add member
        {
        }
        case 2: //Remove member
        {
            u8 count = a_reader.ReadLE<u8>();
            if (count <= 1)
            {
                Leader  = 0;
                Inviter = 0;
                for (int i = 0; i < 10; i++)
                {
                    CPartyObject& member = Member[i];
                    if (member.Character == nullptr)
                    {
                        break;
                    }
                    CGumpStatusbar* gump = (CGumpStatusbar*)g_GumpManager.UpdateContent(
                        member.Character->Serial, 0, GT_STATUSBAR);
                    if (gump != nullptr)
                    {
                        gump->WantRedraw = true;
                    }
                }
                Clear();
                g_GumpManager.UpdateContent(0, 0, GT_PARTY_MANIFEST);
                break;
            }
            Clear();
            Core::TMousePos oldPos = g_MouseManager.GetPosition();
            Core::TMousePos mousePos(76, 30);
            g_MouseManager.SetPosition(mousePos);
            CGumpStatusbar* prevGump = nullptr;

            for (int i = 0; i < count; i++)
            {
                u32 serial          = a_reader.ReadBE<u32>();
                Member[i].Serial    = serial;
                Member[i].Character = g_World->FindWorldCharacter(serial);
                if (i == 0)
                {
                    g_Party.Leader = serial;
                }

                CGumpStatusbar* gump =
                    (CGumpStatusbar*)g_GumpManager.UpdateContent(serial, 0, GT_STATUSBAR);

                if (gump == nullptr)
                {
                    g_Orion.OpenStatus(serial);
                    gump = (CGumpStatusbar*)g_GumpManager.UpdateContent(serial, 0, GT_STATUSBAR);

                    if (serial == g_PlayerSerial)
                    {
                        gump->Minimized = false;
                    }

                    if (prevGump != nullptr)
                    {
                        prevGump->AddStatusbar(gump);
                    }

                    prevGump = gump;
                    mousePos.y += 59;
                    g_MouseManager.SetPosition(mousePos);
                }
                else
                {
                    CPacketStatusRequest(serial).Send();
                    gump->WantRedraw = true;
                }
            }

            g_MouseManager.SetPosition(oldPos);
            g_GumpManager.UpdateContent(0, 0, GT_PARTY_MANIFEST);
            break;
        }
        case 3: //Private party message
        case 4: //Party message
        {
            u32 serial        = a_reader.ReadBE<u32>();
            std::wstring name = a_reader.ReadWStringBE();

            for (int i = 0; i < 10; i++)
            {
                if (Member[i].Serial == serial)
                {
                    std::string str =
                        "[" + Member[i].GetName((int)i) + "]: " + Core::ToString(name);
                    g_Orion.CreateTextMessage(
                        TT_SYSTEM, serial, 3, g_ConfigManager.PartyMessageColor, str);

                    break;
                }
            }

            break;
        }
        case 7: //Party invition
        {
            g_Party.Inviter = a_reader.ReadBE<u32>();
            break;
        }
        default: break;
    }
}
