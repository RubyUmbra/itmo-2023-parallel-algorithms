#include "cube_graph_generator.h"

#ifndef NDEBUG
#include <algorithm>
#include <cassert>
#endif //NDEBUG

graph_t generate_cube_graph(const size_t size) {
    auto point_to_num = [size](const size_t x, const size_t y, const size_t z) {
        return static_cast<int_t>(x + y * size + z * size * size);
    };

    graph_t g(size * size * size);
    for (size_t z = 0; z < size; z++) {
        for (size_t y = 0; y < size; y++) {
            for (size_t x = 0; x < size; x++) {
                const int_t v = point_to_num(x, y, z);
                if (0 < z) {
                    g[v].push_back(point_to_num(x, y, z - 1));
                }
                if (0 < y) {
                    g[v].push_back(point_to_num(x, y - 1, z));
                }
                if (0 < x) {
                    g[v].push_back(point_to_num(x - 1, y, z));
                }
                if (x + 1 < size) {
                    g[v].push_back(point_to_num(x + 1, y, z));
                }
                if (y + 1 < size) {
                    g[v].push_back(point_to_num(x, y + 1, z));
                }
                if (z + 1 < size) {
                    g[v].push_back(point_to_num(x, y, z + 1));
                }
            }
        }
    }

#ifndef NDEBUG
    assert(g.size() == size * size * size);
    for (size_t z = 0; z < size; z++) {
        for (size_t y = 0; y < size; y++) {
            for (size_t x = 0; x < size; x++) {
                const size_t v = point_to_num(x, y, z);
                for (const size_t u: g[v]) {
                    assert(std::find(g[u].begin(), g[u].end(), v) != g[u].end());
                }
            }
        }
    }
#endif //NDEBUG

    return g;
}
