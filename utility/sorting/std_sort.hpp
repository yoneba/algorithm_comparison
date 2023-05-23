#ifndef SOTRING_STD_SORT_HPP
#define SOTRING_STD_SORT_HPP

#if MEASUREMENT
#include "../common/operation_counting.hpp"
#endif

#include <concepts>
#include <functional>
#include <algorithm>
#include <string>

namespace sorting
{
    class stl
    {
    public:
        static constexpr std::string name() { return "stl"; }
        static constexpr std::string full_name() { return "stl"; }
#if MEASUREMENT
        static inline struct operation_counter : public common::operation_counter
        {
            virtual void print() { std::cout << "Cannot measure operation counts." << std::endl; }
        } measurements;
#endif

        template <std::totally_ordered T, class Predicate = std::less<T>>
        static void sort(T *s, T *t, const Predicate &cmp = Predicate())
        {
#if MEASUREMENT
            ++measurements["sort_called"];
#endif
            std::sort(s, t, cmp);
        }

        template <std::totally_ordered T, class Predicate = std::less<T>>
        static void partial_sort(T *s, T *w, T *t, const Predicate &cmp = Predicate())
        {
#if MEASUREMENT
            ++measurements["partial_sort_called"];
#endif
            std::partial_sort(s, w, t, cmp);
        }
    };
}

#endif
