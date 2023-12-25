#include <algorithm>
#include <chrono>
#include <iostream>
#include <numeric>
#include <vector>

#include "cube_graph_generator.h"
#include "bfs_seq.h"
#include "bfs_par.h"
#include "types.h"

constexpr size_t TEST_ITERATIONS = 5;
constexpr size_t TEST_SIZE = 500;
constexpr bool TEST_ITER_OUTPUT = false;

static_assert(static_cast<int_t>(TEST_SIZE * TEST_SIZE * TEST_SIZE) == TEST_SIZE * TEST_SIZE * TEST_SIZE);

class TestCase {
public:
    TestCase(const char* title, const bfs_t bfs)
        : title_(title), bfs_(bfs) {
    }

    std::vector<int_t> measure(const std::vector<std::vector<int_t>>& g) {
        const auto start = std::chrono::high_resolution_clock::now();
        const auto d = bfs_(g, 0);
        const auto end = std::chrono::high_resolution_clock::now();
        durations_.push_back(std::chrono::duration_cast<std::chrono::milliseconds>(end - start));
        return d;
    }

    [[nodiscard]] long avg() const {
        const auto sum = std::accumulate(durations_.begin(), durations_.end(), std::chrono::milliseconds(0)).count();
        return sum / static_cast<long>(durations_.size());
    }

    [[nodiscard]] long min() const {
        return std::min_element(durations_.begin(), durations_.end())->count();
    }

    [[nodiscard]] long max() const {
        return std::max_element(durations_.begin(), durations_.end())->count();
    }

    friend std::ostream& operator<<(std::ostream& os, const TestCase& tc) {
        os << tc.title_ << "avg: " << tc.avg() << " ms, min: " << tc.min() << " ms, max: " << tc.max() << " ms";
        return os;
    }

private:
    const char* title_;

    const bfs_t bfs_;

    std::vector<std::chrono::milliseconds> durations_;
};

bool checkResult(const std::vector<int_t>& expected, const std::vector<int_t>& actual) {
    if (expected.size() != actual.size()) {
        return false;
    }
    for (size_t i = 0; i < expected.size(); i++) {
        if (expected[i] != actual[i]) {
            return false;
        }
    }
    return true;
}

int main() {
    const graph_t g = generate_cube_graph(TEST_SIZE);

    auto seq = TestCase("seq | ", bfs_seq);
    auto par = TestCase("par | ", bfs_par);

    for (size_t i = 0; i < TEST_ITERATIONS; i++) {
        auto d_seq = seq.measure(g);
        if constexpr (TEST_ITER_OUTPUT) {
            std::cout << seq << std::endl;
        }
        auto d_par = par.measure(g);
        if constexpr (TEST_ITER_OUTPUT) {
            std::cout << par << std::endl;
            std::cout << "speed up: " << static_cast<double>(seq.avg()) / static_cast<double>(par.avg()) << std::endl;
            std::cout << "-------------------------------------------------" << std::endl;
        }

        if (!checkResult(d_seq, d_par)) {
            std::cout << "ERROR: Results not equal!" << std::endl;
            return -1;
        }
    }

    std::cout << seq << std::endl;
    std::cout << par << std::endl;
    std::cout << "speed up: " << static_cast<double>(seq.avg()) / static_cast<double>(par.avg()) << std::endl;

    return 0;
}
