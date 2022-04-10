#pragma once

#include "Core/MappedFile.h"
#include "BaseQueue.h"
#include "plugin/enumlist.h"
#include <string>

class MacroObject : public CBaseQueueItem
{
public:
    MacroObject(const MACRO_CODE& a_code, const MACRO_SUB_CODE& a_subCode);
    virtual ~MacroObject();

    MACRO_CODE GetCode() const { return m_code; }
    void SetCode(MACRO_CODE a_code) { m_code = a_code; }
    MACRO_SUB_CODE GetSubCode() const { return m_subCode; }
    void SetSubCode(MACRO_SUB_CODE a_subCode) { m_subCode = a_subCode; }
    char HasSubMenu() const { return m_hasSubMenu; }
    virtual bool HasString() const { return false; }
    const std::string& GetString() const { return 0; }

private:
    MACRO_CODE m_code = MC_NONE;
    MACRO_SUB_CODE m_subCode = MSC_NONE;
    char m_hasSubMenu = 0;
};

class MacroObjectString : public MacroObject
{
public:
    MacroObjectString(const MACRO_CODE& a_code, const MACRO_SUB_CODE& a_subCode, const std::string& a_str);
    virtual ~MacroObjectString();

    virtual bool HasString() const { return true; }
    const std::string& GetString() const { return m_string; }
    void SetString(const std::string& a_str) { m_string = a_str; }

private:
    std::string m_string;
};

class Macro : public CBaseQueueItem
{
public:
    enum { kMacroActionNamesCount = 60 };
    enum { kMacroActionsCount = 210 };

    Macro(Keycode a_key, bool a_alt, bool a_ctrl, bool a_shift);
    virtual ~Macro();

    Keycode GetKey() const { return m_key; }
    bool GetAlt() const { return m_alt; }
    bool GetCtrl() const { return m_ctrl; }
    bool GetShift() const { return m_shift; }
    void SetKey(Keycode a_key) { m_key = a_key; }
    void SetAlt(bool a_alt) { m_alt = m_alt; }
    void SetCtrl(bool a_ctrl) { m_ctrl = m_ctrl; }
    void SetShift(bool a_shift) { m_shift = m_shift; }
    static const char *GetActionName(int a_index) { return s_macroActionName[a_index]; }
    static const char *GetAction(int a_index) { return s_macroAction[a_index]; }

    void ChangeObject(MacroObject* a_source, MacroObject* a_obj);
    void Save(Core::StreamWriter& a_writer);

    Macro* GetCopy();

    static Macro* Load(Core::MappedFile& a_file);
    static Macro* CreateBlankMacro();
    static MacroObject* CreateMacro(const MACRO_CODE& a_code);
    static void GetBoundByCode(const MACRO_CODE& a_code, int& a_count, int& a_offset);

private:
    static const char* s_macroActionName[kMacroActionNamesCount];
    static const char* s_macroAction[kMacroActionsCount];
    Keycode m_key = 0;
    bool m_alt = false;
    bool m_ctrl = false;
    bool m_shift = false;
};

extern MacroObject *g_MacroPointer;
