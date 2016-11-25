#ifndef LINESEGMENT_HPP
#define LINESEGMENT_HPP

#include "Point.hpp"
#include "Rational.hpp"

template<typename T> struct LineSegment
{
    Point<T> p1;
    Point<T> p2;
};

template<typename T> std::ostream& operator<<(
    std::ostream& stream, LineSegment<T> ls)
{
    return stream << ls.p1 << " to " << ls.p2;
}

template<typename T> constexpr Rational<T> Slope(LineSegment<T> ls)
{
    return Normalized(Rational<T>{ls.p2.y - ls.p1.y, ls.p2.x - ls.p1.x});
}

template<typename T> Point<Rational<T>> Intersect(
    LineSegment<T> ls1, LineSegment<T> ls2)
{
    /// http://en.wikipedia.org/wiki/Line%E2%80%93line_intersection
    /// #Given_two_points_on_each_line

    Point<Rational<T>> result;

    T x1y2_y1x2 = ls1.p1.x * ls1.p2.y - ls1.p1.y * ls1.p2.x;
    T x3y4_y3x4 = ls2.p1.x * ls2.p2.y - ls2.p1.y * ls2.p2.x;

    result.x.a = x1y2_y1x2
        * (ls2.p1.x - ls2.p2.x)
        - (ls1.p1.x - ls1.p2.x)
        * x3y4_y3x4;

    result.x.b = (ls1.p1.x - ls1.p2.x)
        * (ls2.p1.y - ls2.p2.y)
        - (ls1.p1.y - ls1.p2.y)
        * (ls2.p1.x - ls2.p2.x);

    result.y.a = x1y2_y1x2
        * (ls2.p1.y - ls2.p2.y)
        - (ls1.p1.y - ls1.p2.y)
        * x3y4_y3x4;

    result.y.b = result.x.b;

    return result;
}

template<typename T> constexpr T CrossZ(LineSegment<T> a, LineSegment<T> b)
{
    return CrossZ(a.p2 - a.p1, b.p2 - b.p1);
}

inline void TestLines()
{
    LineSegment<int64_t> a = { { -2, -4 }, { -1, 4 } };
    LineSegment<int64_t> b = { { 2, -4 }, { 1, 4 } };
    auto intersection = Intersect(a, b);

    std::cout << intersection << " : " << Divided(intersection) << std::endl;
}

using LineSegment8 = LineSegment<int8_t>;
using LineSegment16 = LineSegment<int16_t>;
using LineSegment32 = LineSegment<int32_t>;
using LineSegment64 = LineSegment<int64_t>;

#endif

