#ifndef PRIORITY_QUEUE_FIBONACCI_HEAP_HPP
#define PRIORITY_QUEUE_FIBONACCI_HEAP_HPP

#include <string>
#if MEASUREMENT
#include <iostream>
#endif

namespace priority_queue
{
    template <template <typename> class PR, typename T>
    class fibonacci_node
    {
        static constexpr PR<T> cmp;
        static constexpr int cut = 256;
        fibonacci_node *child, *next, *prev, *parent;
        int degree;

        fibonacci_node(const fibonacci_node &) = delete;
        fibonacci_node &operator=(const fibonacci_node &) = delete;

    protected:
        /**
         * @brief add c as the first child of this.
         * @param c the node to become the child of this.
         */
        void append(fibonacci_node *c)
        {
#if MEASUREMENT
            ++measurements.append_called;
#endif
            c->parent = this, c->next = child, c->prev = nullptr;
            if (child)
                child->prev = c;
            child = c;
            ++degree;
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
            if (prev)
                prev->next = next;
            else
                parent->child = next;
            --parent->degree;
        }

        /**
         * @brief replace this by p.
         * @attention this must not be root.
         * @param p the node to be substituted.
         */
        void replace(fibonacci_node *p)
        {
#if MEASUREMENT
            ++measurements.replace_called;
#endif
            p->parent = parent, p->next = next, p->prev = prev;
            if (next)
                next->prev = p;
            if (prev)
                prev->next = p;
            else
                parent->child = p;
        }

