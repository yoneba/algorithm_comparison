#ifndef PRIORITY_QUEUE_D_ARY_HEAP_HPP
#define PRIORITY_QUEUE_D_ARY_HEAP_HPP

#if MEASUREMENT
#include "../../utility/common/operation_counting.hpp"
#endif

#include <concepts>
#include <string>
#include <format>
#include <algorithm>

namespace priority_queue
{
    template <template <typename> class PR, std::totally_ordered T, int Degree = 0>
    class d_ary_heap
    {
        struct item
        {
            int id;
            T key;
        };

        static constexpr PR<T> cmp;
        item *heap;
        int *track;
        int n_active_item;
        const int n_item;
        const int degree;

        d_ary_heap(const d_ary_heap &) = delete;
        d_ary_heap &operator=(const d_ary_heap &) = delete;

    protected:
        static constexpr int first_child_of(int p, int d)
        {
            if constexpr (Degree > 1)
                return p * Degree + 1;
            else
                return p * d + 1;
        }
        static constexpr int parent_of(int c, int d)
        {
            if constexpr (Degree > 1)
                return (c - 1) / Degree;
            else
                return (c - 1) / d;
        }

        static int finest_sibling_among(item *h, int n, int d, int c)
        {
            if constexpr (Degree == 2)
            {
                c += c + 1 < n && cmp(h[c + 1].key, h[c].key);
            }
            else if (Degree == 3)
            {
                int second = c + 1, third = c + 2;
                if (second < n && cmp(h[second].key, h[c].key))
                    c = second;
                if (third < n && cmp(h[third].key, h[c].key))
                    c = third;
            }
            else if (Degree == 4)
            {
                int second = c + 1, third = c + 2, fourth = c + 3;
                if (second < n && cmp(h[second].key, h[c].key))
                    c = second;
                if (third < n && cmp(h[third].key, h[c].key))
                    c = third;
                if (fourth < n && cmp(h[fourth].key, h[c].key))
                    c = fourth;
            }
            else if (Degree == 5)
            {
                int second = c + 1, third = c + 2, fourth = c + 3, fifth = c + 4;
                if (second < n && cmp(h[second].key, h[c].key))
                    c = second;
                if (third < n && cmp(h[third].key, h[c].key))
                    c = third;
                if (fourth < n && cmp(h[fourth].key, h[c].key))
                    c = fourth;
                if (fifth < n && cmp(h[fifth].key, h[c].key))
                    c = fifth;
            }
            else
            {
                if constexpr (Degree > 1)
                    d = Degree;
                int t = std::min(n, c + d);
                for (int q = c + 1; q < t; q++)
                    if (cmp(h[q].key, h[c].key))
                        c = q;
            }
            return c;
        }

        void sift_down(int id, T key, int pos = 0) const
        {
#if MEASUREMENT
            ++measurements["sift_down_called"];
#endif
            for (int child = first_child_of(pos, degree); child < n_active_item; pos = child, child = first_child_of(pos, degree))
            {
#if MEASUREMENT
                measurements["comparison_in_sift_down"] += std::min(n_active_item - child, degree);
#endif
                child = finest_sibling_among(heap, n_active_item, degree, child);
                item competitor = heap[child];
                if (!cmp(competitor.key, key))
                    break;
#if MEASUREMENT
                ++measurements["move_down"];
#endif
                heap[pos] = competitor, track[competitor.id] = pos;
            }
            heap[pos].id = id, heap[pos].key = key, track[id] = pos;
        }

        void sift_up(int id, T key, int pos) const
        {
#if MEASUREMENT
            ++measurements["sift_up_called"];
            ++measurements["comparison_in_sift_up"];
#endif
            for (int parent = parent_of(pos, degree); pos > 0 && cmp(key, heap[parent].key); pos = parent, parent = parent_of(pos, degree))
            {
#if MEASUREMENT
                ++measurements["move_up"];
                ++measurements["comparison_in_sift_up"];
#endif
                heap[pos] = heap[parent], track[heap[pos].id] = pos;
            }
            heap[pos].id = id, heap[pos].key = key, track[id] = pos;
        }

