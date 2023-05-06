#ifndef DISJOINT_SET_USECASE_HPP
#define DISJOINT_SET_USECASE_HPP

#if MEASUREMENT
#include "../../utility/common/operation_counting.hpp"
#endif

#include "union.hpp"

#include <format>
#include <new>

namespace disjoint_set
{
    template <template <class> class Union = ranked_in_tree, class Find = path_halving>
    class forest_aos
    {
        using trait = Union<Find>;

    public:
        using node = typename trait::node;

    private:
        node *nodes;
        int n_sets;
        const int n_elements;

        forest_aos(const forest_aos &) = delete;
        forest_aos &operator=(const forest_aos &) = delete;

    public:
        static constexpr std::string name() { return "forest_aos"; }
        static constexpr std::string full_name()
        {
            return std::format("forest_aos<{}, {}>", trait::name(), Find::name());
        }

#if MEASUREMENT
        static inline struct operation_counter : public common::operation_counter
        {
            void clear()
            {
                common::operation_counter::clear();
                trait::measurements.clear();
            }
            virtual void print() { trait::measurements.print(); }
        } measurements;
#endif

        explicit forest_aos(int n) : nodes(new node[n]), n_sets(n), n_elements(n)
        {
        }

        int set_count() const { return n_sets; }
        int element_count() const { return n_elements; }

        bool is_representative(node *q) const { return q->is_representative(); }
        bool is_representative(int q) const { return is_representative(nodes + q); }

        void clear()
        {
            for (int i = 0; i < n_elements; i++)
                new (nodes + i) node();
            n_sets = n_elements;
        }

        node *find(node *q) const
        {
            return q->find();
        }
        int find(int q) const { return find(nodes + q) - nodes; }

        bool unite(node *p, node *q)
        {
            bool united = p->unite(q);
            n_sets -= united;
            return united;
        }
        bool unite(int p, int q) { return unite(nodes + p, nodes + q); }

        ~forest_aos()
        {
            delete[] nodes;
        }
    };

    template <class Find = path_halving>
    class forest_soa
    {
        using trait = in_tree<Find>;

        int *parent_of;
        int n_sets;
        const int n_elements;

        forest_soa(const forest_soa &) = delete;
        forest_soa &operator=(const forest_soa &) = delete;

    public:
        static constexpr std::string name() { return "forest_soa"; }
        static constexpr std::string full_name()
        {
            return std::format("forest_soa<{}>", Find::name());
        }

#if MEASUREMENT
        static inline struct operation_counter : public common::operation_counter
        {
            void clear()
            {
                common::operation_counter::clear();
                trait::measurements.clear();
            }
            virtual void print() { trait::measurements.print(); }
        } measurements;
#endif

        explicit forest_soa(int n) : parent_of(new int[n]), n_sets(n), n_elements(n)
        {
            trait::reset(parent_of, n);
        }

        int set_count() const { return n_sets; }
        int element_count() const { return n_elements; }

        bool is_representative(int q) const { return trait::is_representative(parent_of, q); }

        void clear()
        {
            trait::reset(parent_of, n_elements);
            n_sets = n_elements;
        }

        int find(int q) const
        {
            return Find::find(parent_of, q);
        }

        bool unite(int p, int q)
        {
            bool united = trait::unite(parent_of, p, q);
            n_sets -= united;
            return united;
        }

        ~forest_soa()
        {
            delete[] parent_of;
        }
    };

    template <class Find = path_halving>
    class ranked_forest_soa
    {
        using trait = ranked_in_tree<Find>;

        int *parent_of;
        int *rank_of;
        int n_sets;
        const int n_elements;

        ranked_forest_soa(const ranked_forest_soa &) = delete;
        ranked_forest_soa &operator=(const ranked_forest_soa &) = delete;

    public:
        static constexpr std::string name() { return "ranked_forest_soa"; }
        static constexpr std::string full_name()
        {
            return std::format("ranked_forest_soa<{}>", Find::name());
        }

#if MEASUREMENT
        static inline struct operation_counter : public common::operation_counter
        {
            void clear()
            {
                common::operation_counter::clear();
                trait::measurements.clear();
            }
            virtual void print() { trait::measurements.print(); }
        } measurements;
#endif

        explicit ranked_forest_soa(int n) : parent_of(new int[n]), rank_of(new int[n]), n_sets(n), n_elements(n)
        {
            trait::reset(parent_of, rank_of, n_elements);
        }

        int set_count() const { return n_sets; }
        int element_count() const { return n_elements; }

        bool is_representative(int q) const { return trait::is_representative(parent_of, q); }

        void clear()
        {
            trait::reset(parent_of, rank_of, n_elements);
            n_sets = n_elements;
        }

        int find(int q) const
        {
            return Find::find(parent_of, q);
        }

        bool unite(int p, int q)
        {
            bool united = trait::unite(parent_of, rank_of, p, q);
            n_sets -= united;
            return united;
        }

        ~ranked_forest_soa()
        {
            delete[] parent_of, delete[] rank_of;
        }
    };
}

#endif