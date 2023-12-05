#include <cilk/cilk.h>
#include <oneapi/tbb/global_control.h>
#include <oneapi/tbb/parallel_invoke.h>
#include <stddef.h>

#include <algorithm>
#include <cassert>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <numeric>
#include <vector>

constexpr size_t TEST_ITERATIONS = 5;
constexpr size_t TEST_SIZE = 100000000;
constexpr size_t TEST_CUTOFF = 1000;

size_t partition(int* a, size_t l, size_t r) {
    int v = a[(l + r) / 2];
    size_t i = l;
    size_t j = r;
    while (i <= j) {
        while (a[i] < v) {
            i++;
        }
        while (a[j] > v) {
            j--;
        }
        if (i >= j) {
            break;
        }
        std::swap(a[i++], a[j--]);
    }
    return j;
}

void quicksort_seq(int* a, size_t l, size_t r) {
    if (l < r) {
        int q = partition(a, l, r);
        quicksort_seq(a, l, q);
        quicksort_seq(a, q + 1, r);
    }
}

void quicksort_par_cilk(int* a, size_t l, size_t r) {
    if (l < r) {
        int q = partition(a, l, r);
        cilk_scope {
            cilk_spawn quicksort_par_cilk(a, l, q);
            quicksort_par_cilk(a, q + 1, r);
        }
    }
}

void quicksort_par_cilk_opt(int* a, size_t l, size_t r) {
    if (l < r) {
        if (r - l < TEST_CUTOFF) {
            quicksort_seq(a, l, r);
            return;
        }
        int q = partition(a, l, r);
        cilk_scope {
            cilk_spawn quicksort_par_cilk_opt(a, l, q);
            quicksort_par_cilk_opt(a, q + 1, r);
        }
    }
}

void quicksort_par_tbb(int* a, size_t l, size_t r) {
    if (l < r) {
        int q = partition(a, l, r);
        oneapi::tbb::parallel_invoke(
            [=] { quicksort_par_tbb(a, l, q); },
            [=] { quicksort_par_tbb(a, q + 1, r); }
        );
    }
}

void quicksort_par_tbb_opt(int* a, size_t l, size_t r) {
    if (l < r) {
        if (r - l < TEST_CUTOFF) {
            quicksort_seq(a, l, r);
            return;
        }
        int q = partition(a, l, r);
        oneapi::tbb::parallel_invoke(
            [=] { quicksort_par_tbb_opt(a, l, q); },
            [=] { quicksort_par_tbb_opt(a, q + 1, r); }
        );
    }
}

void quicksort(std::vector<int>& a, void (*sort)(int*, size_t, size_t)) {
    sort(a.data(), 0, a.size() - 1);
}

void quicksort_seq(std::vector<int>& a) {
    quicksort(a, quicksort_seq);
}

void quicksort_par_cilk(std::vector<int>& a) {
    quicksort(a, quicksort_par_cilk);
}

void quicksort_par_cilk_opt(std::vector<int>& a) {
    quicksort(a, quicksort_par_cilk_opt);
}

void quicksort_par_tbb(std::vector<int>& a) {
    quicksort(a, quicksort_par_tbb);
}

void quicksort_par_tbb_opt(std::vector<int>& a) {
    quicksort(a, quicksort_par_tbb_opt);
}

void generateTestData(std::vector<int>& vec, size_t size) {
    std::srand(0);
    for (size_t i = 0; i < size; i++) {
        vec.push_back(std::rand());
    }
}

class TestCase {
   public:
    TestCase(const char* title, void (*sort)(std::vector<int>&))
        : title_(title), sort_(sort) {}

    void measure(std::vector<int>& vec, const std::vector<int>& expected) {
        auto start = std::chrono::high_resolution_clock::now();
        sort_(vec);
        auto end = std::chrono::high_resolution_clock::now();
        durations_.push_back(std::chrono::duration_cast<std::chrono::milliseconds>(end - start));
        checkResult(expected, vec);
    }

    friend std::ostream& operator<<(std::ostream& os, const TestCase& tc) {
        auto avg = std::accumulate(tc.durations_.begin(), tc.durations_.end(), std::chrono::milliseconds(0)).count() / tc.durations_.size();
        auto max = std::max_element(tc.durations_.begin(), tc.durations_.end())->count();
        auto min = std::min_element(tc.durations_.begin(), tc.durations_.end())->count();

        os << tc.title_ << "avg: " << avg << " ms, min: " << min << " ms, max: " << max << " ms";
        return os;
    }

   private:
    const char* title_;
    std::vector<std::chrono::milliseconds> durations_;
    void (*sort_)(std::vector<int>&);

    void checkResult(const std::vector<int>& expected, const std::vector<int>& actual) {
        assert(expected.size() == actual.size());
        for (size_t i = 0; i < expected.size(); i++) {
            assert(expected[i] == actual[i]);
        }
    }
};

int main() {
    // Limit total number of tbb worker threads that can be active in the task scheduler to 4 (5-1)
    oneapi::tbb::global_control control(oneapi::tbb::global_control::max_allowed_parallelism, 5);

    auto tc_std = TestCase("             std | ", [](std::vector<int>& vec) { std::sort(vec.begin(), vec.end()); });
    std::vector<TestCase> test_cases = {
        TestCase("      Sequential | ", quicksort_seq),
        TestCase(" Parallel (Cilk) | ", quicksort_par_cilk),
        TestCase("Parallel (Cilk+) | ", quicksort_par_cilk_opt),
        TestCase("  Parallel (TBB) | ", quicksort_par_tbb),
        TestCase(" Parallel (TBB+) | ", quicksort_par_tbb_opt),
    };

    for (size_t i = 0; i < TEST_ITERATIONS; i++) {
        std::vector<int> data;
        generateTestData(data, TEST_SIZE);

        std::vector<int> data_std = data;
        tc_std.measure(data_std, data_std);

        for (auto& tc : test_cases) {
            std::vector<int> data_tc = data;
            tc.measure(data_tc, data_std);
        }
    }

    for (auto& tc : test_cases) {
        std::cout << tc << std::endl;
    }
    std::cout << tc_std << std::endl;

    return 0;
}