    public:
        static constexpr std::string name() { return "d_ary_heap"; }
        static constexpr std::string full_name()
        {
            if constexpr (Degree > 1)
                return std::format("d_ary_heap<{}>", Degree);
            else
                return "d_ary_heap<dynamic>";
        }
#if MEASUREMENT
        static inline struct operation_counter : public common::operation_counter
        {
            virtual void print()
            {
                common::operation_counter::print({"sift_down_called",
                                                  "move_down",
                                                  "sift_up_called",
                                                  "move_up",
                                                  "comparison_in_sift_down",
                                                  "comparison_in_sift_up",
                                                  "insert_called",
                                                  "improve_key_called",
                                                  "worsen_key_called",
                                                  "remove_top_called",
                                                  "remove_called"});
            }
        } measurements;
#endif
        explicit d_ary_heap(int n, int d) : heap(new item[n]), track(new int[n]), n_active_item(0), n_item(n), degree(d)
        {
#if MEASUREMENT
            ++measurements["constructor_called"];
#endif
            for (int i = 0; i < n; i++)
                track[i] = -1;
        }
        const T &operator[](int id) const { return heap[track[id]].key; }

        bool empty() const { return n_active_item == 0; }
        int size() const { return n_active_item; }
        int capacity() const { return n_item; }
        int top() const { return empty() ? -1 : heap[0].id; }
        bool contains(int id) const { return track[id] != -1; }

        void clear()
        {
            for (int i = 0; i < n_item; i++)
                track[i] = -1;
            n_active_item = 0;
        }

        void insert(int id, T key)
        {
#if MEASUREMENT
            ++measurements["insert_called"];
#endif
            sift_up(id, key, n_active_item++);
        }

        void improve_key(int id, T key) const
        {
#if MEASUREMENT
            ++measurements["improve_key_called"];
#endif
            sift_up(id, key, track[id]);
        }

        void worsen_key(int id, T key) const
        {
#if MEASUREMENT
            ++measurements["worsen_key_called"];
#endif
            sift_down(id, key, track[id]);
        }

        void remove_top()
        {
#if MEASUREMENT
            ++measurements["remove_top_called"];
#endif
            remove(heap[0].id);
        }

        void remove(int id)
        {
#if MEASUREMENT
            ++measurements["remove_called"];
#endif
            auto [new_id, new_key] = heap[--n_active_item];
            sift_down(new_id, new_key, track[id]);
            track[id] = -1;
        }

        ~d_ary_heap()
        {
            delete[] heap;
            delete[] track;
        }
    };

    template <template <typename> class PR, std::totally_ordered T>
    using binary_heap = d_ary_heap<PR, T, 2>;
    template <template <typename> class PR, std::totally_ordered T>
    using ternary_heap = d_ary_heap<PR, T, 3>;
    template <template <typename> class PR, std::totally_ordered T>
    using quaternary_heap = d_ary_heap<PR, T, 4>;
    template <template <typename> class PR, std::totally_ordered T>
    using quinary_heap = d_ary_heap<PR, T, 5>;
    template <template <typename> class PR, std::totally_ordered T>
    using senary_heap = d_ary_heap<PR, T, 6>;
    template <template <typename> class PR, std::totally_ordered T>
    using septenary_heap = d_ary_heap<PR, T, 7>;
    template <template <typename> class PR, std::totally_ordered T>
    using octonary_heap = d_ary_heap<PR, T, 8>;
    template <template <typename> class PR, std::totally_ordered T>
    using novenary_heap = d_ary_heap<PR, T, 9>;
    template <template <typename> class PR, std::totally_ordered T>
    using denary_heap = d_ary_heap<PR, T, 10>;
    template <template <typename> class PR, std::totally_ordered T>
    using multiary_heap = d_ary_heap<PR, T>;
}

#endif
