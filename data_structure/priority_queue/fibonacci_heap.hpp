#ifndef PRIORITY_QUEUE_FIBONACCI_HEAP_HPP
#define PRIORITY_QUEUE_FIBONACCI_HEAP_HPP

#if MEASUREMENT
#include "../../utility/common/operation_counting.hpp"
#endif

#include <string>

namespace priority_queue
{
    template <template <typename> class PR>
    class fibonacci
    {
        template <class Node>
        using predicate = PR<typename std::remove_reference<decltype(Node::key)>::type>;

        static constexpr int cut = 256;

    public:
        static constexpr std::string name() { return "fibonacci"; }
        static constexpr std::string full_name() { return "fibonacci"; }

        /**
         * @brief add c as the first child of self.
         * @param self the node to become the parent of q.
         * @param q the node to become the child of self.
         */
        template <class Node>
        static void append(Node *self, Node *q)
        {
#if MEASUREMENT
            ++measurements["append_called"];
#endif
            Node *c = self->child;
            q->parent = self, q->next = c, q->prev = nullptr;
            if (c != nullptr)
                c->prev = q;
            self->child = q;
            ++self->degree;
        }

        /**
         * @brief prune the subtree rooted at self.
         * @attention self must not be root.
         * @param self the node to be pruned.
         */
        template <class Node>
        static void prune(Node *self)
        {
#if MEASUREMENT
            ++measurements["prune_called"];
#endif
            Node *f = self->prev, *g = self->next, *p = self->parent;
            if (g != nullptr)
                g->prev = f;
            if (f != nullptr)
                f->next = g;
            else
                p->child = g;
            --p->degree;
        }

        /**
         * @brief replace self by p.
         * @attention self must not be root.
         * @param self the node to be replaced.
         * @param q the node to be substituted.
         */
        template <class Node>
        static void replace(Node *self, Node *q)
        {
#if MEASUREMENT
            ++measurements["replace_called"];
#endif
            Node *f = self->prev, *g = self->next, *p = self->parent;
            q->parent = p, q->next = g, q->prev = f;
            if (g != nullptr)
                g->prev = q;
            if (f != nullptr)
                f->next = q;
            else
                p->child = q;
        }

    public:
#if MEASUREMENT
        static inline struct operation_counter : public common::operation_counter
        {
            virtual void print()
            {
                common::operation_counter::print({"append_called",
                                                  "prune_called",
                                                  "replace_called",
                                                  "mark_called",
                                                  "unmark_called",
                                                  "merge_called",
                                                  "insert_called",
                                                  "improve_key_called",
                                                  "restructuring_by_improve_key",
                                                  "worsen_key_called",
                                                  "restructuring_by_worsen_key",
                                                  "heavy_restructuring_by_worsen_key",
                                                  "leave_root_called",
                                                  "restructuring_by_leave_root",
                                                  "leave_non_root_called",
                                                  "restructuring_by_leave_non_root",
                                                  "heavy_restructuring_by_leave_non_root",
                                                  "induced_pruning",
                                                  "nodes_expanded",
                                                  "bucket_scan",
                                                  "comparison"});
            }
        } measurements;
#endif
        template <class Node>
        static void clear(Node *self)
        {
            self->child = nullptr;
            self->degree = 0;
        }

        template <class Node>
        static bool is_active(const Node *self) { return self->degree != -1; }

        /**
         * @brief called before self will be inserted into the heap.
         * @param key to be assigned to the node.
         */
        template <class Node, typename T>
        static void activate(Node *self, T key)
        {
            clear(self);
            self->key = key;
        }

        /**
         * @brief called when self become outside the heap.
         */
        template <class Node>
        static void deactivate(Node *self)
        {
            self->degree = -1;
        }

        template <class Node>
        static int is_marked(const Node *self) { return self->degree & cut; }

        /**
         * @brief make self marked.
         * @attention self must not be root.
         */
        template <class Node>
        static void mark(Node *self)
        {
#if MEASUREMENT
            ++measurements["mark_called"];
#endif
            self->degree |= cut;
        }

        /**
         * @brief make self unmarked.
         * @attention there is no effect when self is root.
         */
        template <class Node>
        static void unmark(Node *self)
        {
#if MEASUREMENT
            ++measurements["unmark_called"];
#endif
            self->degree &= cut - 1;
        }

        /**
         * @brief merge two trees into one.
         * @param p the root node of a tree.
         * @param q the root node of the other tree.
         * @return the root node of the new tree.
         */
        template <class Node>
        static Node *merge(Node *p, Node *q)
        {
#if MEASUREMENT
            ++measurements["merge_called"];
            ++measurements["comparison"];
#endif
            constexpr predicate<Node> cmp;
            if (cmp(p->key, q->key))
                std::swap(p, q);
            append(q, p);
            return q;
        }

