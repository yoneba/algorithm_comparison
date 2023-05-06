#ifndef LINKED_TOPOLOGY_HPP
#define LINKED_TOPOLOGY_HPP

#include <utility>
#include <string>

namespace linked
{
    class linear
    {
    public:
        static constexpr std::string name() { return "linear"; }
        static constexpr std::string full_name() { return "linear"; }

        template <class Node>
        static const Node *terminal(const Node *self) { return nullptr; }
        static int terminal(int self) { return -1; }

        template <class Node>
        static void clear_front(Node *self) { self->next = nullptr; }
        static void clear_front(int next_of[], int self) { next_of[self] = -1; }
        template <class Node>
        static void clear_back(Node *self) { self->prev = nullptr; }
        static void clear_back(int prev_of[], int self) { prev_of[self] = -1; }
        template <class Node>
        static void clear_both(Node *self)
        {
            clear_front(self);
            clear_back(self);
        }
        static void clear_both(int next_of[], int prev_of[], int self)
        {
            clear_front(next_of, self);
            clear_back(prev_of, self);
        }

        template <class Node>
        static bool is_front_empty(const Node *self) { return self->next == nullptr; }
        static bool is_front_empty(const int next_of[], int self) { return next_of[self] == -1; }
        template <class Node>
        static bool is_back_empty(const Node *self) { return self->prev == nullptr; }
        static bool is_back_empty(const int prev_of[], int self) { return prev_of[self] == -1; }
        template <class Node>
        static bool is_both_empty(const Node *self)
        {
            return is_front_empty(self) && is_back_empty(self);
        }
        static bool is_both_empty(const int next_of[], const int prev_of[], int self)
        {
            return is_front_empty(next_of, self) && is_back_empty(prev_of, self);
        }

        template <class Node>
        static void push_front(Node *self, Node *p)
        {
            Node *q = self->next;
            if (q != nullptr)
                q->prev = p;
            p->next = q;
            p->prev = self;
            self->next = p;
        }
        static void push_front(int next_of[], int prev_of[], int self, int p)
        {
            int q = next_of[self];
            if (q != -1)
                prev_of[q] = p;
            next_of[p] = q;
            prev_of[p] = self;
            next_of[self] = p;
        }

        template <class Node>
        static void push_back(Node *self, Node *p)
        {
            Node *q = self->prev;
            if (q != nullptr)
                q->next = p;
            p->prev = q;
            p->next = self;
            self->prev = p;
        }
        static void push_back(int next_of[], int prev_of[], int self, int p)
        {
            int q = prev_of[self];
            if (q != -1)
                next_of[q] = p;
            prev_of[p] = q;
            next_of[p] = self;
            prev_of[self] = p;
        }

        template <class Node>
        static void pop_front(Node *self)
        {
            Node *p = self->next->next;
            if (p != nullptr)
                p->prev = self;
            self->next = p;
        }
        static void pop_front(int next_of[], int prev_of[], int self)
        {
            int p = next_of[next_of[self]];
            if (p != -1)
                prev_of[p] = self;
            next_of[self] = p;
        }

        template <class Node>
        static void pop_back(Node *self)
        {
            Node *p = self->prev->prev;
            if (p != nullptr)
                p->next = self;
            self->prev = p;
        }
        static void pop_back(int next_of[], int prev_of[], int self)
        {
            int p = prev_of[prev_of[self]];
            if (p != -1)
                next_of[p] = self;
            prev_of[self] = p;
        }

        template <class Node>
        static void leave(const Node *self)
        {
            Node *p = self->next, *q = self->prev;
            if (p != nullptr)
                p->prev = q;
            if (q != nullptr)
                q->next = p;
        }
        static void leave(int next_of[], int prev_of[], int self)
        {
            int p = next_of[self], q = prev_of[self];
            if (p != -1)
                prev_of[p] = q;
            if (q != -1)
                next_of[q] = p;
        }

        template <class Node>
        static void rejoin(const Node *self)
        {
            Node *p = self->next, *q = self->prev;
            if (p != nullptr)
                p->prev = self;
            if (q != nullptr)
                q->next = self;
        }
        static void rejoin(int next_of[], int prev_of[], int self)
        {
            int p = next_of[self], q = prev_of[self];
            if (p != -1)
                prev_of[p] = self;
            if (q != -1)
                next_of[q] = self;
        }

        template <class Node>
        static void detach_front(Node *self)
        {
            Node *p = self->next;
            if (p != nullptr)
            {
                p->prev = nullptr;
                self->next = nullptr;
            }
        }
        static void detach_front(int next_of[], int prev_of[], int self)
        {
            int p = next_of[self];
            if (p != -1)
            {
                prev_of[p] = -1;
                next_of[self] = -1;
            }
        }

        template <class Node>
        static void detach_back(Node *self)
        {
            Node *p = self->prev;
            if (p != nullptr)
            {
                p->next = nullptr;
                self->prev = nullptr;
            }
        }
        static void detach_back(int next_of[], int prev_of[], int self)
        {
            int p = prev_of[self];
            if (p != -1)
            {
                next_of[p] = -1;
                prev_of[self] = -1;
            }
        }

