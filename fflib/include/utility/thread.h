#ifndef _THREAD_H_
#define _THREAD_H_

#include<pthread.h>
#include <list>
#include <vector>
using namespace std;

#include "task_queue_i.h"

class task_queue_t: public task_queue_i
{
public:
    task_queue_t():
        m_flag(true),
        m_tasklist(new task_list_t())
    {
        //! 初始化锁变量和条件变量
        pthread_mutex_init(&m_mutex, NULL);
        pthread_cond_init(&m_cond, NULL);
    }
    ~task_queue_t()
    {
        pthread_mutex_destroy(&m_mutex);
        pthread_cond_destroy(&m_cond);
        delete m_tasklist;
    }
    void close()
    {
        m_flag = false;
        pthread_cond_broadcast(&m_cond);
    }

    void produce(const task_t& task_)
    {
        pthread_mutex_lock(&m_mutex);
        //! 条件满足唤醒等待线程
        if (m_tasklist->empty())
        {
            pthread_cond_signal(&m_cond);
        }
        m_tasklist->push_back(task_);

        pthread_mutex_unlock(&m_mutex);
    }

    int   comsume(task_t& task_)
    {
        pthread_mutex_lock(&m_mutex);

        //! 当没有作业时，就等待直到条件满足被唤醒
        while (m_tasklist->empty())
        {
            if (false == m_flag)
            {
                pthread_mutex_unlock(&m_mutex);
                return -1;
            }
            pthread_cond_wait(&m_cond, &m_mutex);
        }

        task_ = m_tasklist->front();
        m_tasklist->pop_front();
    
        pthread_mutex_unlock(&m_mutex);
        return 0;
    }

    task_list_t* comsume_all(task_list_t* p)
    {
        pthread_mutex_lock(&m_mutex);

        //! 当没有作业时，就等待直到条件满足被唤醒
        while (m_tasklist->empty())
        {
            if (false == m_flag)
            {
                pthread_mutex_unlock(&m_mutex);
                return NULL;
            }
            pthread_cond_wait(&m_cond, &m_mutex);
        }

        task_list_t* tmp = m_tasklist;
        m_tasklist = p;
        pthread_mutex_unlock(&m_mutex);
        return tmp;
    }
private:
    volatile bool         m_flag;
    task_list_t*            m_tasklist;
    //! 定义一个锁变量和条件变量
    pthread_mutex_t m_mutex;
    pthread_cond_t m_cond;
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
    task_list_t*   comsume_all(task_list_t* ){ return NULL; }
    void run()
    {
        task_queue_t* p = new task_queue_t();

        pthread_mutex_lock(&m_mutex);
        m_tqs.push_back(p);
        pthread_mutex_unlock(&m_mutex);

        task_list_t* tasklist= new task_list_t();
        tasklist = p->comsume_all(tasklist);
        while (tasklist)
        {
            for(task_list_t::iterator it = tasklist->begin(); it != tasklist->end(); ++it)
            {
                (*it).run();
            }
            tasklist->clear();
            tasklist = p->comsume_all(tasklist);
        }
        delete tasklist;
    }

    ~task_queue_pool_t()
    {
        task_queue_vt_t::iterator it = m_tqs.begin();
        for (; it != m_tqs.end(); ++it)
        {
            delete (*it);
        }
        m_tqs.clear();
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

private:
    pthread_mutex_t       m_mutex;
    task_queue_vt_t         m_tqs;
};


class thread_t
{
    static void* thread_func(void* p_)
    {
        task_t* t = (task_t*)p_;
        t->run();
        delete  t;
        return 0;
    }
public:
    thread_t(){}
    ~thread_t(){}

    int create_thread(task_t func, int num)
    {
        for (int i = 0; i < num; ++i)
        {
            pthread_t ntid;
            task_t* t = new task_t(func.func, func.data);
            if (0 == ::pthread_create(&ntid, NULL, thread_func, t))
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
