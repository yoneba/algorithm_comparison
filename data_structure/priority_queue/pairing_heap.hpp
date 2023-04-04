#ifndef PRIORITY_QUEUE_PAIRING_HEAP_HPP
#define PRIORITY_QUEUE_PAIRING_HEAP_HPP

#include <string>
#if MEASUREMENT
#include <iostream>
#endif

namespace priority_queue
{
    template <template <typename> class PR, typename T>
    class pairing_node
    {
        static constexpr PR<T> cmp;
        pairing_node *child, *next, *prev;

        pairing_node(const pairing_node &) = delete;
        pairing_node &operator=(const pairing_node &) = delete;

    protected:
        /**
         * @brief set c as the only child of this.
         * @attention this must not have children.
         * @param c the node to become the child of this.
         */
        void attach(pairing_node *c)
        {
#if MEASUREMENT
            ++measurements.attach_called;
#endif
            c->next = nullptr, c->prev = this;
            child = c;
        }

        /**
         * @brief add c as the first child of this.
         * @param c the node to become the child of this.
         */
        void append(pairing_node *c)
        {
#if MEASUREMENT
            ++measurements.append_called;
#endif
            c->next = child, c->prev = this;
            if (child)
                child->prev = c;
            child = c;
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
        void replace(pairing_node *p)
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

        /**
         * @brief consolidate p and it's sibling nodes into a single node.
         * @param p the first node of siblings.
         * @return the resulting consolidated node.
         */
        static pairing_node *consolidate(pairing_node *p)
        {
#if MEASUREMENT
            ++measurements.consolidate_called;
#endif
            pairing_node *m = nullptr;
            for (pairing_node *q = p->next; q; q = p->next)
            {
#if MEASUREMENT
                ++measurements.left_to_right_merger;
#endif
                pairing_node *r = q->next;
                p = p->merge(q);
                if (!r)
                    break;
                p->next = m, m = p;
                p = r;
            }
            while (m)
            {
#if MEASUREMENT
                ++measurements.right_to_left_merger;
#endif
                pairing_node *n = m->next;
                p = p->merge(m);
                m = n;
            }
            return p;
        }

    public:
#if MEASUREMENT
        static inline struct measurements
        {
            long long constructor_called = 0;
            long long attach_called = 0;
            long long append_called = 0;
            long long prune_called = 0;
            long long replace_called = 0;
            long long consolidate_called = 0;
            long long left_to_right_merger = 0;
            long long right_to_left_merger = 0;
            long long join_called = 0;
            long long merge_called = 0;
            long long improve_key_called = 0;
            long long restructuring_by_improve_key = 0;
            long long worsen_key_called = 0;
            long long restructuring_by_worsen_key = 0;
            long long leave_root_called = 0;
            long long restructuring_by_leave_root = 0;
            long long leave_internal_called = 0;
            long long restructuring_by_leave_internal = 0;
            long long nodes_expanded() const { return consolidate_called + left_to_right_merger + right_to_left_merger; }
            long long comparison = 0;

            void reset()
            {
                *this = measurements();
            }

            void print(const char *prefix = "") const
            {
                std::cout << prefix << "constructor_called = " << constructor_called << std::endl;
                std::cout << prefix << "attach_called = " << attach_called << std::endl;
                std::cout << prefix << "append_called = " << append_called << std::endl;
                std::cout << prefix << "prune_called = " << prune_called << std::endl;
                std::cout << prefix << "replace_called = " << replace_called << std::endl;
                std::cout << prefix << "consolidate_called = " << consolidate_called << std::endl;
                std::cout << prefix << " |- left_to_right_merger = " << left_to_right_merger << std::endl;
                std::cout << prefix << " |- right_to_left_merger = " << right_to_left_merger << std::endl;
                std::cout << prefix << "join_called = " << join_called << std::endl;
                std::cout << prefix << "merge_called = " << merge_called << std::endl;
                std::cout << prefix << "improve_key_called = " << improve_key_called << std::endl;
                std::cout << prefix << " |- restructuring_by_improve_key = " << restructuring_by_improve_key << std::endl;
                std::cout << prefix << "worsen_key_called = " << worsen_key_called << std::endl;
                std::cout << prefix << " |- restructuring_by_worsen_key = " << restructuring_by_worsen_key << std::endl;
                std::cout << prefix << "leave_root_called = " << leave_root_called << std::endl;
                std::cout << prefix << " |- restructuring_by_leave_root = " << restructuring_by_leave_root << std::endl;
                std::cout << prefix << "leave_internal_called = " << leave_internal_called << std::endl;
                std::cout << prefix << " |- restructuring_by_leave_internal = " << restructuring_by_leave_internal << std::endl;
                std::cout << prefix << "nodes_expanded = " << nodes_expanded() << std::endl;
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

        pairing_node() : prev(nullptr)
        {
#if MEASUREMENT
            ++measurements.constructor_called;
#endif
        }

        bool is_active() const { return prev != nullptr; }

        /**
         * @brief called when this become outside the heap.
         */
        void deactivate()
        {
            prev = nullptr;
        }

        /**
         * @brief this will be inserted into the heap rooted at r.
         * @attention this can be inactive, but must not have children.
         * @attention r may be nullptr.
         * @param r the root node of the heap into which this will be inserted.
         * @param key will be set to this->key.
         * @return the new root node.
         */
        pairing_node *join(pairing_node *r, T key)
        {
#if MEASUREMENT
            ++measurements.join_called;
            measurements.comparison += r != nullptr;
#endif
            this->key = key;
            if (r == nullptr)
                this->child = nullptr;
            else if (cmp(r->key, key))
            {
                this->child = nullptr;
                r->append(this);
                return r;
            }
            else
                this->attach(r);
            this->prev = this;
            return this;
        }

        /**
         * @brief merge two trees into one.
         * @param p the root node of the other tree.
         * @return the root node of the new tree.
         */
        pairing_node *merge(pairing_node *p)
        {
#if MEASUREMENT
            ++measurements.merge_called;
            ++measurements.comparison;
#endif
            pairing_node *q = this;
            if (cmp(q->key, p->key))
                q = p, p = this;
            p->append(q);
            return p;
        }

        /**
         * @brief improve this->key to key.
         * @param r the root node of the heap containing this.
         * @param key will be set to this->key.
         * @return the new root node.
         */
        pairing_node *improve_key(pairing_node *r, T key)
        {
#if MEASUREMENT
            ++measurements.improve_key_called;
            measurements.comparison += r != this;
#endif
            this->key = key;
            if (r == this || !cmp(key, this->prev->key))
                return r;
#if MEASUREMENT
            ++measurements.restructuring_by_improve_key;
#endif
            this->prune();
            return r->merge(this);
        }

        /**
         * @brief worsen this->key to key.
         * @param r the root node of the heap containing this.
         * @param key will be set to this->key.
         * @return the newly substituted node (possibly this again).
         */
        pairing_node *worsen_key(pairing_node *r, T key)
        {
#if MEASUREMENT
            ++measurements.worsen_key_called;
#endif
            this->key = key;
            if (this->child == nullptr)
                return this;
#if MEASUREMENT
            ++measurements.comparison;
            ++measurements.restructuring_by_worsen_key;
#endif
            pairing_node *c = consolidate(this->child);
            if (cmp(c->key, key))
            {
                if (r != this)
                    this->replace(c);
                this->child = nullptr;
                c->append(this);
                return c;
            }
            this->attach(c);
            return this;
        }

        /**
         * @brief this will leave the heap.
         * @attention this must be root.
         * @return the newly substituted node.
         */
        pairing_node *leave_root()
        {
#if MEASUREMENT
            ++measurements.leave_root_called;
#endif
            pairing_node *c = this->child;
            if (c != nullptr)
            {
#if MEASUREMENT
                ++measurements.restructuring_by_leave_root;
#endif
                c = consolidate(c);
            }
            this->deactivate();
            return c;
        }

        /**
         * @brief this will leave the heap.
         * @attention this must not be root.
         * @return the newly substituted node.
         */
        pairing_node *leave_internal()
        {
#if MEASUREMENT
            ++measurements.leave_internal_called;
#endif
            pairing_node *c = this->child;
            if (c != nullptr)
            {
#if MEASUREMENT
                ++measurements.restructuring_by_leave_internal;
#endif
                c = consolidate(c);
                this->replace(c);
            }
            else
                this->prune();
            this->deactivate();
            return c;
        }
    };

    template <template <typename> class PR, typename T>
    class pairing_heap
    {
        using node = pairing_node<PR, T>;

        static constexpr PR<T> cmp;
        node *nodes;
        node *summit;
        int n_active_nodes;
        const int n_nodes;

        pairing_heap(const pairing_heap &) = delete;
        pairing_heap &operator=(const pairing_heap &) = delete;

    public:
        static constexpr std::string name()
        {
            return "pairing_heap";
        }
#if MEASUREMENT
        static inline struct measurements
        {
            void reset()
            {
                node::measurements.reset();
            }

            void print(const char *prefix = "") const
            {
                node::measurements.print(prefix);
            }

            void println(const char *prefix = "") const
            {
                print(prefix);
                std::cout << std::endl;
            }
        } measurements;
#endif
        explicit pairing_heap(int n, int d = 0)
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
            p->leave_internal();
            --n_active_nodes;
        }

        ~pairing_heap()
        {
            delete[] nodes;
        }
    };

    template <template <typename> class PR, typename T>
    class pairing_forest
    {
        using node = pairing_node<PR, T>;

        static constexpr PR<T> cmp;
        node *nodes;
        node **roots;
        int *root_id_of;
        int n_active_nodes;
        const int n_nodes;
        const int n_trees;

        pairing_forest(const pairing_forest &) = delete;
        pairing_forest &operator=(const pairing_forest &) = delete;

    public:
        static constexpr std::string name()
        {
            return "pairing_forest";
        }
#if MEASUREMENT
        static inline struct measurements
        {
            void reset()
            {
                node::measurements.reset();
            }

            void print(const char *prefix = "") const
            {
                node::measurements.print(prefix);
            }

            void println(const char *prefix = "") const
            {
                print(prefix);
                std::cout << std::endl;
            }
        } measurements;
#endif
        explicit pairing_forest(int n_elements, int n_groups)
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
                p->leave_internal();
            --n_active_nodes;
        }

        ~pairing_forest()
        {
            delete[] nodes;
            delete[] roots;
            delete[] root_id_of;
        }
    };
}

#endif
