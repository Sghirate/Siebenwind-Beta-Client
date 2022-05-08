#pragma once

#include "BaseQueue.h"

class COptionsMacroManager : public CBaseQueue
{
public:
    COptionsMacroManager();
    virtual ~COptionsMacroManager();
    void LoadFromMacro();
};

extern COptionsMacroManager g_OptionsMacroManager;
