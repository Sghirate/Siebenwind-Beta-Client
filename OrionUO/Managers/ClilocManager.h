#pragma once

#include "BaseQueue.h"
#include "Core/MappedFile.h"
#include <map>
#include <string>

typedef std::map<u32, std::string> TClilocMap;

class Cliloc : public CBaseQueueItem
{
public:
    Cliloc(const std::string& a_lang);
    virtual ~Cliloc();

    const std::string& GetLanguage() const { return m_language; }
    bool IsLoaded() const { return m_loaded; }
    Core::MappedFile& GetFile() { return m_file; }
    std::string GetA(int a_id, bool a_toCamelCase = false, std::string a_result = {});  // FIXME
    std::wstring GetW(int a_id, bool a_toCamelCase = false, std::string a_result = {}); // FIXME

private:
    std::string Load(u32& a_id);
    std::wstring CamelCaseTest(bool a_toCamelCase, const std::string& a_result);
    std::wstring GetX(int a_id, bool a_toCamelCase, const std::string& a_result);

private:
    std::string m_language = "";
    bool m_loaded          = false;
    Core::MappedFile m_file;
    // System (id < 1000000)
    TClilocMap m_system;
    // Regular (id >= 1000000 && id < 3000000)
    TClilocMap m_regular;
    // Support (id >= 3000000)
    TClilocMap m_support;
};

class ClilocManager : public CBaseQueue
{
public:
    ClilocManager();
    virtual ~ClilocManager();

    Cliloc* GetCliloc(const std::string& a_lang);
    std::wstring
    ParseArgumentsToClilocString(int a_cliloc, bool a_toCamelCase, const std::wstring& a_args);

private:
    class Cliloc* m_lastCliloc = nullptr;
    class Cliloc* m_enuCliloc  = nullptr;
};

extern ClilocManager g_ClilocManager;
