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
    int width;
    int height;

    T& operator()(int x, int y) { return data[y * width + x]; }
    const T& operator()(int x, int y) const { return data[y * width + x]; }
};

#endif
