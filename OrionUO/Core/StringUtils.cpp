#include "StringUtils.h"
#include "Core/Minimal.h"
#include "Core/Log.h"
#include <algorithm>
#include <charconv>
#include <codecvt>
#include <cassert>
#include <cstring>
#include <locale>
#include <vector>
#if defined(ORION_WINDOWS)
#define WIN32_LEAN_AND_MEAN
#define NOWINMESSAGES
#define NOWINSTYLES
#define NOSYSMETRICS
#define NOMENUS
#define NOICONS
#define NOKEYSTATES
#define NOSYSCOMMANDS
#define NORASTEROPS
#define NOSHOWWINDOW
#define OEMRESOURCE
#define NOATOM
#define NOCLIPBOARD
#define NOCOLOR
#define NOCTLMGR
#define NODRAWTEXT
#define NOGDI
#define NOKERNEL
#define NOUSER
//#define NONLS <-- needed for UTF+WideChar functions
#define NOMB
#define NOMEMMGR
#define NOMETAFILE
#define NOMINMAX
#define NOMSG
#define NOOPENFILE
#define NOSCROLL
#define NOSERVICE
#define NOSOUND
#define NOTEXTMETRIC
#define NOWH
#define NOWINOFFSETS
#define NOCOMM
#define NOKANJI
#define NOHELP
#define NOPROFILER
#define NODEFERWINDOWPOS
#define NOMCX
#include <windows.h>
#endif

namespace
{
#if !defined(ORION_WINDOWS)
static void EnsureLocale()
{
    static bool isInitialized = false;
    if (!isInitialized) 
    {
        setlocale(LC_ALL, "");
        isInitialized = true;
    }
}
#endif
}

