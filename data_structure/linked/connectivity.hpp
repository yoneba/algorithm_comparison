#ifndef LINKED_CONNECTIVITY_HPP
#define LINKED_CONNECTIVITY_HPP

#include "topology.hpp"

#include <type_traits>
#include <format>

namespace linked
{
    template <class Node>
    inline Node *next(Node *self) { return static_cast<Node *>(self->next); }
    template <class Node>
    inline Node *prev(Node *self) { return static_cast<Node *>(self->prev); }

    template <class Topology = linear>
    class singly
    {
    public:
        static constexpr std::string name() { return "singly"; }
        static constexpr std::string full_name()
        {
            return std::format("singly<{}>", Topology::name());
        }

        template <class Node>
        static void clear(Node *self) { self->next = const_cast<Node *>(Topology::terminal(self)); }
        static void clear(int next_of[], int self) { next_of[self] = Topology::terminal(self); }

        template <class Node>
        static bool is_empty(const Node *self) { return self->next == Topology::terminal(self); }
        static bool is_empty(const int next_of[], int self) { return next_of[self] == Topology::terminal(self); }

        template <class Node>
        static void push_front(Node *self, Node *p)
        {
            p->next = self->next;
            self->next = p;
        }
        static void push_front(int next_of[], int self, int p)
        {
            next_of[p] = next_of[self];
            next_of[self] = p;
        }

        template <class Node>
        static void pop_front(Node *self)
        {
            self->next = self->next->next;
        }
        static void pop_front(int next_of[], int self)
        {
            next_of[self] = next_of[next_of[self]];
        }

        template <class Node>
        static void restore_front(Node *self, const Node *p)
        {
            self->next = p;
        }
        static void restore_front(int next_of[], int self, int p)
        {
            next_of[self] = p;
        }

        template <class Node>
        static void swap(Node *p, Node *q)
        {
            std::swap(p->next, q->next);
        }
        static void swap(int next_of[], int p, int q)
        {
            std::swap(next_of[p], next_of[q]);
        }

        class node
        {
            template <class Node>
            friend Node *next(Node *);
            friend class singly;

            node(const node &) = delete;
            node &operator=(const node &) = delete;

        protected:
            node *next;

        public:
            node() = default;

            void clear() { singly::clear(this); }
            bool is_empty() const { return singly::is_empty(this); }
            void push_front(node *p) { singly::push_front(this, p); }
            void pop_front() { singly::pop_front(this); }
            void restore_front(const node *p) { singly::restore_front(this, p); }
        };
    };

    template <class Topology = circular>
    class doubly
    {
    public:
        static constexpr std::string name() { return "doubly"; }
        static constexpr std::string full_name()
        {
            return std::format("doubly<{}>", Topology::name());
        }

        template <class Node>
        static void clear(Node *self)
        {
            if constexpr (std::is_same_v<Topology, linear>)
                linear::clear_front(self);
            else
                Topology::clear(self);
        }
        static void clear(int next_of[], int prev_of[], int self)
        {
            if constexpr (std::is_same_v<Topology, linear>)
                linear::clear_front(next_of, self);
            else
                Topology::clear(next_of, prev_of, self);
        }

        template <class Node>
        static bool is_empty(const Node *self)
        {
            if constexpr (std::is_same_v<Topology, linear>)
                linear::is_front_empty(self);
            else
                Topology::is_empty(self);
        }
        static bool is_empty(const int next_of[], const int prev_of[], int self)
        {
            if constexpr (std::is_same_v<Topology, linear>)
                linear::is_front_empty(next_of, self);
            else
                Topology::is_empty(next_of, self);
        }

        class node
        {
            template <class Node>
            friend Node *next(Node *);
            template <class Node>
            friend Node *prev(Node *);
            friend class doubly;
            friend class linear;
            friend class circular;

            node(const node &) = delete;
            node &operator=(const node &) = delete;

        protected:
            node *next, *prev;

        public:
            node() = default;

            void clear() { doubly::clear(this); }
            bool is_empty() const { doubly::is_empty(this); }
            void push_front(node *p) { Topology::push_front(this, p); }
            void push_back(node *p) { Topology::push_back(this, p); }
            void pop_front() { Topology::pop_front(this); }
            void pop_back() { Topology::pop_back(this); }
            void leave() const { Topology::leave(this); }
            void rejoin() const { Topology::rejoin(this); }
        };
    };
}

#endif