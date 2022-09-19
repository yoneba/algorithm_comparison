#ifndef SINGLY_LINKED_NODE_HPP
#define SINGLY_LINKED_NODE_HPP

#include <utility>

namespace collection
{
    class singly_linked_node_base
    {
    protected:
        singly_linked_node_base *next;

    public:
        singly_linked_node_base *front() { return next; }
        const singly_linked_node_base *front() const { return next; }

        void clear()
        {
            next = this;
        }

        bool empty() const { return next == this; }

        void push_front(singly_linked_node_base *p)
        {
            p->next = next;
            next = p;
        }

        void pop_front()
        {
            next = next->next;
        }

        void restore_front(singly_linked_node_base *p)
        {
            next = p;
        }

        static void swap(singly_linked_node_base *p, singly_linked_node_base *q)
        {
            std::swap(p->next, q->next);
        }
    };
}

#endif
