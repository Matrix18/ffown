#ifndef _TASK_QUEUE_IMPL_H_
#define _TASK_QUEUE_IMPL_H_

#include<pthread.h>
#include <list>
using namespace std;

#include "task_queue_i.h"
#include "lock.h"

class task_queue_t: public task_queue_i
{
public:
    task_queue_t():
        m_flag(true),
        m_index(0),
        m_tasklist(&m_tasklist_cache[m_index]),
        m_cond(m_mutex)
    {
    }
    ~task_queue_t()
    {
    }
    void close()
    {
        m_flag = false;
        m_cond.broadcast();
    }

    void multi_produce(const task_list_t& task_)
    {
        lock_guard_t lock(m_mutex);
        //! 条件满足唤醒等待线程
        if (m_tasklist->empty())
        {
            m_cond.signal();
        }
        for(task_list_t::const_iterator it = task_.begin(); it != task_.end(); ++it)
        {
            m_tasklist->push_back(*it);
        }
    }
    void produce(const task_t& task_)
    {        
        lock_guard_t lock(m_mutex);
        //! 条件满足唤醒等待线程
        if (m_tasklist->empty())
        {
            m_cond.signal();
        }
        m_tasklist->push_back(task_);
    }

    int   comsume(task_t& task_)
    {
        lock_guard_t lock(m_mutex);
        //! 当没有作业时，就等待直到条件满足被唤醒
        while (m_tasklist->empty())
        {
            if (false == m_flag)
            {
                return -1;
            }
            m_cond.wait();
        }

        task_ = m_tasklist->front();
        m_tasklist->pop_front();

        return 0;
    }

    task_list_t* comsume_all()
    {
        lock_guard_t lock(m_mutex);

        //! 当没有作业时，就等待直到条件满足被唤醒
        while (m_tasklist->empty())
        {
            if (false == m_flag)
            {
                return NULL;
            }
            m_cond.wait();
        }

        task_list_t* tmp = m_tasklist;
        m_tasklist = &m_tasklist_cache[(++m_index) % 2];

        return tmp;
    }
private:
    volatile bool                     m_flag;
    int                                    m_index;
    task_list_t                         m_tasklist_cache[2];
    task_list_t* volatile            m_tasklist;

    mutex_t                            m_mutex;
    condition_var_t                 m_cond;
};

class task_queue_pool_t: public task_queue_i
{
    typedef vector<task_queue_t*>    task_queue_vt_t;
    static void task_func(void* pd_)
    {
        task_queue_pool_t* t = (task_queue_pool_t*)pd_;
        t->run();
    }
public:
    static task_t gen_task(task_queue_pool_t* p)
    {
        return task_t(&task_func, p);
    }
public:
    task_queue_pool_t()
    {
        pthread_mutex_init(&m_mutex, NULL);
    }

    int   comsume(task_t& task_){ return -1; }
    task_list_t*   comsume_all(){ return NULL; }

    void run()
    {
        task_queue_t* p = new task_queue_t();

        pthread_mutex_lock(&m_mutex);
        m_tqs.push_back(p);
        pthread_mutex_unlock(&m_mutex);

        task_list_t* tasklist = p->comsume_all();
        while (tasklist)
        {
            for(task_list_t::iterator it = tasklist->begin(); it != tasklist->end(); ++it)
            {
                (*it).run();
            }
            tasklist->clear();
            tasklist = p->comsume_all();
        }
    }

    ~task_queue_pool_t()
    {
        task_queue_vt_t::iterator it = m_tqs.begin();
        for (; it != m_tqs.end(); ++it)
        {
            delete (*it);
        }
        m_tqs.clear();
        pthread_mutex_destroy(&m_mutex);
    }

    void close()
    {
        task_queue_vt_t::iterator it = m_tqs.begin();
        for (; it != m_tqs.end(); ++it)
        {
            (*it)->close();
        }
    }

    void produce(const task_t& task_)
    {
        m_tqs[(long)(task_.data) % m_tqs.size()]->produce(task_);
    }
    void multi_produce(const task_list_t& task_)
    {
        static unsigned int i = 0;
        m_tqs[i++ % m_tqs.size()]->multi_produce(task_);
    }
private:
    pthread_mutex_t       m_mutex;
    task_queue_vt_t         m_tqs;
};


#endif
