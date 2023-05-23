#ifndef PRIORITY_QUEUE_PAIRING_HEAP_HPP
#define PRIORITY_QUEUE_PAIRING_HEAP_HPP

#if MEASUREMENT
#include "../../utility/common/operation_counting.hpp"
#endif

#include <string>

namespace priority_queue
{
    template <template <typename> class PR>
    class pairing
    {
        template <class Node>
        using predicate = PR<typename std::remove_reference<decltype(Node::key)>::type>;

    public:
        static constexpr std::string name() { return "pairing"; }
        static constexpr std::string full_name() { return "pairing"; }

        /**
         * @brief set c as the only child of self.
         * @attention self must not have children.
         * @param self the node to become the parent of q.
         * @param q the node to become the child of self.
         */
        template <class Node>
        static void attach(Node *self, Node *q)
        {
#if MEASUREMENT
            ++measurements["attach_called"];
#endif
            q->next = nullptr, q->prev = self;
            self->child = q;
        }

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
            q->next = c, q->prev = self;
            if (c != nullptr)
                c->prev = q;
            self->child = q;
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
            Node *p = self->prev, *q = self->next;
            if (q != nullptr)
                q->prev = p;
            if (p->child == self)
                p->child = q;
            else
                p->next = q;
        }

        /**
         * @brief replace self by p.
         * @attention self must not be root.
         * @param self the node to be replaced.
         * @param r the node to be substituted.
         */
        template <class Node>
        static void replace(Node *self, Node *r)
        {
#if MEASUREMENT
            ++measurements["replace_called"];
#endif
            Node *p = self->prev, *q = self->next;
            r->next = q, r->prev = p;
            if (q != nullptr)
                q->prev = r;
            if (p->child == self)
                p->child = r;
            else
                p->next = r;
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
         * @brief consolidate p and it's sibling nodes into a single node.
         * @param p the first node of siblings.
         * @return the resulting consolidated node.
         */
        template <class Node>
        static Node *consolidate(Node *p)
        {
#if MEASUREMENT
            ++measurements["consolidate_called"];
            ++measurements["nodes_expanded"];
#endif
            Node *m = nullptr;
            for (Node *q = p->next; q != nullptr; q = p->next)
            {
#if MEASUREMENT
                ++measurements["nodes_expanded"];
#endif
                Node *r = q->next;
                p = merge(p, q);
                if (r == nullptr)
                    break;
                p->next = m, m = p;
                p = r;
            }
            while (m != nullptr)
            {
#if MEASUREMENT
                ++measurements["nodes_expanded"];
#endif
                Node *n = m->next;
                p = merge(p, m);
                m = n;
            }
            return p;
        }

    public:
#if MEASUREMENT
        static inline struct operation_counter : public common::operation_counter
        {
            virtual void print()
            {
                common::operation_counter::print({"attach_called",
                                                  "append_called",
                                                  "prune_called",
                                                  "replace_called",
                                                  "consolidate_called",
                                                  "join_called",
                                                  "merge_called",
                                                  "improve_key_called",
                                                  "restructuring_by_improve_key",
                                                  "worsen_key_called",
                                                  "restructuring_by_worsen_key",
                                                  "leave_root_called",
                                                  "restructuring_by_leave_root",
                                                  "leave_non_root_called",
                                                  "restructuring_by_leave_non_root",
                                                  "nodes_expanded",
                                                  "comparison"});
            }
        } measurements;
#endif
        /**
         * @brief test whether self is inside the heap.
         */
        template <class Node>
        static bool is_active(const Node *self)
        {
            return self->prev != nullptr;
        }

        /**
         * @brief called when self become outside the heap.
         */
        template <class Node>
        static void deactivate(Node *self)
        {
            self->prev = nullptr;
        }

        /**
         * @brief self will be inserted into the heap rooted at r.
         * @attention self can be inactive, but must not have children.
         * @attention r may be nullptr.
         * @param self the node to be inserted into the heap.
         * @param r the root node of the heap into which self will be inserted.
         * @param key will be set to self->key.
         * @return the new root node.
         */
        template <class Node, typename T>
        static Node *join(Node *self, Node *r, T key)
        {
#if MEASUREMENT
            ++measurements["join_called"];
            measurements["comparison"] += r != nullptr;
#endif
            self->key = key;
            if (r == nullptr)
                self->child = nullptr;
            else if (PR<T>()(r->key, key))
            {
                self->child = nullptr;
                append(r, self);
                return r;
            }
            else
                attach(self, r);
            self->prev = self;
            return self;
        }

        /**
         * @brief improve self->key to key.
         * @param self the node whose key will be improved.
         * @param r the root node of the heap containing self.
         * @param key will be set to self->key.
         * @return the new root node.
         */
        template <class Node, typename T>
        static Node *improve_key(Node *self, Node *r, T key)
        {
#if MEASUREMENT
            ++measurements["improve_key_called"];
            measurements["comparison"] += r != self;
#endif
            self->key = key;
            if (r == self || !PR<T>()(key, self->prev->key))
                return r;
#if MEASUREMENT
            ++measurements["restructuring_by_improve_key"];
#endif
            prune(self);
            return merge(r, self);
        }

        /**
         * @brief worsen self->key to key.
         * @param self the node whose key will be worsened.
         * @param r the root node of the heap containing self.
         * @param key will be set to self->key.
         * @return the newly substituted node (possibly self again).
         */
        template <class Node, typename T>
        static Node *worsen_key(Node *self, Node *r, T key)
        {
#if MEASUREMENT
            ++measurements["worsen_key_called"];
#endif
            self->key = key;
            if (self->child == nullptr)
                return self;
#if MEASUREMENT
            ++measurements["comparison"];
            ++measurements["restructuring_by_worsen_key"];
#endif
            Node *c = consolidate(self->child);
            if (PR<T>()(c->key, key))
            {
                if (r != self)
                    replace(self, c);
                self->child = nullptr;
                append(c, self);
                return c;
            }
            attach(self, c);
            return self;
        }

        /**
         * @brief self will leave the heap.
         * @attention self must be root.
         * @param self the node to leave the heap.
         * @return the newly substituted node.
         */
        template <class Node>
        static Node *leave_root(Node *self)
        {
#if MEASUREMENT
            ++measurements["leave_root_called"];
#endif
            Node *c = self->child;
            if (c != nullptr)
            {
#if MEASUREMENT
                ++measurements["restructuring_by_leave_root"];
#endif
                c = consolidate(c);
            }
            deactivate(self);
            return c;
        }

        /**
         * @brief self will leave the heap.
         * @attention self must not be root.
         * @param self the node to leave the heap.
         * @return the newly substituted node.
         */
        template <class Node>
        static Node *leave_non_root(Node *self)
        {
#if MEASUREMENT
            ++measurements["leave_non_root_called"];
#endif
            Node *c = self->child;
            if (c != nullptr)
            {
#if MEASUREMENT
                ++measurements["restructuring_by_leave_non_root"];
#endif
                c = consolidate(c);
                replace(self, c);
            }
            else
                prune(self);
            deactivate(self);
            return c;
        }

        template <typename T>
        class node
        {
            friend class pairing;

            node *child, *next, *prev;

            node(const node &) = delete;
            node &operator=(const node &) = delete;

        public:
            T key;

            node() : prev(nullptr) {}

            bool is_active() const { return pairing::is_active(this); }
            void deactivate() { pairing::deactivate(this); }
            node *join(node *r, T key) { return pairing::join(this, r, key); }
            node *improve_key(node *r, T key) { return pairing::improve_key(this, r, key); }
            node *worsen_key(node *r, T key) { return pairing::worsen_key(this, r, key); }
            node *leave_root() { return pairing::leave_root(this); }
            node *leave_non_root() { return pairing::leave_non_root(this); }
        };
    };
}

#endif
