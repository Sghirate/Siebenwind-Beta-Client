#include "SiebenwindClient.h"
#include "Core/CommandLine.h"
#include "Core/Platform.h"
#include "plugin/plugininterface.h"
#include <vector>

namespace
{
const std::string g_7wWindowTitle = "Siebenwind Beta-Client " + SiebenwindClient::GetBuildInfo();
std::pair<std::string, std::string> g_7wStrings[] = {
    { "Save Password", "Passwort speichern" },
    { "Auto Login", "Auto-Login" },
    { "Log in to Ultima Online", "Anmelden bei Siebenwind" },
    { "Account Name", "Benutzer" },
    { "Password", "Passwort" },
};
std::pair<std::wstring, std::wstring> g_7wWStrings[] = {
    { L"Quit Ultima Online", L"Siebenwind beenden." },
    { L"Next screen", L"Anmelden" },
    { L"Click here to enter your user name", L"Hier den Benutzernamen eingeben." },
    { L"Click here to enter your password", L"Hier das Passwort eingeben." },
    { L"Save your password in config file",
      L"Das Passwort in der Konfigurationsdatei im Klartext (!) speichern." },
    { L"Auto LogIn (without selection screens)", L"Automatischer Login ohne Charakter-Auswahl." },
};
} // namespace

std::string SiebenwindClient::GetBuildInfo()
{
    /*DWORD dwSize                = 0;
    BYTE* pVersionInfo          = NULL;
    VS_FIXEDFILEINFO* pFileInfo = NULL;
    UINT pLenFileInfo           = 0;

    const char* exePath = Core::CommandLine::GetArgv()[0];

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
        !VerQueryValue(pVersionInfo, TEXT("\\"), (LPVOID*)&pFileInfo, &pLenFileInfo))
    {
        // Free memory.
        delete[] pVersionInfo;
        return "";
    }

    // Get build number and free memory.
    DWORD dwBuild = (pFileInfo->dwFileVersionLS) & 0xffff;
    delete[] pVersionInfo;

    std::ostringstream stream;
    stream << dwBuild;
    return "Build " + stream.str();*/
    return "DEV BUILD";
}

const std::string& SiebenwindClient::LocalizeA(const std::string& untranslated)
{
    for (const auto& str : g_7wStrings)
        if (str.first == untranslated)
            return str.second;
    return untranslated;
}

const std::wstring& SiebenwindClient::LocalizeW(const std::wstring& untranslated)
{
    for (const auto& str : g_7wWStrings)
        if (str.first == untranslated)
            return str.second;
    return untranslated;
}

const std::string& SiebenwindClient::GetWindowTitle()
{
    return g_7wWindowTitle;
}
