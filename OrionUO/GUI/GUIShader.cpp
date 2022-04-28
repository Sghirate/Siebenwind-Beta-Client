#include "GUIShader.h"

CGUIShader::CGUIShader(CGLShader *shader, bool enabled)
    : CBaseGUI(GOT_SHADER, 0, 0, 0, 0, 0)
    , m_Shader(shader)
{
    Enabled = enabled;
}

CGUIShader::~CGUIShader()
{
}

void CGUIShader::Draw(bool checktrans)
{
    if (Enabled && m_Shader != nullptr)
    {
        m_Shader->Use();
    }
    else
    {
        UnuseShader();
    }
}
