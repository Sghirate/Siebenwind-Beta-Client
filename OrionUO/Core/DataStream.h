#pragma once

#include "Minimal.h"
#include <stdint.h>
#include <string>
#include <vector>

namespace Core
{

struct StreamWriter
{
    StreamWriter();
    StreamWriter(size_t a_size, bool a_autoResize = true);
    virtual ~StreamWriter();

    std::vector<u8> GetData() const { return m_buffer; }
    u8* GetBuffer() { return &m_buffer[0]; }
    size_t GetSize() { return m_buffer.size(); }
    void Resize(size_t a_newSize, bool a_resetPtr = false);
    void ResetPtr() { m_ptr = &m_buffer[0]; }
    void Move(intptr_t a_offset);
    void WriteBE(const u8* a_buffer, size_t a_size, intptr_t a_offset = 0);
    void WriteLE(const u8* a_buffer, size_t a_size, intptr_t a_offset = 0);

    void WriteBuffer() {}

    template <typename T>
    void WriteBE(T a_value)
    {
        WriteBE((u8*)&a_value, sizeof(T));
    }
    template <typename T>
    void WriteLE(T a_value)
    {
        WriteLE((u8*)&a_value, sizeof(T));
    }
    template <typename T, size_t N>
    void WriteArratBE(T (&a_arr)[N])
    {
        WriteBE((u8*)a_arr, sizeof(T) * N);
    }
    template <typename T, size_t N>
    void WriteArratLE(T (&a_arr)[N])
    {
        WriteLE((u8*)a_arr, sizeof(T) * N);
    }

    void WriteString(
        const std::string& a_str,
        size_t a_length = 0,
        bool a_nullTerminated = true,
        intptr_t a_offset = 0);
    void WriteWString(
        const std::wstring& a_str,
        size_t a_length = 0,
        bool a_bigEndian = true,
        bool a_nullTerminated = true,
        intptr_t a_offset = 0);

protected:
    bool m_autoResize = true;
    u8* m_ptr = nullptr;
    std::vector<u8> m_buffer;
};
struct StreamReader
{
    StreamReader();
    StreamReader(u8* a_buffer, size_t a_size);
    virtual ~StreamReader();

    void SetData(u8* a_buffer, size_t a_size, size_t a_position = 0);
    u8* GetBuffer(size_t a_offset = 0) const { return m_buffer + a_offset; }
    size_t GetSize() const { return m_size; }
    u8* GetPtr() const { return m_ptr; }
    u8* GetEnd() const { return m_end; }
    void SetPtr(u8* a_ptr) { m_ptr = a_ptr; }
    void ResetPtr() { m_ptr = m_buffer; }
    bool IsEOF() { return m_ptr >= m_end; }
    void Move(intptr_t a_offset) { m_ptr += a_offset; }
    void ReadDataBE(u8* a_data, size_t a_size, intptr_t a_offset = 0);
    void ReadDataLE(u8* a_data, size_t a_size, intptr_t a_offset = 0);
    std::string ReadString(size_t a_length = 0, intptr_t a_offset = 0);
    std::wstring ReadWStringBE(size_t a_length = 0, intptr_t a_offset = 0);
    std::wstring ReadWStringLE(size_t a_length = 0, intptr_t a_offset = 0);
    template <typename T>
    T ReadBE(intptr_t a_offset = 0)
    {
        T result;
        ReadDataBE((u8*)&result, sizeof(T));
        return result;
    }
    template <typename T>
    T ReadLE(intptr_t a_offset = 0)
    {
        T result;
        ReadDataLE((u8*)&result, sizeof(T));
        return result;
    }

protected:
    u8* m_buffer = nullptr;
    u8* m_ptr = nullptr;
    u8* m_end = nullptr;
    size_t m_size = 0;
};

} // namespace Core
