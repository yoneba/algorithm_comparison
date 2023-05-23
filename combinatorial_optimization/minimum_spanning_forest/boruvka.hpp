#ifndef MINIMUM_SPANNING_FOREST_BORUVKA_HPP
#define MINIMUM_SPANNING_FOREST_BORUVKA_HPP

#if MEASUREMENT
#include "../../utility/common/operation_counting.hpp"
#endif

#include "basic.hpp"
#include "../../data_structure/disjoint_set/forest.hpp"

#include <concepts>
#include <utility>
#include <format>

namespace minimum_spanning_forest
{
    template <template <typename> class PR>
    class boruvka
    {
    public:
        static constexpr std::string name() { return "boruvka"; }
        static constexpr std::string full_name()
        {
            return std::format("boruvka<{}>", disjoint_set::in_tree<>::full_name());
        }
#if MEASUREMENT
        static inline struct operation_counter : public common::operation_counter
        {
            void clear()
            {
                common::operation_counter::clear();
                disjoint_set::in_tree<>::measurements.clear();
            }
            virtual void print()
            {
                common::operation_counter::print({"boruvka_step", "edge_processed"});
                std::cout << "[disjoint_set] " << std::endl;
                disjoint_set::in_tree<>::measurements.print();
            }
        } measurements;
#endif

        template <std::totally_ordered T>
        static int find(edge<T> edges[], int n, const int m)
        {
            constexpr PR<T> cmp;
            int *parent_of = new int[n];
            disjoint_set::in_tree<>::reset(parent_of, n);
            edge<T> *cheapest = new edge<T>[n];
            for (int v = 0; v < n; v++)
                cheapest[v].nullify();
            edge<T> *tail = edges + m;
            while (true)
            {
#if MEASUREMENT
                ++measurements["boruvka_step"];
#endif
                for (edge<T> *e = edges; e < tail; e++)
                {
                    int u = disjoint_set::path_halving::find(parent_of, e->src);
                    int v = disjoint_set::path_halving::find(parent_of, e->dst);
                    if (u == v)
                    {
                        *(e--) = *(--tail);
                        continue;
                    }
#if MEASUREMENT
                    ++measurements["edge_processed"];
#endif
                    if (cheapest[u].is_null() || e->strictly_compare(cheapest[u], cmp))
                        cheapest[u] = *e;
                    if (cheapest[v].is_null() || e->strictly_compare(cheapest[v], cmp))
                        cheapest[v] = *e;
                }
                if (tail == edges)
                    break;
                for (int u = 0; u < n; u++)
                {
                    const edge<T> &e = cheapest[u];
                    if (!disjoint_set::in_tree<>::is_representative(parent_of, u) || e.is_null())
                        continue;
                    int v = disjoint_set::path_halving::find(parent_of, e.src) ^ disjoint_set::path_halving::find(parent_of, e.dst) ^ u;
                    if (e == cheapest[v] && u < v)
                        cheapest[u].nullify();
                    else
                        disjoint_set::in_tree<>::link(parent_of, v, u);
                }
            }
            for (int v = 0; v < n; v++)
                if (!disjoint_set::in_tree<>::is_representative(parent_of, v))
                    *(tail++) = cheapest[v];
            delete[] parent_of, delete[] cheapest;
            return static_cast<int>(tail - edges);
        }
    };
}

#endif
