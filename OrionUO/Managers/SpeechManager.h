#pragma once

#include "../IndexObject.h"

class CLangCode
{
public:
    int Code = 0;
    string Abbreviature = "";
    u32 Unknown = 0;
    string Language = "";
    string Country = "";

    CLangCode() {}
    CLangCode(const std::string &abbreviature, int code, const std::string &language, const std::string &country)
        : Abbreviature(abbreviature)
        , Code(code)
        , Language(language)
        , Country(country)
    {
    }
    virtual ~CLangCode() {}
};

class CSpeechItem
{
public:
    u16 Code = 0;
    std::wstring Data;

    bool CheckStart = false;
    bool CheckEnd = false;

    CSpeechItem() {}
    CSpeechItem(u16 code, const std::wstring &data);
    virtual ~CSpeechItem() {}
};

class CSpeechManager
{
    CLangCode *CurrentLanguage = nullptr;

private:
    std::vector<CSpeechItem> m_SpeechEntries;
    std::vector<CLangCode> m_LangCodes;
    bool m_Loaded = false;

public:
    CSpeechManager();
    ~CSpeechManager();

    bool LoadSpeech();
    bool LoadLangCodes();
    void GetKeywords(const wchar_t *text, std::vector<u32> &codes);
};

extern CSpeechManager g_SpeechManager;
