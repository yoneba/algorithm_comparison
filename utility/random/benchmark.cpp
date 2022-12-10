#define MEASUREMENT false

#include "random.hpp"

#include <iostream>
#include <string>
#include <chrono>

template <class Random>
void benchmark(long long range, long long howmany, long long result[])
{
    using namespace std::chrono;
    std::cerr << Random::name() << ' ' << std::flush;
#if MEASUREMENT
#endif
    time_point<high_resolution_clock> tick = high_resolution_clock::now();
    Random::sampling(range, howmany, result);
    std::cerr << duration_cast<duration<double>>(high_resolution_clock::now() - tick).count() << " sec" << std::endl;
    for (int i = 0; i < 10 && i < howmany; i++)
        std::cout << result[i] << ' ';
    std::cout << std::endl;
#if MEASUREMENT
#endif
}

void validate_input_size(long long range, long long howmany, long long howmany_limit)
{
    if (howmany <= 0 || howmany_limit < howmany)
        throw std::out_of_range("the number of elements to obtain must be in (0," + std::to_string(howmany_limit) + "]");
    if (range < howmany)
        throw std::out_of_range("range must not be less than the number of elements to obtain");
}

int main(int argc, char *argv[])
{
    constexpr long long howmany_limit = 100000000;
    static long long result[howmany_limit];
    long long range = 0, howmany = 0;
    try
    {
        if (argc != 3)
            throw std::invalid_argument("usage: ./benchmark.out {range} {howmany}");
        else
            range = std::stoll(argv[1]), howmany = std::stoll(argv[2]);
        validate_input_size(range, howmany, howmany_limit);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    benchmark<random::rough>(range, howmany, result);
    benchmark<random::shuffle>(range, howmany, result);
    benchmark<random::merge>(range, howmany, result);
}
