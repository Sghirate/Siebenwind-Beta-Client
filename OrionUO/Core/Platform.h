#pragma once

#include "Core/Rect.h"
#include <filesystem>
#include <stdarg.h>

#ifndef MAX_PATH
#define MAX_PATH 256
#endif

namespace Core
{

struct Platform
{

static const char* GetCallStack();
static void* LoadOject(const std::filesystem::path& a_path);
static void* LoadFunction(void* a_object, const char* a_name);
static void UnloadObject(void* a_object);
static std::filesystem::path GetBinaryPath();
static std::filesystem::path GetModulePath();
static bool SetClipboardText(const char* a_text);
static bool HasClipboardText();
static const char* GetClipboardText();
static Core::Rect<int> GetDisplayArea(int a_display = 0);

};

} // namespace Core