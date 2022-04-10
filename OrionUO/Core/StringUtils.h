#pragma once

#include <string>

namespace Core
{

std::string Iso8859ToUtf8(const std::string& iso8859);
std::wstring Utf8ToUtf16(const std::string& utf8);
std::wstring Iso8859ToUtf16(const std::string& utf8);
std::string EncodeUTF8(const std::wstring& a_str);
std::wstring DecodeUTF8(const std::string& a_str);
std::string ToCamelCaseA(const std::string& a_str);
std::wstring ToCamelCaseW(const std::wstring& a_str);
std::string ToString(const std::wstring& a_str);
std::wstring ToWString(const std::string& a_str);
std::string Trim(const std::string& a_str);
int ToInt(const std::string& a_str);
std::string ToLowerA(const std::string& a_str);
std::string ToUpperA(const std::string& a_str);
std::wstring ToLowerW(const std::wstring& a_str);
std::wstring ToUpperW(const std::wstring& a_str);
bool ToBool(const std::string& a_str);

} // namespace Core
