#ifndef MINIMUM_SPANNING_FOREST_KRUSKAL_HPP
#define MINIMUM_SPANNING_FOREST_KRUSKAL_HPP

#include "basic.hpp"
#include "../../data_structure/disjoint_set/forest.hpp"
#include "../../utility/sorting/index.hpp"

#include <concepts>
#include <format>
#include <utility>
#if MEASUREMENT
#include <iostream>
#endif

namespace minimum_spanning_forest
{
    template <template <typename> class PR, class DisjointSet = disjoint_set::ranked_forest_soa<>>
    class kruskal_quick
    {
    public:
        static constexpr std::string name() { return "kruskal_quick"; }
        static constexpr std::string full_name()
        {
            return std::format("kruskal_quick<{}>", DisjointSet::full_name());
        }
#if MEASUREMENT
        static inline struct operation_counter : public common::operation_counter
        {
            void clear()
            {
                common::operation_counter::clear();
                DisjointSet::measurements.clear();
            }
            virtual void print()
            {
                common::operation_counter::print({"edge_processed"});
                std::cout << "[disjoint_set] " << std::endl;
                DisjointSet::measurements.print();
            }
        } measurements;
#endif

        template <std::totally_ordered T>
        static edge<T> *find(edge<T> *s, edge<T> *t, DisjointSet &ds, edge<T> *tail)
        {
            constexpr PR<edge<T>> cmp;
            if (t - s <= 1)
            {
#if MEASUREMENT
                ++measurements["edge_processed"];
#endif
                if (t != s && ds.unite(s->src, s->dst))
                    *(tail++) = *s;
                return tail;
            }
            edge<T> *p = t - s > 2 ? sorting::quick::partition(s, t, cmp) : t - 1;
            if (cmp(*p, *s))
                std::swap(*p, *s);
            tail = find(s, p, ds, tail);
            if (ds.set_count() > 1)
                tail = find(p, t, ds, tail);
            return tail;
        }

        template <std::totally_ordered T>
        static int find(edge<T> edges[], int n, const int m)
        {
            DisjointSet ds(n);
            edge<T> *tail = find(edges, edges + m, ds, edges);
            return static_cast<int>(tail - edges);
        }
    };

    template <template <typename> class PR, class DisjointSet = disjoint_set::ranked_forest_soa<>, class Heap = sorting::heap<>>
    class kruskal_heap
    {
    public:
        static constexpr std::string name() { return "kruskal_heap"; }
        static constexpr std::string full_name()
        {
            return std::format("kruskal_heap<{}, {}>", DisjointSet::full_name(), Heap::full_name());
        }
#if MEASUREMENT
        static inline struct operation_counter : public common::operation_counter
        {
            void clear()
            {
                common::operation_counter::clear();
                DisjointSet::measurements.clear();
                Heap::measurements.clear();
            }
            virtual void print()
            {
                common::operation_counter::print({"edge_processed"});
                std::cout << "[disjoint_set] " << std::endl;
                DisjointSet::measurements.print();
                std::cout << "[heap] " << std::endl;
                Heap::measurements.print();
            }
        } measurements;
#endif

        template <std::totally_ordered T>
        static int find(edge<T> edges[], int n, const int m)
        {
            constexpr PR<edge<T>> cmp;
            DisjointSet ds(n);
            edge<T> *tail = edges, *summit = edges + m - 1;
            Heap::heapify(summit, -m, cmp);
            for (int j = 0; j < m && ds.set_count() > 1; j++)
            {
#if MEASUREMENT
                ++measurements["edge_processed"];
#endif
                edge<T> edge = *summit;
                Heap::sift_down(summit, j + 1 - m, edges[j], cmp);
                if (ds.unite(edge.src, edge.dst))
                    *(tail++) = edge;
            }
            return static_cast<int>(tail - edges);
        }
    };

    template <template <typename> class PR, class DisjointSet = disjoint_set::ranked_forest_soa<>, class Sorting = sorting::intro<>>
    class kruskal_sort
    {
    public:
        static constexpr std::string name() { return "kruskal_sort"; }
        static constexpr std::string full_name()
        {
            return std::format("kruskal_sort<{}, {}>", DisjointSet::full_name(), Sorting::full_name());
        }
#if MEASUREMENT
        static inline struct operation_counter : public common::operation_counter
        {
            void clear()
            {
                common::operation_counter::clear();
                DisjointSet::measurements.clear();
                Sorting::measurements.clear();
            }
            virtual void print()
            {
                std::cout << "[disjoint_set] " << std::endl;
                DisjointSet::measurements.print();
                std::cout << "[sorting] " << std::endl;
                Sorting::measurements.print();
            }
        } measurements;
#endif

        template <std::totally_ordered T>
        static int find(edge<T> edges[], int n, const int m)
        {
            constexpr PR<edge<T>> cmp;
            DisjointSet ds(n);
            edge<T> *tail = edges;
            Sorting::sort(edges, edges + m, cmp);
            for (edge<T> *e = edges; e < edges + m && ds.set_count() > 1; e++)
                if (ds.unite(e->src, e->dst))
                    *(tail++) = *e;
            return static_cast<int>(tail - edges);
        }
    };
}

#endif
