#include "union_find.hpp"

#include <assert.h>

template <class DisjointSet>
void test_disjoint_set()
{
    DisjointSet ds(6);
    assert(ds.unite(3, 5) == true);
    assert(ds.find(3) == ds.find(5));
    assert(ds.find(3) != ds.find(4));
    assert(ds.unite(3, 5) == false);
    assert(ds.unite(3, 4) == true);
    assert(ds.find(3) == ds.find(4));
    assert(ds.find(3) != ds.find(2));
    assert(ds.unite(1, 2) == true);
    assert(ds.find(1) == ds.find(2));
    assert(ds.find(1) != ds.find(5));
    assert(ds.unite(1, 3) == true);
    assert(ds.unite(1, 5) == false);
    assert(ds.find(4) == ds.find(1));
    assert(ds.unite(1, 0) == true);
    assert(ds.find(4) == ds.find(0));
    ds.clear();
    assert(ds.set_count() == 6);
    assert(ds.element_count() == 6);
    assert(ds.unite(5, 1) == true);
    assert(ds.find(1) == ds.find(5));
    assert(ds.find(3) != ds.find(4));
    assert(ds.unite(3, 4) == true);
    assert(ds.find(3) == ds.find(4));
    assert(ds.unite(4, 5) == true);
    assert(ds.find(3) == ds.find(1));
    assert(ds.unite(1, 4) == false);
    assert(ds.find(3) != ds.find(2));
}

int main()
{
    using namespace disjoint_set;
    test_disjoint_set<union_find_forest_aos<find_strategy::nothing_special>>();
    test_disjoint_set<union_find_forest_aos<find_strategy::path_compression>>();
    test_disjoint_set<union_find_forest_aos<find_strategy::path_halving>>();
    test_disjoint_set<union_find_forest_aos<find_strategy::path_halving_eager>>();
    test_disjoint_set<union_find_forest<find_strategy::nothing_special>>();
    test_disjoint_set<union_find_forest<find_strategy::path_compression>>();
    test_disjoint_set<union_find_forest<find_strategy::path_halving>>();
    test_disjoint_set<union_find_forest<find_strategy::path_halving_eager>>();
}