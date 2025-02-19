#include "GUIBlending.h"

CGUIBlending::CGUIBlending(bool enabled, GLenum sFactor, GLenum dFactor)
    : CBaseGUI(GOT_BLENDING, 0, 0, 0, 0, 0)
    , SFactor(sFactor)
    , DFactor(dFactor)
{
    Enabled = enabled;
}

CGUIBlending::~CGUIBlending()
{
}

void CGUIBlending::Draw(bool checktrans)
{
    if (Enabled)
    {
        glEnable(GL_BLEND);
        glBlendFunc(SFactor, DFactor);
    }
    else
    {
        glDisable(GL_BLEND);
    }
}
