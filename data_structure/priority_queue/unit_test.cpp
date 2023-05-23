#include "d_ary_heap.hpp"
#include "pointer_based_heap.hpp"

#include <assert.h>
#include <functional>
#include <type_traits>

template <template <template <typename> class, typename> class PriorityQueue>
void test_priority_queue()
{
    PriorityQueue<std::less, double> pq(6, 3);
    assert(pq.capacity() == 6);
    assert(pq.size() == 0);
    assert(pq.empty() == true);
    pq.insert(3, 0.0);
    assert(pq.empty() == false);
    assert(pq.size() == 1);
    assert(pq[3] == 0.0);
    assert(pq.top() == 3);
    assert(pq.contains(3) == true);
    assert(pq.contains(4) == false);
    pq.insert(4, 1.0);
    assert(pq.contains(4) == true);
    assert(pq.top() == 3);
    pq.improve_key(4, -2.0);
    assert(pq.top() == 4);
    assert(pq[4] == -2.0);
    pq.insert(1, -1.0);
    pq.insert(5, 3.0);
    pq.insert(0, 2.0);
    pq.insert(2, 4.0);
    pq.worsen_key(4, 5.0);
    assert(pq.top() == 1);
    assert(pq.size() == 6);
    pq.remove(0);
    assert(pq.size() == 5);
    pq.remove_top();
    assert(pq.size() == 4);
    assert(pq.top() == 3);
    pq.remove_top();
    assert(pq.top() == 5);
    assert(pq.contains(5) == true);
    pq.remove_top();
    assert(pq.contains(5) == false);
    assert(pq.top() == 2);
    assert(pq.size() == 2);
    assert(pq.empty() == false);
    pq.clear();
    assert(pq.top() == -1);
    assert(pq.size() == 0);
    assert(pq.empty() == true);
}

int main()
{
    test_priority_queue<priority_queue::binary_heap>();
    test_priority_queue<priority_queue::ternary_heap>();
    test_priority_queue<priority_queue::quaternary_heap>();
    test_priority_queue<priority_queue::quinary_heap>();
    test_priority_queue<priority_queue::senary_heap>();
    test_priority_queue<priority_queue::septenary_heap>();
    test_priority_queue<priority_queue::octonary_heap>();
    test_priority_queue<priority_queue::novenary_heap>();
    test_priority_queue<priority_queue::denary_heap>();
    test_priority_queue<priority_queue::multiary_heap>();
    test_priority_queue<priority_queue::pairing_heap>();
    test_priority_queue<priority_queue::rank_pairing_heap>();
    test_priority_queue<priority_queue::fibonacci_heap>();
}