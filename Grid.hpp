#ifndef Grid_hpp
#define Grid_hpp

template<typename T> struct Grid
{
    T* data;
    int width;
    int height;

    T& operator()(int x, int y)
    {
        return data[y * width + x];
    }

    T operator()(int x, int y) const
    {
        return data[y * width + x];
    }

    int Count() const { return width * height; }
};

#endif