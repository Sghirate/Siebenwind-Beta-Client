#include "PartyObject.h"
#include "GameWorld.h"
#include "GameCharacter.h"

CPartyObject::CPartyObject()
{
}

std::string CPartyObject::GetName(int index)
{
    if (Serial != 0u)
    {
        if (Character == nullptr)
        {
            Character = g_World->FindWorldCharacter(Serial);
        }
        if (Character != nullptr)
        {
            return Character->GetName();
        }
    }

    char buf[10] = { 0 };
    sprintf_s(buf, "[%i]", index);

    return string(buf);
}
