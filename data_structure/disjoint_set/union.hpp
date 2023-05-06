#ifndef DISJOINT_SET_UNION_HPP
#define DISJOINT_SET_UNION_HPP

#if MEASUREMENT
#include "../../utility/common/operation_counting.hpp"
#endif

#include "find.hpp"

#include <format>

namespace disjoint_set
{
    template <class Find = path_halving>
    class in_tree
    {
    public:
#if MEASUREMENT
        static inline struct operation_counter : public common::operation_counter
        {
            void clear()
            {
                common::operation_counter::clear();
                Find::measurements.clear();
            }
            virtual void print()
            {
                Find::measurements.print();
                common::operation_counter::print({"link_called", "unite_called"});
            }
        } measurements;
#endif
        static constexpr std::string name()
        {
            return "in_tree";
        }
        static constexpr std::string full_name()
        {
            return std::format("in_tree<{}>", Find::name());
        }

        static void reset(int parent_of[], int n)
        {
            for (int i = 0; i < n; i++)
                parent_of[i] = i;
        }

        template <class Node>
        static bool is_representative(const Node *self) { return self->parent == self; }
        static bool is_representative(const int parent_of[], int self) { return parent_of[self] == self; }

        template <class Node>
        static void link(Node *p, Node *q)
        {
#if MEASUREMENT
            ++measurements["link_called"];
#endif
            q->parent = p;
        }
        static void link(int parent_of[], int p, int q)
        {
#if MEASUREMENT
            ++measurements["link_called"];
#endif
            parent_of[q] = p;
        }

        template <class Node>
        static bool unite(Node *p, Node *q)
        {
#if MEASUREMENT
            ++measurements["unite_called"];
#endif
            p = Find::find(p), q = Find::find(q);
            if (p == q)
                return false;
            link(p, q);
            return true;
        }
        static bool unite(int parent_of[], int p, int q)
        {
#if MEASUREMENT
            ++measurements["unite_called"];
#endif
            p = Find::find(parent_of, p), q = Find::find(parent_of, q);
            if (p == q)
                return false;
            link(parent_of, p, q);
            return true;
        }

        class node
        {
            friend class in_tree;
            friend class path_preserving;
            friend class path_compression;
            friend class path_halving;
            friend class path_halving_dull;

            node(const node &) = delete;
            node &operator=(const node &) = delete;

        protected:
            node *parent;

        public:
            node() { parent = this; };

            bool is_representative() const { return in_tree::is_representative(this); }
            node *find() { return Find::find(this); }
            void link(node *q) { return in_tree::link(this, q); }
            bool unite(node *q) { return in_tree::unite(this, q); }
        };
    };

    template <class Find = path_halving>
    class ranked_in_tree
    {
    public:
#if MEASUREMENT
        static inline struct operation_counter : public common::operation_counter
        {
            void clear()
            {
                common::operation_counter::clear();
                Find::measurements.clear();
            }
            virtual void print()
            {
                Find::measurements.print();
                common::operation_counter::print({"link_called", "unite_called", "rank_incremented"});
            }
        } measurements;
#endif
        static constexpr std::string name()
        {
            return "ranked_in_tree";
        }
        static constexpr std::string full_name()
        {
            return std::format("ranked_in_tree<{}>", Find::name());
        }

        static void reset(int parent_of[], int rank_of[], int n)
        {
            for (int i = 0; i < n; i++)
            {
                parent_of[i] = i;
                rank_of[i] = 0;
            }
        }

        template <class Node>
        static bool is_representative(const Node *self) { return self->parent == self; }
        static bool is_representative(const int parent_of[], int self) { return parent_of[self] == self; }

        template <class Node>
        static void link(Node *p, Node *q)
        {
#if MEASUREMENT
            ++measurements["link_called"];
#endif
            if (p->rank < q->rank)
                p->parent = q;
            else
            {
                q->parent = p;
                if (p->rank == q->rank)
                {
#if MEASUREMENT
                    ++measurements["rank_incremented"];
#endif
                    ++p->rank;
                }
            }
        }
        static void link(int parent_of[], int rank_of[], int p, int q)
        {
#if MEASUREMENT
            ++measurements["link_called"];
#endif
            if (rank_of[p] < rank_of[q])
                parent_of[p] = q;
            else
            {
                parent_of[q] = p;
                if (rank_of[p] == rank_of[q])
                {
#if MEASUREMENT
                    ++measurements["rank_incremented"];
#endif
                    ++rank_of[p];
                }
            }
        }

        template <class Node>
        static bool unite(Node *p, Node *q)
        {
#if MEASUREMENT
            ++measurements["unite_called"];
#endif
            p = Find::find(p), q = Find::find(q);
            if (p == q)
                return false;
            link(p, q);
            return true;
        }
        static bool unite(int parent_of[], int rank_of[], int p, int q)
        {
#if MEASUREMENT
            ++measurements["unite_called"];
#endif
            p = Find::find(parent_of, p), q = Find::find(parent_of, q);
            if (p == q)
                return false;
            link(parent_of, rank_of, p, q);
            return true;
        }

        class node
        {
            friend class ranked_in_tree;
            friend class path_preserving;
            friend class path_compression;
            friend class path_halving;
            friend class path_halving_dull;

            node(const node &) = delete;
            node &operator=(const node &) = delete;

        protected:
            node *parent;
            int rank;

        public:
            node() : rank(0) { parent = this; };

            bool is_representative() const { return ranked_in_tree::is_representative(this); }
            node *find() { return Find::find(this); }
            void link(node *q) { return ranked_in_tree::link(this, q); }
            bool unite(node *q) { return ranked_in_tree::unite(this, q); }
        };
    };
}

#endif