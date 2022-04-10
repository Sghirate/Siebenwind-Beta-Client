#pragma once

#include <type_traits>

template <class T, std::enable_if_t<std::is_enum_v<T>, int> = 0>
inline T operator~(T a)
{
    return static_cast<T>(~static_cast<std::underlying_type<T>::type>(a));
}
template <class T>
inline T operator|(T a, T b)
{
    return static_cast<T>(
        static_cast<std::underlying_type<T>::type>(a) |
        static_cast<std::underlying_type<T>::type>(b));
}
template <class T>
inline T operator&(T a, T b)
{
    return static_cast<T>(
        static_cast<std::underlying_type<T>::type>(a) &
        static_cast<std::underlying_type<T>::type>(b));
}
template <class T>
inline T operator^(T a, T b)
{
    return static_cast<T>(
        static_cast<std::underlying_type<T>::type>(a) ^
        static_cast<std::underlying_type<T>::type>(b));
}
template <class T>
inline T& operator|=(T& a, T b)
{
    a = static_cast<T>(
        static_cast<std::underlying_type<T>::type>(a) |
        static_cast<std::underlying_type<T>::type>(b));
    return a;
}
template <class T>
inline T& operator&=(T& a, T b)
{
    a = static_cast<T>(
        static_cast<std::underlying_type<T&>::type>(a) &
        static_cast<std::underlying_type<T>::type>(b));
    return a;
}
template <class T>
inline T& operator^=(T& a, T b)
{
    a = static_cast<T>(
        static_cast<std::underlying_type<T>::type&>(a) ^
        static_cast<std::underlying_type<T>::type>(b));
    return a;
}