        /**
         * @brief insert a node to the heap.
         * @param self the node to be inserted into the heap.
         * @param buckets the buckets to maintain the roots of the heap forest.
         * @param f the node to be newly inserted to the heap.
         */
        template <class Node>
        static void insert(Node *buckets[], Node *f)
        {
#if MEASUREMENT
            ++measurements["insert_called"];
#endif
            Node **r = buckets + f->degree;
            while (*r != nullptr)
            {
                f = merge(f, *r);
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
        template <class Node>
        static void reinsert(Node *buckets[], Node *f)
        {
            prune(f);
            unmark(f);
            insert(buckets, f);
        }

        /**
         * @brief process a node whose child was pruned.
         * @param buckets the buckets to maintain the roots of the heap forest.
         * @param f the node whose child was pruned.
         */
        template <class Node>
        static void propagate_pruning(Node *buckets[], Node *f)
        {
            Node *p = f->parent;
            while (is_marked(f))
            {
#if MEASUREMENT
                ++measurements["induced_pruning"];
#endif
                reinsert(buckets, f);
                f = p, p = f->parent;
            }
            if (p != nullptr)
                mark(f);
            else
            {
                Node **r = buckets + f->degree;
                if (*r == nullptr)
                    *r = f, *(r + 1) = nullptr;
                else
                    *(r + 1) = merge(f, *r), *r = nullptr;
            }
        }

        /**
         * @brief insert multiple nodes in a list.
         * @attention f must not be nullptr.
         * @param buckets the buckets to maintain the roots of the heap forest.
         * @param f the first node of the list to be inserted to the heap.
         */
        template <class Node>
        static void insert_successively(Node *buckets[], Node *f)
        {
            do
            {
#if MEASUREMENT
                ++measurements["nodes_expanded"];
#endif
                Node *g = f->next;
                unmark(f);
                insert(buckets, f);
                f = g;
            } while (f != nullptr);
        }

        /**
         * @brief improve self->key to key.
         * @param self the node whose key will be improved.
         * @param buckets the buckets to maintain the roots of the heap forest.
         * @param key will be set to self->key.
         */
        template <class Node, typename T>
        static void improve_key(Node *self, Node *buckets[], T key)
        {
#if MEASUREMENT
            ++measurements["improve_key_called"];
            measurements["comparison"] += self->parent != nullptr;
#endif
            self->key = key;
            if (Node *p = self->parent; p != nullptr && PR<T>()(key, p->key))
            {
#if MEASUREMENT
                ++measurements["restructuring_by_improve_key"];
#endif
                reinsert(buckets, self);
                propagate_pruning(buckets, p);
            }
        }

        /**
         * @brief worsen self->key to key.
         * @param self the node whose key will be worsened.
         * @param buckets the buckets to maintain the roots of the heap forest.
         * @param key will be set to self->key.
         */
        template <class Node, typename T>
        static void worsen_key(Node *self, Node *buckets[], T key)
        {
#if MEASUREMENT
            ++measurements["worsen_key_called"];
#endif
            self->key = key;
            if (Node *f = self->child; f != nullptr)
            {
#if MEASUREMENT
                ++measurements["restructuring_by_worsen_key"];
#endif
                clear(self);
                insert_successively(buckets, f);
                if (Node *p = self->parent; p != nullptr)
                {
#if MEASUREMENT
                    ++measurements["heavy_restructuring_by_worsen_key"];
#endif
                    reinsert(buckets, self);
                    propagate_pruning(buckets, p);
                }
            }
        }

        /**
         * @brief self will leave the heap.
         * @attention self must be root.
         * @param self the node to leave the heap.
         * @param buckets the buckets to maintain the roots of the heap forest.
         */
        template <class Node>
        static void leave_root(Node *self, Node *buckets[])
        {
#if MEASUREMENT
            ++measurements["leave_root_called"];
#endif
            buckets[self->degree] = nullptr;
            if (Node *f = self->child; f != nullptr)
            {
#if MEASUREMENT
                ++measurements["restructuring_by_leave_root"];
#endif
                insert_successively(buckets, f);
            }
            deactivate(self);
        }

        /**
         * @brief self will leave the heap.
         * @attention self must not be root.
         * @param self the node to leave the heap.
         * @param buckets the buckets to maintain the roots of the heap forest.
         */
        template <class Node>
        static void leave_non_root(Node *self, Node *buckets[])
        {
#if MEASUREMENT
            ++measurements["leave_non_root_called"];
#endif
            prune(self);
            if (Node *f = self->child; f != nullptr)
            {
#if MEASUREMENT
                ++measurements["heavy_restructuring_by_leave_non_root"];
#endif
                insert_successively(buckets, f);
            }
#if MEASUREMENT
            ++measurements["restructuring_by_leave_non_root"];
#endif
            propagate_pruning(buckets, self->parent);
            deactivate(self);
        }

        /**
         * @brief find the summit pointer in the heap.
         * @param r the buckets to maintain the roots of the heap forest.
         * @param n_active_nodes the number of active nodes currently in the heap.
         * @param nil the returned value when the buckets are all empty.
         * @return the summit pointer.
         */
        template <class Node>
        static Node *find_summit(const Node *const *r, int n_active_nodes, const Node *nil)
        {
            constexpr predicate<Node> cmp;
            const Node *summit = nil;
            for (int a = 1, b = 1; n_active_nodes >= a; ++r, a += b, b = a - b)
            {
#if MEASUREMENT
                ++measurements["bucket_scan"];
#endif
                if (*r != nullptr && (summit == nil || cmp((*r)->key, summit->key)))
                    summit = *r;
            }
            return const_cast<Node *>(summit);
        }

        template <typename T>
        class node
        {
            friend class fibonacci;

            node *child, *next, *prev, *parent;
            int degree;

            node(const node &) = delete;
            node &operator=(const node &) = delete;

        public:
            T key;

            node() : degree(-1) {}

            bool is_active() const { return fibonacci::is_active(this); }
            void activate(T key) { fibonacci::activate(this, key); }
            void deactivate() { fibonacci::deactivate(this); }
            void insert(node *buckets[]) { return fibonacci::insert(buckets, this); }
            void improve_key(node *buckets[], T key) { return fibonacci::improve_key(this, buckets, key); }
            void worsen_key(node *buckets[], T key) { return fibonacci::worsen_key(this, buckets, key); }
            void leave_root(node *buckets[]) { return fibonacci::leave_root(this, buckets); }
            void leave_non_root(node *buckets[]) { return fibonacci::leave_non_root(this, buckets); }
        };
    };
}

#endif
