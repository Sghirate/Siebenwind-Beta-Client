// #if defined(ORION_WINDOWS)

// #include "Core/MappedFile.h"
// #include "Core/Platform.h"
// #include "Core/StringUtils.h"
// #include "Logging.h"
// #include "VMQuery.h"
// #include "plugin/enumlist.h"
// #include <psapi.h>
// #include <tlhelp32.h>
// #include <windows.h>

// std::string GetMemStorageText(int storage)
// {
//     switch (storage)
//     {
//         case MEM_FREE: return "Free";
//         case MEM_RESERVE: return "Reserve";
//         case MEM_IMAGE: return "Image";
//         case MEM_MAPPED: return "Mapped";
//         case MEM_PRIVATE: return "Private";
//         default: break;
//     }

//     return "Unknown";
// }

// inline BOOL ModuleFind(const HANDLE& snapshot, void* pvBaseAddr, PMODULEENTRY32 pme)
// {
//     for (BOOL ok = Module32First(snapshot, pme); ok; ok = Module32Next(snapshot, pme))
//     {
//         if (pme->modBaseAddr == pvBaseAddr)
//             return TRUE;
//     }

//     return FALSE;
// }

// void DumpRegionInfo(const HANDLE& snapshot, HANDLE hProcess, VMQUERY& vmq)
// {
//     if (vmq.dwRgnStorage == MEM_FREE)
//         return;

//     std::string filePath = "";

//     if (vmq.pvRgnBaseAddress != nullptr)
//     {
//         MODULEENTRY32 me = { sizeof(me) };
//         if (ModuleFind(snapshot, vmq.pvRgnBaseAddress, &me))
//         {
//             filePath = Core::ToString(me.szExePath);
//         }
//         else
//         {
//             wchar_t filename[MAX_PATH + 1];
//             u32 d = GetMappedFileName(hProcess, vmq.pvRgnBaseAddress, filename, MAX_PATH);

//             if (d)
//                 filePath = Core::ToString(filename);
//         }
//     }

//     if (vmq.fRgnIsAStack)
//         filePath = "Thread Stack";

//     CRASHLOG(
//         "Address: 0x%08X => 0x%08X Storage: '%s' size: %i blocks: %i path: %s\n",
//         vmq.pvBlkBaseAddress,
//         (u8*)vmq.pvBlkBaseAddress + vmq.RgnSize,
//         GetMemStorageText(vmq.dwBlkStorage).c_str(),
//         vmq.RgnSize,
//         vmq.dwRgnBlocks,
//         filePath.c_str());
// }

// void DumpLibraryInformation()
// {
// #if USE_DEBUG_FUNCTION_NAMES == 1
//     CRASHLOG("trace functions:\n");
//     for (const std::string& str : g_DebugFuncStack)
//         CRASHLOG("%s\n", str.c_str());

//     static bool libraryInfoPrinted = false;

//     if (!libraryInfoPrinted)
//     {
//         libraryInfoPrinted = true;

//         CRASHLOG("Library information:\n");

//         u32 processId = 0;
//         GetWindowThreadProcessId(g_OrionWindow.Handle, &processId);

//         HANDLE process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);

//         if (process == nullptr)
//         {
//             CRASHLOG("::OpenProcess failed!\n");
//             return;
//         }

//         HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, processId);

//         if (snapshot == nullptr)
//         {
//             CRASHLOG("::CreateToolhelp32Snapshot failed!\n");
//             CloseHandle(process);
//             return;
//         }

//         void* address = nullptr;

//         while (true)
//         {
//             VMQUERY vmq = { 0 };

//             if (!VMQuery(process, address, &vmq))
//                 break;

//             DumpRegionInfo(snapshot, process, vmq);

//             address = ((u8*)vmq.pvRgnBaseAddress + vmq.RgnSize);
//         }

//         CloseHandle(snapshot);
//         CloseHandle(process);
//     }
// #endif
// }

// void DumpCurrentRegistersInformation(CONTEXT* CR)
// {
// #if defined(_WIN64)
//     CRASHLOG(
//         "EAX=0x%016LX, EBX=0x%016LX, ECX=0x%016LX, EDX=0x%016LX\n",
//         CR->Rax,
//         CR->Rbx,
//         CR->Rcx,
//         CR->Rdx);
//     CRASHLOG(
//         "ESI=0x%016LX, EDI=0x%016LX, ESP=0x%016LX, EBP=0x%016LX\n",
//         CR->Rsi,
//         CR->Rdi,
//         CR->Rsp,
//         CR->Rbp);
//     CRASHLOG("EIP=0x%016LX, EFLAGS=0x%016LX\n\n", CR->Rip, CR->EFlags);

//     CRASHLOG("Bytes at EIP:\n");
//     CRASHLOG_DUMP((u8*)CR->Rip, 16);

//     CRASHLOG("Bytes at ESP:\n");
//     CRASHLOG_DUMP((u8*)CR->Rsp, 64);
// #else
//     CRASHLOG(
//         "EAX=0x%08X, EBX=0x%08X, ECX=0x%08X, EDX=0x%08X\n", CR->Eax, CR->Ebx, CR->Ecx, CR->Edx);
//     CRASHLOG(
//         "ESI=0x%08X, EDI=0x%08X, ESP=0x%08X, EBP=0x%08X\n", CR->Esi, CR->Edi, CR->Esp, CR->Ebp);
//     CRASHLOG("EIP=0x%08X, EFLAGS=0x%08X\n\n", CR->Eip, CR->EFlags);

