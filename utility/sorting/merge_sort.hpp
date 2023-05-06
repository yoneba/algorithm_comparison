#ifndef SOTRING_MERGE_SORT_HPP
#define SOTRING_MERGE_SORT_HPP

#if MEASUREMENT
#include "../common/operation_counting.hpp"
#endif

#include "insertion_sort.hpp"

#include <utility>
#include <functional>
#include <format>

namespace sorting
{
    template <int ThresholdOfSwitching = 48>
    class merge
    {
    public:
        static constexpr std::string name() { return "merge"; }
        static constexpr std::string full_name()
        {
            return std::format("merge<{}>", ThresholdOfSwitching);
        }
#if MEASUREMENT
        static inline struct operation_counter : public common::operation_counter
        {
            void clear()
            {
                common::operation_counter::clear();
                insertion::measurements.clear();
            }
            virtual void print()
            {
                std::cout << "[merge] " << (*this)["sort_called"] << " times" << std::endl;
                common::operation_counter::print({"comparison", "item_moved", "combine_called"});
                std::cout << "[insertion] " << insertion::measurements["sort_called"] << " times" << std::endl;
                insertion::measurements.print();
            }
        } measurements;
#endif

        template <typename T, class Predicate = std::less<T>>
        static void combine(T *s1, T *t1, T *s2, T *t2, T *res, const Predicate &cmp = Predicate())
        {
#if MEASUREMENT
            ++measurements["combine_called"];
            ++measurements["comparison"];
#endif
            if (cmp(t2[-1], t1[-1]))
                std::swap(s1, s2), std::swap(t1, t2);
            do
            {
                *res++ = cmp(*s2, *s1) ? *s2++ : *s1++;
#if MEASUREMENT
                ++measurements["comparison"];
                ++measurements["item_moved"];
#endif
            } while (s1 < t1);
            if (res != s2)
                do
                {
                    *res++ = *s2++;
#if MEASUREMENT
                    ++measurements["item_moved"];
#endif
                } while (s2 < t2);
        }

        template <typename T, class Predicate = std::less<T>>
        static void sort(T *s, T *t, T *temp, const Predicate &cmp = Predicate())
        {
#if MEASUREMENT
            ++measurements["sort_called"];
#endif
            int half = static_cast<int>(t - s + 1) >> 1, quarter = half >> 1;
            if (quarter < ThresholdOfSwitching >> 2)
                return insertion::sort(s, t, cmp);
            sort(t - half, t - quarter, temp, cmp);
            sort(t - quarter, t, temp, cmp);
            combine(t - half, t - quarter, t - quarter, t, temp, cmp);
            sort(s, s + quarter, t - half, cmp);
            sort(s + quarter, t - half, t - half, cmp);
            combine(s, s + quarter, s + quarter, t - half, s + half, cmp);
            combine(temp, temp + half, s + half, t, s, cmp);
        }

        template <typename T, class Predicate = std::less<T>>
        static void sort(T *s, T *t, const Predicate &cmp = Predicate())
        {
#if MEASUREMENT
            ++measurements["sort_called"];
#endif
            T *temp = new T[(t - s + 1) >> 1];
            sort(s, t, temp, cmp);
            delete[] temp;
        }
    };
}

#endif
