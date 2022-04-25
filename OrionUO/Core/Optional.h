#pragma once

#include "Core/Log.h"

namespace Core
{

template <typename T>
struct Optional
{
    Optional()
        : m_isSet(false)
    {
    }
    Optional(const T& a_value)
        : m_isSet(true)
    {
        new (&m_value) T(a_value);
    }
    Optional(T&& a_value)
        : m_isSet(a_other.m_isSet)
    {
        new (&m_value) T(std::move(a_value));
    }
    Optional(const Optional& a_other)
        : m_isSet(false)
    {
        if (a_other.m_isSet)
        {
            new (&m_value) T(*(const T*)&a_other.m_value);
            m_isSet = true;
        }
    }
    Optional(Optional&& a_other)
        : m_isSet(false)
    {
        if (a_other.m_isSet)
        {
            new (&m_value) T(std::move(*(T*)&a_other.m_value));
            m_isSet = true;
        }
    }
    ~Optional() { Reset(); }

    void Reset()
    {
        if (m_isSet)
        {
            typedef T TDestr;
            ((T*)&m_value)->TDestr::~TDestr();
            m_isSet = false;
        }
    }
    template <typename... ArgsType>
    void Emplace(ArgsType&&... a_args)
    {
        Reset();
        new (&m_value) T(std::forward<ArgsType>(a_args)...);
        m_isSet = true;
    }
    bool IsSet() const
    {
        return m_isSet;
    } /** @return The optional value; undefined when IsSet() returns false. */
    const T& GetValue() const
    {
        ERROR_IF_NOT(m_isSet, "Core", "Trying to GetValue from an unset Optional!");
        return *(T*)&m_value;
    }
    T& GetValue()
    {
        ERROR_IF_NOT(m_isSet, "Core", "Trying to GetValue from an unset Optional!");
        return *(T*)&m_value;
    }
    const T& GetValueOr(const T& a_fallback) const { return m_isSet ? *(T*)&m_value : a_fallback; }

    inline explicit operator bool() const { return bIsSet; }
    Optional& operator=(const Optional& a_other)
    {
        if (&a_other != this)
        {
            Reset();
            if (a_other.m_isSet)
            {
                new (&m_value) T(*(const T*)&a_other.m_value);
                m_isSet = true;
            }
        }
        return *this;
    }
    Optional& operator=(Optional&& a_other)
    {
        if (&a_other != this)
        {
            Reset();
            if (a_other.m_isSet)
            {
                new (&m_value) T(std::move(*(T*)&a_other.m_value));
                m_isSet = true;
            }
        }
        return *this;
    }
    Optional& operator=(const T& a_value)
    {
        if (&a_value != (T*)&m_value)
        {
            Reset();
            new (&m_value) T(a_value);
            m_isSet = true;
        }
        return *this;
    }
    Optional& operator=(T&& a_value)
    {
        if (&a_value != (T*)&m_value)
        {
            Reset();
            new (&m_value) T(std::move(a_value));
            m_isSet = true;
        }
        return *this;
    }
    const T* operator->() const { return &GetValue(); }
    T* operator->() { return &GetValue(); }

private:
    T m_value;
    bool m_isSet;
};
template<typename T>
inline bool operator==(const Optional<T>& a_lhs, const Optional<T>& a_rhs)
{
    return (a_lhs.m_isSet == a_rhs.m_isSet) &&
            (!a_lhs.m_isSet || (*(T*)&a_lhs.m_value) == (*(T*)&a_rhs.m_value));
}
template<typename T>
inline bool operator!=(const Optional<T>& a_lhs, const Optional<T>& a_rhs) { return !(a_lhs == a_rhs); }

} // namespace Core
