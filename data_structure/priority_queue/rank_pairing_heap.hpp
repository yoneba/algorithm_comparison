#ifndef PRIORITY_QUEUE_RANK_PAIRING_HEAP_HPP
#define PRIORITY_QUEUE_RANK_PAIRING_HEAP_HPP

#include <string>
#include <cmath>
#include <algorithm>
#if MEASUREMENT
#include <iostream>
#endif

namespace priority_queue
{
    template <template <typename> class PR, typename T>
    class rank_pairing_node
    {
        static constexpr PR<T> cmp;
        rank_pairing_node *child, *next, *prev;
        int rank;

        rank_pairing_node(const rank_pairing_node &) = delete;
        rank_pairing_node &operator=(const rank_pairing_node &) = delete;

    protected:
        /**
         * @brief add c as the first child of this.
         * @param c the node to become the child of this.
         */
        void append(rank_pairing_node *c)
        {
#if MEASUREMENT
            ++measurements.append_called;
#endif
            c->next = child, c->prev = this;
            if (child)
                child->prev = c;
            child = c;
            ++rank;
        }

        /**
         * @brief prune the subtree rooted at this.
         * @attention this must not be root.
         */
        void prune()
        {
#if MEASUREMENT
            ++measurements.prune_called;
#endif
            if (next)
                next->prev = prev;
            if (prev->child == this)
                prev->child = next;
            else
                prev->next = next;
        }

        /**
         * @brief replace this by p.
         * @attention this must not be root.
         * @param p the node to be substituted.
         */
        void replace(rank_pairing_node *p)
        {
#if MEASUREMENT
            ++measurements.replace_called;
#endif
            p->next = next, p->prev = prev;
            if (next)
                next->prev = p;
            if (prev->child == this)
                prev->child = p;
            else
                prev->next = p;
        }

    public:
#if MEASUREMENT
        static inline struct measurements
        {
            long long constructor_called = 0;
            long long append_called = 0;
            long long prune_called = 0;
            long long replace_called = 0;
            long long merge_called = 0;
            long long insert_called = 0;
            long long improve_key_called = 0;
            long long restructuring_by_improve_key = 0;
            long long worsen_key_called = 0;
            long long restructuring_by_worsen_key = 0;
            long long leave_root_called = 0;
            long long restructuring_by_leave_root = 0;
            long long leave_internal_called = 0;
            long long restructuring_by_leave_internal = 0;
            long long rank_adjustment = 0;
            long long nodes_expanded = 0;
            long long bucket_scan = 0;
            long long comparison = 0;

            void print(const char *prefix = "") const
            {
                std::cout << prefix << "constructor_called = " << constructor_called << std::endl;
                std::cout << prefix << "append_called = " << append_called << std::endl;
                std::cout << prefix << "prune_called = " << prune_called << std::endl;
                std::cout << prefix << "replace_called = " << replace_called << std::endl;
                std::cout << prefix << "merge_called = " << merge_called << std::endl;
                std::cout << prefix << "insert_called = " << insert_called << std::endl;
                std::cout << prefix << "improve_key_called = " << improve_key_called << std::endl;
                std::cout << prefix << " |- restructuring_by_improve_key = " << restructuring_by_improve_key << std::endl;
                std::cout << prefix << "worsen_key_called = " << worsen_key_called << std::endl;
                std::cout << prefix << " |- restructuring_by_worsen_key = " << restructuring_by_worsen_key << std::endl;
                std::cout << prefix << "leave_root_called = " << leave_root_called << std::endl;
                std::cout << prefix << " |- restructuring_by_leave_root = " << restructuring_by_leave_root << std::endl;
                std::cout << prefix << "leave_internal_called = " << leave_internal_called << std::endl;
                std::cout << prefix << " |- restructuring_by_leave_internal = " << restructuring_by_leave_internal << std::endl;
                std::cout << prefix << "rank_adjustment = " << rank_adjustment << std::endl;
                std::cout << prefix << "nodes_expanded = " << nodes_expanded << std::endl;
                std::cout << prefix << "bucket_scan = " << bucket_scan << std::endl;
                std::cout << prefix << "comparison = " << comparison << std::endl;
            }

            void println(const char *prefix = "") const
            {
                print(prefix);
                std::cout << std::endl;
            }
        } measurements;
#endif
        T key;

        rank_pairing_node() : rank(-1)
        {
#if MEASUREMENT
            ++measurements.constructor_called;
#endif
        }