//     CRASHLOG("Bytes at EIP:\n");
//     CRASHLOG_DUMP((u8*)CR->Eip, 16);

//     CRASHLOG("Bytes at ESP:\n");
//     CRASHLOG_DUMP((u8*)CR->Esp, 64);
// #endif
// }

// std::vector<u32> FindPattern(u8* ptr, int size, const std::vector<u8>& pattern)
// {
//     std::vector<u32> result;
//     int patternSize = (int)pattern.size();
//     int count       = size - patternSize - 1;
//     for (int i = 0; i < count; i++)
//     {
//         if (!memcmp(&ptr[0], &pattern[0], patternSize))
//             result.push_back(0x00400000 + (int)i);

//         ptr++;
//     }
//     return result;
// }

// LONG __stdcall OrionUnhandledExceptionFilter(struct _EXCEPTION_POINTERS* exceptionInfo)
// {
//     static int errorCount    = 0;
//     static u32 lastErrorTime = 0;

//     u32 ticks = GetTickCount();
//     errorCount++;

//     if (exceptionInfo->ExceptionRecord)
//     {
// #if defined(_WIN64)
//         CRASHLOG(
//             "Unhandled exception #%i: 0x%016LX at %016LX\n",
//             errorCount,
//             exceptionInfo->ExceptionRecord->ExceptionCode,
//             exceptionInfo->ExceptionRecord->ExceptionAddress);
// #else
//         CRASHLOG(
//             "Unhandled exception #%i: 0x%08X at %08X\n",
//             errorCount,
//             exceptionInfo->ExceptionRecord->ExceptionCode,
//             exceptionInfo->ExceptionRecord->ExceptionAddress);
// #endif

//         if (errorCount > 10 && (ticks - lastErrorTime) < 5000)
//         {
//             CRASHLOG(Core::Platform::GetCallStack());

//             DumpLibraryInformation();

//             DumpCurrentRegistersInformation(exceptionInfo->ContextRecord);

//             Core::MappedFile file;

//             wchar_t fileName[MAX_PATH] = { 0 };
//             GetModuleFileNameW(0, fileName, MAX_PATH);
//             bool crashlog = false;
//             if (file.Load(fileName))
//             {
//                 std::vector<u8> pattern;
// #if defined(_WIN64)
//                 u8* eipBytes = (u8*)exceptionInfo->ContextRecord->Rip;
// #else
//                 u8* eipBytes = (u8*)exceptionInfo->ContextRecord->Eip;
// #endif

//                 for (int i = 0; i < 16; i++)
//                     pattern.push_back(eipBytes[i]);

//                 auto list = FindPattern(file.GetBuffer(), (int)file.GetSize(), pattern);
//                 for (int item : list)
//                 {
//                     CRASHLOG("Address in exe (by EIP): 0x%08X\n", item);
//                 }

//                 file.Unload();
//                 if (g_PacketManager.m_PacketsStack.empty())
//                 {
//                     CRASHLOG("\nPackets stack is empty.\n");
//                 }
//                 else
//                 {
//                     CRASHLOG("\nPackets in stack:\n");
//                     for (deque<vector<u8>>::iterator i = g_PacketManager.m_PacketsStack.begin();
//                          i != g_PacketManager.m_PacketsStack.end();
//                          ++i)
//                     {
//                         CRASHLOG("Packet data:\n");
//                         CRASHLOG_DUMP((u8*)i->data(), int(i->size()));
//                     }
//                 }
//                 crashlog = true;
//             }

//             g_CrashLogger.Close();
//             std::wstring crashlogPath = L"\"" + g_CrashLogger.GetPath().wstring() + L"\"";
//             const wchar_t* path       = crashlogPath.c_str();
//             STARTUPINFOW si;
//             PROCESS_INFORMATION pi;
//             ZeroMemory(&si, sizeof(si));
//             si.cb = sizeof(si);
//             ZeroMemory(&pi, sizeof(pi));
//             bool reportSent = CreateProcessW(
//                 L"OrionCrashReporter.exe",
//                 crashlogPath.data(),    // Command line
//                 nullptr, // Process handle not inheritable
//                 nullptr, // Thread handle not inheritable
//                 FALSE,   // Set handle inheritance to FALSE
//                 0,       // No creation flags
//                 nullptr, // Use parent's environment block
//                 nullptr, // Use parent's starting directory
//                 &si,     // Pointer to STARTUPINFO structure
//                 &pi);

//             g_Orion.Uninstall();

//             if (!reportSent)
//             {
//                 std::string msg = "Orion client performed an unrecoverable operation.";
//                 if (crashlog)
//                     msg += "\nCrashlog has been created in crashlogs folder.";
//                 msg += "\nTerminating...";

//                 MessageBoxA(0, msg.c_str(), 0, MB_ICONSTOP | MB_OK);
//             }

//             ExitProcess(1);
//         }

//         if (ticks - lastErrorTime > 5000)
//         {
//             errorCount    = 0;
//             lastErrorTime = ticks;
//         }

//         if (errorCount == 1)
//             DumpCurrentRegistersInformation(exceptionInfo->ContextRecord);
//     }

//     //	return EXCEPTION_CONTINUE_SEARCH;
//     return EXCEPTION_CONTINUE_EXECUTION;
// }
// #endif // ORION_WINDOWS
