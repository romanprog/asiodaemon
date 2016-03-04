#include "BTreeStore.hpp"


BTreeStore::BTreeStore(size_t size)
    :sz(size)
{ }

BTreeStore::~BTreeStore()
{
    _free(_root);
}

void BTreeStore::insert(const void *data)
{
    _insert(data, _root);
}

bool BTreeStore::search(const void *data)
{
    if (_search(data, _root) != nullptr)
        return true;

    return false;
}

void BTreeStore::_insert(const void *data, Node *&_node)
{

    if (_node == nullptr) {
        _create(data, _node);
        return;
    }

    int cmpres = memcmp(data, _node->data, sz);

    if (!cmpres)
        return;

    if (cmpres < 0)
        _insert(data, _node->left);
    else
        _insert(data, _node->rigth);
}

Node *BTreeStore::_search(const void *data, Node *&_node)
{
    if (_node == nullptr)
        return nullptr;

    int cmpres = memcmp(data, _node->data, sz);

    if (!cmpres)
        return _node;

    if (cmpres < 0)
        return _search(data, _node->left);
    else
        return _search(data, _node->rigth);


}

void BTreeStore::_create(const void *data, Node *&_node)
{
    _node= new Node;
    _node->data = malloc(sz);
    memcpy(_node->data, data, sz);
    ++unit_counter;
}

void BTreeStore::_free(Node *&_node)
{
    if (_node == nullptr)
        return;

    _free(_node->left);
    _free(_node->rigth);

    free(_node->data);
    delete _node;
}
