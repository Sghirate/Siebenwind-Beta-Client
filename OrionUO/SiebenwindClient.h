// MIT License
// Copyright (C) December 2018 heppcatt

#pragma once

#include <string>
#include <sstream>
#include <windows.h>

using namespace std;

class SiebenwindClient
{
public:
    static const string WindowTitle;
    static string GetBuildInfo();
    
    static std::string Iso8859ToUtf8(const std::string &iso8859);
    static std::wstring Utf8ToUtf16(const std::string &utf8);
    static std::wstring Iso8859ToUtf16(const std::string &iso8859);

    static string LocalizeA(const string &untranslated);
    static wstring LocalizeW(const wstring &untranslated);
};
