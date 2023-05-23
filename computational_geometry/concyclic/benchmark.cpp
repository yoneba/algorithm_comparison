#define MEASUREMENT false

#include "problem.hpp"
#include "inscribed_angle_approach.hpp"

#include <iostream>
#include <chrono>
#include <string>

template <class Concyclic>
void benchmark(const auto &problem)
{
    using namespace std::chrono;
    auto &[points, n_points] = problem;
#if MEASUREMENT
    Concyclic::measurements.clear();
#endif
    std::cerr << Concyclic::full_name() << ' ' << std::flush;
    time_point<high_resolution_clock> tick = high_resolution_clock::now();
    long long result = Concyclic::count(points, n_points);
    std::cerr << duration_cast<duration<double>>(high_resolution_clock::now() - tick).count() << " sec" << std::endl;
    std::cout << result << std::endl;
#if MEASUREMENT
    Concyclic::measurements.println();
#endif
}

int main(int argc, char *argv[])
{
    constexpr int width_limit = 10000;
    constexpr int height_limit = 10000;
    constexpr int n_points_limit = 10000;
    static concyclic::problem<double, width_limit, height_limit, n_points_limit> problem;
    try
    {
        if (argc == 2 && argv[1] == std::string("-"))
            problem.read_from_stdin();
        else if (argc != 4)
            throw std::invalid_argument("usage: ./benchmark.out {width} {height} {#points}");
        else
            problem.randomly_generate(std::stoi(argv[1]), std::stoi(argv[2]), std::stoi(argv[3]));
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    benchmark<concyclic::inscribed_angle_sorting<sorting::heap<2>>>(problem);
    benchmark<concyclic::inscribed_angle_sorting<sorting::heap<3>>>(problem);
    benchmark<concyclic::inscribed_angle_sorting<sorting::heap<4>>>(problem);
    benchmark<concyclic::inscribed_angle_sorting<sorting::heap<5>>>(problem);
    benchmark<concyclic::inscribed_angle_sorting<sorting::intro<>>>(problem);
    benchmark<concyclic::inscribed_angle_sorting<sorting::merge<>>>(problem);
    benchmark<concyclic::inscribed_angle_sorting<sorting::stl>>(problem);
    if (problem.n_points <= 1000)
    {
        benchmark<concyclic::inscribed_angle_sorting<sorting::insertion>>(problem);
        benchmark<concyclic::inscribed_angle_sorting<sorting::insertion_look_ahead>>(problem);
        benchmark<concyclic::inscribed_angle_sorting<sorting::insertion_after_bubbling>>(problem);
        benchmark<concyclic::inscribed_angle_selection>(problem);
    }
}