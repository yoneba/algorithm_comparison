#include "list.hpp"

#include <assert.h>

void test_doubly_linked_list()
{
    using namespace linked;
    list<double> dl(6);
    assert(dl.capacity() == 6);
    for (int i = 0; i < 6; i++)
        dl.item_at(i)->value = i;
    assert(dl.index_of(dl.item_at(5)) == 5);
    assert(dl.is_empty());
    dl.push_front(dl.item_at(2));
    assert(!dl.is_empty());
    dl.push_back(3);
    dl.push_back(0);
    assert(dl.size() == 3);
    double s = 0;
    for (auto *p = dl.last(); p != dl.terminal(); p = prev(p))
    {
        if (p->value == 2)
            assert(dl.index_of(p) == 2);
        s += p->value;
    }
    for (auto *p = dl.first(); p != dl.terminal(); p = next(p))
    {
        s += p->value;
    }
    assert(s == 10);
    assert(dl.first()->value == 2);
    assert(dl.last()->value == 0);
    dl.pop_back();
    assert(dl.last()->value == 3);
    dl.remove(2);
    assert(dl.first()->value == 3);
    dl.clear();
    assert(dl.last() == dl.terminal());
    assert(dl.size() == 0);
}

void test_singly_linked_list()
{
    using namespace linked;
    list<double, singly> sl(6);
    assert(sl.capacity() == 6);
    for (int i = 0; i < 6; i++)
        sl.item_at(i)->value = i;
    assert(sl.index_of(sl.item_at(5)) == 5);
    assert(sl.is_empty());
    sl.push_front(sl.item_at(2));
    assert(!sl.is_empty());
    sl.push_front(3);
    assert(sl.size() == 2);
    double s = 0;
    for (auto *p = sl.first(); p != sl.terminal(); p = next(p))
    {
        if (p->value == 2)
            assert(sl.index_of(p) == 2);
        s += p->value;
    }
    assert(s == 5);
    assert(sl.first()->value == 3);
    sl.pop_front();
    assert(sl.first()->value == 2);
    sl.clear();
    assert(sl.is_empty());
    assert(sl.size() == 0);
}

int main()
{
    test_singly_linked_list();
    test_doubly_linked_list();
}