        template <class Node>
        static void swap_front(Node *p, Node *q)
        {
            Node *r = p->next, *s = q->next;
            if (s != nullptr)
                s->prev = p;
            if (r != nullptr)
                r->prev = q;
            q->next = r;
            p->next = s;
        }
        static void swap_front(int next_of[], int prev_of[], int p, int q)
        {
            int r = next_of[p], s = next_of[q];
            if (s != -1)
                prev_of[s] = p;
            if (r != -1)
                prev_of[r] = q;
            next_of[q] = r;
            next_of[p] = s;
        }

        template <class Node>
        static void swap_back(Node *p, Node *q)
        {
            Node *r = p->prev, *s = q->prev;
            if (s != nullptr)
                s->next = p;
            if (r != nullptr)
                r->next = q;
            q->prev = r;
            p->prev = s;
        }
        static void swap_back(int next_of[], int prev_of[], int p, int q)
        {
            int r = prev_of[p], s = prev_of[q];
            if (s != -1)
                next_of[s] = p;
            if (r != -1)
                next_of[r] = q;
            prev_of[q] = r;
            prev_of[p] = s;
        }
    };

    class circular
    {
    public:
        static constexpr std::string name() { return "circular"; }
        static constexpr std::string full_name() { return "circular"; }

        template <class Node>
        static const Node *terminal(const Node *self) { return self; }
        static int terminal(int self) { return self; }

        template <class Node>
        static void clear(Node *self)
        {
            self->next = self;
            self->prev = self;
        }
        static void clear(int next_of[], int prev_of[], int self)
        {
            next_of[self] = self;
            prev_of[self] = self;
        }

        template <class Node>
        static bool is_empty(const Node *self) { return self->next == self; }
        static bool is_empty(const int next_of[], int self) { return next_of[self] == self; }

        template <class Node>
        static void push_front(Node *self, Node *p)
        {
            Node *q = self->next;
            q->prev = p;
            p->next = q;
            p->prev = self;
            self->next = p;
        }
        static void push_front(int next_of[], int prev_of[], int self, int p)
        {
            int q = next_of[self];
            prev_of[q] = p;
            next_of[p] = q;
            prev_of[p] = self;
            next_of[self] = p;
        }

        template <class Node>
        static void push_back(Node *self, Node *p)
        {
            Node *q = self->prev;
            q->next = p;
            p->prev = q;
            p->next = self;
            self->prev = p;
        }
        static void push_back(int next_of[], int prev_of[], int self, int p)
        {
            int q = prev_of[self];
            next_of[q] = p;
            prev_of[p] = q;
            next_of[p] = self;
            prev_of[self] = p;
        }

        template <class Node>
        static void pop_front(Node *self)
        {
            Node *p = self->next->next;
            p->prev = self;
            self->next = p;
        }
        static void pop_front(int next_of[], int prev_of[], int self)
        {
            int p = next_of[next_of[self]];
            prev_of[p] = self;
            next_of[self] = p;
        }

        template <class Node>
        static void pop_back(Node *self)
        {
            Node *p = self->prev->prev;
            p->next = self;
            self->prev = p;
        }
        static void pop_back(int next_of[], int prev_of[], int self)
        {
            int p = prev_of[prev_of[self]];
            next_of[p] = self;
            prev_of[self] = p;
        }

        template <class Node>
        static void leave(const Node *self)
        {
            Node *p = self->next, *q = self->prev;
            p->prev = q;
            q->next = p;
        }
        static void leave(int next_of[], int prev_of[], int self)
        {
            int p = next_of[self], q = prev_of[self];
            prev_of[p] = q;
            next_of[q] = p;
        }

        template <class Node>
        static void rejoin(const Node *self)
        {
            self->next->prev = self;
            self->prev->next = self;
        }
        static void rejoin(int next_of[], int prev_of[], int self)
        {
            prev_of[next_of[self]] = self;
            next_of[prev_of[self]] = self;
        }

        template <class Node>
        static void absorb_front(Node *self, Node *p)
        {
            Node *q = self->next, *r = p->prev;
            q->prev = r;
            r->next = q;
            p->prev = self;
            self->next = p;
        }
        static void absorb_front(int next_of[], int prev_of[], int self, int p)
        {
            int q = next_of[self], r = prev_of[p];
            prev_of[q] = r;
            next_of[r] = q;
            prev_of[p] = self;
            next_of[self] = p;
        }

        template <class Node>
        static void absorb_back(Node *self, Node *p)
        {
            Node *q = self->prev, *r = p->next;
            q->next = r;
            r->prev = q;
            p->next = self;
            self->prev = p;
        }
        static void absorb_back(int next_of[], int prev_of[], int self, int p)
        {
            int q = prev_of[self], r = next_of[p];
            next_of[q] = r;
            prev_of[r] = q;
            next_of[p] = self;
            prev_of[self] = p;
        }

        template <class Node>
        static void swap(Node *p, Node *q)
        {
            p->next->prev = q;
            q->next->prev = p;
            std::swap(p->next, q->next);
            std::swap(p->prev, q->prev);
            p->prev->next = p;
            q->prev->next = q;
        }
        static void swap(int next_of[], int prev_of[], int p, int q)
        {
            prev_of[next_of[p]] = q;
            prev_of[next_of[q]] = p;
            std::swap(next_of[p], next_of[q]);
            std::swap(prev_of[p], prev_of[q]);
            next_of[prev_of[p]] = p;
            next_of[prev_of[q]] = q;
        }
    };
}

#endif