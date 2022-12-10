#ifndef DISJOINT_SET_PROBLEM_HPP
#define DISJOINT_SET_PROBLEM_HPP

#include "../../utility/random/random.hpp"

#include <iostream>
#include <string>
#include <format>

namespace disjoint_set
{
    struct query
    {
        enum struct command
        {
            unite = 0,
            same = 1,
        } command;
        int x;
        int y;

        static query randomly_generate(int n_elements)
        {
            return query{
                .command = static_cast<enum struct query::command>(random::rand(2)),
                .x = random::rand(n_elements),
                .y = random::rand(n_elements),
            };
        }

        void read_from_stdin(int n_elements)
        {
            auto &[command, x, y] = *this;
            std::cin >> (int &)command >> x >> y;
            if (command != command::unite && command != command::same)
                throw std::out_of_range("command must be one of 0 (unite) or 1 (same)");
            if (x < 0 || x >= n_elements || y < 0 || y >= n_elements)
                throw std::out_of_range(std::format("element index must be in the range of [0, {})", n_elements));
        }

        void write_to_stdout()
        {
            printf("%d %d %d", static_cast<int>(command), x, y);
        }

        void write_line_to_stdout()
        {
            write_to_stdout();
            printf("\n");
        }
    };

    template <int NElementsLimit, int NQueriesLimit>
    struct problem
    {
        query queries[NQueriesLimit];
        int n_elements;
        int n_queries;

        problem() = default;
        problem &operator=(const problem &source)
        {
            for (int q = 0; q < source.n_queries; q++)
                queries[q] = source.queries[q];
            n_elements = source.n_elements;
            n_queries = source.n_queries;
            return *this;
        }

        static void validate_input_size(int n_elements, int n_queries)
        {
            if (n_elements <= 0 || NElementsLimit < n_elements)
                throw std::out_of_range("the number of elements must be in (0," + std::to_string(NElementsLimit) + "]");
            if (n_queries <= 0 || NQueriesLimit < n_queries)
                throw std::out_of_range("the number of queries must be in (0," + std::to_string(NQueriesLimit) + "]");
        }

        void randomly_generate(int n_elements, int n_queries)
        {
            validate_input_size(n_elements, n_queries);
            this->n_elements = n_elements, this->n_queries = n_queries;
            for (int i = 0; i < n_queries; i++)
                queries[i] = query::randomly_generate(n_elements);
        }

        void read_from_stdin()
        {
            std::ios_base::sync_with_stdio(false);
            std::cin.tie(nullptr);
            std::cin.exceptions(std::ios::eofbit | std::ios::failbit | std::ios::badbit);
            std::cin >> n_elements >> n_queries;
            validate_input_size(n_elements, n_queries);
            for (int i = 0; i < n_queries; i++)
                queries[i].read_from_stdin(n_elements);
            std::cin.exceptions(std::ios::goodbit);
            std::cin.tie(&std::cout);
            std::ios_base::sync_with_stdio(true);
        }

        void write_to_stdout()
        {
            printf("%d %d\n", n_elements, n_queries);
            for (int i = 0; i < n_queries; i++)
                queries[i].write_line_to_stdout();
        }

        void write_line_to_stdout()
        {
            write_to_stdout();
            printf("\n");
        }
    };
}

#endif
