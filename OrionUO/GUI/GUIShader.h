#pragma once

#include "BaseGUI.h"
#include "GLEngine/GLShader.h"

class CGUIShader : public CBaseGUI
{
private:
    CGLShader* m_Shader{ nullptr };

public:
    CGUIShader(CGLShader* shader, bool enabled);
    virtual ~CGUIShader();

    virtual void Draw(bool checktrans = false);
};
