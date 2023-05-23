#ifndef MINIMUM_SPANNING_FOREST_BASIC_HPP
#define MINIMUM_SPANNING_FOREST_BASIC_HPP

#include <concepts>

namespace minimum_spanning_forest
{
    template <std::totally_ordered T>
    struct edge
    {
        int src, dst;
        T cost;

        void nullify() { src = -1; }
        bool is_null() const { return src == -1; }

        bool operator==(const edge &e) const
        {
            return src == e.src && dst == e.dst;
        }

        bool operator!=(const edge &e) const
        {
            return src != e.src || dst != e.dst;
        }

        bool operator<(const edge &e) const
        {
            return cost < e.cost;
        }

        bool operator<=(const edge &e) const
        {
            return cost <= e.cost;
        }

        bool operator>(const edge &e) const
        {
            return cost > e.cost;
        }

        bool operator>=(const edge &e) const
        {
            return cost >= e.cost;
        }

        template <class PR>
        bool strictly_compare(const edge &e, const PR &cmp) const
        {
            return cmp(cost, e.cost) || cost == e.cost && (src < e.src || src == e.src && dst < e.dst);
        }
    };
}

#endif
