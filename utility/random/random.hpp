#ifndef RANDOM_HPP
#define RANDOM_HPP

#include "../sorting/merge_sort.hpp"

#include <concepts>
#include <random>
#include <functional>
#include <string>

namespace random
{
    template <std::integral T>
    inline T rand(T n)
    {
        static std::mt19937_64 mt(std::random_device{}());
        std::uniform_int_distribution<T> uid(0, n - 1);
        return uid(mt);
    }

    class rough
    {
    public:
        static constexpr std::string name()
        {
            return "rough";
        }

        template <std::integral T>
        static void sampling(T n, T k, T result[])
        {
            static std::mt19937_64 mt(std::random_device{}());
            std::uniform_real_distribution<> urd;
            while (k--)
                result[k] = n = pow(urd(mt), 1.0 / (k + 1)) * n;
        }
    };

    class shuffle
    {
    public:
        static constexpr std::string name()
        {
            return "shuffle";
        }

        template <std::integral T>
        static void sampling(T n, T k, T result[])
        {
            static std::mt19937_64 mt(std::random_device{}());
            std::pair<T, T> *rnd = new std::pair<T, T>[k];
            for (T i = 0; i < k; i++)
                rnd[i] = std::make_pair(static_cast<T>(mt() % (n - k + i + 1)), n - k + i);
            T j = k;
            for (T i = k - 1; i >= 0; i--)
            {
                if (rnd[i].first < n - k)
                    rnd[--j] = rnd[i];
                else
                {
                    result[j - 1 - i] = rnd[rnd[i].first - n + k].second;
                    rnd[rnd[i].first - n + k].second = rnd[i].second;
                }
            }
            sorting::merge<>::sort(rnd + j, rnd + k, [](auto a, auto b)
                                   { return a.first < b.first; });
            result[j] = rnd[j].first;
            for (T i = j + 1; i < k; i++)
                result[i] = rnd[i].first != rnd[i - 1].first ? rnd[i].first : rnd[i].second;
            delete[] rnd;
        }
    };

    class merge
    {
    public:
        static constexpr std::string name()
        {
            return "merge";
        }

        template <std::integral T>
        static void sampling(T n, T k, T result[], T temp[])
        {
            static std::mt19937_64 mt(std::random_device{}());
            if (k == 1)
            {
                *result = mt() % n;
                return;
            }
            T half = k >> 1, p = 0, q = half;
            sampling(n, k - half, result + half, result);
            sampling(n - k + half, half, temp, result);
            for (T i = 0; i < q; i++)
                result[i] = q == k || temp[p] + q - half < result[q] ? temp[p++] + q - half : result[q++];
        }

        template <std::integral T>
        static void sampling(T n, T k, T result[])
        {
            T *temp = new T[k >> 1];
            sampling(n, k, result, temp);
            delete[] temp;
        }
    };

    template <std::integral T>
    void alias_distribution(T dist[], T mean, int n, int another[])
    {
        for (int j = 0; j < n; j++)
            another[j] = j;
        int i = 0;
        for (int j = n - 1; i < j; j--)
        {
            if (dist[j] >= mean)
                continue;
            while (dist[i] <= mean)
                ++i;
            dist[i] -= mean - dist[j];
            another[j] = i;
        }
        for (int j = 0; j <= i; j++)
        {
            if (dist[j] >= mean || another[j] != j)
                continue;
            while (dist[i] <= mean)
                ++i;
            dist[i] -= mean - dist[j];
            another[j] = i;
        }
    }
}

#endif
