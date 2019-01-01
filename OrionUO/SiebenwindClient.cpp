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