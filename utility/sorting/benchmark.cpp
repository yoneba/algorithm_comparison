#define MEASUREMENT true

#include "index.hpp"
#include "problem.hpp"

#include <iostream>
#include <string>
#include <chrono>

template <class Sorting>
void benchmark(const auto &problem)
{
    using namespace std::chrono;
    auto &[items, n_items] = problem;
    using item_type = typename std::remove_const<typename std::remove_reference<decltype(problem.items[0])>::type>::type;
    item_type *result = new item_type[n_items];
    for (int i = 0; i < n_items; i++)
        result[i] = items[i];
#if MEASUREMENT
    Sorting::measurements = Sorting::measurements::measurements();
#endif
    std::cerr << Sorting::name() << ' ' << std::flush;
    time_point<high_resolution_clock> tick = high_resolution_clock::now();
    Sorting::sort(result, result + n_items);
    std::cerr << duration_cast<duration<double>>(high_resolution_clock::now() - tick).count() << " sec" << std::endl;
    for (int i = 0; i < 10 && i < n_items; i++)
        std::cout << result[i] << ' ';
    std::cout << std::endl;
#if MEASUREMENT
    Sorting::measurements.println();
#endif
    delete[] result;
}

int main(int argc, char *argv[])
{
    constexpr int n_items_limit = 50000000;
    static sorting::problem<double, n_items_limit> problem;
    try
    {
        if (argc <= 1)
            problem.read_from_stdin();
        else if (argc != 2)
            throw std::invalid_argument("usage: ./benchmark.out {#items}");
        else
            problem.randomly_generate(std::stoi(argv[1]));
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    if (problem.n_items <= 10000)
    {
        benchmark<sorting::insertion>(problem);
        benchmark<sorting::insertion_look_ahead>(problem);
    }
    benchmark<sorting::heap<3, sorting::sift_down_strategy::normal>>(problem);
    benchmark<sorting::heap<3, sorting::sift_down_strategy::bounce>>(problem);
    benchmark<sorting::merge<32>>(problem);
    benchmark<sorting::merge<48>>(problem);
    benchmark<sorting::merge<64>>(problem);
    benchmark<sorting::quick>(problem);
    benchmark<sorting::intro<32>>(problem);
    benchmark<sorting::intro<48>>(problem);
    benchmark<sorting::intro<64>>(problem);
    benchmark<sorting::stl>(problem);
}
