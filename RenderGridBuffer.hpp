#ifndef RenderGridBuffer_hpp
#define RenderGridBuffer_hpp

#include "Grid.hpp"
#include "Matrix4x4.hpp"

struct RenderGridBuffer
{
    Matrix4x4<float> matrix = Identity4x4<float>();
    std::vector<float> vertexData;

    void Generate(const Grid& source, Point<int> start, Point<int> size);
};

#endif