        bool is_active() const { return rank != -1; }

        /**
         * @brief called before this will be inserted into the heap.
         * @param key to be assigned to the node.
         */
        void activate(T key)
        {
            this->child = nullptr;
            this->rank = 0;
            this->key = key;
        }

        /**
         * @brief called when this become outside the heap.
         */
        void deactivate()
        {
            rank = -1;
        }

        /**
         * @brief compute the correct rank.
         * @attention this must be root.
         * @return the correct rank of this.
         */
        int rank_root() const
        {
            return child ? child->rank + 1 : 0;
        }

        /**
         * @brief compute the correct rank.
         * @attention this must not be root.
         * @return the correct rank of this.
         */
        int rank_internal() const
        {
            int rank_l = child ? child->rank : -1;
            int rank_r = next ? next->rank : -1;
            return std::max(rank_l, rank_r) + (std::abs(rank_l - rank_r) <= 1);
        }

        /**
         * @brief merge two trees into one.
         * @param p the root node of the other tree.
         * @return the root node of the new tree.
         */
        rank_pairing_node *merge(rank_pairing_node *p)
        {
#if MEASUREMENT
            ++measurements.merge_called;
            ++measurements.comparison;
#endif
            rank_pairing_node *q = this;
            if (cmp(q->key, p->key))
                q = p, p = this;
            p->append(q);
            return p;
        }

        /**
         * @brief insert a node to the heap.
         * @param buckets the buckets to maintain the roots of the heap forest.
         * @param q the node to be newly inserted to the heap.
         */
        static void insert(rank_pairing_node *buckets[], rank_pairing_node *q)
        {
#if MEASUREMENT
            ++measurements.insert_called;
#endif
            rank_pairing_node **r = buckets + q->rank;
            while (*r != nullptr)
            {
                q = q->merge(*r);
                *(r++) = nullptr;
            }
            *r = q;
            q->prev = nullptr;
        }

        /**
         * @brief process a node whose child's rank has decreased.
         * @param buckets the buckets to maintain the roots of the heap forest.
         * @param q the node whose child's rank has decreased.
         */
        static void adjust_rank(rank_pairing_node *buckets[], rank_pairing_node *q)
        {
            for (rank_pairing_node *p = q->prev; p != nullptr; p = q->prev)
            {
                int rank = q->rank_internal();
                if (rank == q->rank)
                    return;
#if MEASUREMENT
                ++measurements.rank_adjustment;
#endif
                q->rank = rank;
                q = p;
            }
            if (int rank = q->rank_root(); rank != q->rank)
            {
#if MEASUREMENT
                ++measurements.rank_adjustment;
#endif
                buckets[q->rank] = nullptr;
                q->rank = rank;
                insert(buckets, q);
            }
        }

        /**
         * @brief insert multiple nodes in a list.
         * @attention p must not be nullptr.
         * @param buckets the buckets to maintain the roots of the heap forest.
         * @param p the first node of the list to be inserted to the heap.
         */
        static void insert_successively(rank_pairing_node *buckets[], rank_pairing_node *p)
        {
            do
            {
#if MEASUREMENT
                ++measurements.nodes_expanded;
#endif
                rank_pairing_node *q = p->next;
                insert(buckets, p);
                p = q;
            } while (p != nullptr);
        }

        /**
         * @brief improve this->key to key.
         * @param buckets the buckets to maintain the roots of the heap forest.
         * @param key will be set to this->key.
         */
        void improve_key(rank_pairing_node *buckets[], T key)
        {
#if MEASUREMENT
            ++measurements.improve_key_called;
            measurements.comparison += this->prev != nullptr;
#endif
            this->key = key;
            if (rank_pairing_node *p = this->prev; p != nullptr && cmp(key, p->key))
            {
#if MEASUREMENT
                ++measurements.restructuring_by_improve_key;
#endif
                this->prune();
                adjust_rank(buckets, p);
                insert(buckets, this);
            }
        }

        /**
         * @brief worsen this->key to key.
         * @param buckets the buckets to maintain the roots of the heap forest.
         * @param key will be set to this->key.
         */
        void worsen_key(rank_pairing_node *buckets[], T key)
        {
#if MEASUREMENT
            ++measurements.worsen_key_called;
#endif
            this->key = key;
            if (rank_pairing_node *q = this->child; q != nullptr)
            {
#if MEASUREMENT
                ++measurements.restructuring_by_worsen_key;
#endif
                this->child = nullptr;
                adjust_rank(buckets, this);
                insert_successively(buckets, q);
            }
        }

