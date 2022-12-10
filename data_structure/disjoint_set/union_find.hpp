#ifndef DISJOINT_SET_UNION_FIND_HPP
#define DISJOINT_SET_UNION_FIND_HPP

#include <string>
#include <format>
#if MEASUREMENT
#include <iostream>
#endif

namespace disjoint_set
{
    enum struct find_strategy
    {
        nothing_special,
        path_compression,
        path_halving,
        path_halving_eager,
    };

    constexpr std::string to_string(find_strategy find_strategy)
    {
        switch (find_strategy)
        {
        case find_strategy::nothing_special:
            return "nothing_special";
        case find_strategy::path_compression:
            return "path_compression";
        case find_strategy::path_halving:
            return "path_halving";
        case find_strategy::path_halving_eager:
            return "path_halving_eager";
        default:
            return "";
        }
    }

    template <find_strategy FindStrategy = find_strategy::path_halving>
    class union_find_node
    {
        union_find_node *parent;
        int rank;

    public:
#if MEASUREMENT
        static inline struct measurements
        {
            long long constructor_called = 0;
            long long find_called = 0;
            long long path_lifting = 0;
            long long path_compaction = 0;
            long long unite_called = 0;
            long long rank_incremented = 0;
            long long union_carried_out = 0;

            void print(const char *prefix = "") const
            {
                std::cout << prefix << "constructor_called = " << constructor_called << std::endl;
                std::cout << prefix << "find_called = " << find_called << std::endl;
                std::cout << prefix << " |- path_lifting = " << path_lifting << std::endl;
                std::cout << prefix << " |- path_compaction = " << path_compaction << std::endl;
                std::cout << prefix << "unite_called = " << unite_called << std::endl;
                std::cout << prefix << " |- rank_incremented = " << rank_incremented << std::endl;
                std::cout << prefix << " |- union_carried_out = " << union_carried_out << std::endl;
            }

            void println(const char *prefix = "") const
            {
                print(prefix);
                std::cout << std::endl;
            }
        } measurements;
#endif
        union_find_node() : rank(0)
        {
#if MEASUREMENT
            ++measurements.constructor_called;
#endif
            parent = this;
        }

        union_find_node *find_with_nothing_special()
        {
            union_find_node *q = this;
            while (q != q->parent)
            {
#if MEASUREMENT
                ++measurements.path_lifting;
#endif
                q = q->parent;
            }
            return q;
        }

        union_find_node *find_with_path_compression()
        {
            union_find_node *q = this, *r = q;
            while (r != r->parent)
            {
#if MEASUREMENT
                ++measurements.path_lifting;
#endif
                r = r->parent;
            }
#if MEASUREMENT
            ++measurements.path_lifting;
#endif
            for (union_find_node *p = q->parent; p != r; p = q->parent)
            {
#if MEASUREMENT
                ++measurements.path_lifting;
                ++measurements.path_compaction;
#endif
                q->parent = r;
                q = p;
            }
            return r;
        }

        union_find_node *find_with_path_halving()
        {
            union_find_node *q = this;
#if MEASUREMENT
            ++measurements.path_lifting;
#endif
            for (union_find_node *p = parent; p != q; p = q->parent)
            {
#if MEASUREMENT
                ++measurements.path_lifting;
                ++measurements.path_lifting;
                ++measurements.path_compaction;
#endif
                q = q->parent = p->parent;
            }
            return q;
        }

        union_find_node *find_with_path_halving_eager()
        {
            union_find_node *q = this;
#if MEASUREMENT
            ++measurements.path_lifting;
#endif
            for (union_find_node *p = parent; p != q; p = q->parent)
            {
#if MEASUREMENT
                ++measurements.path_lifting;
#endif
                union_find_node *r = p->parent;
                if (r == p)
                    return p;
#if MEASUREMENT
                ++measurements.path_lifting;
                ++measurements.path_compaction;
#endif
                q = q->parent = r;
            }
            return q;
        }

        union_find_node *find()
        {
#if MEASUREMENT
            ++measurements.find_called;
#endif
            if constexpr (FindStrategy == find_strategy::nothing_special)
                return find_with_nothing_special();
            else if (FindStrategy == find_strategy::path_compression)
                return find_with_path_compression();
            else if (FindStrategy == find_strategy::path_halving_eager)
                return find_with_path_halving_eager();
            else
                return find_with_path_halving();
        }

        static bool unite(union_find_node *p, union_find_node *q)
        {
#if MEASUREMENT
            ++measurements.unite_called;
#endif
            p = p->find(), q = q->find();
            if (p == q)
                return false;
            if (p->rank < q->rank)
                p->parent = q;
            else
            {
                q->parent = p;
                if (p->rank == q->rank)
                {
#if MEASUREMENT
                    ++measurements.rank_incremented;
#endif
                    ++p->rank;
                }
            }
#if MEASUREMENT
            ++measurements.union_carried_out;
#endif
            return true;
        }
    };

    template <find_strategy FindStrategy = find_strategy::path_halving>
    class union_find_forest_aos
    {
        union_find_node<FindStrategy> *nodes;
        int n_sets;
        const int n_elements;

        union_find_forest_aos(const union_find_forest_aos &) = delete;
        union_find_forest_aos &operator=(const union_find_forest_aos &) = delete;

    public:
        static constexpr std::string name()
        {
            return std::format("union_find_forest_aos<{}>", to_string(FindStrategy));
        }
#if MEASUREMENT
        static inline struct measurements
        {
            measurements()
            {
                union_find_node<FindStrategy>::measurements = union_find_node<FindStrategy>::measurements::measurements();
            }

            void print(const char *prefix = "") const
            {
                union_find_node<FindStrategy>::measurements.print(prefix);
            }

            void println(const char *prefix = "") const
            {
                union_find_node<FindStrategy>::measurements.println(prefix);
            }
        } measurements;
#endif
        explicit union_find_forest_aos(int n) : nodes(new union_find_node<FindStrategy>[n]), n_sets(n), n_elements(n)
        {
        }

