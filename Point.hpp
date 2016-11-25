#ifndef POINT_HPP
#define POINT_HPP

#include "Rational.hpp"
#include "Math.hpp"

template<typename T> struct Point
{
    T x;
    T y;

    Point<T>& operator+=(Point<T> other)
    {
        x += other.x;
        y += other.y;
        return *this;
    }

    Point<T>& operator-=(Point<T> other)
    {
        x -= other.x;
        y -= other.y;
        return *this;
    }
    
    Point<T> Restricted(T lowX, T highX, T lowY, T highY) const
    {
        return {
            ::Restricted(x, lowX, highX),
            ::Restricted(y, lowY, highY)};
    }

    template<typename T2> Point<T2> Cast() const
    {
        return {
            static_cast<T2>(x),
            static_cast<T2>(y)};
    }
};

template<typename T> constexpr Point<T> operator+(Point<T> a, Point<T> b)
{
    return { a.x + b.x, a.y + b.y };
}

template<typename T> constexpr Point<T> operator-(Point<T> a, Point<T> b)
{
    return { a.x - b.x, a.y - b.y };
}

template<typename T> constexpr Point<T> operator*(Point<T> p, T multiplier)
{
    return { p.x * multiplier, p.y * multiplier };
}

template<typename T> constexpr Point<T> operator/(Point<T> p, T divisor)
{
    return { p.x / divisor, p.y / divisor };
}

template<typename T> constexpr Point<T> operator-(Point<T> p)
{
    return {-p.x, -p.y};
}

template<typename T> std::ostream& operator<<(std::ostream& stream, Point<T> p)
{
    return stream << '(' << p.x << ", " << p.y << ')';
}

template<typename T> constexpr Point<T> Divided(Point<Rational<T>> p)
{
    return { p.x.a / p.x.b, p.y.a / p.y.b };
}

template<typename T> constexpr bool operator==(Point<T> a, Point<T> b)
{
    return a.x == b.x && a.y == b.y;
}

template<typename T> constexpr bool operator!=(Point<T> a, Point<T> b)
{
    return a.x != b.x || a.y != b.y;
}

template<typename T> constexpr T LengthSquared(Point<T> p)
{
    return p.x * p.x + p.y * p.y;
}

template<typename T> constexpr T CrossZ(Point<T> a, Point<T> b)
{
    return a.x * b.y - a.y * b.x;
}

using Point8 = Point<int8_t>;
using Point16 = Point<int16_t>;
using Point32 = Point<int32_t>;
using Point64 = Point<int64_t>;

#endif
