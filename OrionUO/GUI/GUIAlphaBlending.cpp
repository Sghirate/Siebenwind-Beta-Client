#include "GUIAlphaBlending.h"

CGUIAlphaBlending::CGUIAlphaBlending(bool enabled, float alpha)
    : CGUIBlending(enabled, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)
    , Alpha(alpha)
{
}

CGUIAlphaBlending::~CGUIAlphaBlending()
{
}

void CGUIAlphaBlending::Draw(bool checktrans)
{
    CGUIBlending::Draw(checktrans);

    if (Enabled)
    {
        glColor4f(1.0f, 1.0f, 1.0f, Alpha);
    }
    else
    {
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    }
}
