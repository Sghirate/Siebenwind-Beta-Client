#include "GUIHTMLResizepic.h"
#include "GUIHTMLGump.h"

CGUIHTMLResizepic::CGUIHTMLResizepic(
    class CGUIHTMLGump *htmlGump, int serial, u16 graphic, int x, int y, int width, int height)
    : CGUIResizepic(serial, graphic, x, y, width, height)
    , m_HTMLGump(htmlGump)
{
}

CGUIHTMLResizepic::~CGUIHTMLResizepic()
{
}

void CGUIHTMLResizepic::Scroll(bool up, int delay)
{
    if (m_HTMLGump != nullptr)
    {
        m_HTMLGump->Scroll(up, delay);
    }
}
