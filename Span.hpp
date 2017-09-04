#ifndef Span_hpp
#define Span_hpp

template<typename T> struct Span
{
    T* data;
    int count;

    inline T* begin() { return data; }
    inline T* end() { return data + count; }

    inline const T* begin() const { return data; }
    inline const T* end() const { return data + count; }
};

template<typename T> struct Span2D
{
    T* data;
    int major;
    int minor;

    T& operator()(int majorIndex, int minorIndex)
    {
        return data[majorIndex * minor + minorIndex];
    }

    const T& operator()(int majorIndex, int minorIndex) const
    {
        return data[majorIndex * minor + minorIndex];
    }

    int Count() const { return minor * major; }
};

#endif
