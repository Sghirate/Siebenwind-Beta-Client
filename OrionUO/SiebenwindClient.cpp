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
