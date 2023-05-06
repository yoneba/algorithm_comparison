#ifndef CONCYCLIC_INSCRIBED_ANGLE_APPROACH_HPP
#define CONCYCLIC_INSCRIBED_ANGLE_APPROACH_HPP

#if MEASUREMENT
#include "../../utility/common/operation_counting.hpp"
#endif

#include "basic.hpp"
#include "../../utility/sorting/index.hpp"

#include <format>
#include <concepts>

namespace concyclic
{
    template <std::floating_point T>
    inline T find_tan_inscribed_angle(const point<T> &a, const point<T> &b, const point<T> &c)
    {
        point<T> p = c - a, q = c - b;
        T x = p.real() * q.real() + p.imag() * q.imag(), y = p.imag() * q.real() - p.real() * q.imag();
        return x == 0 ? std::numeric_limits<T>::infinity() : y / x;
    }

    template <class Sorting = sorting::intro<>>
    class inscribed_angle_sorting
    {
    public:
        static constexpr std::string name() { return "inscribed_angle_sorting"; }
        static constexpr std::string full_name()
        {
            return std::format("inscribed_angle_sorting<{}>", Sorting::full_name());
        }
#if MEASUREMENT
        static inline struct operation_counter : public common::operation_counter
        {
            void clear()
            {
                common::operation_counter::clear();
                Sorting::measurements.clear();
            }
            virtual void print() { Sorting::measurements.print(); }
        } measurements;
#endif

        template <std::floating_point T>
        static long long scan(T tangents[], int n)
        {
            long long counter = 0;
            tangents[n] = -std::numeric_limits<T>::infinity();
            for (int k = 1; k < n; k++)
            {
                if (tangents[k - 1] == tangents[k] && tangents[k + 1] != tangents[k])
                {
                    if (k == 1 || tangents[k - 2] != tangents[k])
                        ++counter;
                    else if (k == 2 || tangents[k - 3] != tangents[k])
                        --counter;
                }
            }
            return counter;
        }

        template <std::floating_point T>
        static long long count(const point<T> points[], int n_points)
        {
            long long counter = 0;
            T *tangents = new T[n_points - 1];
            for (int i = 3; i < n_points; i++)
            {
                for (int j = 2; j < i; j++)
                {
                    for (int k = 0; k < j; k++)
                        tangents[k] = find_tan_inscribed_angle(points[i], points[j], points[k]);
                    Sorting::sort(tangents, tangents + j);
                    counter += scan(tangents, j);
                }
            }
            delete[] tangents;
            return counter;
        }
    };

    class inscribed_angle_selection
    {
    public:
        static constexpr std::string name() { return "inscribed_angle_selection"; }
        static constexpr std::string full_name() { return "inscribed_angle_selection"; }
#if MEASUREMENT
        static inline struct operation_counter : public common::operation_counter
        {
            virtual void print() { common::operation_counter::print({"comparison", "item_moved", "selection_executed"}); }
        } measurements;
#endif

        template <std::floating_point T>
        static long long count(const point<T> points[], int n_points)
        {
            long long counter = 0;
            T *tangents = new T[n_points - 1];
            for (int i = 3; i < n_points; i++)
            {
                for (int j = 2; j < i; j++)
                {
                    for (int k = 0; k < j; k++)
                        tangents[k] = find_tan_inscribed_angle(points[i], points[j], points[k]);
#if MEASUREMENT
                    ++measurements["selection_executed"];
#endif
                    for (T *tail = tangents + j - 1; tail > tangents; tail--)
                    {
                        T angle = *tail, *org = tail + 1;
                        for (T *head = tangents; head < tail; head++)
                        {
#if MEASUREMENT
                            ++measurements["comparison"];
#endif
                            if (*head == angle)
                            {
#if MEASUREMENT
                                ++measurements["item_moved"];
#endif
                                *(head--) = *(--tail);
                            }
                        }
                        if (org - tail == 2)
                            ++counter;
                        if (org - tail == 3)
                            --counter;
                    }
                }
            }
            delete[] tangents;
            return counter;
        }
    };
}

#endif
