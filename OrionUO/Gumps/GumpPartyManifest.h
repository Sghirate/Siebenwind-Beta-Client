#pragma once

#include "Core/Minimal.h"
#include "Gump.h"

class CGumpPartyManifest : public CGump
{
private:
    bool m_CanLoot = false;

public:
    CGumpPartyManifest(u32 serial, short x, short y, bool canLoot);
    virtual ~CGumpPartyManifest();

     void UpdateContent()  override;

    GUMP_BUTTON_EVENT_H;
};
