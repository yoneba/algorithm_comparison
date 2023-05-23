#define MEASUREMENT true

#include "problem.hpp"
#include "prim.hpp"
#include "kruskal.hpp"
#include "boruvka.hpp"

#include <iostream>
#include <string>
#include <chrono>
#include <functional>

template <class MinimumSpanningTree>
void benchmark_edge_list(const auto &adj_list, auto &edge_list)
{
    using namespace std::chrono;
    edge_list.convert_from_adjacency_list(adj_list);
    auto &[edges, n_vertices, n_edges] = edge_list;
    using item_type = typename std::remove_reference<decltype(edges[0].cost)>::type;
#if MEASUREMENT
    MinimumSpanningTree::measurements.clear();
#endif
    std::cerr << MinimumSpanningTree::full_name() << ' ' << std::flush;
    time_point<high_resolution_clock> tick = high_resolution_clock::now();
    int n_added_edges = MinimumSpanningTree::find(edges, n_vertices, n_edges);
    std::cerr << duration_cast<duration<double>>(high_resolution_clock::now() - tick).count() << " sec" << std::endl;
    item_type weight = 0;
    for (int e = 0; e < n_added_edges; e++)
        weight += edges[e].cost;
    std::cout << "components: " << n_vertices - n_added_edges << ", weight: " << weight << std::endl;
#if MEASUREMENT
    MinimumSpanningTree::measurements.println();
#endif
}

template <class MinimumSpanningTree>
void benchmark_adjacency_list(const auto &adj_list)
{
    using namespace std::chrono;
    auto &[inc, dst, cost, n_vertices] = adj_list;
    using item_type = typename std::remove_const<typename std::remove_reference<decltype(cost[0])>::type>::type;
    int *pred = new int[n_vertices];
#if MEASUREMENT
    MinimumSpanningTree::measurements.clear<item_type>();
#endif
    std::cerr << MinimumSpanningTree::full_name() << ' ' << std::flush;
    time_point<high_resolution_clock> tick = high_resolution_clock::now();
    int n_added_edges = MinimumSpanningTree::find(inc, dst, cost, n_vertices, pred);
    std::cerr << duration_cast<duration<double>>(high_resolution_clock::now() - tick).count() << " sec" << std::endl;
    item_type weight = 0;
    for (int v = 0; v < n_vertices; v++)
        if (pred[v] < inc[n_vertices])
            weight += cost[pred[v]];
    std::cout << "components: " << n_vertices - n_added_edges << ", weight: " << weight << std::endl;
#if MEASUREMENT
    MinimumSpanningTree::measurements.println<item_type>();
#endif
    delete[] pred;
}

int main(int argc, char *argv[])
{
    using namespace minimum_spanning_forest;
    using namespace priority_queue;
    using namespace disjoint_set;
    constexpr int n_vertices_limit = 10000000, n_edges_limit = 20000000;
    constexpr double absolute_cost_limit = 100000000;
    static edge_list_form<double, n_vertices_limit, n_edges_limit, absolute_cost_limit> edge_list;
    try
    {
        if (argc == 2 && argv[1] == std::string("-"))
            edge_list.read_from_stdin();
        else if (argc != 4)
            throw std::invalid_argument("usage: ./benchmark.out {#vertices} {#edges} {#max absolute cost}");
        else
            edge_list.randomly_generate(std::stoi(argv[1]), std::stoi(argv[2]), std::stod(argv[3]));
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    static adjacency_list_form<double, n_vertices_limit, n_edges_limit, absolute_cost_limit> adj_list;
    adj_list.convert_from_edge_list(edge_list);
    benchmark_adjacency_list<prim<std::less, pairing_heap>>(adj_list);
    benchmark_adjacency_list<prim<std::less, rank_pairing_heap>>(adj_list);
    benchmark_adjacency_list<prim<std::less, fibonacci_heap>>(adj_list);
    benchmark_adjacency_list<prim<std::less, binary_heap>>(adj_list);
    benchmark_adjacency_list<prim<std::less, ternary_heap>>(adj_list);
    benchmark_adjacency_list<prim<std::less, quaternary_heap>>(adj_list);
    benchmark_adjacency_list<prim<std::less, multiary_heap>>(adj_list);
    benchmark_edge_list<kruskal_quick<std::less, forest_aos<>>>(adj_list, edge_list);
    benchmark_edge_list<kruskal_quick<std::less, ranked_forest_soa<>>>(adj_list, edge_list);
    benchmark_edge_list<kruskal_heap<std::less, forest_aos<>, sorting::heap<>>>(adj_list, edge_list);
    benchmark_edge_list<kruskal_heap<std::less, ranked_forest_soa<>, sorting::heap<>>>(adj_list, edge_list);
    benchmark_edge_list<kruskal_sort<std::less, forest_aos<>, sorting::intro<>>>(adj_list, edge_list);
    benchmark_edge_list<kruskal_sort<std::less, ranked_forest_soa<>, sorting::intro<>>>(adj_list, edge_list);
    benchmark_edge_list<boruvka<std::less>>(adj_list, edge_list);
}