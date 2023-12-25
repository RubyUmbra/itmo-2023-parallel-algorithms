#ifndef TYPES_H
#define TYPES_H
#include <cstddef>
#include <cstdint>
#include <vector>

using int_t = int32_t;
using graph_t = std::vector<std::vector<int_t>>;
using bfs_t = std::vector<int_t> (*)(const graph_t&, int_t);

#endif //TYPES_H
