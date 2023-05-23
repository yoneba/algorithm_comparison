#ifndef MINIMUM_SPANNING_FOREST_PRIM_HPP
#define MINIMUM_SPANNING_FOREST_PRIM_HPP

#if MEASUREMENT
#include "../../utility/common/operation_counting.hpp"
#endif

#include "../../data_structure/priority_queue/d_ary_heap.hpp"
#include "../../data_structure/priority_queue/pointer_based_heap.hpp"

#include <concepts>
#include <format>
#include <algorithm>

namespace minimum_spanning_forest
{
    template <template <typename> class PR, template <template <typename> class, typename> class PriorityQueue = priority_queue::pairing_heap>
    class prim
    {
    public:
        static constexpr std::string name() { return "prim"; }
        static constexpr std::string full_name()
        {
            return std::format("prim<{}>", PriorityQueue<PR, double>::full_name());
        }
#if MEASUREMENT
        static inline struct operation_counter : public common::operation_counter
        {
            template <std::totally_ordered T>
            void clear()
            {
                common::operation_counter::clear();
                PriorityQueue<PR, T>::measurements.clear();
            }
            template <std::totally_ordered T>
            void print()
            {
                PriorityQueue<PR, T>::measurements.print();
            }
            template <std::totally_ordered T>
            void println()
            {
                print<T>();
                std::cout << std::endl;
            }
        } measurements;
#endif

        template <std::totally_ordered T>
        static int find(const int inc[], const int dest[], const T cost[], const int n, int pred[])
        {
            constexpr PR<T> cmp;
            int n_added_edges = n;
            PriorityQueue<PR, T> pq(n, std::max((inc[n] >> 1) / n, 3));
            for (int v = 0; v < n; v++)
                pred[v] = -1;
            for (int s = 0; s < n; s++)
            {
                if (pred[s] != -1)
                    continue;
                --n_added_edges;
                pred[s] = inc[n];
                for (int u = s; u != -1; u = pq.top())
                {
                    if (u != s)
                        pq.remove_top();
                    for (int e = inc[u]; e < inc[u + 1]; e++)
                    {
                        const int v = dest[e];
                        if (pred[v] == -1)
                            pred[v] = e, pq.insert(v, cost[e]);
                        else if (pq.contains(v) && cmp(cost[e], pq[v]))
                            pred[v] = e, pq.improve_key(v, cost[e]);
                    }
                }
            }
            return n_added_edges;
        }
    };
}

#endif
