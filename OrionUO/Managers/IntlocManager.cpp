#include "IntlocManager.h"
#include "ClilocManager.h"
#include "Core/StringUtils.h"
#include "OrionApplication.h"

IntlocManager g_IntlocManager;

Intloc::Intloc(int a_fileIndex, const std::string& a_lang)
{
    m_loaded    = false;
    m_language  = a_lang;
    m_fileIndex = a_fileIndex;

    if (m_language.length() != 0u)
    {
        char fileName[64];
        std::snprintf(fileName, 64, "intloc%02i.%s", a_fileIndex, a_lang.c_str());
        std::filesystem::path filePath = g_App.GetGameDir() / fileName;
        if (m_file.Load(filePath))
        {
            while (!m_file.IsEOF())
            {
                u32 code = m_file.ReadBE<u32>();

                if (code == 'TEXT')
                {
                    int len = m_file.ReadBE<i32>();
                    u8* end = m_file.GetPtr() + len;
                    while (m_file.GetPtr() < end && !m_file.IsEOF())
                        m_strings.push_back(Core::DecodeUTF8(m_file.ReadString()));
                }
                else if (code == 'FORM')
                {
                    m_file.Move(4);
                }
                else if (code == 'INFO')
                {
                    int len = m_file.ReadBE<i32>();
                    m_file.Move(len + 1);
                }
                else if (code == 'DATA' || code == 'LANG')
                {
                }
                else
                {
                    break;
                }
            }
            m_loaded = !m_strings.empty();
        }
    }
}

Intloc::~Intloc()
{
    m_file.Unload();
    m_strings.clear();
}

std::wstring Intloc::Get(int a_id, bool a_toCamelCase)
{
    if (a_id < (int)m_strings.size())
    {
        if (a_toCamelCase)
            return Core::ToCamelCaseW(m_strings[a_id]);
        return m_strings[a_id];
    }
    return {};
}

IntlocManager::IntlocManager()
{
}

IntlocManager::~IntlocManager()
{
}

Intloc* IntlocManager::GetIntloc(int a_fileIndex, const std::string& a_lang)
{
    QFOR(obj, m_Items, Intloc*)
    {
        if (obj->GetLanguage() == a_lang && obj->GetFileIndex() == a_fileIndex)
            return obj->IsLoaded() ? obj : nullptr;
    }

    {
        Intloc* obj = (Intloc*)Add(new Intloc(a_fileIndex, a_lang));
        if (obj->IsLoaded())
            return obj;
    }

    QFOR(obj, m_Items, Intloc*)
    {
        if (obj->GetLanguage() == "enu" && obj->GetFileIndex() == a_fileIndex)
            return obj->IsLoaded() ? obj : nullptr;
    }

    return nullptr;
}

std::wstring IntlocManager::GetIntloc(const std::string& a_lang, u32 a_clilocID, bool a_isNewCliloc)
{
    std::string language = Core::ToLowerA(a_lang);
    if (language.length() == 0u)
        language = "enu";

    std::wstring str = {};
    if (str.length() == 0u)
        str = g_ClilocManager.GetCliloc(a_lang)->GetW(a_clilocID, true);

    return str;
}
