#include "File.h"

namespace Core
{

File::File(std::FILE* a_file) noexcept
    : m_file(a_file)
{
}

File::File(const std::filesystem::path& a_path, const char* a_mode) noexcept
{
    Open(a_path, a_mode);
}

File::File(File&& a_other) noexcept
    : m_file(std::exchange(a_other.m_file, nullptr))
{
}

File::~File()
{
    Close();
}

File& File::operator=(File&& a_other) noexcept
{
    Close();
    m_file = std::exchange(a_other.m_file, nullptr);
    return *this;
}

void File::Open(const std::filesystem::path& a_path, const char* a_mode)
{
    Close();
    m_file = std::fopen(a_path.string().c_str(), a_mode);
}

void File::Close()
{
    if (m_file)
        std::fclose(m_file);
    m_file = nullptr;
}

long File::Size()
{
    long cur = Tell();
    Seek(0, SEEK_END);
    long size = Tell();
    Seek(cur, SEEK_SET);
    return size;
}

long File::Tell() const
{
    return std::ftell(m_file);
}

int File::Seek(long a_offset, int a_origin)
{
    return std::fseek(m_file, a_offset, a_origin);
}

int File::Flush()
{
    return std::fflush(m_file);
}

int File::Eof()
{
    return std::feof(m_file);
}

char* File::Gets(char* a_buffer, int a_count)
{
    return std::fgets(a_buffer, a_count, m_file);
}

size_t File::Read(void* a_buffer, size_t a_size, size_t a_count)
{
    return std::fread(a_buffer, a_size, a_count, m_file);
}

size_t File::Write(const void* a_buffer, size_t a_size, size_t a_count)
{
    return std::fwrite(a_buffer, a_size, a_count, m_file);
}

void File::PrintChar(char a_c)
{
    fputc(a_c, m_file);
}

void File::Print(const char* a_format, ...)
{
    va_list args;
    va_start(args, a_format);
    PrintV(a_format, args);
    vfprintf(m_file, a_format, args);
    va_end(args);
}

void File::Print(const wchar_t* a_format, ...)
{
    va_list args;
    va_start(args, a_format);
    PrintV(a_format, args);
    va_end(args);
}

void File::PrintV(const char* a_format, va_list a_args)
{
    vfprintf(m_file, a_format, a_args);
}

void File::PrintV(const wchar_t* a_format, va_list a_args)
{
    vfwprintf(m_file, a_format, a_args);
}


} // namespace Core
