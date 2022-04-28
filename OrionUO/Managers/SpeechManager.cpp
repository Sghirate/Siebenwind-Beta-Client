#include "SpeechManager.h"
#include "Core/DataStream.h"
#include "Core/Log.h"
#include "Core/MappedFile.h"
#include "Core/StringUtils.h"
#include "GameVars.h"
#include "Globals.h"
#include "FileManager.h"
#include "../Config.h"
#include <assert.h>

CSpeechManager g_SpeechManager;

CSpeechItem::CSpeechItem(u16 code, const std::wstring &data)
    : Code(code)
    , Data(data)
{
    if ((Data.length() != 0u) && Data[Data.length() - 1] == L'*')
    {
        CheckEnd = true;
        Data.resize(Data.length() - 1);
    }

    if ((Data.length() != 0u) && Data[0] == L'*')
    {
        CheckStart = true;
        Data.erase(Data.begin());
    }

    //LOG(L"[0x%04X]=(cs=%i, ce=%i) %s\n", m_Code, m_CheckStart, m_CheckEnd, m_Data.c_str());
}

CSpeechManager::CSpeechManager()
{
}

CSpeechManager::~CSpeechManager()
{
    m_SpeechEntries.clear();
    m_LangCodes.clear();
}

bool CSpeechManager::LoadSpeech()
{
    LoadLangCodes();

    for (int i = 0; i < (int)m_LangCodes.size(); i++)
    {
        if (m_LangCodes[i].Abbreviature == g_Language)
        {
            CurrentLanguage = &m_LangCodes[i];
            break;
        }
    }

    if (CurrentLanguage == nullptr)
    {
        CurrentLanguage = &m_LangCodes[0];
        g_Language = m_LangCodes[0].Abbreviature;
    }
    LOG_INFO("SpeechManager", "Selected language: %s", g_Language.c_str());

    Core::StreamReader reader;
    std::vector<u8> tempData;
    bool isUOP = false;
    if (g_FileManager.m_MainMisc.GetBuffer() != nullptr)
    {
        CUopBlockHeader *block = g_FileManager.m_MainMisc.GetBlock(0x0891F809004D8081);
        if (block != nullptr)
        {
            tempData = g_FileManager.m_MainMisc.GetData(*block);
            reader.SetData(&tempData[0], tempData.size());
            isUOP = true;
        }
    }

    if (reader.GetBuffer() == nullptr)
    {
        reader.SetData(g_FileManager.m_SpeechMul.GetBuffer(), g_FileManager.m_SpeechMul.GetSize());
    }

    if (isUOP)
    {
        LOG_INFO("SpeechManager", "Loading speech from UOP");
        reader.Move(2);
        std::wstring mainData = reader.ReadWStringLE(reader.GetSize() - 2);
        std::vector<std::wstring> list;
        std::wstring temp;
        for (const wchar_t &c : mainData)
        {
            if (c == 0x000D || c == 0x000A)
            {
                if (temp.length() != 0u)
                {
                    list.push_back(temp);
                    temp = {};
                }
            }
            else
            {
                temp.push_back(c);
            }
        }

        if (temp.length() != 0u)
        {
            list.push_back(temp);
            temp = {};
        }

        for (const std::wstring &line : list)
        {
            u16 code = 0xFFFF;
            temp = {};
            for (const wchar_t c : line)
            {
                if (c == 0x0009)
                {
                    if (temp.length() != 0u)
                    {
                        code = std::stoi(temp);
                        temp = {};
                    }
                }
                else
                {
                    temp.push_back(c);
                }
            }

            if ((temp.length() != 0u) && code != 0xFFFF)
            {
                m_SpeechEntries.push_back(CSpeechItem(code, temp));
            }
        }
    }
    else
    {
        LOG_INFO("SpeechManager", "Loading speech from MUL");
        while (!reader.IsEOF())
        {
            const u16 code = reader.ReadBE<u16>();
            const int len = reader.ReadBE<u16>();
            if (len == 0)
            {
                continue;
            }

            std::wstring str = Core::DecodeUTF8(reader.ReadString(len));
            m_SpeechEntries.push_back(CSpeechItem(code, str));
        }
    }

    LOG_INFO("SpeechManager", "m_SpeechEntries.size()=%zi", m_SpeechEntries.size());
    m_Loaded = true;
    return true;
}

bool CSpeechManager::LoadLangCodes()
{

    m_LangCodes.push_back(CLangCode("enu", 101, "English", "United States"));
    Core::MappedFile &file = g_FileManager.m_LangcodeIff;

    file.ReadString(36);
    while (!file.IsEOF())
    {
        CLangCode langCodeData;
        file.Move(4);

        const u32 entryLen = file.ReadBE<u32>();
        langCodeData.Abbreviature = file.ReadString();
        langCodeData.Code = file.ReadLE<u32>();
        langCodeData.Language = file.ReadString();
        langCodeData.Country = file.ReadString();
        if (((langCodeData.Language.length() + langCodeData.Country.length() + 2) % 2) != 0u)
        {
            int nullTerminator = file.ReadBE<u8>();
            assert(
                nullTerminator == 0 &&
                "speechManager @ 138, invalid null terminator in langcodes.iff");
        }

        m_LangCodes.push_back(langCodeData);
        //LOG("[0x%04X]: %s\n", langCodeData.Code, langCodeData.Abbreviature.c_str());
    }

    //if (m_LangCodes.size() != 135)
    //	return false;

    return true;
}

void CSpeechManager::GetKeywords(const wchar_t *text, std::vector<u32> &codes)
{
    if (!m_Loaded || GameVars::GetClientVersion() < CV_305D)
    {
        return; // But in fact from the client version 2.0.7
    }

    const auto size = (int)m_SpeechEntries.size();
    std::wstring input = Core::ToLowerW(text);
    for (int i = 0; i < size; i++)
    {
        CSpeechItem entry = m_SpeechEntries[i];
        std::wstring data = entry.Data;

        if (data.length() > input.length() || data.length() == 0)
        {
            continue;
        }

        if (!entry.CheckStart)
        {
            std::wstring start = input.substr(0, data.length());
            size_t hits = start.find(data);
            if (hits == std::wstring::npos)
            {
                continue;
            }
        }

        if (!entry.CheckEnd)
        {
            std::wstring end = input.substr(input.length() - data.length());
            size_t hits = end.find(data);
            if (hits == std::wstring::npos)
            {
                continue;
            }
        }

        size_t hits = input.find(data);
        if (hits != std::wstring::npos)
        {
            codes.push_back(entry.Code);
        }
    }
}
