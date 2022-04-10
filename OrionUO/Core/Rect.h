#pragma once

#include "Vec.h"

namespace Core
{

template<typename T>
struct Rect
{
    Vec2<T> pos;
    Vec2<T> size;

    Rect() {}
    Rect(Vec2<T> a_pos, Vec2<T> a_size) : pos(a_pos), size(a_size) {}
    Rect(Vec2<T> a_pos, T a_w, T a_h) : pos(a_pos), size(a_w, a_h) {}
    Rect(T a_x, T a_y, Vec2<T> a_size) : pos(a_x, a_y), size(a_size) {}
    Rect(T a_x, T a_y, T a_w, T a_h) : pos(a_x, a_y), size(a_w, a_h) {}
};

} // namespace Core
