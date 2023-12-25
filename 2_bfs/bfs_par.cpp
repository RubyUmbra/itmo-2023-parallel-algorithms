#include "bfs_par.h"

#include <vector>
#include <atomic>

#include "oneapi/tbb.h"
#include "tbb/parallel_for.h"
#include "tbb/parallel_scan.h"


constexpr int_t NOT_VISITED = -1;
constexpr size_t ZERO = 0;

int_t exclusive_scan(const int_t* in, int_t* out, const size_t size) {
    out[0] = 0;
    const int_t sum = parallel_scan(
        tbb::blocked_range(ZERO, size),
        0,
        [&](const tbb::blocked_range<size_t>& r, int_t sum, const bool is_final_scan) {
            int_t tmp = sum;
            for (size_t i = r.begin(); i < r.end(); ++i) {
                tmp = tmp + in[i];
                if (is_final_scan) {
                    out[i + 1] = tmp;
                }
            }
            return tmp;
        },
        [&](const int_t& a, const int_t& b) { return a + b; });
    return sum;
}

std::vector<int_t> bfs_par(const graph_t& g, const int_t s) {
    // Limit total number of tbb worker threads that can be active in the task scheduler to 4 (5-1)
    oneapi::tbb::global_control control(oneapi::tbb::global_control::max_allowed_parallelism, 5);

    std::vector d(g.size(), NOT_VISITED);

    d[s] = 0;

    std::vector<int_t> frontier(1, s);

    for (int_t level = 1; !frontier.empty(); level++) {
        std::vector<int_t> degs(frontier.size());
        parallel_for(tbb::blocked_range(ZERO, degs.size()), [&](const tbb::blocked_range<size_t>& r) {
            for (size_t i = r.begin(); i < r.end(); ++i) {
                degs[i] = static_cast<int_t>(g[frontier[i]].size());
            }
        });

        std::vector<int_t> shifts(degs.size() + 1);
        const int_t sum = exclusive_scan(degs.data(), shifts.data(), degs.size());

        std::vector next(sum, NOT_VISITED);
        parallel_for(tbb::blocked_range(ZERO, frontier.size()), [&](const tbb::blocked_range<size_t>& r) {
            for (size_t i = r.begin(); i < r.end(); ++i) {
                const int_t u = frontier[i];
                const int_t shift = shifts[i];
                for (size_t j = 0; j < g[u].size(); j++) {
                    const int_t v = g[u][j];
                    int_t not_visited = NOT_VISITED;
                    auto& dv = reinterpret_cast<std::atomic<int_t> &>(d[(v)]);
                    if (dv.compare_exchange_strong(not_visited, level)) {
                        next[shift + j] = v;
                    }
                }
            }
        });

        std::vector<int_t> good(next.size(), 0);
        parallel_for(tbb::blocked_range(ZERO, next.size()), [&](const tbb::blocked_range<size_t>& r) {
            for (size_t i = r.begin(); i < r.end(); ++i) {
                if (next[i] != NOT_VISITED) {
                    good[i] = 1;
                }
            }
        });

        std::vector<int_t> idx(good.size() + 1);
        const int_t out_size = exclusive_scan(good.data(), idx.data(), good.size());

        frontier.resize(out_size);
        parallel_for(tbb::blocked_range(ZERO, next.size()), [&](const tbb::blocked_range<size_t>& r) {
            for (size_t i = r.begin(); i < r.end(); ++i) {
                if (next[i] != NOT_VISITED) {
                    frontier[idx[i]] = next[i];
                }
            }
        });
    }

    return d;
}