        /**
         * @brief this will leave the heap.
         * @attention this must be root.
         * @param buckets the buckets to maintain the roots of the heap forest.
         */
        void leave_root(rank_pairing_node *buckets[])
        {
#if MEASUREMENT
            ++measurements.leave_root_called;
#endif
            buckets[this->rank] = nullptr;
            if (rank_pairing_node *q = this->child; q != nullptr)
            {
#if MEASUREMENT
                ++measurements.restructuring_by_leave_root;
#endif
                insert_successively(buckets, q);
            }
            this->deactivate();
        }

        /**
         * @brief this will leave the heap.
         * @attention this must not be root.
         * @param buckets the buckets to maintain the roots of the heap forest.
         */
        void leave_internal(rank_pairing_node *buckets[])
        {
#if MEASUREMENT
            ++measurements.leave_internal_called;
#endif
            this->prune();
            adjust_rank(buckets, this->prev);
            if (rank_pairing_node *q = this->child; q != nullptr)
            {
#if MEASUREMENT
                ++measurements.restructuring_by_leave_internal;
#endif
                insert_successively(buckets, q);
            }
            this->deactivate();
        }

        /**
         * @brief find the summit pointer in the heap.
         * @param r the buckets to maintain the roots of the heap forest.
         * @param n_active_nodes the number of active nodes currently in the heap.
         * @param nil the returned value when the buckets are all empty.
         * @return the summit pointer.
         */
        static rank_pairing_node *find_summit(rank_pairing_node *const *r, int n_active_nodes, rank_pairing_node *nil)
        {
            rank_pairing_node *summit = nil;
            for (int a = 1, b = 1; n_active_nodes >= a; ++r, a += b, b = a - b)
            {
#if MEASUREMENT
                ++measurements.bucket_scan;
#endif
                if (*r != nullptr && (summit == nil || cmp((*r)->key, summit->key)))
                    summit = *r;
            }
            return summit;
        }
    };

    template <template <typename> class PR, typename T>
    class rank_pairing_heap
    {
        using node = rank_pairing_node<PR, T>;

        static constexpr PR<T> cmp;
        static constexpr int n_buckets = 45;
        node *buckets[n_buckets];
        node *nodes;
        node *summit;
        int n_active_nodes;
        const int n_nodes;

        rank_pairing_heap(const rank_pairing_heap &) = delete;
        rank_pairing_heap &operator=(const rank_pairing_heap &) = delete;

    public:
        static constexpr std::string name()
        {
            return "rank_pairing_heap";
        }
#if MEASUREMENT
        static inline struct measurements
        {
            long long insert_called = 0;

            measurements()
            {
                node::measurements = node::measurements::measurements();
            }

            void print(const char *prefix = "") const
            {
                std::cout << prefix << "insert_called = " << insert_called << std::endl;
                std::cout << "[node]" << std::endl;
                node::measurements.print(prefix);
            }

            void println(const char *prefix = "") const
            {
                print(prefix);
                std::cout << std::endl;
            }
        } measurements;
#endif
        explicit rank_pairing_heap(int n, int d = 0)
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
            ++measurements.insert_called;
#endif
            node *q = nodes + id;
            q->activate(key);
            node::insert(buckets, q);
            if (n_active_nodes++ == 0 || cmp(key, summit->key))
                summit = q;
        }

        void improve_key(int id, T key)
        {
            node *q = nodes + id;
            q->improve_key(buckets, key);
            if (cmp(key, summit->key))
                summit = q;
        }

        void worsen_key(int id, T key)
        {
            node *q = nodes + id;
            q->worsen_key(buckets, key);
            if (q == summit)
                summit = node::find_summit(buckets, n_active_nodes, nodes - 1);
        }

        void remove_top()
        {
            summit->leave_root(buckets);
            --n_active_nodes;
            summit = node::find_summit(buckets, n_active_nodes, nodes - 1);
        }

        void remove(int id)
        {
            node *q = nodes + id;
            if (q == summit)
                return remove_top();
            q->leave_internal(buckets);
            --n_active_nodes;
        }

        ~rank_pairing_heap()
        {
            delete[] nodes;
        }
    };
}

#endif
