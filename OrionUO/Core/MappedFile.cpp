#include "MappedFile.h"
#include "Core/File.h"
#include "Core/Log.h"
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
#define NONLS
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
#else
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#endif

namespace Core
{

MappedFile::MappedFile()
{
}

MappedFile::~MappedFile()
{
    Unload();
}

bool MappedFile::Load(const std::filesystem::path& a_path)
{
    LOG_INFO("Core", "Mmaping  %s", a_path.string().c_str());
    bool result = false;
    if (std::filesystem::exists(a_path))
    {
        Unload();
        m_buffer = Map(a_path, m_size);
        result = m_buffer != nullptr;
        SetData(m_buffer, m_size);

        if (!result)
        {
#if defined(ORION_WINDOWS)
            auto errorCode = GetLastError();
#else
            auto errorCode = errno;
#endif
            LOG_ERROR("CORE", "Failed to memory map, error code: %i", errorCode);
        }
    }
    else
    {
        LOG_WARNING("Core", "File not found %s", a_path.string().c_str());
    }
    return result;
}

void MappedFile::Unload()
{
    if (m_buffer)
        Unmap(m_buffer, m_size);
}

unsigned char* MappedFile::Map(const std::filesystem::path& a_path, size_t& a_length)
{
    unsigned char* buffer = nullptr;
    size_t size = 0;

#if defined(ORION_WINDOWS)
    HANDLE map = 0;
    auto fd = CreateFileW(
        a_path.wstring().c_str(),
        GENERIC_READ,
        FILE_SHARE_READ,
        nullptr,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        nullptr);
    if (fd == INVALID_HANDLE_VALUE)
        return nullptr;

    size = GetFileSize(fd, nullptr);
    if (size == INVALID_FILE_SIZE || size == 0)
        goto fail;

    map = CreateFileMappingA(fd, nullptr, PAGE_READONLY, 0, size, nullptr);
    if (!map)
        goto fail;

    buffer = (unsigned char*)MapViewOfFile(map, FILE_MAP_READ, 0, 0, size);
    CloseHandle(map);
fail:
    CloseHandle(fd);
#else
    int fd = open(a_path.string().c_str(), O_RDONLY);
    if (fd < 0)
        return nullptr;

    size = lseek(fd, 0, SEEK_END);
    if (size <= 0)
        goto fail;

    buffer = (unsigned char*)mmap(nullptr, size, PROT_READ, MAP_SHARED, fd, 0);
    if (buffer == MAP_FAILED)
        buffer = nullptr;
fail:
    close(fd);
#endif

    a_length = size;
    return buffer;
}

void MappedFile::Unmap(unsigned char* a_buffer, size_t a_length)
{
    ERROR_IF( a_buffer == nullptr, "Core", "Unmap requires a valid buffer!");
#if defined(ORION_WINDOWS)
    UnmapViewOfFile(a_buffer);
#else
    munmap(a_buffer, a_length);
#endif
}

} // namespace Core
