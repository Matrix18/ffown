#ifndef _TASK_QUEUE_I_
#define _TASK_QUEUE_I_

#include <list>
#include <vector>
using namespace std;

typedef void (*task_func_t)(void*);
struct task_t
{
    task_t(task_func_t f_ = 0, void* d_ = 0):
        data(d_),
        func(f_)
    {
    }
    void run()
    {
        func(data);
    }
    void*           data;
    task_func_t func;
};

class task_queue_i
{
public:
    typedef list<task_t> task_list_t;
public:
    virtual ~task_queue_i(){}
    virtual void close() = 0;
    virtual void produce(const task_t& task_) =0;
    virtual void multi_produce(const task_list_t& task_) =0;
    virtual int   consume(task_t& task_) = 0;
    virtual task_list_t* consume_all() = 0;
};
#endif
