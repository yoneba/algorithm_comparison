#ifndef SOTRING_INSERTION_SORT_HPP
#define SOTRING_INSERTION_SORT_HPP

#if MEASUREMENT
#include "../common/operation_counting.hpp"
#endif

#include <concepts>
#include <functional>
#include <string>

namespace sorting
{
    class insertion
    {
    public:
        static constexpr std::string name() { return "insertion"; }
        static constexpr std::string full_name() { return "insertion"; }
#if MEASUREMENT
        static inline struct operation_counter : public common::operation_counter
        {
            virtual void print() { common::operation_counter::print({"comparison", "item_moved"}); }
        } measurements;
#endif

        template <std::totally_ordered T, class Predicate = std::less<T>>
        static void sort(T *s, T *t, const Predicate &cmp = Predicate())
        {
#if MEASUREMENT
            ++measurements["sort_called"];
#endif
            for (T *p = s + 1; p < t; p++)
            {
#if MEASUREMENT
                ++measurements["comparison"];
#endif
                if (T tmp = *p; cmp(tmp, p[-1]))
                    [[likely]]
                {
                    T *q = p;
                    do
                    {
                        *q = q[-1];
                        --q;
#if MEASUREMENT
                        ++measurements["item_moved"];
                        measurements["comparison"] += q > s;
#endif
                    } while (q > s && cmp(tmp, q[-1]));
#if MEASUREMENT
                    ++measurements["item_moved"];
#endif
                    *q = tmp;
                }
            }
        }
    };

    class insertion_look_ahead
    {
    public:
        static constexpr std::string name() { return "insertion_look_ahead"; }
        static constexpr std::string full_name() { return "insertion_look_ahead"; }
#if MEASUREMENT
        static inline struct operation_counter : public common::operation_counter
        {
            virtual void print() { common::operation_counter::print({"comparison", "item_moved"}); }
        } measurements;
#endif

        template <std::totally_ordered T, class Predicate = std::less<T>>
        static void sort(T *s, T *t, const Predicate &cmp = Predicate())
        {
#if MEASUREMENT
            ++measurements["sort_called"];
#endif
            for (T *p = s + 1; p < t; p++)
            {
#if MEASUREMENT
                ++measurements["comparison"];
#endif
                if (T tmp = *p; cmp(tmp, p[-1]))
                    [[likely]]
                {
                    T *q = p;
#if MEASUREMENT
                    ++measurements["comparison"];
#endif
                    if (cmp(tmp, *s))
                        [[unlikely]]
                        do
                        {
                            *q = q[-1];
#if MEASUREMENT
                            ++measurements["item_moved"];
#endif
                        } while (--q > s);
                    else
                        [[likely]]
                        do
                        {
                            *q = q[-1];
#if MEASUREMENT
                            ++measurements["item_moved"];
                            ++measurements["comparison"];
#endif
                        } while (cmp(tmp, (--q)[-1]));
#if MEASUREMENT
                    ++measurements["item_moved"];
#endif
                    *q = tmp;
                }
            }
        }
    };

    class insertion_after_bubbling
    {
    public:
        static constexpr std::string name() { return "insertion_after_bubbling"; }
        static constexpr std::string full_name() { return "insertion_after_bubbling"; }
#if MEASUREMENT
        static inline struct operation_counter : public common::operation_counter
        {
            virtual void print() { common::operation_counter::print({"comparison", "item_moved"}); }
        } measurements;
#endif

        template <std::totally_ordered T, class Predicate = std::less<T>>
        static void sort(T *s, T *t, const Predicate &cmp = Predicate())
        {
#if MEASUREMENT
            ++measurements["sort_called"];
#endif
            for (T *p = t - 1; p > s; p--)
            {
#if MEASUREMENT
                ++measurements["comparison"];
#endif
                if (T tmp = *p; cmp(tmp, p[-1]))
                {
                    do
                    {
                        *p = p[-1];
                        --p;
#if MEASUREMENT
                        ++measurements["item_moved"];
                        ++measurements["comparison"];
#endif
                    } while (p > s && cmp(tmp, p[-1]));
#if MEASUREMENT
                    ++measurements["item_moved"];
#endif
                    *p = tmp;
                }
            }
            for (T *p = s + 2; p < t; p++)
            {
#if MEASUREMENT
                ++measurements["comparison"];
#endif
                if (T tmp = *p; cmp(tmp, p[-1]))
                    [[likely]]
                {
                    T *q = p;
                    do
                    {
                        *q = q[-1];
                        --q;
#if MEASUREMENT
                        ++measurements["item_moved"];
                        ++measurements["comparison"];
#endif
                    } while (cmp(tmp, q[-1]));
#if MEASUREMENT
                    ++measurements["item_moved"];
#endif
                    *q = tmp;
                }
            }
        }
    };
}

#endif
