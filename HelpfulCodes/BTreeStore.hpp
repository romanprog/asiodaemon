#ifndef BTREESTORE_HPP
#define BTREESTORE_HPP

#include <iostream>
#include <memory>
#include <cstring>

struct Node
{
    void * data {nullptr};
    Node * left {nullptr};
    Node * rigth {nullptr};
};

class BTreeStore
{
public:
    explicit BTreeStore(size_t size);
    ~BTreeStore();
    void insert(const void * data);
    bool search(const void * data);
    void print();
    inline int count()
    {
        return unit_counter;
    }

private:
    size_t sz{0};
    Node * _root{nullptr};
    int unit_counter{0};

    void _insert(const void * data, Node *&_root);
    Node * _search(const void * data, Node *&_node);
    void _create(const void * data, Node *&_node);
    void _free(Node *&_node);
};

#endif // BTREESTORE_HPP
