#pragma once

#include <filesystem>
#include <stdarg.h>

namespace Core
{

struct File
{
    explicit File() noexcept = default;
    explicit File(std::FILE* a_file) noexcept;
    File(std::nullptr_t) = delete;
    File(const std::filesystem::path& a_path, const char* a_mode) noexcept;
    File(const File&) = delete;
    File(File&& a_other) noexcept;
    ~File();

    File& operator=(const File&) = delete;
    File& operator=(File&& a_other) noexcept;
    [[nodiscard]] explicit operator bool() const noexcept { return m_file != nullptr; }

    void Open(const std::filesystem::path& a_path, const char* a_mode);
    void Close();
    bool IsOpen() const { return m_file != nullptr; }

    long Size();
    long Tell() const;
    int Seek(long a_offset, int a_origin);
    int Flush();
    int Eof();
    char* Gets(char* a_buffer, int a_count);
    size_t Read(void* a_buffer, size_t a_size, size_t a_count);
    size_t Write(const void* a_buffer, size_t a_size, size_t a_count);
    void PrintChar(char a_c);
    void Print(const char* a_format, ...);
    void Print(const wchar_t* a_format, ...);
    void PrintV(const char* a_format, va_list a_args);
    void PrintV(const wchar_t* a_format, va_list a_args);

    template <size_t N>
    char* Gets(char (&a_buffer)[N])
    {
        return Gets(a_buffer, N);
    }
    template <typename T>
    size_t Read(T* a_buffer, size_t a_count)
    {
        return Read(a_buffer, sizeof(T), a_count);
    }
    template <typename T, size_t N>
    size_t Read(T (&a_buffer)[N])
    {
        return Read(a_buffer, sizeof(T), N);
    }
    template <typename T>
    size_t Write(const T* a_buffer, size_t a_count)
    {
        return Write(a_buffer, sizeof(T), a_count);
    }
    template <typename T, size_t N>
    size_t Write(const T (&a_buffer)[N])
    {
        return Write(a_buffer, sizeof(T), N);
    }

private:
    std::FILE* m_file = nullptr;
};

} // namespace Core
