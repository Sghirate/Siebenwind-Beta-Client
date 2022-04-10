#include "ClilocManager.h"
#include "Globals.h"
#include "Core/StringUtils.h"
#include "OrionApplication.h"
#include <vector>

ClilocManager g_ClilocManager;

Cliloc::Cliloc(const std::string& a_lang)
{
    m_loaded   = false;
    m_language = a_lang;
    if (m_language.length() != 0u)
    {
        auto file                  = std::string("Cliloc.") + a_lang;
        std::filesystem::path path = g_App.GetGameDir() / file;
        m_loaded                   = m_file.Load(path);
    }
}

Cliloc::~Cliloc()
{
    m_file.Unload();
    m_system.clear();
    m_regular.clear();
    m_support.clear();
}

std::string Cliloc::Load(u32& a_id)
{
    std::string result;
    if (m_loaded)
    {
        m_file.ResetPtr();
        m_file.Move(6);
        while (!m_file.IsEOF())
        {
            auto currentID = m_file.ReadLE<u32>();
            m_file.Move(1);
            short len = m_file.ReadLE<u16>();
            if (currentID == a_id)
            {
                if (len > 0)
                    result = m_file.ReadString(len);

                if (a_id >= 3000000)
                    m_support[currentID] = result;
                else if (a_id >= 1000000)
                    m_regular[currentID] = result;
                else
                    m_system[currentID] = result;

                return result;
            }

            m_file.Move(len);
        }
    }
    a_id = 0;
    return result;
}

std::wstring Cliloc::CamelCaseTest(bool a_toCamelCase, const std::string& a_result)
{
    if (a_toCamelCase)
        return Core::ToCamelCaseW(Core::DecodeUTF8(a_result));
    return Core::DecodeUTF8(a_result);
}

std::wstring Cliloc::GetX(int a_id, bool a_toCamelCase, const std::string& a_result)
{
    std::string result = a_result;
    if (a_id >= 3000000)
    {
        TClilocMap::iterator it = m_support.find(a_id);
        if (it != m_support.end() && ((*it).second.length() != 0u))
            return CamelCaseTest(a_toCamelCase, (*it).second);
    }
    else if (a_id >= 1000000)
    {
        TClilocMap::iterator it = m_regular.find(a_id);
        if (it != m_regular.end() && ((*it).second.length() != 0u))
            return CamelCaseTest(a_toCamelCase, (*it).second);
    }
    else
    {
        TClilocMap::iterator it = m_system.find(a_id);
        if (it != m_system.end() && ((*it).second.length() != 0u))
            return CamelCaseTest(a_toCamelCase, (*it).second);
    }

    u32 tmpID    = a_id;
    auto loadStr = Load(tmpID);
    if (loadStr.length() != 0u)
        return CamelCaseTest(a_toCamelCase, loadStr);

    if (tmpID == a_id && (loadStr.length() == 0u))
        return {};

    if (m_language != "ENU" && m_language != "enu")
        return g_ClilocManager.GetCliloc("enu")->GetW(a_id, a_toCamelCase, result);

    if (result.length() == 0u)
    {
        char str[50]{};
        sprintf_s(str, "Unknown Cliloc #%i", a_id);
        result = str;
    }

    return CamelCaseTest(a_toCamelCase, result);
}

std::string Cliloc::GetA(int a_id, bool a_toCamelCase, std::string a_result)
{
    return Core::ToString(GetX(a_id, a_toCamelCase, a_result));
}

std::wstring Cliloc::GetW(int a_id, bool a_toCamelCase, std::string a_result)
{
    return GetX(a_id, a_toCamelCase, a_result);
}

ClilocManager::ClilocManager()

{
}

ClilocManager::~ClilocManager()
{
    m_enuCliloc  = nullptr;
    m_lastCliloc = nullptr;
}

Cliloc* ClilocManager::GetCliloc(const std::string& a_lang)
{
    auto language = Core::ToLowerA(a_lang);
    if (language.length() == 0u)
        language = "enu";

    if (language == "enu")
    {
        if (m_enuCliloc == nullptr)
        {
            m_enuCliloc = (Cliloc*)Add(new Cliloc(language));
        }
        return m_enuCliloc;
    }

    if (m_lastCliloc != nullptr && m_lastCliloc->GetLanguage() == language)
        return m_lastCliloc->IsLoaded() ? m_lastCliloc : m_enuCliloc;

    QFOR(obj, m_Items, Cliloc*)
    {
        if (obj->GetLanguage() == language)
        {
            if (!obj->IsLoaded())
                return m_enuCliloc;
            m_lastCliloc = obj;
            return obj;
        }
    }

    Cliloc* obj = (Cliloc*)Add(new Cliloc(language));
    if (!obj->IsLoaded())
        return GetCliloc("enu");

    m_lastCliloc = obj;
    return obj;
}

std::wstring ClilocManager::ParseArgumentsToClilocString(
    int a_cliloc, bool a_toCamelCase, const std::wstring& a_args)
{
    std::wstring args(a_args);
    while ((args.length() != 0u) && args[0] == L'\t')
        args.erase(args.begin());

    std::wstring message = GetCliloc(g_Language)->GetW(a_cliloc, a_toCamelCase);
    std::vector<std::wstring> arguments;
    while (true)
    {
        size_t pos = args.find(L'\t');
        if (pos != std::string::npos)
        {
            arguments.push_back(args.substr(0, pos));
            args = args.substr(pos + 1);
        }
        else
        {
            arguments.push_back(args);
            break;
        }
    }

    for (int i = 0; i < (int)arguments.size(); i++)
    {
        size_t pos1 = message.find(L'~');
        if (pos1 == std::string::npos)
            break;

        size_t pos2 = message.find(L'~', pos1 + 1);
        if (pos2 == std::string::npos)
            break;

        if (arguments[i].length() > 1 && *arguments[i].c_str() == L'#')
        {
            u32 id       = std::stoi(arguments[i].c_str() + 1);
            arguments[i] = GetCliloc(g_Language)->GetW(id, a_toCamelCase);
        }

        message.replace(pos1, pos2 - pos1 + 1, arguments[i]);
    }

    if (a_toCamelCase)
        return ToCamelCaseW(message);
    return message;
}
