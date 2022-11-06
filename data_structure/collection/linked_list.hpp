#ifndef LINKED_LIST_HPP
#define LINKED_LIST_HPP

#include "singly_linked_node.hpp"
#include "doubly_linked_node.hpp"

#include <type_traits>
#include <utility>

namespace collection
{
    template <class NodeBase, typename T>
    class linked_node : public NodeBase
    {
    public:
        T value;

        linked_node *front() { return static_cast<linked_node *>(NodeBase::next); }
        const linked_node *front() const { return static_cast<linked_node *>(NodeBase::next); }
        linked_node *back() { return static_cast<linked_node *>(NodeBase::prev); }
        const linked_node *back() const { return static_cast<linked_node *>(NodeBase::prev); }
    };

    template <class NodeBase>
    class linked_node<NodeBase, void> : public NodeBase
    {
    public:
        linked_node *front() { return static_cast<linked_node *>(NodeBase::next); }
        const linked_node *front() const { return static_cast<linked_node *>(NodeBase::next); }
        linked_node *back() { return static_cast<linked_node *>(NodeBase::prev); }
        const linked_node *back() const { return static_cast<linked_node *>(NodeBase::prev); }
    };

    template <class Node>
    class list_iterator
    {
        Node *pointer;

    public:
        list_iterator(Node *p) : pointer(p) {}
        list_iterator &operator=(Node *p)
        {
            pointer = p;
            return *this;
        }

        Node *address() const { return pointer; }
        Node *operator->() const { return pointer; }
        decltype(auto) operator*() const { return pointer->value; }

        bool operator==(const list_iterator &itr) const { return pointer == itr.pointer; }
        bool operator!=(const list_iterator &itr) const { return pointer != itr.pointer; }

        list_iterator &operator++()
        {
            pointer = pointer->front();
            return *this;
        }
        list_iterator &operator--()
        {
            pointer = pointer->back();
            return *this;
        }
    };

    template <class NodeBase, typename T>
    class linked_list
    {
        NodeBase head;
        linked_node<NodeBase, T> *nodes;
        int n_active_nodes;
        const int n_nodes;

    public:
        explicit linked_list(int n) : nodes(new linked_node<NodeBase, T>[n]), n_active_nodes(0), n_nodes(n)
        {
            head.clear();
        }

        linked_node<NodeBase, T> *node_at(int index) { return nodes + index; }
        const linked_node<NodeBase, T> *cnode_at(int index) const { return nodes + index; }
        int index_of(const linked_node<NodeBase, T> *p) const { return p - nodes; }
        int index_of(const list_iterator<linked_node<NodeBase, T>> &itr) const { return itr.address() - nodes; }
        int index_of(const list_iterator<const linked_node<NodeBase, T>> &itr) const { return itr.address() - nodes; }

        bool empty() const { return n_active_nodes == 0; }
        int size() const { return n_active_nodes; }
        int capacity() const { return n_nodes; }

        list_iterator<linked_node<NodeBase, T>> begin() { return static_cast<linked_node<NodeBase, T> *>(head.front()); }
        list_iterator<linked_node<NodeBase, T>> end() { return static_cast<linked_node<NodeBase, T> *>(&head); }
        list_iterator<linked_node<NodeBase, T>> rbegin() { return static_cast<linked_node<NodeBase, T> *>(head.back()); }
        list_iterator<linked_node<NodeBase, T>> rend() { return static_cast<linked_node<NodeBase, T> *>(&head); }
        list_iterator<const linked_node<NodeBase, T>> cbegin() const { return static_cast<const linked_node<NodeBase, T> *>(head.front()); }
        list_iterator<const linked_node<NodeBase, T>> cend() const { return static_cast<const linked_node<NodeBase, T> *>(&head); }
        list_iterator<const linked_node<NodeBase, T>> crbegin() const { return static_cast<const linked_node<NodeBase, T> *>(head.back()); }
        list_iterator<const linked_node<NodeBase, T>> crend() const { return static_cast<const linked_node<NodeBase, T> *>(&head); }

        void clear()
        {
            head.clear();
            n_active_nodes = 0;
        }

        void push_front(linked_node<NodeBase, T> *p)
        {
            head.push_front(p);
            ++n_active_nodes;
        }

        void push_front(int index)
        {
            push_front(nodes + index);
        }

        void push_back(linked_node<NodeBase, T> *p)
        {
            head.push_back(p);
            ++n_active_nodes;
        }

        void push_back(int index)
        {
            push_back(nodes + index);
        }

        void pop_front()
        {
            head.pop_front();
            --n_active_nodes;
        }

        void pop_back()
        {
            head.pop_back();
            --n_active_nodes;
        }

        void remove(linked_node<NodeBase, T> *p)
        {
            p->leave();
            --n_active_nodes;
        }

        void remove(int index)
        {
            remove(nodes + index);
        }

        ~linked_list()
        {
            delete[] nodes;
        }
    };

    template <typename T>
    using singly_linked_node = linked_node<singly_linked_node_base, T>;
    template <typename T>
    using doubly_linked_node = linked_node<doubly_linked_node_base, T>;
    template <typename T>
    using singly_linked_list = linked_list<singly_linked_node_base, T>;
    template <typename T>
    using doubly_linked_list = linked_list<doubly_linked_node_base, T>;
}

#endif
