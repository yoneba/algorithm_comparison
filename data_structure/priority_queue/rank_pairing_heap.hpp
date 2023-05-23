#ifndef PRIORITY_QUEUE_RANK_PAIRING_HEAP_HPP
#define PRIORITY_QUEUE_RANK_PAIRING_HEAP_HPP

#if MEASUREMENT
#include "../../utility/common/operation_counting.hpp"
#endif

#include <concepts>
#include <string>
#include <cmath>
#include <algorithm>

namespace priority_queue
{
    template <template <typename> class PR>
    class rank_pairing
    {
        template <class Node>
        using predicate = PR<typename std::remove_reference<decltype(Node::key)>::type>;

    public:
        static constexpr std::string name() { return "rank_pairing"; }
        static constexpr std::string full_name() { return "rank_pairing"; }

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
            ++self->rank;
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

#if MEASUREMENT
        static inline struct operation_counter : public common::operation_counter
        {
            virtual void print()
            {
                common::operation_counter::print({"append_called",
                                                  "prune_called",
                                                  "replace_called",
                                                  "merge_called",
                                                  "insert_called",
                                                  "improve_key_called",
                                                  "restructuring_by_improve_key",
                                                  "worsen_key_called",
                                                  "restructuring_by_worsen_key",
                                                  "leave_root_called",
                                                  "restructuring_by_leave_root",
                                                  "leave_non_root_called",
                                                  "restructuring_by_leave_non_root",
                                                  "rank_adjustment",
                                                  "nodes_expanded",
                                                  "bucket_scan",
                                                  "comparison"});
            }
        } measurements;
#endif
        template <class Node>
        static bool is_active(const Node *self)
        {
            return self->rank != -1;
        }

        /**
         * @brief called before self will be inserted into the heap.
         * @param key to be assigned to the node.
         */
        template <class Node, std::totally_ordered T>
        static void activate(Node *self, T key)
        {
            self->child = nullptr;
            self->rank = 0;
            self->key = key;
        }

        /**
         * @brief called when self become outside the heap.
         */
        template <class Node>
        static void deactivate(Node *self)
        {
            self->rank = -1;
        }

        /**
         * @brief compute the correct rank.
         * @attention self must be root.
         * @return the correct rank of self.
         */
        template <class Node>
        static int rank_root(const Node *self)
        {
            return self->child ? self->child->rank + 1 : 0;
        }

        /**
         * @brief compute the correct rank.
         * @attention self must not be root.
         * @return the correct rank of self.
         */
        template <class Node>
        static int rank_non_root(const Node *self)
        {
            int rank_l = self->child ? self->child->rank : -1;
            int rank_r = self->next ? self->next->rank : -1;
            return std::max(rank_l, rank_r) + (std::abs(rank_l - rank_r) <= 1);
        }

        /**
         * @brief insert a node to the heap.
         * @param buckets the buckets to maintain the roots of the heap forest.
         * @param q the node to be newly inserted to the heap.
         */
        template <class Node>
        static void insert(Node *buckets[], Node *q)
        {
#if MEASUREMENT
            ++measurements["insert_called"];
#endif
            Node **r = buckets + q->rank;
            while (*r != nullptr)
            {
                q = merge(q, *r);
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
        template <class Node>
        static void adjust_rank(Node *buckets[], Node *q)
        {
            for (Node *p = q->prev; p != nullptr; p = q->prev)
            {
                int rank = rank_non_root(q);
                if (rank == q->rank)
                    return;
#if MEASUREMENT
                ++measurements["rank_adjustment"];
#endif
                q->rank = rank;
                q = p;
            }
            if (int rank = rank_root(q); rank != q->rank)
            {
#if MEASUREMENT
                ++measurements["rank_adjustment"];
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
        template <class Node>
        static void insert_successively(Node *buckets[], Node *p)
        {
            do
            {
#if MEASUREMENT
                ++measurements["nodes_expanded"];
#endif
                Node *q = p->next;
                insert(buckets, p);
                p = q;
            } while (p != nullptr);
        }

        /**
         * @brief improve self->key to key.
         * @param self the node whose key will be improved.
         * @param buckets the buckets to maintain the roots of the heap forest.
         * @param key will be set to self->key.
         */
        template <class Node, std::totally_ordered T>
        static void improve_key(Node *self, Node *buckets[], T key)
        {
#if MEASUREMENT
            ++measurements["improve_key_called"];
            measurements["comparison"] += self->prev != nullptr;
#endif
            self->key = key;
            if (Node *p = self->prev; p != nullptr && PR<T>()(key, p->key))
            {
#if MEASUREMENT
                ++measurements["restructuring_by_improve_key"];
#endif
                prune(self);
                adjust_rank(buckets, p);
                insert(buckets, self);
            }
        }

        /**
         * @brief worsen self->key to key.
         * @param self the node whose key will be worsened.
         * @param buckets the buckets to maintain the roots of the heap forest.
         * @param key will be set to self->key.
         */
        template <class Node, std::totally_ordered T>
        static void worsen_key(Node *self, Node *buckets[], T key)
        {
#if MEASUREMENT
            ++measurements["worsen_key_called"];
#endif
            self->key = key;
            if (Node *q = self->child; q != nullptr)
            {
#if MEASUREMENT
                ++measurements["restructuring_by_worsen_key"];
#endif
                self->child = nullptr;
                adjust_rank(buckets, self);
                insert_successively(buckets, q);
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
            buckets[self->rank] = nullptr;
            if (Node *q = self->child; q != nullptr)
            {
#if MEASUREMENT
                ++measurements["restructuring_by_leave_root"];
#endif
                insert_successively(buckets, q);
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
            adjust_rank(buckets, self->prev);
            if (Node *q = self->child; q != nullptr)
            {
#if MEASUREMENT
                ++measurements["restructuring_by_leave_non_root"];
#endif
                insert_successively(buckets, q);
            }
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

        template <std::totally_ordered T>
        class node
        {
            friend class rank_pairing;

            node *child, *next, *prev;
            int rank;

            node(const node &) = delete;
            node &operator=(const node &) = delete;

        public:
            T key;

            node() : rank(-1) {}

            bool is_active() const { return rank_pairing::is_active(this); }
            void activate(T key) { rank_pairing::activate(this, key); }
            void deactivate() { rank_pairing::deactivate(this); }
            void insert(node *buckets[]) { return rank_pairing::insert(buckets, this); }
            void improve_key(node *buckets[], T key) { return rank_pairing::improve_key(this, buckets, key); }
            void worsen_key(node *buckets[], T key) { return rank_pairing::worsen_key(this, buckets, key); }
            void leave_root(node *buckets[]) { return rank_pairing::leave_root(this, buckets); }
            void leave_non_root(node *buckets[]) { return rank_pairing::leave_non_root(this, buckets); }
        };
    };
}

#endif
