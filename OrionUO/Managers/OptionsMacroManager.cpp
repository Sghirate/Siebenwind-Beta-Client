#include "OptionsMacroManager.h"
#include "MacroManager.h"
#include "../Macro.h"

COptionsMacroManager g_OptionsMacroManager;

COptionsMacroManager::COptionsMacroManager()
{
}

COptionsMacroManager::~COptionsMacroManager()
{
}

void COptionsMacroManager::LoadFromMacro()
{
    Clear();

    QFOR(obj, g_MacroManager.m_Items, Macro *) { Add(obj->GetCopy()); }

    if (m_Items == nullptr)
    {
        Add(Macro::CreateBlankMacro());
    }
}
