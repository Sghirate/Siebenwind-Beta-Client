#include "DataStream.h"
#include <cstring>

namespace Core
{

StreamWriter::StreamWriter()
{
}

StreamWriter::StreamWriter(size_t a_size, bool a_autoResize)
    : m_autoResize(a_autoResize)
{
    m_buffer.resize(a_size, 0);
    ResetPtr();
}

StreamWriter::~StreamWriter()
{
    m_buffer.clear();
    m_ptr = nullptr;
}

void StreamWriter::Resize(size_t a_newSize, bool a_resetPtr)
{
    m_buffer.resize(a_newSize, 0);
    if (a_resetPtr)
        ResetPtr();
}

void StreamWriter::Move(intptr_t a_offset)
{
    if (m_autoResize)
    {
        for (intptr_t i = 0; i < a_offset; ++i)
            m_buffer.push_back(0);
    }
    else if (m_ptr != nullptr)
    {
        m_ptr += a_offset;
    }
}

void StreamWriter::WriteBE(const u8* a_buffer, size_t a_size, intptr_t a_offset)
{
    if (m_autoResize)
    {
        for (i64 i = (i64)a_size - 1; i >= 0; i--)
            m_buffer.push_back(a_buffer[i]);
    }
    else if (m_ptr != nullptr)
    {
        u8* ptr = m_ptr + a_offset + a_size - 1;
        for (int i = (int)a_size - 1; i >= 0; i--)
            *(ptr - i) = a_buffer[i];
        m_ptr += a_size;
    }
}

void StreamWriter::WriteLE(const u8* a_buffer, size_t a_size, intptr_t a_offset)
{
    if (m_autoResize)
    {
        for (int i = 0; i < (int)a_size; i++)
            m_buffer.push_back(a_buffer[i]);
    }
    else if (m_ptr != nullptr)
    {
        u8* ptr = m_ptr + a_offset;
        for (int i = 0; i < (int)a_size; i++)
            ptr[i] = a_buffer[i];
        m_ptr += a_size;
    }
}

void StreamWriter::WriteString(
    const std::string& a_str, size_t a_length, bool a_nullTerminated, intptr_t a_offset)
{
    if (a_length == 0u)
        a_length = a_str.length();

    if (a_length <= a_str.length())
    {
        WriteLE((u8*)a_str.c_str(), a_length, a_offset);
    }
    else
    {
        WriteLE((u8*)a_str.c_str(), a_str.length(), a_offset);
        Move(a_length - a_str.length());
    }

    if (a_nullTerminated)
    {
        u8 nullTerminator = 0;
        WriteBE((u8*)&nullTerminator, sizeof(u8), a_offset);
    }
}

void StreamWriter::WriteWString(
    const std::wstring& a_str,
    size_t a_length,
    bool a_bigEndian,
    bool a_nullTerminated,
    intptr_t a_offset)
{
    if (a_length == 0u)
        a_length = a_str.length();

    m_ptr += a_offset;
    size_t size = a_str.length();

    if (a_length <= size)
    {
        if (a_bigEndian)
        {
            for (size_t i = 0; i < a_length; i++)
                WriteBE<i16>(a_str[i]);
        }
        else
        {
            for (size_t i = 0; i < a_length; i++)
                WriteLE<i16>(a_str[i]);
        }
    }
    else
    {
        if (a_bigEndian)
        {
            for (size_t i = 0; i < size; i++)
                WriteBE<i16>(a_str[i]);
        }
        else
        {
            for (size_t i = 0; i < size; i++)
                WriteLE<i16>(a_str[i]);
        }
        Move((a_length - size) * 2);
    }

    if (a_nullTerminated)
    {
        u16 nullTerminator = 0;
        WriteBE((u8*)&nullTerminator, sizeof(u16), a_offset);
    }
    m_ptr -= a_offset;
}

StreamReader::StreamReader()
{
}

StreamReader::StreamReader(u8* a_buffer, size_t a_size)
    : m_buffer(a_buffer)
    , m_end(a_buffer + a_size)
    , m_size(a_size)
{
    ResetPtr();
}

StreamReader::~StreamReader()
{
    m_buffer = m_ptr = m_end = nullptr;
    m_size = 0;
}

void StreamReader::SetData(u8* a_buffer, size_t a_size, size_t a_position)
{
    m_buffer = a_buffer;
    m_size = a_size;
    m_end = m_buffer + m_size;
    m_ptr = a_buffer + a_position;
}

void StreamReader::ReadDataBE(u8* a_data, size_t a_size, intptr_t a_offset)
{
    if (m_ptr)
    {
        u8* ptr = m_ptr + a_offset + a_size - 1;
        if (ptr >= m_buffer && ptr <= m_end)
        {
            for (int i = 0; i < (int)a_size; i++)
                a_data[i] = *(ptr - i);

            m_ptr += a_size;
        }
    }
}

void StreamReader::ReadDataLE(u8* a_data, size_t a_size, intptr_t a_offset)
{
    if (m_ptr)
    {
        u8* ptr = m_ptr + a_offset;
        if (ptr >= m_buffer && ptr + a_size <= m_end)
        {
            memcpy(a_data, ptr, a_size);
            m_ptr += a_size;
        }
    }
}

std::string StreamReader::ReadString(size_t a_length, intptr_t a_offset)
{
    u8* ptr = m_ptr + a_offset;
    if (a_length == 0u)
    {
        if (ptr >= m_buffer && ptr <= m_end)
        {
            u8* buf = ptr;
            while (buf <= m_end && (*buf != 0u))
                ++buf;
            a_length = (buf - ptr) + 1;
        }
    }

    auto* buf = (char*)alloca(a_length + 1);
    if (ptr >= m_buffer && ptr + a_length <= m_end)
        ReadDataLE((u8*)buf, a_length, a_offset);
    buf[a_length] = '\0';

    return { buf };
}

std::wstring StreamReader::ReadWStringBE(size_t a_length, intptr_t a_offset)
{
    u8* ptr = m_ptr + a_offset;
    if (a_length == 0u)
    {
        if (ptr >= m_buffer && ptr <= m_end)
        {
            u8* buf = ptr;
            while (buf <= m_end)
            {
                u16 val = ((buf[0] << 8) | buf[1]);
                buf += 2;
                if (val == 0u)
                {
                    break;
                }
            }

            a_length = ((buf - ptr) / 2);
        }
    }

    auto* buf = (wchar_t*)alloca((a_length + 1) * sizeof(wchar_t));
    if (ptr >= m_buffer && ptr + a_length <= m_end)
    {
        for (int i = 0; i < (int)a_length; i++)
            buf[i] = (wchar_t)ReadBE<i16>(a_offset);
    }
    buf[a_length] = 0;

    return { buf };
}

std::wstring StreamReader::ReadWStringLE(size_t a_length, intptr_t a_offset)
{
    u8* ptr = m_ptr + a_offset;
    if (a_length == 0u)
    {
        if (ptr >= m_buffer && ptr <= m_end)
        {
            u8* buf = ptr;
            while (buf <= m_end)
            {
                u16 val = *(u16*)buf;
                buf += 2;
                if (val == 0u)
                {
                    break;
                }
            }

            a_length = ((buf - ptr) / 2);
        }
    }

    auto* buf = (wchar_t*)alloca((a_length + 1) * sizeof(wchar_t));
    if (ptr >= m_buffer && ptr + a_length <= m_end)
    {
        for (int i = 0; i < (int)a_length; i++)
            buf[i] = (wchar_t)ReadLE<i16>(a_offset);
    }
    buf[a_length] = 0;

    return { buf };
}

} // namespace Core
