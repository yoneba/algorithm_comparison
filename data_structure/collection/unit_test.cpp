#include "linked_list.hpp"

#include <assert.h>
#include <functional>

void test_singly_linked_list()
{
    collection::singly_linked_list<double> sl(6);
    assert(sl.capacity() == 6);
    for (int i = 0; i < 6; i++)
        sl.node_at(i)->value = i;
    assert(sl.index_of(sl.cnode_at(5)) == 5);
    assert(sl.empty());
    sl.push_front(sl.node_at(2));
    assert(!sl.empty());
    sl.push_front(3);
    assert(sl.size() == 2);
    double s = 0;
    for (auto itr = sl.cbegin(); itr != sl.cend(); ++itr)
    {
        if (itr->value == 2)
            assert(sl.index_of(itr) == 2);
        s += *itr;
    }
    for (double x : sl)
    {
        s += x;
    }
    assert(s == 10);
    assert(sl.begin()->value == 3);
    sl.pop_front();
    assert(sl.begin()->value == 2);
    sl.clear();
    assert(sl.empty());
    assert(sl.size() == 0);
}

void test_doubly_linked_list()
{
    collection::doubly_linked_list<double> dl(6);
    assert(dl.capacity() == 6);
    for (int i = 0; i < 6; i++)
        dl.node_at(i)->value = i;
    assert(dl.index_of(dl.cnode_at(5)) == 5);
    assert(dl.empty());
    dl.push_front(dl.node_at(2));
    assert(!dl.empty());
    dl.push_back(3);
    dl.push_back(0);
    assert(dl.size() == 3);
    double s = 0;
    for (auto itr = dl.crbegin(); itr != dl.crend(); ++itr)
    {
        if (itr->value == 2)
            assert(dl.index_of(itr) == 2);
        s += *itr;
    }
    for (double x : dl)
    {
        s += x;
    }
    assert(s == 10);
    assert(dl.begin()->value == 2);
    assert(dl.rbegin()->value == 0);
    dl.pop_back();
    assert(dl.rbegin()->value == 3);
    dl.remove(2);
    assert(dl.cbegin()->value == 3);
    dl.clear();
    assert(dl.rbegin() == dl.rend());
    assert(dl.size() == 0);
}

int main()
{
    test_singly_linked_list();
    test_doubly_linked_list();
}