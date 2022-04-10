#pragma once

namespace Core
{

template<typename T>
struct Vec2
{
    T x, y;

    Vec2(T a_value = (T)0) : x(a_value), y(a_value) {}
    Vec2(T a_x, T a_y) : x(a_x), y(a_y) {}

    Vec2 operator+(const Vec2& a_v) const { return Vec2(x + a_v.x, y + a_v.y); }
    Vec2 operator-(const Vec2& a_v) const { return Vec2(x - a_v.x, y - a_v.y); }
    Vec2 operator-() const { return Vec2(-x, -y); }
    Vec2 operator*(const T a_s) const { return Vec2(x * a_s, y * a_s); }
    Vec2 operator/(const T a_s) const { return Vec2(x / a_s, y / a_s); }
    Vec2 operator=(const Vec2& a_v) { x = a_v.x; y = a_v.y; return *this; }
    Vec2 operator+=(const Vec2& a_v) { return *this = (*this + a_v); }
    Vec2 operator-=(const Vec2& a_v) { return *this = (*this - a_v); }
    Vec2 operator*=(const T& a_s) { return *this = (*this * a_s); }
    Vec2 operator/=(T a_d) { return *this = (*this / a_d); }
    bool operator==(const Vec2& a_v) const { return x == a_v.x && y == a_v.y; }
    bool operator!=(const Vec2& a_v) const { return !(*this == a_v); }
    T const& operator[](int a_index) const { return *reinterpret_cast<T*>(reinterpret_cast<void*>(this) + sizeof(T) * a_index); }

    Vec2 set(const T a_x, const T a_y) { x = a_x; y = a_y; return *this; }
    double dot(const Vec2& a_v) const { return (x * a_v.x) + (y * a_v.y); }
    double lengthSquared() const { return this->dot(*this); }
    double length() const { return std::sqrt(lengthSquared()); }
    Vec2 normalize() const { const T len = length(); return (len > 0) ? (*this) / len : (*this); }	
};

template<typename T>
struct Vec3
{
    T x, y, z;

    Vec3(T a_value = (T)0) : x(a_value), y(a_value), z(a_value) {}
    Vec3(T a_x, T a_y, T a_z) : x(a_x), y(a_y), z(a_z) {}

    Vec3 operator+(const Vec3& a_v) const { return Vec3(x + a_v.x, y + a_v.y, z + a_v.z); }
    Vec3 operator-(const Vec3& a_v) const { return Vec3(x - a_v.x, y - a_v.y, z - a_v.z); }
    Vec3 operator-() const { return Vec3(-x, -y, -z); }
    Vec3 operator*(const T a_s) const { return Vec3(x * a_s, y * a_s, z * a_s); }
    Vec3 operator/(const T a_s) const { return Vec3(x / a_s, y / a_s, z / a_s); }
    Vec3 operator=(const Vec3& a_v) { x = a_v.x; y = a_v.y; z = a_v.z; return *this; }
    Vec3 operator+=(const Vec3& a_v) { return *this = (*this + a_v); }
    Vec3 operator-=(const Vec3& a_v) { return *this = (*this - a_v); }
    Vec3 operator*=(const T& a_s) { return *this = (*this * a_s); }
    Vec3 operator/=(T a_d) { return *this = (*this / a_d); }
    bool operator==(const Vec3& a_v) const { return x == a_v.x && y == a_v.y && z == a_v.z; }
    bool operator!=(const Vec3& a_v) const { return !(*this == a_v); }
    T const& operator[](int a_index) const { return *reinterpret_cast<T*>(reinterpret_cast<void*>(this) + sizeof(T) * a_index); }

    Vec3 set(const T a_x, const T a_y, const T a_z) { x = a_x; y = a_y; z = a_z; return *this; }
    double dot(const Vec3& a_v) const { return (x * a_v.x) + (y * a_v.y) + (z * a_v.z); }
    double lengthSquared() const { return this->dot(*this); }
    double length() const { return std::sqrt(lengthSquared()); }
    Vec3 normalize() const { const T len = length(); return (len > 0) ? (*this) / len : (*this); }
    Vec3 cross(const Vec3& a, const Vec3& b) const { Vec3((a.y * b.z) - (a.z * b.y), (a.z * b.x) - (a.x * b.z), (a.x * b.y) - (a.y * b.x)); }
    Vec3 cross(const Vec3& a_v) { return Vec3((y * a_v.z) - (z * a_v.y), (z * a_v.x) - (x * a_v.z), (x * a_v.y) - (y * a_v.x)); }
};

template<typename T>
struct Vec4
{
    T x, y, z, w;

    Vec4(T a_value = (T)0) : x(a_value), y(a_value), z(a_value), w(a_value) {}
    Vec4(T a_x, T a_y, T a_z) : x(a_x), y(a_y), x(a_x) {}

    Vec4 operator+(const Vec4& a_v) const { return Vec4(x + a_v.x, y + a_v.y, z + a_v.z, w + a_v.w); }
    Vec4 operator-(const Vec4& a_v) const { return Vec4(x - a_v.x, y - a_v.y, z - a_v.z, w - a_v.w); }
    Vec4 operator-() const { return Vec4(-x, -y, -z, -w); }
    Vec4 operator*(const T a_s) const { return Vec4(x * a_s, y * a_s, z * a_s, w * a_s); }
    Vec4 operator/(const T a_s) const { return Vec4(x / a_s, y / a_s, z / a_s, w / a_s); }
    Vec4 operator=(const Vec4& a_v) { x = a_v.x; y = a_v.y; z = a_v.z; w = a_v.w; return *this; }
    Vec4 operator+=(const Vec4& a_v) { return *this = (*this + a_v); }
    Vec4 operator-=(const Vec4& a_v) { return *this = (*this - a_v); }
    Vec4 operator*=(const T& a_s) { return *this = (*this * a_s); }
    Vec4 operator/=(T a_d) { return *this = (*this / a_d); }
    bool operator==(const Vec4& a_v) const { return x == a_v.x && y == a_v.y && z == a_v.z && w == a_v.w; }
    bool operator!=(const Vec4& a_v) const { return !(*this == a_v); }
    T const& operator[](int a_index) const { return *reinterpret_cast<T*>(reinterpret_cast<void*>(this) + sizeof(T) * a_index); }

    Vec4 set(const T a_x, const T a_y, const T a_z, const T a_w) { x = a_x; y = a_y; z = a_z; w = a_w; return *this; }
    double dot(const Vec4& a_v) const { return (x * a_v.x) + (y * a_v.y) + (z * a_v.z) + (w * a_v.w); }
    double lengthSquared() const { return this->dot(*this); }
    double length() const { return std::sqrt(lengthSquared()); }
    Vec4 normalize() const { const T len = length(); return (len > 0) ? (*this) / len : (*this); }
};

} // namespace Core
