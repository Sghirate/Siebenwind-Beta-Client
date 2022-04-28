#pragma once

#include "BaseQueue.h"
#include "Core/MappedFile.h"
#include <string>
#include <vector>

class Intloc : public CBaseQueueItem
{
public:
    Intloc(int a_fileIndex, const std::string& a_lang);
    virtual ~Intloc();

    const std::string& GetLanguage() const { return m_language; }
    int GetFileIndex() const { return m_fileIndex; }
    bool IsLoaded() const { return m_loaded; }
    std::wstring Get(int a_id, bool a_toCamelCase = false);

private:
    Core::MappedFile m_file;
    std::string m_language = "";
    int m_fileIndex        = -1;
    bool m_loaded          = false;
    std::vector<std::wstring> m_strings;
};

class IntlocManager : public CBaseQueue
{
private:
    Intloc* GetIntloc(int a_fileIndex, const std::string& a_lang);

public:
    IntlocManager();
    virtual ~IntlocManager();

    std::wstring GetIntloc(const std::string& a_lang, u32 a_clilocID, bool a_isNewCliloc);
};

extern IntlocManager g_IntlocManager;
