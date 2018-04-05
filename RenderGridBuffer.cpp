#include "RenderGridBuffer.hpp"
#include "Span.hpp"
#include <utility>
using namespace std;

static pair<float, float> GetTexCoords(int index)
{
    int offset = index * 64;

    return {
        offset / 1024.0f,
        (offset + 64) / 1024.0f};
}

void RenderGridBuffer::Generate(const Grid& source, Point<int> start, Point<int> size)
{
    vertexData.clear();
    vertexData.reserve(1024);

    Span2D<const uint16_t> span;
    span.major = source.size.x;
    span.minor = source.size.y;
    span.data = source.tiles.data();

    for (int i = 0; i < size.x; ++i)
    {
        for (int j = 0; j < size.y; ++j)
        {
            uint8_t tile = span(start.x + i, start.y + j);
            if (tile == NoTile) continue;
            int xi = tile & 0xf;
            int yi = (tile >> 4) & 0xf;

            auto tcs = GetTexCoords(xi);
            auto tct = GetTexCoords(yi);

            // I don't know how else to close the gaps.
            constexpr float Lip = 1.0f / 1024.0f;
            auto x = static_cast<float>(i) - Lip;
            auto y = static_cast<float>(j) - Lip;
            auto xx = static_cast<float>(i + 1) + Lip;
            auto yy = static_cast<float>(j + 1) + Lip;

            vertexData.push_back(x);
            vertexData.push_back(y);
            vertexData.push_back(tcs.first);
            vertexData.push_back(tct.second);
            vertexData.push_back(1.0f);
            vertexData.push_back(1.0f);
            vertexData.push_back(1.0f);

            vertexData.push_back(x);
            vertexData.push_back(yy);
            vertexData.push_back(tcs.first);
            vertexData.push_back(tct.first);
            vertexData.push_back(1.0f);
            vertexData.push_back(1.0f);
            vertexData.push_back(1.0f);

            vertexData.push_back(xx);
            vertexData.push_back(yy);
            vertexData.push_back(tcs.second);
            vertexData.push_back(tct.first);
            vertexData.push_back(1.0f);
            vertexData.push_back(1.0f);
            vertexData.push_back(1.0f);

            vertexData.push_back(x);
            vertexData.push_back(y);
            vertexData.push_back(tcs.first);
            vertexData.push_back(tct.second);
            vertexData.push_back(1.0f);
            vertexData.push_back(1.0f);
            vertexData.push_back(1.0f);

            vertexData.push_back(xx);
            vertexData.push_back(yy);
            vertexData.push_back(tcs.second);
            vertexData.push_back(tct.first);
            vertexData.push_back(1.0f);
            vertexData.push_back(1.0f);
            vertexData.push_back(1.0f);

            vertexData.push_back(xx);
            vertexData.push_back(y);
            vertexData.push_back(tcs.second);
            vertexData.push_back(tct.second);
            vertexData.push_back(1.0f);
            vertexData.push_back(1.0f);
            vertexData.push_back(1.0f);
        }
    }
}
