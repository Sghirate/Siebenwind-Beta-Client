#pragma once

#include "Core/MappedFile.h"
#include <array>
#include <filesystem>
#include <string>
#include <vector>

namespace Core
{

class TextFileParser
{
    enum { kMaxTokensPerCategory = 32 };

public:
    TextFileParser(
        const std::filesystem::path& a_path,
        const char* a_delimiterTokens = "",
        const char* a_commentTokens = "",
        const char* a_quoteTokens = "");
    ~TextFileParser();

    void Restart();
    bool IsEOF();
    const std::string& GetRawLine() const { return m_rawLine; }

    std::vector<std::string> ReadTokens(bool a_trim = true);
    std::vector<std::string> GetTokens(const char* a_str, bool a_trim = true);

private:
    void GetEOL();
    bool IsDelimiter();
    void SkipToData();
    bool IsComment();
    bool IsQuote();
    bool IsSecondQuote();
    std::string ObtainData();
    std::string ObtainQuotedData();
    void StartupInitalize(const char* a_delimiterTokens, const char* a_commentTokens, const char* a_quoteTokens);
    void SaveRawLine();

private:
    Core::MappedFile m_File;
    std::string m_rawLine = "";
    u8* m_ptr = nullptr;
    u8* m_eol = nullptr;
    u8* m_end = nullptr;
    char m_delimiterTokens[kMaxTokensPerCategory];
    char m_commentTokens[kMaxTokensPerCategory];
    char m_quoteTokens[kMaxTokensPerCategory];
    int m_delimiterTokenCount = 0;
    int m_commentTokenCount = 0;
    int m_quoteTokenCount = 0;
    bool m_trim = false;
};

}; // namespace Core
