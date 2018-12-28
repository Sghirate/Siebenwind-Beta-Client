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
};
