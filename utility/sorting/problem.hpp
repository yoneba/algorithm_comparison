#ifndef SORTING_PROBLEM_HPP
#define SORTING_PROBLEM_HPP

#include "../../utility/random/random.hpp"

#include <iostream>
#include <string>
#include <format>

namespace sorting
{
    template <typename T, int NItemsLimit>
    struct problem
    {
        T items[NItemsLimit];
        int n_items;

        problem() = default;
        problem(const problem &) = delete;
        problem &operator=(const problem &) = delete;

        static void validate_input_size(int n_items)
        {
            if (n_items > NItemsLimit)
                throw std::out_of_range("the number of items must not exceed " + std::to_string(NItemsLimit));
        }

        void randomly_generate(int n_items)
        {
            validate_input_size(n_items);
            this->n_items = n_items;
            for (int i = 0; i < n_items; i++)
                items[i] = static_cast<T>(random::rand((long long)(n_items) * (long long)(n_items)));
        }

        void read_from_stdin()
        {
            std::ios_base::sync_with_stdio(false);
            std::cin.tie(nullptr);
            std::cin.exceptions(std::ios::eofbit | std::ios::failbit | std::ios::badbit);
            std::cin >> n_items;
            validate_input_size(n_items);
            for (int i = 0; i < n_items; i++)
                std::cin >> items[i];
            std::cin.exceptions(std::ios::goodbit);
            std::cin.tie(&std::cout);
            std::ios_base::sync_with_stdio(true);
        }

        void write_to_stdout()
        {
            std::ios_base::sync_with_stdio(false);
            std::cout << n_items << std::endl;
            for (int i = 0; i < n_items; i++)
                std::cout << items[i] << ' ';
            std::cout << std::endl;
            std::ios_base::sync_with_stdio(true);
        }

        void write_line_to_stdout()
        {
            write_to_stdout();
            std::cout << std::endl;
        }
    };
}

#endif
