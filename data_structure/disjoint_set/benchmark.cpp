#define MEASUREMENT true

#include "union_find.hpp"
#include "problem.hpp"

#include <iostream>
#include <string>
#include <chrono>

template <class DisjointSet>
void benchmark(const auto &problem)
{
    using namespace std::chrono;
    auto &[queries, n_elements, n_queries] = problem;
    int result = 0;
#if MEASUREMENT
    DisjointSet::measurements.reset();
#endif
    std::cerr << DisjointSet::name() << ' ' << std::flush;
    time_point<high_resolution_clock> tick = high_resolution_clock::now();
    DisjointSet disjoint_set(n_elements);
    for (int i = 0; i < n_queries; i++)
    {
        auto [command, x, y] = queries[i];
        switch (command)
        {
        case disjoint_set::query::command::unite:
            disjoint_set.unite(x, y);
            break;
        case disjoint_set::query::command::same:
            result += disjoint_set.find(x) == disjoint_set.find(y);
            break;
        }
    }
    std::cerr << duration_cast<duration<double>>(high_resolution_clock::now() - tick).count() << " sec" << std::endl;
    std::cout << result << std::endl;
#if MEASUREMENT
    DisjointSet::measurements.println("  ");
#endif
}

int main(int argc, char *argv[])
{
    using namespace disjoint_set;
    constexpr int N_ELEMENTS = 50000000, N_QUERIES = 50000000;
    static problem<N_ELEMENTS, N_QUERIES> problem;
    try
    {
        if (argc <= 1)
            problem.read_from_stdin();
        else if (argc != 3)
            throw std::invalid_argument("usage: ./benchmark.out {#elements} {#queries}");
        else
            problem.randomly_generate(std::stoi(argv[1]), std::stoi(argv[2]));
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    benchmark<union_find_forest_aos<find_strategy::nothing_special>>(problem);
    benchmark<union_find_forest_aos<find_strategy::path_compression>>(problem);
    benchmark<union_find_forest_aos<find_strategy::path_halving>>(problem);
    benchmark<union_find_forest_aos<find_strategy::path_halving_eager>>(problem);
    benchmark<union_find_forest<find_strategy::nothing_special>>(problem);
    benchmark<union_find_forest<find_strategy::path_compression>>(problem);
    benchmark<union_find_forest<find_strategy::path_halving>>(problem);
    benchmark<union_find_forest<find_strategy::path_halving_eager>>(problem);
}
