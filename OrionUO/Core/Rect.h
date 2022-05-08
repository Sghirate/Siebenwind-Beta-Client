#pragma once

#include "Vec.h"

namespace Core
{

template<typename T>
struct Rect
{
    union
    {
        struct
        {
            Vec2<T> pos;
            Vec2<T> size;
        };
        struct
        {
            T x;
            T y;
            T w;
            T h;
        };
    };

    Rect() {}
    Rect(Vec2<T> a_pos, Vec2<T> a_size) : pos(a_pos), size(a_size) {}
    Rect(Vec2<T> a_pos, T a_w, T a_h) : pos(a_pos), size(a_w, a_h) {}
    Rect(T a_x, T a_y, Vec2<T> a_size) : pos(a_x, a_y), size(a_size) {}
    Rect(T a_x, T a_y, T a_w, T a_h) : pos(a_x, a_y), size(a_w, a_h) {}

    inline void set(const Vec2<T>& a_pos, const Vec2<T>& a_size) { pos = a_pos; size = a_size; }
    inline void set(T a_x, T a_y, T a_w, T a_h) { pos.set(a_x, a_y); size.set(a_w, a_h); }
    inline bool contains(T a_x, T a_y) const { return pos.x <= a_x && a_x <= pos.x + size.x && pos.y <= a_y && a_y <= pos.y + size.y; }
    inline bool contains(const Vec2<T>& a_pos) const { return contains(a_pos.x, a_pos.y); }
};

} // namespace Core
