#ifndef ATOMICTYPES_HPP
#define ATOMICTYPES_HPP

#include "../Logger/Logger.hpp"

#include <atomic>
#include <memory>
#include <mutex>
#include <set>
#include <vector>

namespace Atomic {


template<class T>
struct qnode
{
    T data;
    std::atomic<qnode*> next {(qnode*)(nullptr)};
    qnode(T&& data)
        : data(std::move(data)),
          next{nullptr}
    {}
};

template <class T>
class Queue
{
    qnode<T>* dd = nullptr;
    std::atomic<qnode<T>*> front {(qnode<T>*)(nullptr)};
    std::atomic<qnode<T>*> back {(qnode<T>*)(nullptr)};

public:

    Queue()
    {
        front = nullptr;
        back = nullptr;
    }

    ~Queue() {

        qnode<T>* ret = front.load();
        qnode<T>* nl = nullptr;
        while (!std::atomic_compare_exchange_weak_explicit(&front, &ret, nl,
                                                           std::memory_order_release,
                                                           std::memory_order_relaxed))
        {};

        while (ret) {
            qnode<T>* tmp = ret->next.load();
            delete ret;
            ret = tmp;
        }
    }

    void push_back(T data)
    {
        qnode<T>* new_node = new  qnode<T>(std::move(data));
        qnode<T>* null_pointer = nullptr;
        qnode<T>* back_before = back.load();


        if (front.compare_exchange_weak(null_pointer, new_node)) {
            front = new_node;
            return;
        }

        null_pointer = nullptr;
        qnode<T>* back_tmp = back.load();

        while (!std::atomic_compare_exchange_weak_explicit(&back_tmp->next, &null_pointer, new_node,
                                                           std::memory_order_release,
                                                           std::memory_order_relaxed))
        {

            back_tmp = null_pointer;
            null_pointer = nullptr;

        }
        back = new_node;

    }

    bool get_n_pop(T & res)
    {
        qnode<T>* ret = front.load();

        if (!ret)
            return false;

        while (!std::atomic_compare_exchange_weak_explicit(&front, &ret, ret->next.load(),
                                                           std::memory_order_release,
                                                           std::memory_order_relaxed))
        {
        }

        res = ret->data;
        ret->next = nullptr;

        delete ret;
        return true;

    }
};


template<class T>
struct node
{
    T data;
    node* next;
    node(const T& data) : data(data), next(nullptr) {}
};

template<class T>
class stack
{
    std::atomic<node<T>*> front;
    std::atomic<node<T>*> back;
 public:
    void push(const T& data)
    {
        node<T>* new_node = new node<T>(data);

        // put the current value of head into new_node->next
        new_node->next = front.load(std::memory_order_relaxed);

        // now make new_node the new head, but if the head
        // is no longer what's stored in new_node->next
        // (some other thread must have inserted a node just now)
        // then put that new head into new_node->next and try again
        while(!std::atomic_compare_exchange_weak_explicit(
                                &front,
                                &new_node->next,
                                new_node,
                                std::memory_order_release,
                                std::memory_order_relaxed))
                ; // the body of the loop is empty
// note: the above loop is not thread-safe in at least
// GCC prior to 4.8.3 (bug 60272), clang prior to 2014-05-05 (bug 18899)
// MSVC prior to 2014-03-17 (bug 819819). See member function version for workaround
    }
};

} // namespace

#endif // ATOMICTYPES_HPP
