#ifndef SOTRING_STD_SORT_HPP
#define SOTRING_STD_SORT_HPP

#include <functional>
#include <algorithm>
#include <string>
#if MEASUREMENT
#include <iostream>
#endif

namespace sorting
{
    class stl
    {
    public:
        static constexpr std::string name()
        {
            return "stl";
        }
#if MEASUREMENT
        static inline struct measurements
        {
            long long sort_called = 0;
            long long partial_sort_called = 0;

            void print(const char *prefix = "") const
            {
                std::cout << prefix << "Cannot measure operation counts." << std::endl;
            }

            void println(const char *prefix = "") const
            {
                print(prefix);
                std::cout << std::endl;
            }
        } measurements;
#endif

        template <typename T, class Predicate = std::less<T>>
        static void sort(T *s, T *t, const Predicate &cmp = Predicate())
        {
#if MEASUREMENT
            ++measurements.sort_called;
#endif
            std::sort(s, t, cmp);
        }

        template <typename T, class Predicate = std::less<T>>
        static void partial_sort(T *s, T *w, T *t, const Predicate &cmp = Predicate())
        {
#if MEASUREMENT
            ++measurements.partial_sort_called;
#endif
            std::partial_sort(s, w, t, cmp);
        }
    };
}

#endif
