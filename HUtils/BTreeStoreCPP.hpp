#ifndef BTREESTORE_HPP
#define BTREESTORE_HPP

#include <iostream>
#include <memory>
#include <string>
#include <cstring>
#include <mutex>

namespace btreeCpp {
// btree CPP style version

class Node;

// Node pointer manager.
using Node_ptr = std::unique_ptr<Node>;

// Node class (leafs) for binary tree.
class Node
{
public:
    // Constructor is the only way to fill Node data.
    explicit Node(const void * d_ptr, size_t size);

    // Destructor frees memory for _data pointer.
    ~Node();

    // Const pointer to stored data.
    const void * data() const
    {
        return _data;
    }
    // Pointers to leafs in shared_ptr.
    Node_ptr left_leaf {nullptr};
    Node_ptr rigth_leaf {nullptr};

private:
    // Main data.
    void * _data {nullptr};

};

// Binary tree store release.
class BTreeStore
{
public:

    // Constructor create fixed data size btree store.
    explicit BTreeStore(size_t size);

    // Insert and search functions. Use use data of  * data ptr and size datasize.
    // This functions can not verify data size, which can lead to undesirable consequences.
    void insert(const void * data);
    bool search(const void * data);

    // Same as above, but data size verified. More recomended.
    bool insert(const std::string & data);
    bool search(const std::string & data);

    // Count of storet units.
    const inline int size() const
    {
        return unit_counter;
    }

    // Size of data unit in bytes.
    const inline int datasize() const
    {
        return _datasize;
    }

private:
    BTreeStore(BTreeStore &&) = delete;
    BTreeStore & operator= (BTreeStore &&) = delete;
    // Fixed size od data unit.
    size_t _datasize{0};
    // Root node.
    Node_ptr _root{nullptr};
    // Units counter.
    int unit_counter{0};
    // Mutex fot multithreading.
    std::mutex _mutex;

    // Recursive search and insert.
    // If such record already exists - do nothing.
    void _insert(const void * data, Node_ptr & _node);
    // Recursive search.
    bool _search(const void * data, Node_ptr & _node, bool add_new = false);
    // Create new data unit in _node ptr. Fill leafs with nullptr.
    void _create(const void * data, Node_ptr & _node);
};

} // namespace

#endif // BTREESTORE_HPP
