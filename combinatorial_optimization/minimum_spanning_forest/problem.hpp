#ifndef MINIMUM_SPANNING_FOREST_PROBLEM_HPP
#define MINIMUM_SPANNING_FOREST_PROBLEM_HPP

#include "basic.hpp"
#include "../../utility/random/random.hpp"

#include <concepts>
#include <iostream>
#include <string>
#include <format>

namespace minimum_spanning_forest
{
    template <std::totally_ordered T, int NVerticesLimit, int NEdgesLimit, T AbsoluteCostLimit>
    struct edge_list_form;
    template <std::totally_ordered T, int NVerticesLimit, int NEdgesLimit, T AbsoluteCostLimit>
    struct adjacency_list_form;

    template <std::totally_ordered T, int NVerticesLimit, int NEdgesLimit, T AbsoluteCostLimit>
    struct edge_list_form
    {
        edge<T> edges[NEdgesLimit];
        int n_vertices;
        int n_edges;

        edge_list_form() = default;
        edge_list_form &operator=(const edge_list_form &source)
        {
            for (int j = 0; j < source.n_edges; j++)
                edges[j] = source.edges[j];
            n_vertices = source.n_vertices;
            n_edges = source.n_edges;
            return *this;
        }

        static void validate_input_size(int n_vertices, int n_edges)
        {
            if (n_vertices <= 0 || NVerticesLimit < n_vertices)
                throw std::out_of_range("the number of vertices must be in (0," + std::to_string(NVerticesLimit) + "]");
            if (n_edges <= 0 || NEdgesLimit < n_edges)
                throw std::out_of_range("the number of edges must be in (0," + std::to_string(NEdgesLimit) + "]");
        }

        void randomly_generate(int n_vertices, int n_edges, T max_absolute_cost)
        {
            static long long ran[NEdgesLimit];
            validate_input_size(n_vertices, n_edges);
            if (max_absolute_cost <= 0 || AbsoluteCostLimit < max_absolute_cost)
                throw std::out_of_range("the max absolute cost must be in (0," + std::to_string(AbsoluteCostLimit) + "]");
            this->n_vertices = n_vertices, this->n_edges = n_edges;
            random::merge::sampling((long long)n_vertices * (long long)(n_vertices - 1) / 2, (long long)n_edges, ran);
            std::mt19937_64 mt(std::random_device{}());
            std::uniform_real_distribution<> urd(-max_absolute_cost, max_absolute_cost);
            int k = 0;
            for (long long i = 0; i < (long long)n_vertices; i++)
                while (k < n_edges && ran[k] < i * (i + 1) / 2)
                    edges[k++] = edge{
                        .src = (int)i,
                        .dst = (int)(ran[k] - i * (i - 1) / 2),
                        .cost = urd(mt),
                    };
        }

        void read_from_stdin()
        {
            std::ios_base::sync_with_stdio(false);
            std::cin.tie(nullptr);
            std::cin.exceptions(std::ios::eofbit | std::ios::failbit | std::ios::badbit);
            std::cin >> n_vertices >> n_edges;
            validate_input_size(n_vertices, n_edges);
            for (int j = 0; j < n_edges; j++)
                std::cin >> edges[j].src >> edges[j].dst >> edges[j].cost;
            std::cin.exceptions(std::ios::goodbit);
            std::cin.tie(&std::cout);
            std::ios_base::sync_with_stdio(true);
        }

        void convert_from_adjacency_list(const adjacency_list_form<T, NVerticesLimit, NEdgesLimit, AbsoluteCostLimit> &adj)
        {
            auto &[inc, dst, cost, n] = adj;
            int k = 0;
            for (int i = 0; i < n; i++)
                for (int j = inc[i]; j < inc[i + 1]; j++)
                    if (dst[j] < i)
                        edges[k++] = edge<T>{.src = i, .dst = dst[j], .cost = cost[j]};
            n_vertices = n;
            n_edges = adj.edge_count();
        }

        void write_to_stdout()
        {
            printf("%d %d\n", n_vertices, n_edges);
            for (int j = 0; j < n_edges; j++)
                printf("%d %d %f\n", edges[j].src, edges[j].dst, edges[j].cost);
        }

        void write_line_to_stdout()
        {
            write_to_stdout();
            printf("\n");
        }
    };

    template <std::totally_ordered T, int NVerticesLimit, int NEdgesLimit, T AbsoluteCostLimit>
    struct adjacency_list_form
    {
        int inc[NVerticesLimit + 1];
        int dst[NEdgesLimit << 1];
        T cost[NEdgesLimit << 1];
        int n_vertices;

        adjacency_list_form() = default;
        adjacency_list_form &operator=(const adjacency_list_form &source)
        {
            for (int i = 0; i < source.n_vertices; i++)
            {
                inc[i] = source.inc[i];
            }
            for (int j = 0; j < source.inc[source.n_vertices]; j++)
            {
                dst[j] = source.dst[j];
                cost[j] = source.cost[j];
            }
            n_vertices = source.n_vertices;
            return *this;
        }

        void convert_from_edge_list(const edge_list_form<T, NVerticesLimit, NEdgesLimit, AbsoluteCostLimit> &edge_list)
        {
            auto &[edges, n, m] = edge_list;
            for (int i = 0; i <= n; i++)
                inc[i] = 0;
            for (int j = 0; j < m; j++)
                ++inc[edges[j].src], ++inc[edges[j].dst];
            for (int i = 0; i < n; i++)
                inc[i + 1] += inc[i];
            for (int j = m - 1; j >= 0; j--)
            {
                auto [u, v, c] = edges[j];
                int e = --inc[u], f = --inc[v];
                dst[e] = v, dst[f] = u;
                cost[e] = cost[f] = c;
            }
            n_vertices = n;
        }

        int edge_count() const { return inc[n_vertices] >> 1; }
    };
}

#endif
