#ifndef _THREAD_I_
#define _THREAD_I_

#include<pthread.h>
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

class task_queue_t
{
    typedef list<task_t> task_list_t;

public:
    task_queue_t():
        m_flag(false)
    {
        //! 初始化锁变量和条件变量
        pthread_mutex_init(&m_mutex, NULL);
        pthread_cond_init(&m_cond, NULL);
    }
    ~task_queue_t()
    {
        pthread_mutex_destroy(&m_mutex);
        pthread_cond_destroy(&m_cond);
    }
    int close()
    {
        m_flag = false;
        pthread_cond_broadcast(&m_cond);
        return 0;
    }

    void produce(const task_t& task_)
    {
        pthread_mutex_lock(&m_mutex);
        //! 条件满足唤醒等待线程
        if (m_tasklist.empty())
        {
            pthread_cond_signal(&m_cond);
        }
        m_tasklist.push_back(task_);

        pthread_mutex_unlock(&m_mutex);
    }

    int   comsume(task_t& task_)
    {
        pthread_mutex_lock(&m_mutex);

        //! 当没有作业时，就等待直到条件满足被唤醒
        while (m_tasklist.empty())
        {
            if (false == m_flag)
            {
                pthread_mutex_unlock(&m_mutex);
                return -1;
            }
            pthread_cond_wait(&m_cond, &m_mutex);
        }

        task_ = m_tasklist.front();
        m_tasklist.pop_front();
    
        pthread_mutex_unlock(&m_mutex);
        return 0;
    }

private:
    volatile bool         m_flag;
    task_list_t             m_tasklist;
    //! 定义一个锁变量和条件变量
    pthread_mutex_t m_mutex;
    pthread_cond_t m_cond;
};

class task_queue_group_t
{
    typedef vector<task_queue_t*>    task_queue_vt_t;
public:
    void run(void* pd_ = NULL)
    {
        (void)pd_;
        task_queue_t* p = new task_queue_t();
        m_tqs.push_back(p);
        task_t task_func;
        while (0 == p->comsume(task_func))
        {
            task_func.run();
        }
    }

    ~task_queue_group_t()
    {
        task_queue_vt_t::iterator it = m_tqs.begin();
        for (; it != m_tqs.end(); ++it)
        {
            delete (*it);
        }
        m_tqs.clear();
    }

    void close_all()
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

private:
    task_queue_vt_t         m_tqs;
};


class thread_t
{
    static void* thread_func(void* p_)
    {
        task_t* t = (task_t*)p_;
        t->run();
        return 0;
    }
public:
    thread_t();
    ~thread_t();

    int create_thread(task_t func, int num)
    {
        for (int i = 0; i < num; ++i)
        {
            pthread_t ntid;
            if (0 == ::pthread_create(&ntid, NULL, thread_func, &func))
            {
                m_tid_list.push_back(ntid);
            }
        }
        return 0;
    }
    int join_all()
    {
        list<pthread_t>::iterator it = m_tid_list.begin();
        for (; it != m_tid_list.end(); ++it)
        {
            pthread_join((*it), NULL);
        }
        m_tid_list.clear();
        return 0;
    }

private:
    list<pthread_t> m_tid_list;
};
#endif
