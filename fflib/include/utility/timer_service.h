#ifndef _TIMER_SERVICE_H_
#define _TIMER_SERVICE_H_

#include <sys/epoll.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <assert.h>

#include <list>
using namespace std;

#include "thread.h"
#include "lock.h"

namespace ff {

class timer_service_t 
{
    struct interupt_info_t
    {
        int pair_fds[2];
        interupt_info_t()
        {
            assert(0 == ::socketpair(AF_LOCAL, SOCK_STREAM, 0, pair_fds));
            ::write(pair_fds[1], "0", 1);
        }
        ~interupt_info_t()
        {
            ::close(pair_fds[0]);
            ::close(pair_fds[1]);
        }
        int read_fd() { return pair_fds[0]; }
        int write_fd() { return pair_fds[1]; }
    };
    struct registered_info_t
    {
        registered_info_t(long ms_, const task_t& t_):
            dest_tm(ms_),
            callback(t_)
        {}
        bool is_timeout(long cur_ms_)       { return dest_tm <= cur_ms_; }
        long    dest_tm;
        task_t  callback;
    };
    typedef list<registered_info_t> registered_info_list_t;
public:
    timer_service_t(long tick):
        m_runing(true),
        m_efd(-1),
        m_min_timeout(tick),
        m_cache_list(0),
        m_checking_list(1)
    {
        m_efd = ::epoll_create(16);
        
        struct lambda_t
        {
            static void run(void* p_)
            {
                ((timer_service_t*)p_)->run();
            }
        };
        m_thread.create_thread(task_t(&lambda_t::run, this), 1);
    }
    virtual ~timer_service_t()
    {
        m_runing = false;
        interupt();
        
        ::close(m_efd);
        m_thread.join();
    }

    void timer_callback(long ms_, task_t func)
    {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        long   dest_ms = tv.tv_sec*1000 + tv.tv_usec / 1000 + ms_;
        
        lock_guard_t lock(m_mutex);

        registered_info_list_t::iterator it = m_registered_store.begin();
        while (it != m_registered_store.end())
        {
            if (dest_ms > it->dest_tm)
            {
                m_registered_store.insert(it, registered_info_t(dest_ms, func));
                break;
            }
            ++ it;
        }

        if (it == m_registered_store.end())
        {
            m_registered_store.insert(it, registered_info_t(dest_ms, func));
        }
    }

    void run()
    {
        struct epoll_event ev_set[64];
        //! interupt();
        
        struct timeval tv;
        
        do
        {
            ::epoll_wait(m_efd, ev_set, 64, m_min_timeout);
            
            if (false == m_runing)//! cancel
            {
                break;
            }
            
            gettimeofday(&tv, NULL);
            long cur_ms = tv.tv_sec*1000 + tv.tv_usec / 1000;
            
            process_timer_callback(cur_ms);
            
        }while (true) ;
    }

private:
    void interupt()
    {
        epoll_event ev = { 0, { 0 } };
        ev.events = EPOLLIN | EPOLLPRI | EPOLLOUT | EPOLLHUP;
        
        ::epoll_ctl(m_efd, EPOLL_CTL_ADD, m_interupt_info.read_fd(), &ev);
    }
    void process_timer_callback(long now_)
    {
        lock_guard_t lock(m_mutex);
        
        while (false == m_registered_store.empty())
        {
            registered_info_t& last = m_registered_store.back();
            if (false == last.is_timeout(now_))
            {
                break;
            }
            last.callback.run();
            m_registered_store.pop_back();
        }
    }

private:
    volatile bool            m_runing;
    int                      m_efd;
    volatile long            m_min_timeout;
    int                      m_cache_list;
    int                      m_checking_list;
    registered_info_list_t   m_registered_store;
    interupt_info_t          m_interupt_info;
    thread_t                 m_thread;
    mutex_t                  m_mutex;
};

}

#endif

