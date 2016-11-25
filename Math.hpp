#ifndef Math_hpp
#define Math_hpp

template<typename T> constexpr T Min(T a, T b)
{
    return b < a ? b : a;
}

template<typename T> constexpr T Max(T a, T b)
{
    return b < a ? a : b;
}

template<typename T> constexpr T Restricted(T value, T low, T high)
{
    return Max(Min(value, high), low);
}

#endif