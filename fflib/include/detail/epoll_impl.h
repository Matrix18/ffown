#ifndef _EPOLL_IMPL_H_
#define _EPOLL_IMPL_H_

#include <list>
using namespace std;

#include "epoll_i.h"
#include "task_queue_i.h"
#include "lock.h"

namespace ff {

#define CREATE_EPOLL_SIZE  4096
#define EPOLL_EVENTS_SIZE  128
//! 100 ms
#define EPOLL_WAIT_TIME    100 

class epoll_impl_t: public epoll_i
{
public:
    epoll_impl_t();
    ~epoll_impl_t();

    virtual int event_loop();
    virtual int close();
    virtual int register_fd(epoll_fd_i*);
    virtual int unregister_fd(epoll_fd_i*);
    virtual int mod_fd(epoll_fd_i*);

protected:
    void destory_error_fd();

private:
    volatile bool            m_running;
    int                      m_efd;
    task_queue_i*            m_task_queue;
    int                      m_interunpt_sockets[2];
    //! 待销毁的error socket
    list<epoll_fd_i*>        m_error_fd_set;
    mutex_t                  m_mutex;
};

}
#endif
