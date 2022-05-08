#pragma once

#include "Core/Minimal.h"
#include "Core/Keys.h"
#include "BaseQueue.h"
#include "Platform.h"
#include "plugin/enumlist.h"
#include <filesystem>
#include <string>
#include <vector>

class MacroManager : public CBaseQueue
{
public:
    bool WaitingBandageTarget     = false;
    u32 WaitForTargetTimer        = 0;
    bool SendNotificationToPlugin = false;

private:
    u32 m_NextTimer{ 0 };
    static u8 m_SkillIndexTable[24];

    void ProcessSubMenu();
    Core::EKey ConvertStringToKeyCode(const std::vector<std::string>& a_strings);

public:
    MacroManager();
    virtual ~MacroManager();

    class Macro* FindMacro(Core::EKey key, bool alt, bool ctrl, bool shift);
    bool Convert(const std::filesystem::path& a_path);
    bool Load(const std::filesystem::path& a_path, const std::filesystem::path& a_originalPath);
    void Save(const std::filesystem::path& a_path);
    void LoadFromOptions();
    void ChangePointer(class MacroObject* macro);
    void Execute();
    MACRO_RETURN_CODE Process();
    MACRO_RETURN_CODE Process(class MacroObject* macro);
};

extern MacroManager g_MacroManager;
