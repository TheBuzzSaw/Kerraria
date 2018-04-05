#ifndef Grid_hpp
#define Grid_hpp

#include <vector>
#include <cstdint>
#include <random>
#include "Point.hpp"

constexpr uint16_t NoTile = 0xffff;

struct Grid
{
    std::vector<uint16_t> tiles;
    Point<int> size = {};
};

Grid GenerateSimple(Point<int> size, std::mt19937& mt);

#endif
