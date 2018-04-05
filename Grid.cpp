#include "Grid.hpp"
#include "Span.hpp"
using namespace std;

Grid GenerateSimple(Point<int> size, mt19937& mt)
{
    Grid result;
    
    if (size.x < 1 || size.y < 1)
        return result;
    
    Span2D<uint16_t> span;
    span.major = size.x;
    span.minor = size.y;
    result.tiles.resize(span.Count(), NoTile);
    span.data = result.tiles.data();

    normal_distribution<double> slopeDistribution(0.0, 2.0);
    double previousSlope = 0.0;
    double previousHeight = double(span.minor) / 2.0;
    auto middle = previousHeight;
    int step = 8;

    uniform_int_distribution<uint8_t> grassTopDist(0, 2);
    uniform_int_distribution<uint8_t> stoneDist(0x10, 0x13);

    for (int i = 0; i < span.major; i += step)
    {
        double randomSlope = slopeDistribution(mt) +
            (previousHeight > middle ? -1.0 : 1.0);
        double slope = (randomSlope + previousSlope) / 2.0;
        double height = slope * double(step) + previousHeight;
        
        for (int j = 0; j < step; ++j)
        {
            double midHeight = double(j) * slope + previousHeight;
            auto n = min<int>(span.minor, int(midHeight));
            n = max<int>(n, 1);
            
            for (int k = 0; k < n; ++k)
            {
                span(i + j, n - 1 - k) = k ? stoneDist(mt) : grassTopDist(mt);
            }
        }
        
        previousSlope = slope;
        previousHeight = height;
    }

    return result;
}
