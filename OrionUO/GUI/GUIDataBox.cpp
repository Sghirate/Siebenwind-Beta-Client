#include "GUIDataBox.h"

CGUIDataBox::CGUIDataBox()
    : CGUIPolygonal(GOT_DATABOX, 0, 0, 0, 0)
{
}

CGUIDataBox::~CGUIDataBox()
{
}

void CGUIDataBox::PrepareTextures()
{
    QFOR(item, m_Items, CBaseGUI *)
    item->PrepareTextures();
}

bool CGUIDataBox::EntryPointerHere()
{
    QFOR(item, m_Items, CBaseGUI *)
    {
        if (item->Visible && item->EntryPointerHere())
        {
            return true;
        }
    }

    return false;
}
