#pragma once

#include "Core/Minimal.h"

namespace Core
{

template <size_t BITS>
struct BitSet
{
    struct Reference
    {
        Reference& operator=(bool a_value)
        {
            m_bitSet->Set(m_bit, a_value);
            return *this;
        }
        Reference& Flip()
        {
            m_bitSet->Flip(m_bit);
            return *this;
        }
        bool operator~() const { return !m_bitSet->Get(m_bit); }
        operator bool() const { return m_bitSet->Get(m_bit); }

    private:
        friend struct BitSet<BITS>;
        Reference(BitSet* a_bitSet, int a_bit)
            : m_bitSet(a_bitSet)
            , m_bit(a_bit)
        {
        }

        BitSet* m_bitSet;
        int m_bit;
    };

    BitSet() { Reset(); }

    inline void Reset() { memset(m_bytes, 0, sizeof(u8) * kByteCount); }
    inline bool Get(int a_bit) const
    {
        const u8 idx  = a_bit / 8;
        const u8 mask = 1 << (a_bit % 8);
        return (m_bytes[idx] & mask) == mask;
    }
    inline void Set(int a_bit, bool a_value)
    {
        const u8 idx  = a_bit / 8;
        const u8 mask = 1 << (a_bit % 8);
        if (a_value)
            m_bytes[idx] |= mask;
        else
            m_bytes[idx] &= ~mask;
    }
    inline void Flip(int a_bit)
    {
        const u8 idx  = a_bit / 8;
        const u8 mask = 1 << (a_bit % 8);
        m_bytes[idx] ^= mask;
    }
    Reference operator[](int a_bit) { return Reference(this, a_bit); }

private:
    enum
    {
        kByteCount = (BITS / 8 + (BITS % 8 != 0))
    };
    u8 m_bytes[kByteCount];
};
struct DynamicBitSet
{
    struct Reference
    {
        Reference& operator=(bool a_value)
        {
            m_bitSet->Set(m_bit, a_value);
            return *this;
        }
        Reference& Flip()
        {
            m_bitSet->Flip(m_bit);
            return *this;
        }
        bool operator~() const { return !m_bitSet->Get(m_bit); }
        operator bool() const { return m_bitSet->Get(m_bit); }

    private:
        friend struct DynamicBitSet;
        Reference(DynamicBitSet* a_bitSet, int a_bit)
            : m_bitSet(a_bitSet)
            , m_bit(a_bit)
        {
        }

        DynamicBitSet* m_bitSet;
        int m_bit;
    };

    DynamicBitSet()
        : m_bytes(nullptr)
    {
        Reset();
    }
    DynamicBitSet(const DynamicBitSet& a_other)
        : m_byteCount(a_other.m_byteCount)
    {
        if (a_other.m_bytes != nullptr)
        {
            m_bytes = (u8*)malloc(m_byteCount);
            memcpy(m_bytes, a_other.m_bytes, m_byteCount);
        }
        else
        {
            m_bytes = nullptr;
        }
    }
    DynamicBitSet(DynamicBitSet&& a_other)
        : m_byteCount(std::exchange(a_other.m_byteCount, 0))
        , m_bytes(std::exchange(a_other.m_bytes, nullptr))
    {
    }
    ~DynamicBitSet() { Reset(); }

    inline void Init(size_t a_bits)
    {
        Reset();
        m_byteCount = (a_bits / 8 + (a_bits % 8 != 0));
        m_bytes     = (u8*)malloc(m_byteCount);
        memset(m_bytes, 0, m_byteCount);
    }
    inline void Reset()
    {
        m_byteCount = 0;
        if (m_bytes)
            delete (m_bytes);
        m_bytes = nullptr;
    }
    inline bool Get(int a_bit) const
    {
        const u8 idx  = a_bit / 8;
        const u8 mask = 1 << (a_bit % 8);
        return (m_bytes && idx < m_byteCount) ? ((m_bytes[idx] & mask) == mask) : nullptr;
    }
    inline void Set(int a_bit, bool a_value)
    {
        const u8 idx  = a_bit / 8;
        const u8 mask = 1 << (a_bit % 8);
        if (!m_bytes || idx >= m_byteCount)
            return;
        if (a_value)
            m_bytes[idx] |= mask;
        else
            m_bytes[idx] &= ~mask;
    }
    inline void Flip(int a_bit)
    {
        const u8 idx  = a_bit / 8;
        const u8 mask = 1 << (a_bit % 8);
        if (m_bytes && idx < m_byteCount)
            m_bytes[idx] ^= mask;
    }
    Reference operator[](int a_bit) { return Reference(this, a_bit); }

private:
    size_t m_byteCount;
    u8* m_bytes;
};

} // namespace Core
