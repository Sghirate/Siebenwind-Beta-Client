// MIT License
// Copyright (C) December 2018 heppcatt

#include "SiebenwindClient.h"

const string SiebenwindClient::WindowTitle =
    "Siebenwind Beta-Client " + SiebenwindClient::GetBuildInfo();

string SiebenwindClient::GetBuildInfo()
{
    DWORD dwSize = 0;
    BYTE *pVersionInfo = NULL;
    VS_FIXEDFILEINFO *pFileInfo = NULL;
    UINT pLenFileInfo = 0;

	// Get the name of the executable.
    char filename[MAX_PATH];
    GetModuleFileNameA(NULL, filename, MAX_PATH);

    // Get the size of the version info.
    dwSize = GetFileVersionInfoSizeA(filename, NULL);
    if (dwSize == 0)
    {
        return "";
	}

    // Allocate memory.
    pVersionInfo = new BYTE[dwSize];

    if (!GetFileVersionInfoA(filename, 0, dwSize, pVersionInfo) ||
        !VerQueryValue(pVersionInfo, TEXT("\\"), (LPVOID *)&pFileInfo, &pLenFileInfo))
    {
        // Free memory.
        delete[] pVersionInfo;
        return "";
    }

    // Get build number and free memory.
    DWORD dwBuild = (pFileInfo->dwFileVersionLS) & 0xffff;
    delete[] pVersionInfo;

    ostringstream stream;
    stream << dwBuild;
    return "Build " + stream.str();
}

// Convert ISO 8859-1 to UTF-8.
std::string SiebenwindClient::Iso8859ToUtf8(const string &iso8859)
{
    string utf8;
    for (std::string::const_iterator it = iso8859.begin(); it != iso8859.end(); ++it)
    {
        uint8_t ch = *it;
        if (ch < 0x80) {
            utf8.push_back(ch);
        }
        else {
            utf8.push_back(0xc0 | ch >> 6);
            utf8.push_back(0x80 | (ch & 0x3f));
        }
    }
    return utf8;
}

// Convert UTF-8 to UTF-16.
// This is from https://stackoverflow.com/questions/7153935.
std::wstring SiebenwindClient::Utf8ToUtf16(const std::string& utf8)
{
    std::vector<unsigned long> unicode;
    size_t i = 0;
    while (i < utf8.size())
    {
        unsigned long uni;
        size_t todo;
        bool error = false;
        unsigned char ch = utf8[i++];
        if (ch <= 0x7F)
        {
            uni = ch;
            todo = 0;
        }
        else if (ch <= 0xBF)
        {
            throw std::logic_error("not a UTF-8 string");
        }
        else if (ch <= 0xDF)
        {
            uni = ch & 0x1F;
            todo = 1;
        }
        else if (ch <= 0xEF)
        {
            uni = ch & 0x0F;
            todo = 2;
        }
        else if (ch <= 0xF7)
        {
            uni = ch & 0x07;
            todo = 3;
        }
        else
        {
            throw std::logic_error("not a UTF-8 string");
        }
        for (size_t j = 0; j < todo; ++j)
        {
            if (i == utf8.size())
                throw std::logic_error("not a UTF-8 string");
            unsigned char ch = utf8[i++];
            if (ch < 0x80 || ch > 0xBF)
                throw std::logic_error("not a UTF-8 string");
            uni <<= 6;
            uni += ch & 0x3F;
        }
        if (uni >= 0xD800 && uni <= 0xDFFF)
            throw std::logic_error("not a UTF-8 string");
        if (uni > 0x10FFFF)
            throw std::logic_error("not a UTF-8 string");
        unicode.push_back(uni);
    }
    std::wstring utf16;
    for (size_t i = 0; i < unicode.size(); ++i)
    {
        unsigned long uni = unicode[i];
        if (uni <= 0xFFFF)
        {
            utf16 += (wchar_t)uni;
        }
        else
        {
            uni -= 0x10000;
            utf16 += (wchar_t)((uni >> 10) + 0xD800);
            utf16 += (wchar_t)((uni & 0x3FF) + 0xDC00);
        }
    }
    return utf16;
}

// Convert directly from ISO 8859-1 to UTF-16.
std::wstring SiebenwindClient::Iso8859ToUtf16(const std::string& utf8)
{
    return SiebenwindClient::Utf8ToUtf16(SiebenwindClient::Iso8859ToUtf8(utf8));
}

string SiebenwindClient::LocalizeA(const string &untranslated)
{
    if (untranslated == "Save Password")
    {
        return "Passwort speichern";
    }
    else if (untranslated == "Auto Login")
    {
        return "Auto-Login";
    }
    else if (untranslated == "Log in to Ultima Online")
    {
        return "Anmelden bei Siebenwind";
    }
    else if (untranslated == "Account Name")
    {
        return "Benutzer";
    }
    else if (untranslated == "Password")
    {
        return "Passwort";
    }
    /* use this for new entries:
    else if (untranslated == "")
    {
        return "";
    }
    else if (untranslated == "")
    {
        return "";
    }
    else if (untranslated == "")
    {
        return "";
    }
    */

    return untranslated;
}

wstring SiebenwindClient::LocalizeW(const wstring &untranslated)
{
    if (untranslated == L"Quit Ultima Online")
    {
        return L"Siebenwind beenden.";
    }
    else if (untranslated == L"Next screen")
    {
        return L"Anmelden";
    }
    else if (untranslated == L"Click here to enter your user name")
    {
        return L"Hier den Benutzernamen eingeben.";
    }
    else if (untranslated == L"Click here to enter your password")
    {
        return L"Hier das Passwort eingeben.";
    }
    else if (untranslated == L"Save your password in config file")
    {
        return L"Das Passwort in der Konfigurationsdatei im Klartext (!) speichern.";
    }
    else if (untranslated == L"Auto LogIn (without selection screens)")
    {
        return L"Automatischer Login ohne Charakter-Auswahl.";
    }
    /* use this for new entries:
    else if (untranslated == L"")
    {
        return L"";
    }
    else if (untranslated == L"")
    {
        return L"";
    }
    else if (untranslated == L"")
    {
        return L"";
    }
    */

    return untranslated;
}