    public:
#if MEASUREMENT
        static inline struct measurements
        {
            long long constructor_called = 0;
            long long append_called = 0;
            long long prune_called = 0;
            long long replace_called = 0;
            long long mark_called = 0;
            long long unmark_called = 0;
            long long merge_called = 0;
            long long insert_called = 0;
            long long improve_key_called = 0;
            long long restructuring_by_improve_key = 0;
            long long worsen_key_called = 0;
            long long restructuring_by_worsen_key = 0;
            long long heavy_restructuring_by_worsen_key = 0;
            long long leave_root_called = 0;
            long long restructuring_by_leave_root = 0;
            long long leave_internal_called = 0;
            long long restructuring_by_leave_internal = 0;
            long long heavy_restructuring_by_leave_internal = 0;
            long long induced_pruning = 0;
            long long nodes_expanded = 0;
            long long bucket_scan = 0;
            long long comparison = 0;

            void print(const char *prefix = "") const
            {
                std::cout << prefix << "constructor_called = " << constructor_called << std::endl;
                std::cout << prefix << "append_called = " << append_called << std::endl;
                std::cout << prefix << "prune_called = " << prune_called << std::endl;
                std::cout << prefix << "replace_called = " << replace_called << std::endl;
                std::cout << prefix << "mark_called = " << mark_called << std::endl;
                std::cout << prefix << "unmark_called = " << unmark_called << std::endl;
                std::cout << prefix << "merge_called = " << merge_called << std::endl;
                std::cout << prefix << "insert_called = " << insert_called << std::endl;
                std::cout << prefix << "improve_key_called = " << improve_key_called << std::endl;
                std::cout << prefix << " |- restructuring_by_improve_key = " << restructuring_by_improve_key << std::endl;
                std::cout << prefix << "worsen_key_called = " << worsen_key_called << std::endl;
                std::cout << prefix << " |- restructuring_by_worsen_key = " << restructuring_by_worsen_key << std::endl;
                std::cout << prefix << " |- heavy_restructuring_by_worsen_key = " << heavy_restructuring_by_worsen_key << std::endl;
                std::cout << prefix << "leave_root_called = " << leave_root_called << std::endl;
                std::cout << prefix << " |- restructuring_by_leave_root = " << restructuring_by_leave_root << std::endl;
                std::cout << prefix << "leave_internal_called = " << leave_internal_called << std::endl;
                std::cout << prefix << " |- restructuring_by_leave_internal = " << restructuring_by_leave_internal << std::endl;
                std::cout << prefix << " |- heavy_restructuring_by_leave_internal = " << heavy_restructuring_by_leave_internal << std::endl;
                std::cout << prefix << "induced_pruning = " << induced_pruning << std::endl;
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

        fibonacci_node() : degree(-1)
        {
#if MEASUREMENT
            ++measurements.constructor_called;
#endif
        }

        void clear()
        {
            child = nullptr;
            degree = 0;
        }

        bool is_active() const { return degree != -1; }

        /**
         * @brief called before this will be inserted into the heap.
         * @param key to be assigned to the node.
         */
        void activate(T key)
        {
            this->clear();
            this->key = key;
        }

        /**
         * @brief called when this become outside the heap.
         */
        void deactivate()
        {
            degree = -1;
        }

        int is_marked() const { return degree & cut; }

        /**
         * @brief make this marked.
         * @attention this must not be root.
         */
        void mark()
        {
#if MEASUREMENT
            ++measurements.mark_called;
#endif
            degree |= cut;
        }

        /**
         * @brief make this unmarked.
         * @attention there is no effect when this is root.
         */
        void unmark()
        {
#if MEASUREMENT
            ++measurements.unmark_called;
#endif
            degree &= cut - 1;
        }

        /**
         * @brief merge two trees into one.
         * @param p the root node of the other tree.
         * @return the root node of the new tree.
         */
        fibonacci_node *merge(fibonacci_node *p)
        {
#if MEASUREMENT
            ++measurements.merge_called;
            ++measurements.comparison;
#endif
            fibonacci_node *q = this;
            if (cmp(q->key, p->key))
                q = p, p = this;
            p->append(q);
            return p;
        }

        /**
         * @brief insert a node to the heap.
         * @param buckets the buckets to maintain the roots of the heap forest.
         * @param f the node to be newly inserted to the heap.
         */
        static void insert(fibonacci_node *buckets[], fibonacci_node *f)
        {
#if MEASUREMENT
            ++measurements.insert_called;
#endif
            fibonacci_node **r = buckets + f->degree;
            while (*r != nullptr)
            {
                f = f->merge(*r);
                *(r++) = nullptr;
            }
            *r = f;
            f->parent = nullptr;
        }

        /**
         * @brief reinsert a node in the heap.
         * @attention f must not be root.
         * @param buckets the buckets to maintain the roots of the heap forest.
         * @param f the node to be reinserted to the heap.
         */
        static void reinsert(fibonacci_node *buckets[], fibonacci_node *f)
        {
            f->prune();
            f->unmark();
            insert(buckets, f);
        }

        /**
         * @brief process a node whose child was pruned.
         * @param buckets the buckets to maintain the roots of the heap forest.
         * @param f the node whose child was pruned.
         */
        static void propagate_pruning(fibonacci_node *buckets[], fibonacci_node *f)
        {
            fibonacci_node *p = f->parent;
            while (f->is_marked())
            {
#if MEASUREMENT
                ++measurements.induced_pruning;
#endif
                reinsert(buckets, f);
                f = p, p = f->parent;
            }
            if (p != nullptr)
                f->mark();
            else
            {
                fibonacci_node **r = buckets + f->degree;
                if (*r == nullptr)
                    *r = f, *(r + 1) = nullptr;
                else
                    *(r + 1) = f->merge(*r), *r = nullptr;
            }
        }

        /**
         * @brief insert multiple nodes in a list.
         * @attention f must not be nullptr.
         * @param buckets the buckets to maintain the roots of the heap forest.
         * @param f the first node of the list to be inserted to the heap.
         */
        static void insert_successively(fibonacci_node *buckets[], fibonacci_node *f)
        {
            do
            {
#if MEASUREMENT
                ++measurements.nodes_expanded;
#endif
                fibonacci_node *g = f->next;
                f->unmark();
                insert(buckets, f);
                f = g;
            } while (f != nullptr);
        }

        /**
         * @brief improve this->key to key.
         * @param buckets the buckets to maintain the roots of the heap forest.
         * @param key will be set to this->key.
         */
        void improve_key(fibonacci_node *buckets[], T key)
        {
#if MEASUREMENT
            ++measurements.improve_key_called;
            measurements.comparison += this->parent != nullptr;
#endif
            this->key = key;
            if (fibonacci_node *p = this->parent; p != nullptr && cmp(key, p->key))
            {
#if MEASUREMENT
                ++measurements.restructuring_by_improve_key;
#endif
                reinsert(buckets, this);
                propagate_pruning(buckets, p);
            }
        }

        /**
         * @brief worsen this->key to key.
         * @param buckets the buckets to maintain the roots of the heap forest.
         * @param key will be set to this->key.
         */
        void worsen_key(fibonacci_node *buckets[], T key)
        {
#if MEASUREMENT
            ++measurements.worsen_key_called;
#endif
            this->key = key;
            if (fibonacci_node *f = this->child; f != nullptr)
            {
#if MEASUREMENT
                ++measurements.restructuring_by_worsen_key;
#endif
                this->clear();
                insert_successively(buckets, f);
                if (fibonacci_node *p = this->parent; p != nullptr)
                {
#if MEASUREMENT
                    ++measurements.heavy_restructuring_by_worsen_key;
#endif
                    reinsert(buckets, this);
                    propagate_pruning(buckets, p);
                }
            }
        }

        /**
         * @brief this will leave the heap.
         * @attention this must be root.
         * @param buckets the buckets to maintain the roots of the heap forest.
         */
        void leave_root(fibonacci_node *buckets[])
        {
#if MEASUREMENT
            ++measurements.leave_root_called;
#endif
            buckets[this->degree] = nullptr;
            if (fibonacci_node *f = this->child; f != nullptr)
            {
#if MEASUREMENT
                ++measurements.restructuring_by_leave_root;
#endif
                insert_successively(buckets, f);
            }
            this->deactivate();
        }

        /**
         * @brief this will leave the heap.
         * @attention this must not be root.
         * @param buckets the buckets to maintain the roots of the heap forest.
         */
        void leave_internal(fibonacci_node *buckets[])
        {
#if MEASUREMENT
            ++measurements.leave_internal_called;
#endif
            this->prune();
            if (fibonacci_node *f = this->child; f != nullptr)
            {
#if MEASUREMENT
                ++measurements.heavy_restructuring_by_leave_internal;
#endif
                insert_successively(buckets, f);
            }
#if MEASUREMENT
            ++measurements.restructuring_by_leave_internal;
#endif
            propagate_pruning(buckets, this->parent);
            this->deactivate();
        }

        /**
         * @brief find the summit pointer in the heap.
         * @param r the buckets to maintain the roots of the heap forest.
         * @param n_active_nodes the number of active nodes currently in the heap.
         * @param nil the returned value when the buckets are all empty.
         * @return the summit pointer.
         */
        static fibonacci_node *find_summit(fibonacci_node *const *r, int n_active_nodes, fibonacci_node *nil)
        {
            fibonacci_node *summit = nil;
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
    class fibonacci_heap
    {
        using node = fibonacci_node<PR, T>;

        static constexpr PR<T> cmp;
        static constexpr int n_buckets = 45;
        node *buckets[n_buckets];
        node *nodes;
        node *summit;
        int n_active_nodes;
        const int n_nodes;

        fibonacci_heap(const fibonacci_heap &) = delete;
        fibonacci_heap &operator=(const fibonacci_heap &) = delete;

    public:
        static constexpr std::string name()
        {
            return "fibonacci_heap";
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
        explicit fibonacci_heap(int n, int d = 0)
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
            node *f = nodes + id;
            f->activate(key);
            node::insert(buckets, f);
            if (n_active_nodes++ == 0 || cmp(key, summit->key))
                summit = f;
        }

        void improve_key(int id, T key)
        {
            node *f = nodes + id;
            f->improve_key(buckets, key);
            if (cmp(key, summit->key))
                summit = f;
        }

        void worsen_key(int id, T key)
        {
            node *f = nodes + id;
            f->worsen_key(buckets, key);
            if (f == summit)
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
            node *f = nodes + id;
            if (f == summit)
                return remove_top();
            f->leave_internal(buckets);
            --n_active_nodes;
        }

        ~fibonacci_heap()
        {
            delete[] nodes;
        }
    };
}

#endif
