#ifndef PRIORITY_QUEUE_ADDRESSABLE_HEAP_HPP
#define PRIORITY_QUEUE_ADDRESSABLE_HEAP_HPP

#if MEASUREMENT
#include "../../utility/common/operation_counting.hpp"
#endif

#include "pairing_heap.hpp"
#include "rank_pairing_heap.hpp"
#include "fibonacci_heap.hpp"

#include <concepts>
#include <format>

namespace priority_queue
{
    template <template <template <typename> class> class Modality, template <typename> class PR, std::totally_ordered T>
    class monolithic_heap
    {
        using node = typename Modality<PR>::template node<T>;
        static constexpr PR<T> cmp;
        node *nodes;
        node *summit;
        int n_active_nodes;
        const int n_nodes;

        monolithic_heap(const monolithic_heap &) = delete;
        monolithic_heap &operator=(const monolithic_heap &) = delete;

    public:
        static constexpr std::string name()
        {
            return "monolithic_heap";
        }
        static constexpr std::string full_name()
        {
            return std::format("{}_heap", Modality<PR>::name());
        }
#if MEASUREMENT
        static inline struct operation_counter : public common::operation_counter
        {
            void clear()
            {
                common::operation_counter::clear();
                Modality<PR>::measurements.clear();
            }
            virtual void print() { Modality<PR>::measurements.print(); }
        } measurements;
#endif
        explicit monolithic_heap(int n, int d = 0)
            : nodes(new node[n]), summit(nullptr), n_active_nodes(0), n_nodes(n)
        {
        }
        const T &operator[](int id) { return nodes[id].key; }

        bool empty() const { return n_active_nodes == 0; }
        int size() const { return n_active_nodes; }
        int capacity() const { return n_nodes; }
        int top() const { return summit != nullptr ? static_cast<int>(summit - nodes) : -1; }
        bool contains(int id) const { return nodes[id].is_active(); }

        void clear()
        {
            for (int i = 0; i < n_nodes; i++)
                nodes[i].deactivate();
            summit = nullptr;
            n_active_nodes = 0;
        }

        void insert(int id, T key)
        {
            node *p = nodes + id;
            summit = p->join(summit, key);
            ++n_active_nodes;
        }

        void improve_key(int id, T key)
        {
            node *p = nodes + id;
            summit = p->improve_key(summit, key);
        }

        void worsen_key(int id, T key)
        {
            node *p = nodes + id;
            node *q = p->worsen_key(summit, key);
            if (p == summit)
                summit = q;
        }

        void remove_top()
        {
            summit = summit->leave_root();
            --n_active_nodes;
        }

        void remove(int id)
        {
            node *p = nodes + id;
            if (p == summit)
                return remove_top();
            p->leave_non_root();
            --n_active_nodes;
        }

        ~monolithic_heap()
        {
            delete[] nodes;
        }
    };

    template <template <template <typename> class> class Modality, template <typename> class PR, std::totally_ordered T>
    class distributed_heap
    {
        using node = typename Modality<PR>::template node<T>;
        static constexpr PR<T> cmp;
        static constexpr int n_buckets = 45;
        node *buckets[n_buckets];
        node *nodes;
        node *summit;
        int n_active_nodes;
        const int n_nodes;

        distributed_heap(const distributed_heap &) = delete;
        distributed_heap &operator=(const distributed_heap &) = delete;

    protected:
        void update_summit()
        {
            summit = Modality<PR>::find_summit(buckets, n_active_nodes, nodes - 1);
        }

    public:
        static constexpr std::string name()
        {
            return "distributed_heap";
        }
        static constexpr std::string full_name()
        {
            return std::format("{}_heap", Modality<PR>::name());
        }
#if MEASUREMENT
        static inline struct operation_counter : public common::operation_counter
        {
            void clear()
            {
                common::operation_counter::clear();
                Modality<PR>::measurements.clear();
            }
            virtual void print() { Modality<PR>::measurements.print(); }
        } measurements;
#endif
        explicit distributed_heap(int n, int d = 0)
            : buckets{}, nodes(new node[n]), summit(nodes - 1), n_active_nodes(0), n_nodes(n)
        {
        }
        const T &operator[](int id) { return nodes[id].key; }

        bool empty() const { return n_active_nodes == 0; }
        int size() const { return n_active_nodes; }
        int capacity() const { return n_nodes; }
        int top() const { return static_cast<int>(summit - nodes); }
        bool contains(int id) const { return nodes[id].is_active(); }

        void clear()
        {
            for (int i = 0; i < n_buckets; i++)
                buckets[i] = nullptr;
            for (int i = 0; i < n_nodes; i++)
                nodes[i].deactivate();
            summit = nodes - 1;
            n_active_nodes = 0;
        }

        void insert(int id, T key)
        {
#if MEASUREMENT
            ++measurements["insert_called"];
#endif
            node *p = nodes + id;
            p->activate(key);
            p->insert(buckets);
            if (n_active_nodes++ == 0 || cmp(key, summit->key))
                summit = p;
        }

