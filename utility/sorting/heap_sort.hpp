#ifndef SOTRING_HEAP_SORT_HPP
#define SOTRING_HEAP_SORT_HPP

#include <functional>
#include <format>
#if MEASUREMENT
#include <iostream>
#endif

namespace sorting
{
    enum struct sift_down_strategy
    {
        normal,
        bounce,
    };

    constexpr std::string to_string(sift_down_strategy sift_down_strategy)
    {
        switch (sift_down_strategy)
        {
        case sift_down_strategy::normal:
            return "normal";
        case sift_down_strategy::bounce:
            return "bounce";
        default:
            return "";
        }
    }

    template <int Degree = 3, sift_down_strategy SiftDownStrategy = sift_down_strategy::normal>
    class heap
    {
    public:
        static constexpr std::string name()
        {
            return std::format("heap<{}, {}>", Degree, to_string(SiftDownStrategy));
        }
#if MEASUREMENT
        static inline struct measurements
        {
            long long comparison = 0;
            long long item_moved = 0;
            long long sift_up_called = 0;
            long long sift_down_called = 0;
            long long heapify_called = 0;
            long long sort_called = 0;
            long long partial_sort_called = 0;

            void reset()
            {
                *this = measurements();
            }

            void print(const char *prefix = "") const
            {
                std::cout << prefix << "comparison = " << comparison << std::endl;
                std::cout << prefix << "item_moved = " << item_moved << std::endl;
                std::cout << prefix << "sift_up_called = " << sift_up_called << std::endl;
                std::cout << prefix << "sift_down_called = " << sift_down_called << std::endl;
                std::cout << prefix << "heapify_called = " << heapify_called << std::endl;
            }

            void println(const char *prefix = "") const
            {
                print(prefix);
                std::cout << std::endl;
            }
        } measurements;
#endif

        static constexpr int first_child_of(int p)
        {
            return p * Degree - 1;
        }
        static constexpr int parent_of(int c)
        {
            return (c + 1) / Degree;
        }

        /**
         *
         * @param h h[0] is the top of the heap.
         * @param n non positive; h[n + 1]..h[0] comprises a heap.
         * @param c non positive; the index of the first sibling.
         * @param cmp a comparison function.
         */
        template <typename T, class Predicate = std::less<T>>
        static int finest_sibling_among(T *h, int n, int c, const Predicate &cmp = Predicate())
        {
#if MEASUREMENT
            measurements.comparison += Degree - 1;
#endif
            if constexpr (Degree == 2)
            {
                c -= c - 1 > n && cmp(h[c - 1], h[c]);
            }
            else if (Degree == 3)
            {
                int second = c - 1, third = c - 2;
                if (second > n && cmp(h[second], h[c]))
                    c = second;
                if (third > n && cmp(h[third], h[c]))
                    c = third;
            }
            else if (Degree == 4)
            {
                int second = c - 1, third = c - 2, fourth = c - 3;
                if (second > n && cmp(h[second], h[c]))
                    c = second;
                if (third > n && cmp(h[third], h[c]))
                    c = third;
                if (fourth > n && cmp(h[fourth], h[c]))
                    c = fourth;
            }
            else if (Degree == 5)
            {
                int second = c - 1, third = c - 2, fourth = c - 3, fifth = c - 4;
                if (second > n && cmp(h[second], h[c]))
                    c = second;
                if (third > n && cmp(h[third], h[c]))
                    c = third;
                if (fourth > n && cmp(h[fourth], h[c]))
                    c = fourth;
                if (fifth > n && cmp(h[fifth], h[c]))
                    c = fifth;
            }
            else
            {
                int t = std::max(n, c - Degree);
                for (int q = c - 1; q > t; q--)
                    if (cmp(h[q], h[c]))
                        c = q;
            }
            return c;
        }

