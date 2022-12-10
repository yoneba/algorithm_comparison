#ifndef CONCYCLIC_BASIC_HPP
#define CONCYCLIC_BASIC_HPP

#include <complex>
#include <limits>
#include <concepts>

namespace concyclic
{
    template <typename T>
    using point = std::complex<T>;

    template <typename T>
    inline bool same_arg(const point<T> &a, const point<T> &b)
    {
        return a.real() * b.imag() == a.imag() * b.real();
    }

    template <typename T>
    inline bool on_line(const point<T> &a, const point<T> &b, const point<T> &c)
    {
        return same_arg((a - c), (b - c));
    }

    template <typename T>
    inline bool on_circle(const point<T> &a, const point<T> &b, const point<T> &c, const point<T> &d)
    {
        return same_arg((a - c) * (b - d), (a - d) * (b - c));
    }

    template <typename T>
    inline long long search_all_patterns(const point<T> points[], const int n)
    {
        long long discovered = 0;
        for (int i = 3; i < n; i++)
            for (int j = 2; j < i; j++)
                for (int k = 1; k < j; k++)
                    for (int l = 0; l < k; l++)
                        if (on_circle(points[i], points[j], points[k], points[l]))
                            ++discovered;
        return discovered;
    }
}

#endif