        void improve_key(int id, T key)
        {
            node *p = nodes + id;
            p->improve_key(buckets, key);
            if (cmp(key, summit->key))
                summit = p;
        }

        void worsen_key(int id, T key)
        {
            node *p = nodes + id;
            p->worsen_key(buckets, key);
            if (p == summit)
                update_summit();
        }

        void remove_top()
        {
            summit->leave_root(buckets);
            --n_active_nodes;
            update_summit();
        }

        void remove(int id)
        {
            node *p = nodes + id;
            if (p == summit)
                return remove_top();
            p->leave_non_root(buckets);
            --n_active_nodes;
        }

        ~distributed_heap()
        {
            delete[] nodes;
        }
    };

    template <template <template <typename> class> class Modality, template <typename> class PR, std::totally_ordered T>
    class monolithic_heap_group
    {
        using node = typename Modality<PR>::template node<T>;
        static constexpr PR<T> cmp;
        node *nodes;
        node **roots;
        int *root_id_of;
        int n_active_nodes;
        const int n_nodes;
        const int n_trees;

        monolithic_heap_group(const monolithic_heap_group &) = delete;
        monolithic_heap_group &operator=(const monolithic_heap_group &) = delete;

    public:
        static constexpr std::string name()
        {
            return "monolithic_heap_group";
        }
        static constexpr std::string full_name()
        {
            return std::format("{}_heap_group", Modality<PR>::name());
        }
#if MEASUREMENT
        static inline struct operation_counter : public common::operation_counter
        {
            void clear()
            {
                common::operation_counter::clear();
                Modality<PR>::measurements.clear();
            }
            virtual void print() { Modality<PR>::measurements.print(); }
        } measurements;
#endif
        explicit monolithic_heap_group(int n_elements, int n_groups)
            : nodes(new node[n_elements]), roots(new node *[n_groups] {}), root_id_of(new int[n_elements]), n_active_nodes(0), n_nodes(n_elements), n_trees(n_groups)
        {
        }
        const T &operator[](int element_id) { return nodes[element_id].key; }

        bool empty() const { return n_active_nodes == 0; }
        bool empty(int group_id) const { return roots[group_id] == nullptr; }
        int size() const { return n_active_nodes; }
        int capacity() const { return n_nodes; }
        int group_count() const { return n_trees; }
        int top(int group_id) const { return empty(group_id) ? -1 : static_cast<int>(roots[group_id] - nodes); }
        bool is_active(int element_id) const { return nodes[element_id].is_active(); }
        int group_of(int element_id) const { return root_id_of[element_id]; }

        void clear()
        {
            for (int h = 0; h < n_trees; h++)
                roots[h] = nullptr;
            for (int i = 0; i < n_nodes; i++)
                nodes[i].deactivate();
            n_active_nodes = 0;
        }

        void insert(int element_id, T key, int group_id)
        {
            node *p = nodes + element_id;
            roots[group_id] = p->join(roots[group_id], key);
            root_id_of[element_id] = group_id;
            ++n_active_nodes;
        }

        void improve_key(int element_id, T key)
        {
            node *p = nodes + element_id;
            int group_id = root_id_of[element_id];
            roots[group_id] = p->improve_key(roots[group_id], key);
        }

        void worsen_key(int element_id, T key)
        {
            node *p = nodes + element_id;
            int group_id = root_id_of[element_id];
            node *summit = roots[group_id];
            node *q = p->worsen_key(summit, key);
            if (p == summit)
                roots[group_id] = q;
        }

        void remove_top(int group_id)
        {
            roots[group_id] = roots[group_id]->leave_root();
            --n_active_nodes;
        }

        void remove(int element_id)
        {
            node *p = nodes + element_id;
            int group_id = root_id_of[element_id];
            node *summit = roots[group_id];
            if (p == summit)
                roots[group_id] = p->leave_root();
            else
                p->leave_non_root();
            --n_active_nodes;
        }

        ~monolithic_heap_group()
        {
            delete[] nodes;
            delete[] roots;
            delete[] root_id_of;
        }
    };

    template <template <typename> class PR, std::totally_ordered T>
    using pairing_heap = monolithic_heap<pairing, PR, T>;
    template <template <typename> class PR, std::totally_ordered T>
    using rank_pairing_heap = distributed_heap<rank_pairing, PR, T>;
    template <template <typename> class PR, std::totally_ordered T>
    using fibonacci_heap = distributed_heap<fibonacci, PR, T>;
    template <template <typename> class PR, std::totally_ordered T>
    using pairing_heap_group = monolithic_heap_group<pairing, PR, T>;
    template <template <typename> class PR, std::totally_ordered T>
    using rank_pairing_heap_group = monolithic_heap_group<rank_pairing, PR, T>;
}

#endif
