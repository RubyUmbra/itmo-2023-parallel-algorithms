#include "bfs_par.h"

#include <atomic>

#include "datapar.hpp"

constexpr int_t NOT_VISITED = -1;

std::vector<int_t> bfs_par(const graph_t& g, const int_t s) {
    auto plus = [](const int_t a, const int_t b) { return a + b; };
    auto is_visited = [](const int_t v) { return v != NOT_VISITED; };

    std::vector d(g.size(), NOT_VISITED);

    d[s] = 0;
    pasl::pctl::parray frontier(1, s);

    for (int_t level = 1; frontier.size() != 0; level++) {
        pasl::pctl::parray<int_t> degs(frontier.size(), [&](const long i) { return g[frontier[i]].size(); });
        pasl::pctl::parray<int_t> shifts = scan(degs.cbegin(), degs.cend(), 0, plus,
                                                pasl::pctl::forward_exclusive_scan);
        const int_t sum = shifts[shifts.size() - 1] + degs[degs.size() - 1];

        pasl::pctl::parray next(sum, NOT_VISITED);
        pasl::pctl::parallel_for(static_cast<long>(0), frontier.size(), [&](const long i) {
            const int_t u = frontier[i];
            const int_t shift = shifts[i];
            for (int_t j = 0; j < g[u].size(); j++) {
                const int_t v = g[u][j];
                int_t not_visited = NOT_VISITED;
                auto& dv = reinterpret_cast<std::atomic<int_t> &>(d[(v)]);
                if (dv.compare_exchange_strong(not_visited, level)) {
                    next[shift + j] = v;
                }
            }
        });

        frontier = pasl::pctl::filter(next.cbegin(), next.cend(), is_visited);
    }

    return d;
}
