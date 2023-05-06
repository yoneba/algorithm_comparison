#ifndef DISJOINT_SET_FIND_HPP
#define DISJOINT_SET_FIND_HPP

#if MEASUREMENT
#include "../../utility/common/operation_counting.hpp"
#endif

#include <string>

namespace disjoint_set
{
    class path_preserving
    {
    public:
        static constexpr std::string name() { return "path_preserving"; }
        static constexpr std::string full_name() { return "path_preserving"; }

#if MEASUREMENT
        static inline struct operation_counter : public common::operation_counter
        {
            virtual void print() { common::operation_counter::print({"find_called", "path_lifting"}); }
        } measurements;
#endif

        template <class Node>
        static Node *find(Node *q)
        {
#if MEASUREMENT
            ++measurements["find_called"];
#endif
            for (Node *p = q->parent; q != p; p = q->parent)
            {
#if MEASUREMENT
                ++measurements["path_lifting"];
#endif
                q = p;
            }
            return q;
        }
        static int find(int parent_of[], int q)
        {
#if MEASUREMENT
            ++measurements["find_called"];
#endif
            for (int p = parent_of[q]; q != p; p = parent_of[q])
            {
#if MEASUREMENT
                ++measurements["path_lifting"];
#endif
                q = p;
            }
            return q;
        }
    };

    class path_compression
    {
    public:
        static constexpr std::string name() { return "path_compression"; }
        static constexpr std::string full_name() { return "path_compression"; }

#if MEASUREMENT
        static inline struct operation_counter : public common::operation_counter
        {
            virtual void print() { common::operation_counter::print({"find_called", "path_lifting", "path_compaction"}); }
        } measurements;
#endif

        template <class Node>
        static Node *find(Node *q)
        {
#if MEASUREMENT
            ++measurements["find_called"];
#endif
            Node *r = q;
            for (Node *p = r->parent; r != p; p = r->parent)
            {
#if MEASUREMENT
                ++measurements["path_lifting"];
#endif
                r = p;
            }
            for (Node *p = q->parent; p != r; p = q->parent)
            {
#if MEASUREMENT
                ++measurements["path_lifting"];
                ++measurements["path_compaction"];
#endif
                q->parent = r;
                q = p;
            }
            return r;
        }
        static int find(int parent_of[], int q)
        {
#if MEASUREMENT
            ++measurements["find_called"];
#endif
            int r = q;
            for (int p = parent_of[r]; r != p; p = parent_of[r])
            {
#if MEASUREMENT
                ++measurements["path_lifting"];
#endif
                r = p;
            }
            for (int p = parent_of[q]; p != r; p = parent_of[q])
            {
#if MEASUREMENT
                ++measurements["path_lifting"];
                ++measurements["path_compaction"];
#endif
                parent_of[q] = r;
                q = p;
            }
            return r;
        }
    };

    class path_halving
    {
    public:
        static constexpr std::string name() { return "path_halving"; }
        static constexpr std::string full_name() { return "path_halving"; }

#if MEASUREMENT
        static inline struct operation_counter : public common::operation_counter
        {
            virtual void print() { common::operation_counter::print({"find_called", "path_lifting", "path_compaction"}); }
        } measurements;
#endif

        template <class Node>
        static Node *find(Node *q)
        {
#if MEASUREMENT
            ++measurements["find_called"];
#endif
            for (Node *p = q->parent; p != q; p = q->parent)
            {
#if MEASUREMENT
                ++measurements["path_lifting"];
#endif
                Node *r = p->parent;
                if (r == p)
                    return p;
#if MEASUREMENT
                ++measurements["path_lifting"];
                ++measurements["path_compaction"];
#endif
                q = q->parent = r;
            }
            return q;
        }
        static int find(int parent_of[], int q)
        {
#if MEASUREMENT
            ++measurements["find_called"];
#endif
            for (int p = parent_of[q]; p != q; p = parent_of[q])
            {
#if MEASUREMENT
                ++measurements["path_lifting"];
#endif
                int r = parent_of[p];
                if (r == p)
                    return p;
#if MEASUREMENT
                ++measurements["path_lifting"];
                ++measurements["path_compaction"];
#endif
                q = parent_of[q] = r;
            }
            return q;
        }
    };

    class path_halving_dull
    {
    public:
        static constexpr std::string name() { return "path_halving_dull"; }
        static constexpr std::string full_name() { return "path_halving_dull"; }

#if MEASUREMENT
        static inline struct operation_counter : public common::operation_counter
        {
            virtual void print() { common::operation_counter::print({"find_called", "path_lifting", "path_compaction"}); }
        } measurements;
#endif

        template <class Node>
        static Node *find(Node *q)
        {
#if MEASUREMENT
            ++measurements["find_called"];
#endif
            for (Node *p = q->parent; p != q; p = q->parent)
            {
#if MEASUREMENT
                measurements["path_lifting"] += 2;
                ++measurements["path_compaction"];
#endif
                q = q->parent = p->parent;
            }
            return q;
        }
        static int find(int parent_of[], int q)
        {
#if MEASUREMENT
            ++measurements["find_called"];
#endif
            for (int p = parent_of[q]; p != q; p = parent_of[q])
            {
#if MEASUREMENT
                measurements["path_lifting"] += 2;
                ++measurements["path_compaction"];
#endif
                q = parent_of[q] = parent_of[p];
            }
            return q;
        }
    };
}

#endif