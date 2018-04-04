#ifndef Grid_hpp
#define Grid_hpp

#include <vector>
#include <cstdint>
#include <random>
#include "Point.hpp"

class Grid
{
    std::vector<uint16_t> _tiles;
    Point<int> _size = {};

public:
    Grid() = default;
    Grid(Grid&&) = default;
    Grid(const Grid&) = default;
    ~Grid() = default;

    Grid& operator=(Grid&&) = default;
    Grid& operator=(const Grid&) = default;
};

#endif
