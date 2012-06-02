#ifndef _TIMER_SERVICE_H_
#define _TIMER_SERVICE_H_
#include <sys/epoll.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/time.h>

#include <list>
using namespace std;

#include "thread.h"
#include "lock.h"

class timer_service_t 
{
    struct interunpt_info_t
    {
        int pipe_fds[2];
        interunpt_info_t()
        {
            ::pipe(pipe_fds);
            ::fcntl(pipe_fds[0], F_SETFL, O_NONBLOCK);
            ::fcntl(pipe_fds[1], F_SETFL, O_NONBLOCK);
        }
        ~interunpt_info_t()
        {
            ::close(pipe_fds[0]);
            ::close(pipe_fds[1]);
        }
        int read_fd() { return pipe_fds[0]; }
    };
    struct registered_info_t
    {
        registered_info_t(long ms_, const task_t& t_):
            ticks(ms_),
            callback(t_)
        {}
        void dec(long t_)       { ticks -= t_; }
        bool is_timeout(){ return ticks <= 0; }
        long    ticks;
        task_t  callback;
    };
    typedef list<registered_info_t> registered_info_list_t;
public:
    timer_service_t();
    virtual ~timer_service_t();

    void timer_callback(long ms_, task_t func);

    void run();

private:
    void interunpt();
    long process_timer_callback(long cost_ms_);
private:
    volatile bool            m_runing;
    int                      m_efd;
    volatile long            m_min_timeout;
    registered_info_list_t*  m_registered_data;
    interunpt_info_t         m_interunpt_info;
    thread_t                 m_thread;
    mutex_t                  m_mutex;
};

timer_service_t::timer_service_t():
    m_runing(true),
    m_efd(-1),
    m_min_timeout(-1),
    m_registered_data(new registered_info_list_t())
{
    m_efd = ::epoll_create(16);

    struct lambda_t
    {
        static void run(void* p_)
        {
            ((timer_service_t*)p_)->run();
        }
    };
    m_thread.create_thread(task_t(&lambda_t::run, NULL), 1);
}

timer_service_t::~timer_service_t()
{
    m_thread.join();
}

void timer_service_t::run()
{
    struct epoll_event ev_set[64];
    interunpt();
    int cur_min_timeout = 0;

    struct timeval begin;
    struct timeval end;
    gettimeofday(&begin, NULL);
    
    cur_min_timeout = m_min_timeout;
    do
    {
        int nfds= ::epoll_wait(m_efd, ev_set, 64, cur_min_timeout);
        gettimeofday(&end, NULL);
        
        long cost_ms = (end.tv_sec - begin.tv_sec)*1000 + (end.tv_usec - begin.tv_usec);
        long left_ms = cur_min_timeout > cost_ms? cur_min_timeout - cost_ms: 0;

        if (false == m_runing)//! error
        {
            break;
        }

        if (nfds > 0)//! interupt
        {
            lock_guard_t lock(m_mutex);
            cur_min_timeout = left_ms < m_min_timeout? left_ms: m_min_timeout;
            continue;
        }
        //! timeout
        cur_min_timeout = process_timer_callback(cost_ms);
        gettimeofday(&begin, NULL);
    }while (true) ;
}

void timer_service_t::timer_callback(long ms_, task_t func)
{
    lock_guard_t lock(m_mutex);
    m_registered_data->push_back(registered_info_t(ms_, func));

    if (ms_ < m_min_timeout)
    {
        //! 需要调整一下定时器精度
        m_min_timeout = ms_;
        interunpt();
    }
    
}

void timer_service_t::interunpt()
{
    epoll_event ev = { 0, { 0 } };
    ev.events = EPOLLIN | EPOLLPRI | EPOLLOUT | EPOLLHUP | EPOLLET;
    ev.data.fd= m_interunpt_info.read_fd();
    epoll_ctl(m_efd, EPOLL_CTL_ADD, ev.data.fd, &ev);
}

long timer_service_t::process_timer_callback(long cost_ms_)
{
    lock_guard_t lock(m_mutex);
    return -1;
}
#endif