        /**
         *
         * @param h h[0] is the top of the heap.
         * @param n non positive; the position where the new element will firstly be placed at.
         * @param a an element to be inserted.
         * @param cmp a comparison function.
         */
        template <typename T, class Predicate = std::less<T>>
        static void sift_up(T *h, int c, T a, const Predicate &cmp = Predicate())
        {
#if MEASUREMENT
            ++measurements.sift_up_called;
            ++measurements.comparison;
#endif
            for (int p = parent_of(c); c < 0 && cmp(a, h[p]); c = p, p = parent_of(c))
            {
#if MEASUREMENT
                ++measurements.item_moved;
                ++measurements.comparison;
#endif
                h[c] = h[p];
            }
#if MEASUREMENT
            ++measurements.item_moved;
#endif
            h[c] = a;
        }

        template <typename T, class Predicate = std::less<T>>
        static void sift_down_normal(T *h, int n, T a, const Predicate &cmp = Predicate(), int r = 0)
        {
            for (int c = first_child_of(r); c > n; r = c, c = first_child_of(r))
            {
                c = finest_sibling_among(h, n, c, cmp);
#if MEASUREMENT
                ++measurements.comparison;
#endif
                if (!cmp(h[c], a))
                    break;
#if MEASUREMENT
                ++measurements.item_moved;
#endif
                h[r] = h[c];
            }
#if MEASUREMENT
            ++measurements.item_moved;
#endif
            h[r] = a;
        }

        template <typename T, class Predicate = std::less<T>>
        static void sift_down_with_bounce(T *h, int n, T a, const Predicate &cmp = Predicate(), int r = 0)
        {
            for (int c = first_child_of(r); c > n; r = c, c = first_child_of(r))
            {
                c = finest_sibling_among(h, n, c, cmp);
#if MEASUREMENT
                ++measurements.item_moved;
#endif
                h[r] = h[c];
            }
            sift_up(h, r, a, cmp);
        }

        /**
         *
         * @param h h[0] is the top of the heap.
         * @param n non positive; h[n + 1]..h[0] comprises a heap.
         * @param a an element regarded as being placed at h[r].
         * @param cmp a comparison function.
         * @param r non positive; the root index to apply sift_down.
         */
        template <typename T, class Predicate = std::less<T>>
        static void sift_down(T *h, int n, T a, const Predicate &cmp = Predicate(), int r = 0)
        {
#if MEASUREMENT
            ++measurements.sift_down_called;
#endif
            if constexpr (SiftDownStrategy == sift_down_strategy::bounce)
                sift_down_with_bounce(h, n, a, cmp, r);
            else
                sift_down_normal(h, n, a, cmp, r);
        }

        /**
         *
         * @param h h[0] is the top of the heap.
         * @param n non positive; h[n + 1]..h[0] comprises a heap.
         * @param cmp a comparison function.
         */
        template <typename T, class Predicate = std::less<T>>
        static void heapify(T *h, int n, const Predicate &cmp = Predicate())
        {
#if MEASUREMENT
            ++measurements.heapify_called;
#endif
            for (int i = parent_of(n + 1); i <= 0; i++)
                sift_down(h, n, h[i], cmp, i);
        }

        template <typename T, class Predicate = std::less<T>>
        static void partial_sort(T *s, T *w, T *t, const Predicate &cmp = Predicate())
        {
#if MEASUREMENT
            ++measurements.partial_sort_called;
#endif
            heapify(t - 1, s - t, cmp);
            while (++s <= w)
            {
                T tmp = t[-1];
                sift_down(t - 1, s - t, s[-1], cmp);
                s[-1] = tmp;
            }
        }

        template <typename T, class Predicate = std::less<T>>
        static void sort(T *s, T *t, const Predicate &cmp = Predicate())
        {
#if MEASUREMENT
            ++measurements.sort_called;
#endif
            heapify(t - 1, s - t, cmp);
            while (++s < t)
            {
                T tmp = t[-1];
                sift_down(t - 1, s - t, s[-1], cmp);
                s[-1] = tmp;
            }
        }
    };
}

#endif
