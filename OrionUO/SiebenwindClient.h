#pragma once

#include <string>
#include <sstream>

struct SiebenwindClient
{
    static std::string GetBuildInfo();

    static const std::string& LocalizeA(const std::string& a_untranslated);
    static const std::wstring& LocalizeW(const std::wstring& a_untranslated);
    static const std::string& GetWindowTitle();
};
