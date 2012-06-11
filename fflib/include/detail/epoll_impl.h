#ifndef _EPOLL_IMPL_H_
#define _EPOLL_IMPL_H_

#include <list>
using namespace std;

#include "epoll_i.h"
#include "task_queue_i.h"

#define CREATE_EPOLL_SIZE  4096
#define EPOLL_EVENTS_SIZE  128
//! 1s
#define EPOLL_WAIT_TIME    -1 

class epoll_impl_t: public epoll_i
{
public:
    epoll_impl_t(task_queue_i* tqg_);
    ~epoll_impl_t();

    virtual int event_loop();
    virtual int close();
    virtual int register_fd(epoll_fd_i*);
    virtual int unregister_fd(epoll_fd_i*);
    virtual int mod_fd(epoll_fd_i*);

private:
    volatile bool            m_running;
    int                      m_efd;
    task_queue_i*            m_task_queue;
};

#endif
