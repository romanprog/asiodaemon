#include "BTreeStoreCPP.hpp"
#include "MLocker.hpp"

namespace btreeCpp {

BTreeStore::BTreeStore(size_t size)
    :_datasize(size)
{ }

void BTreeStore::insert(const void *data)
{
    _insert(data, _root);
}

bool BTreeStore::search(const void *data)
{
    return _search(data, _root);
}

bool BTreeStore::insert(const std::string &data)
{
    if (data.size() != _datasize)
        return false;
    MLocker Lock(&_mutex);
    _insert(data.data(), _root);
    return true;
}

bool BTreeStore::search(const std::string &data)
{
    if (data.size() != _datasize)
        return false;

    return true;
}

void BTreeStore::_insert(const void *data, Node_ptr & _node)
{
    // Search and add.
    _search(data, _node, true);
}

bool BTreeStore::_search(const void *data, Node_ptr & _node, bool add_new)
{
    // if node
    if (!_node) {
        if (add_new) {
            _create(data, _node);
            return true;
        } else {
            return false;
        }
    }

    int cmpres = memcmp(data, _node.get()->data(), _datasize);

    if (!cmpres)
        return true;

    if (cmpres < 0)
        // Search in left branch (leaf).
        return _search(data, _node.get()->left_leaf, add_new);
    else
        // Search in rigth branch (leaf).
        return _search(data, _node.get()->rigth_leaf, add_new);
}

void BTreeStore::_create(const void *data, Node_ptr & _node)
{
    _node.reset(new Node(data, _datasize));
    ++unit_counter;
}


Node::Node(const void *d_ptr, size_t size)
{
    _data = malloc(size);
    memcpy(_data, d_ptr, size);
}

Node::~Node()
{
    free(_data);
}


} // namespace
