#ifndef SOTRING_QUICK_SORT_HPP
#define SOTRING_QUICK_SORT_HPP

#if MEASUREMENT
#include "../common/operation_counting.hpp"
#endif

#include "insertion_sort.hpp"
#include "heap_sort.hpp"

#include <utility>
#include <functional>
#include <format>

namespace sorting
{
    class quick
    {
    public:
        static constexpr std::string name() { return "quick"; }
        static constexpr std::string full_name() { return "quick"; }
#if MEASUREMENT
        static inline struct operation_counter : public common::operation_counter
        {
            virtual void print() { common::operation_counter::print({"comparison", "item_moved", "partition_called"}); }
        } measurements;
#endif

        template <typename T, class Predicate = std::less<T>>
        static T *partition(T *s, T *t, const Predicate &cmp = Predicate())
        {
#if MEASUREMENT
            ++measurements["partition_called"];
#endif
            T *p = s + ((t - s) >> 1), pivot;
            p = cmp(*s, t[-1]) ? cmp(*s, *p) ? cmp(*p, t[-1]) ? p : t - 1 : s : cmp(*p, *s) ? cmp(t[-1], *p) ? p : t - 1
                                                                                            : s;
#if MEASUREMENT
            measurements["comparison"] += p == s ? 2 : 3;
#endif
            pivot = *p, *p = *s, p = s;
            while (true)
            {
#if MEASUREMENT
                ++measurements["comparison"];
#endif
                while (cmp(*(++s), pivot))
                {
#if MEASUREMENT
                    ++measurements["comparison"];
#endif
                }
#if MEASUREMENT
                ++measurements["comparison"];
#endif
                while (cmp(pivot, *(--t)))
                {
#if MEASUREMENT
                    ++measurements["comparison"];
#endif
                }
                if (t <= s)
                    [[unlikely]]
                {
#if MEASUREMENT
                    measurements["item_moved"] += 2;
#endif
                    *p = *t;
                    *t = pivot;
                    return t;
                }
#if MEASUREMENT
                measurements["item_moved"] += 2;
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
                    ++measurements["comparison"];
#endif
                    if (cmp(*w, *s))
                    {
#if MEASUREMENT
                        measurements["item_moved"] += 2;
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
            ++measurements["sort_called"];
#endif
            if (t - s <= 2)
            {
#if MEASUREMENT
                measurements["comparison"] += t - s == 2;
#endif
                if (t - s == 2 && cmp(*t, *s))
                {
#if MEASUREMENT
                    measurements["item_moved"] += 2;
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
            ++measurements["partial_sort_called"];
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
        static constexpr std::string name() { return "intro"; }
        static constexpr std::string full_name()
        {
            return std::format("intro<{}>", ThresholdOfSwitching);
        }
#if MEASUREMENT
        static inline struct operation_counter : public common::operation_counter
        {
            void clear()
            {
                common::operation_counter::clear();
                quick::measurements.clear();
                insertion::measurements.clear();
                heap<>::measurements.clear();
            }
            virtual void print()
            {
                std::cout << "[quick] " << (*this)["sort_called"] << " times" << std::endl;
                quick::measurements.print();
                std::cout << "[insertion] " << insertion::measurements["sort_called"] << " times" << std::endl;
                insertion::measurements.print();
                std::cout << "[heap] " << heap<>::measurements["sort_called"] << " times" << std::endl;
                heap<>::measurements.print();
            }
        } measurements;
#endif

        template <typename T, class Predicate = std::less<T>>
        static void sort(T *s, T *t, const Predicate &cmp = Predicate(), int hold = 48)
        {
#if MEASUREMENT
            ++measurements["sort_called"];
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
            ++measurements["partial_sort_called"];
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
