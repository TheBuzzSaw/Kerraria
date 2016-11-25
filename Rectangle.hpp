#ifndef RECTANGLE_HPP
#define RECTANGLE_HPP

#include "LineSegment.hpp"
#include <algorithm>

template<typename T> struct Rectangle
{
    Point<T> low;
    Point<T> high;
};

template<typename T> std::ostream& operator<<(
    std::ostream& stream, Rectangle<T> r)
{
    return stream << "low " << r.low << " high " << r.high;
}

template<typename T> constexpr bool IsValid(Rectangle<T> r)
{
    return r.low.x <= r.high.x && r.low.y <= r.high.y;
}

template<typename T> inline void SetLowToHigh(T& a, T& b)
{
    if (b < a) std::swap(a, b);
}

template<typename T> Rectangle<T> MakeRectangle(LineSegment<T> ls)
{
    Rectangle<T> r = { ls.p1, ls.p2 };
    SetLowToHigh(r.low.x, r.high.x);
    SetLowToHigh(r.low.y, r.high.y);

    return r;
}

template<typename T> constexpr bool Contains(
    Rectangle<T> outer, Rectangle<T> inner)
{
    return
        outer.low.x < inner.low.x &&
        inner.high.x < outer.high.x &&
        outer.low.y < inner.low.y &&
        inner.high.y < outer.high.y;
}

template<typename T> constexpr bool Overlaps(
    Rectangle<T> r1, Rectangle<T> r2)
{
    return !(
        r1.high.x < r2.low.x ||
        r2.high.x < r1.low.x ||
        r1.high.y < r2.low.y ||
        r2.high.y < r1.low.y);
}

using Rectangle8 = Rectangle<int8_t>;
using Rectangle16 = Rectangle<int16_t>;
using Rectangle32 = Rectangle<int32_t>;
using Rectangle64 = Rectangle<int64_t>;
#endif

