#ifndef QUICK_SORT_HPP
#define QUICK_SORT_HPP

#include "insertion_sort.hpp"
#include "heap_sort.hpp"

#include <utility>
#include <functional>
#include <format>
#if MEASUREMENT
#include <iostream>
#endif

namespace sorting
{
    class quick
    {
    public:
        static constexpr std::string name()
        {
            return "quick";
        }
#if MEASUREMENT
        static inline struct measurements
        {
            long long comparison = 0;
            long long item_moved = 0;
            long long partition_called = 0;
            long long sort_called = 0;
            long long partial_sort_called = 0;

            void print(const char *prefix = "") const
            {
                std::cout << prefix << sort_called << " times" << std::endl;
                std::cout << prefix << "comparison = " << comparison << std::endl;
                std::cout << prefix << "item_moved = " << item_moved << std::endl;
                std::cout << prefix << "partition_called = " << partition_called << std::endl;
            }

            void println(const char *prefix = "") const
            {
                print(prefix);
                std::cout << std::endl;
            }
        } measurements;
#endif

        template <typename T, class Predicate = std::less<T>>
        static T *partition(T *s, T *t, const Predicate &cmp = Predicate())
        {
            T *p = s + ((t - s) >> 1), pivot;
            p = cmp(*s, t[-1]) ? cmp(*s, *p) ? cmp(*p, t[-1]) ? p : t - 1 : s : cmp(*p, *s) ? cmp(t[-1], *p) ? p : t - 1
                                                                                            : s;
#if MEASUREMENT
            measurements.comparison += p == s ? 2 : 3;
#endif
            pivot = *p, *p = *s, p = s;
            while (true)
            {
#if MEASUREMENT
                ++measurements.comparison;
#endif
                while (cmp(*(++s), pivot))
                {
#if MEASUREMENT
                    ++measurements.comparison;
#endif
                }
#if MEASUREMENT
                ++measurements.comparison;
#endif
                while (cmp(pivot, *(--t)))
                {
#if MEASUREMENT
                    ++measurements.comparison;
#endif
                }
                if (t <= s)
                    [[unlikely]]
                {
#if MEASUREMENT
                    measurements.item_moved += 2;
#endif
                    *p = *t;
                    *t = pivot;
                    return t;
                }
#if MEASUREMENT
                measurements.item_moved += 2;
#endif
                std::swap(*s, *t);
            }
        }

        template <typename T, class Predicate = std::less<T>>
        static void split(T *s, T *w, T *t, const Predicate &cmp = Predicate())
        {
            while (s < w && w < t)
            {
                if (t - s == 2)
                {
#if MEASUREMENT
                    ++measurements.comparison;
#endif
                    if (cmp(*w, *s))
                    {
#if MEASUREMENT
                        measurements.item_moved += 2;
#endif
                        std::swap(*w, *s);
                    }
                    return;
                }
                T *p = partition(s, t, cmp);
                if (p < w)
                    s = p + 1;
                else
                    t = p;
            }
        }

        template <typename T, class Predicate = std::less<T>>
        static void sort(T *s, T *t, const Predicate &cmp = Predicate())
        {
#if MEASUREMENT
            ++measurements.sort_called;
#endif
            if (t - s <= 2)
            {
#if MEASUREMENT
                ++measurements.comparison;
#endif
                if (t - s == 2 && cmp(*t, *s))
                {
#if MEASUREMENT
                    measurements.item_moved += 2;
#endif
                    std::swap(*t, *s);
                }
                return;
            }
            T *p = partition(s, t, cmp);
            sort(s, p, cmp);
            sort(p + 1, t, cmp);
        }

        template <typename T, class Predicate = std::less<T>>
        static void partial_sort(T *s, T *w, T *t, const Predicate &cmp = Predicate())
        {
#if MEASUREMENT
            ++measurements.partial_sort_called;
#endif
            while (s < w && w < t)
            {
                T *p = partition(s, t, cmp);
                if (p < w)
                {
                    sort(s, p, cmp);
                    s = p + 1;
                }
                else
                    t = p;
            }
            if (s < w)
                sort(s, w, cmp);
        }
    };

    template <int ThresholdOfSwitching = 48>
    class intro
    {
    public:
        static constexpr std::string name()
        {
            return std::format("intro<{}>", ThresholdOfSwitching);
        }
#if MEASUREMENT
        static inline struct measurements
        {
            long long sort_called = 0;
            long long partial_sort_called = 0;

            measurements()
            {
                quick::measurements = quick::measurements::measurements();
                insertion::measurements = insertion::measurements::measurements();
                heap<>::measurements = heap<>::measurements::measurements();
            }

            void print(const char *prefix = "") const
            {
                std::cout << "[quick] " << sort_called << " times" << std::endl;
                quick::measurements.print(prefix);
                std::cout << "[insertion] " << insertion::measurements.sort_called << " times" << std::endl;
                insertion::measurements.print(prefix);
                std::cout << "[heap] " << heap<>::measurements.sort_called << " times" << std::endl;
                heap<>::measurements.print(prefix);
            }

            void println(const char *prefix = "") const
            {
                print(prefix);
                std::cout << std::endl;
            }
        } measurements;
#endif

        template <typename T, class Predicate = std::less<T>>
        static void sort(T *s, T *t, const Predicate &cmp = Predicate(), int hold = 48)
        {
#if MEASUREMENT
            ++measurements.sort_called;
#endif
            if (t - s < ThresholdOfSwitching)
                return insertion::sort(s, t, cmp);
            if (hold <= 0)
                [[unlikely]]
                return heap<>::sort(s, t, cmp);
            T *p = quick::partition(s, t, cmp);
            sort(s, p, cmp, hold - 1);
            sort(p + 1, t, cmp, hold - 1);
        }

        template <typename T, class Predicate = std::less<T>>
        static void partial_sort(T *s, T *w, T *t, const Predicate &cmp = Predicate(), int hold = 32)
        {
#if MEASUREMENT
            ++measurements.partial_sort_called;
#endif
            while (s < w && w < t)
            {
                if (hold-- == 0)
                    return heap<>::sort(s, w, t, cmp);
                T *p = quick::partition(s, t, cmp);
                if (p < w)
                {
                    sort(s, p, cmp, hold);
                    s = p + 1;
                }
                else
                    t = p;
            }
            if (s < w)
                sort(s, w, cmp, hold);
        }
    };
}

#endif
