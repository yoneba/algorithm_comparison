#ifndef COLLECTION_DOUBLY_LINKED_NODE_HPP
#define COLLECTION_DOUBLY_LINKED_NODE_HPP

#include <utility>

namespace collection
{
    class doubly_linked_node_base
    {
    protected:
        doubly_linked_node_base *next;
        doubly_linked_node_base *prev;

    public:
        doubly_linked_node_base *front() { return next; }
        const doubly_linked_node_base *front() const { return next; }
        doubly_linked_node_base *back() { return prev; }
        const doubly_linked_node_base *back() const { return prev; }

        void clear()
        {
            next = this, prev = this;
        }

        bool empty() const { return next == this; }

        void push_front(doubly_linked_node_base *p)
        {
            next->prev = p;
            p->next = next;
            p->prev = this;
            next = p;
        }

        void push_back(doubly_linked_node_base *p)
        {
            prev->next = p;
            p->prev = prev;
            p->next = this;
            prev = p;
        }

        void pop_front()
        {
            next = next->next;
            next->prev = this;
        }

        void pop_back()
        {
            prev = prev->prev;
            prev->next = this;
        }

        void leave() const
        {
            next->prev = prev;
            prev->next = next;
        }

        void rejoin()
        {
            next->prev = this;
            prev->next = this;
        }

        void absorb_front(doubly_linked_node_base *p)
        {
            next->prev = p->prev;
            p->prev->next = next;
            p->prev = this;
            next = p;
        }

        void absorb_back(doubly_linked_node_base *p)
        {
            prev->next = p->next;
            p->next->prev = prev;
            p->next = this;
            prev = p;
        }

        static void swap(doubly_linked_node_base *p, doubly_linked_node_base *q)
        {
            p->next->prev = q;
            q->next->prev = p;
            std::swap(p->next, q->next);
            std::swap(p->prev, q->prev);
            p->prev->next = p;
            q->prev->next = q;
        }
    };
}

#endif
