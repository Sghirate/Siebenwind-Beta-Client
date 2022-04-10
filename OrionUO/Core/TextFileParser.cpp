#include "Core/TextFileParser.h"

namespace Core
{

TextFileParser::TextFileParser(
        const std::filesystem::path& a_path,
        const char* a_delimiterTokens,
        const char* a_commentTokens,
        const char* a_quoteTokens)
{
    if (!a_path.empty())
        m_File.Load(a_path);

    StartupInitalize(a_delimiterTokens, a_commentTokens, a_quoteTokens);
}

TextFileParser::~TextFileParser()
{
    m_File.Unload();
}

void TextFileParser::StartupInitalize(const char* a_delimiterTokens, const char* a_commentTokens, const char* a_quoteTokens)
{
    memset(&m_delimiterTokens[0], 0, sizeof(m_delimiterTokens));
    m_delimiterTokenCount = (int)strlen(a_delimiterTokens);
    if (m_delimiterTokenCount > kMaxTokensPerCategory) m_delimiterTokenCount = kMaxTokensPerCategory;
    if (m_delimiterTokenCount != 0)
        memcpy(&m_delimiterTokens[0], &a_delimiterTokens[0], m_delimiterTokenCount);

    memset(&m_commentTokens[0], 0, sizeof(m_commentTokens));
    m_commentTokenCount = (int)strlen(a_commentTokens);
    if (m_commentTokenCount > kMaxTokensPerCategory) m_commentTokenCount = kMaxTokensPerCategory;
    if (m_commentTokenCount != 0)
        memcpy(&m_commentTokens[0], &a_commentTokens[0], m_commentTokenCount);

    memset(&m_quoteTokens[0], 0, sizeof(m_quoteTokens));
    m_quoteTokenCount = (int)strlen(a_quoteTokens);
    if (m_quoteTokenCount > kMaxTokensPerCategory) m_quoteTokenCount = kMaxTokensPerCategory;
    if (m_quoteTokenCount != 0)
        memcpy(&m_quoteTokens[0], &a_quoteTokens[0], m_quoteTokenCount);

    m_end = m_File.GetBuffer() + m_File.GetSize();
}

void TextFileParser::Restart()
{
    m_File.ResetPtr();
}

bool TextFileParser::IsEOF()
{
    return m_ptr >= m_end;
}

void TextFileParser::GetEOL()
{
    m_eol = m_File.GetPtr();
    if (m_eol < m_end)
    {
        while (m_eol < m_end && (*m_eol != 0u))
        {
            if (*m_eol == '\n')
                break;
            ++m_eol;
        }
    }
}

bool TextFileParser::IsDelimiter()
{
    for (int i = 0; i < m_delimiterTokenCount; i++)
    {
        if (*m_ptr == m_delimiterTokens[i])
            return true;
    }
    return false;
}

void TextFileParser::SkipToData()
{
    while (m_ptr < m_eol && (*m_ptr != 0u) && IsDelimiter())
        ++m_ptr;
}

bool TextFileParser::IsComment()
{
    bool result = (*m_ptr == '\n');
    for (int i = 0; i < m_commentTokenCount && !result; i++)
    {
        result = (*m_ptr == m_commentTokens[i]);
        const bool isTwinToken = m_commentTokens[i] == m_commentTokens[i + 1];
        if (result && i + 1 < m_commentTokenCount && isTwinToken && m_ptr + 1 < m_eol)
        {
            result = (m_ptr[0] == m_ptr[1]);
            i++;
        }
    }
    return result;
}

bool TextFileParser::IsQuote()
{
    bool result = (*m_ptr == '\n');
    for (int i = 0; i < m_quoteTokenCount; i += 2)
    {
        if (*m_ptr == m_quoteTokens[i] || *m_ptr == m_quoteTokens[i + 1])
        {
            result = true;
            break;
        }
    }
    return result;
}

bool TextFileParser::IsSecondQuote()
{
    if (*m_ptr == '\n')
        return true;
    for (int i = 0; i < m_quoteTokenCount; i += 2)
    {
        if (*m_ptr == m_quoteTokens[i + 1])
            return true;
    }
    return false;
}

std::string TextFileParser::ObtainData()
{
    std::string result{};
    while (m_ptr < m_end && (*m_ptr != 0u) && *m_ptr != '\n')
    {
        if (IsDelimiter())
            break;
        if (IsComment())
        {
            m_ptr = m_eol;
            break;
        }

        if (*m_ptr != '\r' && (!m_trim || (*m_ptr != ' ' && *m_ptr != '\t')))
            result.push_back(*m_ptr);
        m_ptr++;
    }
    return result;
}

std::string TextFileParser::ObtainQuotedData()
{
    bool exit = false;
    std::string result{};
    for (int i = 0; i < m_quoteTokenCount; i += 2)
    {
        if (*m_ptr == m_quoteTokens[i])
        {
            char endQuote = m_quoteTokens[i + 1];
            exit = true;
            m_ptr++;
            u8 *ptr = m_ptr;
            while (ptr < m_eol && (*ptr != 0u) && *ptr != '\n' && *ptr != endQuote)
                ++ptr;

            size_t size = ptr - m_ptr;
            if (size > 0)
            {
                result.resize(size + 1);
                memcpy(&result[0], &m_ptr[0], size);
                result[size] = 0;
                for (int j = (int)size - 1; j >= 0 && result[j] == '\r'; j--)
                    result[j] = 0;

                m_ptr = ptr;
                if (m_ptr < m_eol && *m_ptr == endQuote)
                    ++m_ptr;
            }
            break;
        }
    }
    if (!exit)
        result = ObtainData();
    return result;
}

void TextFileParser::SaveRawLine()
{
    size_t size = m_eol - m_ptr;
    if (size > 0)
    {
        m_rawLine.resize(size, 0);
        memcpy(&m_rawLine[0], &m_ptr[0], size);

        while ((m_rawLine.length() != 0u) && (m_rawLine[size - 1] == '\r' || m_rawLine[size - 1] == '\n'))
            m_rawLine.resize(m_rawLine.length() - 1);
    }
    else
    {
        m_rawLine = "";
    }
}

std::vector<std::string> TextFileParser::ReadTokens(bool a_trim)
{
    m_trim = a_trim;
    std::vector<std::string> result;

    if (!IsEOF())
    {
        m_ptr = m_File.GetPtr();
        GetEOL();
        SaveRawLine();
        while (m_ptr < m_eol)
        {
            SkipToData();
            if (IsComment())
                break;

            std::string buf = ObtainQuotedData();
            if (buf.length() != 0u)
                result.push_back(buf);
            else if (IsSecondQuote())
                ++m_ptr;
        }
        m_File.SetPtr(m_eol + 1);
    }
    return result;
}

std::vector<std::string> TextFileParser::GetTokens(const char* a_str, bool a_trim)
{
    m_trim = a_trim;
    std::vector<std::string> result;

    u8* oldEnd = m_end;
    m_ptr = (u8*)a_str;
    m_end = (u8*)a_str + strlen(a_str);
    m_eol = m_end;

    SaveRawLine();
    while (m_ptr < m_eol)
    {
        SkipToData();
        if (IsComment())
            break;
        std::string buf = ObtainQuotedData();
        if (buf.length() != 0u)
            result.push_back(buf);
    }
    m_end = oldEnd;
    return result;
}

}; // namespace Core
