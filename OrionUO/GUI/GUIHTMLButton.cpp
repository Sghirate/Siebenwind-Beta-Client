#include "GUIHTMLButton.h"
#include "GUIHTMLGump.h"
#include "Globals.h"
#include <GL/glew.h>

CGUIHTMLButton::CGUIHTMLButton(
    CGUIHTMLGump* htmlGump,
    int serial,
    u16 graphic,
    u16 graphicSelected,
    u16 graphicPressed,
    int x,
    int y)
    : CGUIButton(serial, graphic, graphicSelected, graphicPressed, x, y)
    , m_HTMLGump(htmlGump)
{
}

CGUIHTMLButton::~CGUIHTMLButton()
{
}

void CGUIHTMLButton::SetShaderMode()
{
    glUniform1iARB(g_ShaderDrawMode, SDM_NO_COLOR);
}

void CGUIHTMLButton::Scroll(bool up, int delay)
{
    if (m_HTMLGump != nullptr)
    {
        m_HTMLGump->Scroll(up, delay);
    }
}