namespace Core
{

const std::string EmpyString;

// Convert ISO 8859-1 to UTF-8.
std::string Iso8859ToUtf8(const std::string& a_iso8859)
{
    std::string utf8;
    utf8.reserve(a_iso8859.size() * 2);
    for (std::string::const_iterator it = a_iso8859.begin(); it != a_iso8859.end(); ++it)
    {
        u8 ch = *it;
        if (ch < 0x80)
        {
            utf8.push_back(ch);
        }
        else
        {
            utf8.push_back(0xc0 | ch >> 6);
            utf8.push_back(0x80 | (ch & 0x3f));
        }
    }
    return utf8;
}

// Convert UTF-8 to UTF-16.
// This is from https://stackoverflow.com/questions/7153935.
std::wstring Utf8ToUtf16(const std::string& a_utf8)
{
    static std::vector<unsigned long> unicode;
    unicode.clear();
    unicode.reserve(a_utf8.size());

    size_t i = 0;
    while (i < a_utf8.size())
    {
        unsigned long uni;
        size_t todo;
        unsigned char ch = a_utf8[i++];
        if (ch <= 0x7F)
        {
            uni  = ch;
            todo = 0;
        }
        else if (ch <= 0xBF)
        {
#if defined(__cpp_exceptions)
            throw std::logic_error("not a UTF-8 string");
#else
            assert(false && "not a UTF-8 string");
#endif
        }
        else if (ch <= 0xDF)
        {
            uni  = ch & 0x1F;
            todo = 1;
        }
        else if (ch <= 0xEF)
        {
            uni  = ch & 0x0F;
            todo = 2;
        }
        else if (ch <= 0xF7)
        {
            uni  = ch & 0x07;
            todo = 3;
        }
        else
        {
#if defined(__cpp_exceptions)
            throw std::logic_error("not a UTF-8 string");
#else
            assert(false && "not a UTF-8 string");
#endif
        }
        for (size_t j = 0; j < todo; ++j)
        {
            if (i == a_utf8.size())
            {
#if defined(__cpp_exceptions)
                throw std::logic_error("not a UTF-8 string");
#else
                assert(false && "not a UTF-8 string");
#endif
            }
            unsigned char ch = a_utf8[i++];
            if (ch < 0x80 || ch > 0xBF)
            {
#if defined(__cpp_exceptions)
            throw std::logic_error("not a UTF-8 string");
#else
            assert(false && "not a UTF-8 string");
#endif
            }
            uni <<= 6;
            uni += ch & 0x3F;
        }
        if (uni >= 0xD800 && uni <= 0xDFFF)
            {
#if defined(__cpp_exceptions)
            throw std::logic_error("not a UTF-8 string");
#else
            assert(false && "not a UTF-8 string");
#endif
            }
        if (uni > 0x10FFFF)
            {
#if defined(__cpp_exceptions)
            throw std::logic_error("not a UTF-8 string");
#else
            assert(false && "not a UTF-8 string");
#endif
            }
        unicode.push_back(uni);
    }
    std::wstring utf16;
    utf16.reserve(unicode.size() * 2);
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

std::wstring Iso8859ToUtf16(const std::string& utf8)
{
    return Utf8ToUtf16(Iso8859ToUtf8(utf8));
}

void EncodeUTF8(const std::wstring& a_str, std::string& inout_buffer)
{
#if defined(ORION_WINDOWS)
    int size = ::WideCharToMultiByte(CP_UTF8, 0, &a_str[0], (int)a_str.size(), nullptr, 0, nullptr, nullptr);
    if (size > 0)
    {
        inout_buffer.resize(size + 1);
        ::WideCharToMultiByte(CP_UTF8, 0, &a_str[0], (int)a_str.size(), &inout_buffer[0], size, nullptr, nullptr);
        inout_buffer.resize(size); // inout_buffer[size] = 0;
    }
#else
    EnsureLocale();
    mbstate_t state{};
    auto p = a_str.data();
    const size_t size = wcsrtombs(nullptr, &p, 0, &state);
    if (size != -1)
    {
        inout_buffer.resize(size + 1);
        wcsrtombs(&inout_buffer[0], &p, size, &state);
        inout_buffer.resize(size); // inout_buffer[size] = 0;
    }
#endif
}

std::string EncodeUTF8(const std::wstring& a_str)
{
    std::string result{};
    EncodeUTF8(a_str, result);
    return result;
}

void DecodeUTF8(const std::string& a_str, std::wstring& inout_buffer)
{
#if defined(ORION_WINDOWS)
    int size = ::MultiByteToWideChar(CP_UTF8, 0, &a_str[0], (int)a_str.size(), nullptr, 0);
    if (size > 0)
    {
        inout_buffer.resize(size + 1);
        ::MultiByteToWideChar(CP_UTF8, 0, &a_str[0], (int)a_str.size(), &inout_buffer[0], size);
        inout_buffer.resize(size); // inout_buffer[size] = 0;
    }
#else
    EnsureLocale();
    mbstate_t state{};
    auto p = a_str.data();
    const size_t size = mbsrtowcs(nullptr, &p, 0, &state);
    if (size != -1)
    {
        inout_buffer.resize(size + 1);
        mbsrtowcs(&inout_buffer[0], &p, size, &state);
        inout_buffer.resize(size); // inout_buffer[size] = 0;
    }
    else
    {
        LOG_ERROR("Core", "DecodeUTF8 Failed: %s", a_str.c_str());
        inout_buffer = std::wstring(L"Invalid UTF8 sequence found");
    }
#endif
}

std::wstring DecodeUTF8(const std::string& a_str)
{
    std::wstring result = {};
    DecodeUTF8(a_str, result);
    return result;
}

bool IsPrintable(wchar_t a_wideChar)
{
    return iswprint(a_wideChar);
}

std::string ToCamelCaseA(const std::string& a_str)
{
    std::string result(a_str);
    int offset = 'a' - 'A';
    bool lastSpace = true;
    for (char &c : result)
    {
        if (c == ' ')
        {
            lastSpace = true;
        }
        else if (lastSpace)
        {
            lastSpace = false;

            if (c >= 'a' && c <= 'z')
                c -= offset;
        }
    }
    return result;
}

std::wstring ToCamelCaseW(const std::wstring& a_str)
{
    std::wstring result(a_str);
    int offset = L'a' - L'A';
    bool lastSpace = true;
    for (wchar_t &c : result)
    {
        if (c == L' ')
        {
            lastSpace = true;
        }
        else if (lastSpace)
        {
            lastSpace = false;

            if (c >= L'a' && c <= L'z')
                c -= offset;
        }
    }
    return result;
}

std::string ToString(const std::wstring& a_str)
{
    return EncodeUTF8(a_str);
}
std::wstring ToWString(const std::string& a_str)
{
    return DecodeUTF8(a_str);
}

std::string Trim(const std::string& a_str)
{
    std::string::const_iterator it = a_str.begin();
    for (; it != a_str.end() && (isspace(*it) != 0); ++it) {}

    std::string::const_reverse_iterator rit = a_str.rbegin();
    for (; rit.base() != it && (isspace(*rit) != 0); ++rit) {}

    return std::string(it, rit.base());
}

int ToInt(const std::string& a_str)
{
    return atoi(a_str.c_str());
}

std::string ToLowerA(const std::string& a_str)
{
    std::string result(a_str);
#if defined(ORION_WINDOWS)
    if (result.length())
        _strlwr(&result[0]);
#else
    std::transform(result.begin(), result.end(), result.begin(), [](auto c) { return std::tolower(c); });
#endif
    return result;
}

std::string ToUpperA(const std::string& a_str)
{
    std::string result(a_str);
#if defined(ORION_WINDOWS)
    if (result.length())
        _strupr(&result[0]);
#else
    std::transform(result.begin(), result.end(), result.begin(), [](auto c) { return std::toupper(c); });
#endif
    return result;
}

std::wstring ToLowerW(const std::wstring& a_str)
{
    std::wstring result(a_str);
#if defined(ORION_WINDOWS)
    if (result.length())
        _wcslwr(&result[0]);
#else
    std::transform(result.begin(), result.end(), result.begin(), [](auto c) { return std::towlower(c); });
#endif
    return result;
}

std::wstring ToUpperW(const std::wstring& a_str)
{
    std::wstring result(a_str);
#if defined(ORION_WINDOWS)
    if (result.length())
        _wcsupr(&result[0]);
#else
    std::transform(result.begin(), result.end(), result.begin(), [](auto c) { return std::towupper(c); });
#endif
    return result;
}

bool ToBool(const std::string& a_str)
{
    static const char* trueValues[] =
    {
        "on",
        "yes",
        "true",
        "1",
    };
    for (size_t i = 0; i < sizeof(trueValues) / sizeof(trueValues[0]); ++i)
    {
        if (std::strcmp(a_str.c_str(), trueValues[i]) == 0)
            return true;
    }
    return false;
}

bool ToString(i8 a_value, std::string& out_string)
{
    out_string = std::to_string(a_value);
    return true;
}

bool FromString(const std::string& a_string, i8& out_value)
{
    auto [ptr, ec] { std::from_chars(a_string.data(), a_string.data() + a_string.size(), out_value) };
    return ec == std::errc();
}

bool ToString(u8 a_value, std::string& out_string)
{
    out_string = std::to_string(a_value);
    return true;
}

bool FromString(const std::string& a_string, u8& out_value)
{
    auto [ptr, ec] { std::from_chars(a_string.data(), a_string.data() + a_string.size(), out_value) };
    return ec == std::errc();
}

bool ToString(i16 a_value, std::string& out_string)
{
    out_string = std::to_string(a_value);
    return true;
}

bool FromString(const std::string& a_string, i16& out_value)
{
    auto [ptr, ec] { std::from_chars(a_string.data(), a_string.data() + a_string.size(), out_value) };
    return ec == std::errc();
}

bool ToString(u16 a_value, std::string& out_string)
{
    out_string = std::to_string(a_value);
    return true;
}

bool FromString(const std::string& a_string, u16& out_value)
{
    auto [ptr, ec] { std::from_chars(a_string.data(), a_string.data() + a_string.size(), out_value) };
    return ec == std::errc();
}

bool ToString(i32 a_value, std::string& out_string)
{
    out_string = std::to_string(a_value);
    return true;
}

bool FromString(const std::string& a_string, i32& out_value)
{
    auto [ptr, ec] { std::from_chars(a_string.data(), a_string.data() + a_string.size(), out_value) };
    return ec == std::errc();
}

bool ToString(u32 a_value, std::string& out_string)
{
    out_string = std::to_string(a_value);
    return true;
}

bool FromString(const std::string& a_string, u32& out_value)
{
    auto [ptr, ec] { std::from_chars(a_string.data(), a_string.data() + a_string.size(), out_value) };
    return ec == std::errc();
}

bool ToString(i64 a_value, std::string& out_string)
{
    out_string = std::to_string(a_value);
    return true;
}

bool FromString(const std::string& a_string, i64& out_value)
{
    auto [ptr, ec] { std::from_chars(a_string.data(), a_string.data() + a_string.size(), out_value) };
    return ec == std::errc();
}

bool ToString(u64 a_value, std::string& out_string)
{
    out_string = std::to_string(a_value);
    return true;
}

bool FromString(const std::string& a_string, u64& out_value)
{
    auto [ptr, ec] { std::from_chars(a_string.data(), a_string.data() + a_string.size(), out_value) };
    return ec == std::errc();
}

bool ToString(float a_value, std::string& out_string)
{
    out_string = std::to_string(a_value);
    return true;
}

bool FromString(const std::string& a_string, float& out_value)
{
    auto [ptr, ec] { std::from_chars(a_string.data(), a_string.data() + a_string.size(), out_value) };
    return ec == std::errc();
}

bool ToString(double a_value, std::string& out_string)
{
    out_string = std::to_string(a_value);
    return true;
}

bool FromString(const std::string& a_string, double& out_value)
{
    auto [ptr, ec] { std::from_chars(a_string.data(), a_string.data() + a_string.size(), out_value) };
    return ec == std::errc();
}

bool ToString(const std::string& a_value, std::string& out_string)
{
    out_string = a_value;
    return true;
}

bool FromString(const std::string& a_value, std::string& out_value)
{
    out_value = a_value;
    return true;
}


} // namespace Core
