#include "atomic_queue.hpp"

#include <iostream>

#include "testutils.hpp"


DECLARE_TEST("base Exception safety")

struct Obj {
    bool will_throw_;
    std::size_t idx_;

    Obj(std::size_t idx, bool will_throw = false)
        : idx_(idx), will_throw_(will_throw)
    {}

    Obj(const Obj& other) : idx_(other.idx_)
    { if(other.will_throw_) throw other.will_throw_; }
};


const std::size_t NUM_PUSHES = 0xFF;

int main()
{
    std::size_t idx = 0;
    aq::atomic_queue_base<Obj> ao;

    ao.push_back(Obj(idx++));
    ao.push_back(Obj(idx++));

    TEST_ASSERT(ao.size() == 2);

    try {
        ao.push_back(Obj(idx, true));
    }
    catch(bool)
    {
        std::cout<<"Caught exception. Hope everyone is still ok!\n";
    }
    idx++;

    TEST_ASSERT(ao.size() == 2);

    ao.push_back(Obj(idx++));
    ao.push_back(Obj(idx++));
    TEST_ASSERT(ao.size() == 4);

    Obj* o;

    o = ao.pop_front();
    TEST_ASSERT(o->idx_ == 0); --idx; ao.deallocate(o);
    o = ao.pop_front();
    TEST_ASSERT(o->idx_ == 1); --idx; ao.deallocate(o);
    o = ao.pop_front();
    TEST_ASSERT(o->idx_ == 3); --idx; ao.deallocate(o);
    o = ao.pop_front();
    TEST_ASSERT(o->idx_ == 4); --idx; ao.deallocate(o);

    TEST_ASSERT(ao.size() == 0);

    return CONCLUDE_TEST();
}
