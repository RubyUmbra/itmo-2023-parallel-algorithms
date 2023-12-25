#include "bfs_seq.h"

#include <queue>

constexpr int_t NOT_VISITED = -1;

std::vector<int_t> bfs_seq(const graph_t& g, const int_t s) {
    std::queue<int_t> queue;
    std::vector d(g.size(), NOT_VISITED);

    queue.push(s);
    d[s] = 0;

    while (!queue.empty()) {
        const int_t u = queue.front();
        queue.pop();
        for (size_t i = 0; i < g[u].size(); i++) {
            if (const int_t v = g[u][i]; d[v] == NOT_VISITED) {
                queue.push(v);
                d[v] = d[u] + 1;
            }
        }
    }

    return d;
}
