#ifndef RATIONAL_HPP
#define RATIONAL_HPP

#include <iostream>
#include <cstdint>

template<typename T> struct Rational
{
    T a;
    T b;

    Rational<T>& operator+=(Rational<T> other)
    {
        a = a * other.b + other.a * b;
        b *= other.b;
        return *this;
    }

    Rational<T>& operator-=(Rational<T> other)
    {
        a = a * other.b - other.a * b;
        b *= other.b;
        return *this;
    }

    Rational<T>& operator*=(Rational<T> other)
    {
        a *= other.a;
        b *= other.b;
        return *this;
    }

    Rational<T>& operator/=(Rational<T> other)
    {
        a *= other.b;
        b *= other.a;
        return *this;
    }
};

template<typename T>
std::ostream& operator<<(std::ostream& stream, Rational<T> r)
{
    return stream << r.a << '/' << r.b;
}

template<typename T> constexpr Rational<T> Normalized(Rational<T> r)
{
    return r.b < 0 ? Rational<T>{-r.a, -r.b} : Rational<T>{r.a, r.b};
}

template<typename T> Rational<T> Reduced(Rational<T> r)
{
    if (r.b == 0) return r;
    if (r.a == 0) return {0,1};

    r = Normalized(r);

    bool isNegative = r.a < 0;

    if (isNegative) r.a = -r.a;

    if (!(r.a & 1) && !(r.b & 1)) r.a >>= 1, r.b >>= 1;

    const T* low = r.a < r.b ? &r.a : &r.b;
    for (T i = 3; i <= *low; i += 2)
    {
        if (!(r.a % i) && !(r.b % i)) r.a /= i, r.b /= i;
    }

    if (isNegative) r.a = -r.a;

    return r;
}

template<typename T> bool operator==(Rational<T> a, Rational<T> b)
{
    a = Normalized(a);
    b = Normalized(b);
    return a.a * b.b == b.a * a.b;
}

template<typename T> bool operator!=(Rational<T> a, Rational<T> b)
{
    a = Normalized(a);
    b = Normalized(b);
    return a.a * b.b != b.a * a.b;
}

template<typename T> bool operator<(Rational<T> a, Rational<T> b)
{
    a = Normalized(a);
    b = Normalized(b);
    return a.a * b.b < b.a * a.b;
}

template<typename T> bool operator>(Rational<T> a, Rational<T> b)
{
    a = Normalized(a);
    b = Normalized(b);
    return a.a * b.b > b.a * a.b;
}

template<typename T> bool operator<=(Rational<T> a, Rational<T> b)
{
    a = Normalized(a);
    b = Normalized(b);
    return a.a * b.b <= b.a * a.b;
}

template<typename T> bool operator>=(Rational<T> a, Rational<T> b)
{
    a = Normalized(a);
    b = Normalized(b);
    return a.a * b.b >= b.a * a.b;
}

typedef Rational<int8_t> Rational8;
typedef Rational<int16_t> Rational16;
typedef Rational<int32_t> Rational32;
typedef Rational<int64_t> Rational64;

inline void Test(Rational64 a, Rational64 b)
{
    std::cout << a << " < " << b << " : " << (a < b) << std::endl;
}

inline void TestRationals()
{
    Test({1, 4}, {1, 3});
    Test({1, 3}, {1, 4});
    Test({1, 3}, {2, 4});
    Test({-1, 4}, {-1, 3});
    Test({-1, 3}, {-1, 4});
    Test({-1, 4}, {1, 3});
    Test({1, 4}, {-1, 3});
    Test({-1, -4}, {-1, -3});
    Test({1, -3}, {-1, 4});
    Test({-1, 3}, {1, -4});

    Rational64 r{6, -21};
    std::cout << r << " --> " << Reduced(r) << std::endl;
}

#endif

