#ifndef LINKED_LIST_HPP
#define LINKED_LIST_HPP

#include "connectivity.hpp"

#include <format>

namespace linked
{
    template <typename T, class NodeBase>
    class pack : public NodeBase
    {
    public:
        T value;
    };

    template <class NodeBase>
    class pack<void, NodeBase> : public NodeBase
    {
    };

    template <typename T, template <class> class Connectivity = doubly>
    class list
    {
        using node_base = typename Connectivity<circular>::node;

    public:
        using item = pack<T, node_base>;

    private:
        node_base head;
        item *items;
        int n_active;
        const int n_entire;

    public:
        static constexpr std::string name() { return "list"; }
        static constexpr std::string full_name()
        {
            return std::format("list<{}>", Connectivity::name());
        }

        explicit list(int n) : items(new item[n]), n_active(0), n_entire(n)
        {
            head.clear();
        }

        item *item_at(int index) { return items + index; }
        int index_of(const item *p) const { return p - items; }

        bool is_empty() const { return n_active == 0; }
        int size() const { return n_active; }
        int capacity() const { return n_entire; }

        item *first() { return static_cast<item *>(next(&head)); }
        item *last() { return static_cast<item *>(prev(&head)); }
        const item *terminal() const { return static_cast<const item *>(circular::terminal(&head)); }

        void clear()
        {
            head.clear();
            n_active = 0;
        }

        void push_front(item *p)
        {
            head.push_front(p);
            ++n_active;
        }
        void push_front(int index) { push_front(items + index); }

        void push_back(item *p)
        {
            head.push_back(p);
            ++n_active;
        }
        void push_back(int index) { push_back(items + index); }

        void pop_front()
        {
            head.pop_front();
            --n_active;
        }

        void pop_back()
        {
            head.pop_back();
            --n_active;
        }

        void remove(item *p)
        {
            p->leave();
            --n_active;
        }
        void remove(int index) { remove(items + index); }

        ~list()
        {
            delete[] items;
        }
    };
}

#endif
