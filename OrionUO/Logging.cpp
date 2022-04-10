#include "Logging.h"

// CLogger g_Logger;
// CLogger g_CrashLogger;

// CLogger::CLogger()
// {
// }

// CLogger::~CLogger()
// {
//     Close();
// }

// void CLogger::Close()
// {
//     if (m_file.IsOpen())
//     {
//         LOG("Log closed.\n");
//         m_file.Close();
//     }
// }

// void CLogger::Init(const std::filesystem::path& a_path)
// {
//     Close();

//     m_file.Open(a_path, "w");
//     if (this == &g_Logger)
//     {
//         LOG("Log opened.\n");
//     }
//     m_path = a_path;
// }

// void CLogger::Print(const char* a_format, ...)
// {
//     if (!m_file.IsOpen())
//         return;

//     va_list args;
//     va_start(args, a_format);
//     m_file.PrintV(a_format, args);
//     va_end(args);
//     m_file.Flush();
// }

// void CLogger::PrintV(const char* a_format, va_list a_args)
// {
//     if (!m_file.IsOpen())
//         return;

//     m_file.PrintV(a_format, a_args);
//     m_file.Flush();
// }

// void CLogger::Print(const wchar_t* a_format, ...)
// {
//     if (!m_file.IsOpen())
//         return;

//     va_list args;
//     va_start(args, a_format);
//     m_file.PrintV(a_format, args);
//     va_end(args);
//     m_file.Flush();
// }

// void CLogger::PrintV(const wchar_t* a_format, va_list a_args)
// {
//     if (!m_file.IsOpen())
//         return;

//     m_file.PrintV(a_format, a_args);
//     m_file.Flush();
// }

// void CLogger::Dump(u8* a_buf, int a_size)
// {
//     if(!m_file.IsOpen())
//         return;

//     int num_lines = a_size / 16;
//     if (a_size % 16 != 0)
//         num_lines++;

//     for (int line = 0; line < num_lines; line++)
//     {
//         int row = 0;
//         m_file.Print("%04X: ", line * 16);
//         for (row = 0; row < 16; row++)
//         {
//             if (line * 16 + row < a_size)
//                 m_file.Print("%02X ", a_buf[line * 16 + row]);
//             else
//                 m_file.Print("-- ");
//         }

//         m_file.Print(": ");
//         for (row = 0; row < 16; row++)
//         {
//             if (line * 16 + row < a_size)
//                 m_file.PrintChar(isprint(a_buf[line * 16 + row]) != 0 ? a_buf[line * 16 + row] : '.');
//         }
//         m_file.PrintChar('\n');
//     }
//     m_file.Flush();
// }
