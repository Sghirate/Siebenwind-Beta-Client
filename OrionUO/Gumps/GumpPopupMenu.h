#pragma once

#include "Gump.h"

namespace Core { struct PacketReader; }

class CPopupMenuItemInfo
{
public:
    u32 Cliloc = 0;
    u16 Index = 0;
    u16 Flags = 0;
    u16 Color = 0;
    u16 ReplaceColor = 0;

    CPopupMenuItemInfo() {}
};

class CGumpPopupMenu : public CGump
{
    int Width = 0;
    int Height = 0;

private:
    CGUIColoredPolygone *m_Polygone{ nullptr };

public:
    CGumpPopupMenu(u32 serial, short x, short y);
    virtual ~CGumpPopupMenu();

    static const int ID_GPM_MAXIMIZE = 0xDEADBEEF;

    virtual void PrepareContent();

    static void Parse(Core::PacketReader &reader);

    GUMP_BUTTON_EVENT_H;
};

extern CGumpPopupMenu *g_PopupMenu;
