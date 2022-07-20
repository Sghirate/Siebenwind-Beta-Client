#pragma once

#include "Core/Minimal.h"
#include <string>

namespace Core
{

extern const std::string EmptyString;

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

bool ToString(i8 a_value, std::string& out_string);
bool FromString(const std::string& a_string, i8& out_value);
bool ToString(u8 a_value, std::string& out_string);
bool FromString(const std::string& a_string, u8& out_value);
bool ToString(i16 a_value, std::string& out_string);
bool FromString(const std::string& a_string, i16& out_value);
bool ToString(u16 a_value, std::string& out_string);
bool FromString(const std::string& a_string, u16& out_value);
bool ToString(i32 a_value, std::string& out_string);
bool FromString(const std::string& a_string, i32& out_value);
bool ToString(u32 a_value, std::string& out_string);
bool FromString(const std::string& a_string, u32& out_value);
bool ToString(i64 a_value, std::string& out_string);
bool FromString(const std::string& a_string, i64& out_value);
bool ToString(u64 a_value, std::string& out_string);
bool FromString(const std::string& a_string, u64& out_value);
bool ToString(float a_value, std::string& out_string);
bool FromString(const std::string& a_string, float& out_value);
bool ToString(double a_value, std::string& out_string);
bool FromString(const std::string& a_string, double& out_value);
bool ToString(const std::string& a_value, std::string& out_string);
bool FromString(const std::string& a_value, std::string& out_value);

} // namespace Core