        int set_count() const { return n_sets; }
        int element_count() const { return n_elements; }

        void clear()
        {
            for (int i = 0; i < n_elements; i++)
                nodes[i].union_find_node<FindStrategy>::union_find_node();
            n_sets = n_elements;
        }

        int find(int q) const
        {
            return nodes[q].find() - nodes;
        }

        bool unite(int p, int q)
        {
            bool united = union_find_node<FindStrategy>::unite(nodes + p, nodes + q);
            n_sets -= united;
            return united;
        }

        ~union_find_forest_aos()
        {
            delete[] nodes;
        }
    };

    template <find_strategy FindStrategy = find_strategy::path_halving>
    class union_find_forest
    {
        int *parent_of;
        int *rank_of;
        int n_sets;
        const int n_elements;

        union_find_forest(const union_find_forest &) = delete;
        union_find_forest &operator=(const union_find_forest &) = delete;

    public:
        static constexpr std::string name()
        {
            return std::format("union_find_forest<{}>", to_string(FindStrategy));
        }
#if MEASUREMENT
        static inline struct measurements
        {
            long long constructor_called = 0;
            long long find_called = 0;
            long long path_lifting = 0;
            long long path_compaction = 0;
            long long unite_called = 0;
            long long rank_incremented = 0;
            long long union_carried_out = 0;

            void print(const char *prefix = "") const
            {
                std::cout << prefix << "constructor_called = " << constructor_called << std::endl;
                std::cout << prefix << "find_called = " << find_called << std::endl;
                std::cout << prefix << " |- path_lifting = " << path_lifting << std::endl;
                std::cout << prefix << " |- path_compaction = " << path_compaction << std::endl;
                std::cout << prefix << "unite_called = " << unite_called << std::endl;
                std::cout << prefix << " |- rank_incremented = " << rank_incremented << std::endl;
                std::cout << prefix << " |- union_carried_out = " << union_carried_out << std::endl;
            }

            void println(const char *prefix = "") const
            {
                print(prefix);
                std::cout << std::endl;
            }
        } measurements;
#endif
        explicit union_find_forest(int n) : parent_of(new int[n]), rank_of(new int[n]{}), n_sets(n), n_elements(n)
        {
#if MEASUREMENT
            ++measurements.constructor_called;
#endif
            for (int i = 0; i < n; i++)
                parent_of[i] = i;
        }

        int set_count() const { return n_sets; }
        int element_count() const { return n_elements; }

        void clear()
        {
            for (int i = 0; i < n_elements; i++)
            {
                parent_of[i] = i;
                rank_of[i] = 0;
            }
            n_sets = n_elements;
        }

        int find_with_nothing_special(int q)
        {
            while (q != parent_of[q])
            {
#if MEASUREMENT
                ++measurements.path_lifting;
#endif
                q = parent_of[q];
            }
            return q;
        }

        int find_with_path_compression(int q)
        {
            int r = q;
            while (r != parent_of[r])
            {
#if MEASUREMENT
                ++measurements.path_lifting;
#endif
                r = parent_of[r];
            }
#if MEASUREMENT
            ++measurements.path_lifting;
#endif
            for (int p = parent_of[q]; p != r; p = parent_of[q])
            {
#if MEASUREMENT
                ++measurements.path_lifting;
                ++measurements.path_compaction;
#endif
                parent_of[q] = r;
                q = p;
            }
            return r;
        }

        int find_with_path_halving(int q)
        {
#if MEASUREMENT
            ++measurements.path_lifting;
#endif
            for (int p = parent_of[q]; p != q; p = parent_of[q])
            {
#if MEASUREMENT
                ++measurements.path_lifting;
                ++measurements.path_lifting;
                ++measurements.path_compaction;
#endif
                q = parent_of[q] = parent_of[p];
            }
            return q;
        }

        int find_with_path_halving_eager(int q)
        {
#if MEASUREMENT
            ++measurements.path_lifting;
#endif
            for (int p = parent_of[q]; p != q; p = parent_of[q])
            {
#if MEASUREMENT
                ++measurements.path_lifting;
#endif
                int r = parent_of[p];
                if (r == p)
                    return p;
#if MEASUREMENT
                ++measurements.path_lifting;
                ++measurements.path_compaction;
#endif
                q = parent_of[q] = r;
            }
            return q;
        }

        int find(int q)
        {
#if MEASUREMENT
            ++measurements.find_called;
#endif
            if constexpr (FindStrategy == find_strategy::nothing_special)
                return find_with_nothing_special(q);
            else if (FindStrategy == find_strategy::path_compression)
                return find_with_path_compression(q);
            else if (FindStrategy == find_strategy::path_halving_eager)
                return find_with_path_halving_eager(q);
            else
                return find_with_path_halving(q);
        }

        bool unite(int p, int q)
        {
#if MEASUREMENT
            ++measurements.unite_called;
#endif
            p = find(p), q = find(q);
            if (p == q)
                return false;
            if (rank_of[p] < rank_of[q])
                parent_of[p] = q;
            else
            {
                parent_of[q] = p;
                if (rank_of[p] == rank_of[q])
                {
#if MEASUREMENT
                    ++measurements.rank_incremented;
#endif
                    ++rank_of[p];
                }
            }
#if MEASUREMENT
            ++measurements.union_carried_out;
#endif
            --n_sets;
            return true;
        }

        ~union_find_forest()
        {
            delete[] parent_of;
            delete[] rank_of;
        }
    };
}

#endif
