#ifndef _THREAD_H_
#define _THREAD_H_

struct task_t
{
    void* data;
};

class task_queue_t
{
public:
    int open();
    int close();
    void produce(const task_t& task_);
    int   comsume(task_t& task_);
};

class thread_t
{
public:
    thread_t();
    ~thread_t();

    template<typename T>
    int create_thread(T func, int num);
    int join_all();
private:
    task_queue_t        m_queue;
};
#endif
