#pragma once

// #include "Core/Minimal.h"
// #include "Core/File.h"

// #define LOG_LEVEL 2

// #if LOG_LEVEL == 1
// #define INITLOGGER(path)
// #define LOG(...) fprintf(stdout, " LOG: " __VA_ARGS__)
// #define LOG_DUMP(...)      //LogDump(stdout, __VA_ARGS__)
// #define SAFE_LOG_DUMP(...) //LogDump(stdout, __VA_ARGS__)
// #elif LOG_LEVEL == 2
// #define INITLOGGER(path) g_Logger.Init(path);
// #define LOG(...) g_Logger.Print(__VA_ARGS__) //fprintf(stdout, " LOG: " __VA_ARGS__)
// #define LOG_DUMP(...)                        //LogDump(stdout, __VA_ARGS__)
// #define SAFE_LOG_DUMP(...)                   //LogDump(stdout, __VA_ARGS__)
// #else                                        //LOG_LEVEL == 0
// #define INITLOGGER(path)
// #define LOG(...)
// #define LOG_DUMP(...)
// #define SAFE_LOG_DUMP(...)
// #endif //LOG_LEVEL!=0

// #define INITCRASHLOGGER(path) g_CrashLogger.Init(path);
// #define CRASHLOG g_CrashLogger.Print
// #define CRASHLOG_DUMP g_CrashLogger.Dump

// class CLogger
// {
// public:
//     CLogger();
//     ~CLogger();

//     void Close();
//     void Init(const std::filesystem::path& a_path);
//     void Print(const char* a_format, ...);
//     void PrintV(const char* a_format, va_list a_args);
//     void Print(const wchar_t* a_format, ...);
//     void PrintV(const wchar_t* a_format, va_list a_args);
//     void Dump(u8* a_buf, int a_size);
//     bool IsReady() const { return m_file.IsOpen(); }
//     const std::filesystem::path& GetPath() const { return m_path; }

// private:
//     std::filesystem::path m_path;
//     Core::File m_file;
// };

// extern CLogger g_Logger;
// extern CLogger g_CrashLogger;
