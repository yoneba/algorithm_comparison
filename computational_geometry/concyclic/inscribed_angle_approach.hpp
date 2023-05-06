#ifndef CONCYCLIC_INSCRIBED_ANGLE_APPROACH_HPP
#define CONCYCLIC_INSCRIBED_ANGLE_APPROACH_HPP

#include "basic.hpp"
#include "../../utility/sorting/index.hpp"

#include <format>
#include <concepts>
#if MEASUREMENT
#include <iostream>
#endif

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
        static inline struct measurements
        {
            void reset()
            {
                Sorting::measurements.reset();
            }

            void print(const char *prefix = "") const
            {
                std::cout << prefix << "sort_called = " << Sorting::measurements.sort_called << std::endl;
                Sorting::measurements.print(prefix);
            }

            void println(const char *prefix = "") const
            {
                print(prefix);
                std::cout << std::endl;
            }
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
        static inline struct measurements
        {
            long long comparison = 0;
            long long item_moved = 0;
            long long selection_executed = 0;

            void reset()
            {
                *this = measurements();
            }

            void print(const char *prefix = "") const
            {
                std::cout << prefix << "comparison = " << comparison << std::endl;
                std::cout << prefix << "item_moved = " << item_moved << std::endl;
                std::cout << prefix << "selection_executed = " << selection_executed << std::endl;
            }

            void println(const char *prefix = "") const
            {
                print(prefix);
                std::cout << std::endl;
            }
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
                    ++measurements.selection_executed;
#endif
                    for (T *tail = tangents + j - 1; tail > tangents; tail--)
                    {
                        T angle = *tail, *org = tail + 1;
                        for (T *head = tangents; head < tail; head++)
                        {
#if MEASUREMENT
                            ++measurements.comparison;
#endif
                            if (*head == angle)
                            {
#if MEASUREMENT
                                ++measurements.item_moved;
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
