#ifndef CONCYCLIC_PROBLEM_HPP
#define CONCYCLIC_PROBLEM_HPP

#include "../../utility/random/random.hpp"
#include "basic.hpp"

#include <iostream>
#include <string>
#include <format>
#include <concepts>

namespace concyclic
{
    template <std::floating_point T, int WidthLimit, int HeightLimit, int NPointsLimit>
    struct problem
    {
        point<T> points[NPointsLimit];
        int n_points;

        problem() = default;
        problem(const problem &) = delete;
        problem &operator=(const problem &) = delete;

        static void validate_input_size(int width, int height, int n_points)
        {
            if (n_points <= 0 || NPointsLimit < n_points)
                throw std::out_of_range("the number of points must be in (0," + std::to_string(NPointsLimit) + "]");
            if (width <= 0 || height <= 0 || width > WidthLimit || height > HeightLimit)
                throw std::out_of_range(std::format("the following required: 0 < width <= {} and 0 < height <= {}", WidthLimit, HeightLimit));
            if (width * height < n_points)
                throw std::out_of_range("the product of width and height must not be less than the number of points");
        }

        void randomly_generate(int width, int height, int n_points)
        {
            static long long ran[NPointsLimit];
            validate_input_size(width, height, n_points);
            this->n_points = n_points;
            random::merge::sampling((long long)width * (long long)height, (long long)n_points, ran);
            for (int i = 0; i < n_points; i++)
                points[i] = point<T>(static_cast<T>(ran[i] % height), static_cast<T>(ran[i] / height));
        }

        void read_from_stdin()
        {
            std::ios_base::sync_with_stdio(false);
            std::cin.tie(nullptr);
            std::cin.exceptions(std::ios::eofbit | std::ios::failbit | std::ios::badbit);
            std::cin >> n_points;
            validate_input_size(WidthLimit, HeightLimit, n_points);
            for (int i = 0; i < n_points; i++)
            {
                std::complex<int> p;
                std::cin >> p;
                if (p.real() < 0 || p.imag() < 0 || p.real() >= WidthLimit || p.imag() >= HeightLimit)
                    throw std::out_of_range(std::format("points must be in the range of [0, {}] * [0, {})", WidthLimit, HeightLimit));
                points[i] = point<T>(static_cast<T>(p.real()), static_cast<T>(p.imag()));
            }
            std::cin.exceptions(std::ios::goodbit);
            std::cin.tie(&std::cout);
            std::ios_base::sync_with_stdio(true);
        }

        void write_to_stdout()
        {
            printf("%d\n", n_points);
            for (int i = 0; i < n_points; i++)
                printf("(%d,%d)\n", static_cast<int>(points[i].real()), static_cast<int>(points[i].imag()));
        }

        void write_line_to_stdout()
        {
            write_to_stdout();
            printf("\n");
        }
    };
}

#endif
