#ifndef _EPOLL_IMPL_H_
#define _EPOLL_IMPL_H_

#include <list>
using namespace std;

#include "epoll_i.h"

#define CREATE_EPOLL_SIZE  10000
#define EPOLL_EVENTS_SIZE  10000
//! 1s
#define EPOLL_WAIT_TIME     1000 

class epoll_impl_t: public epoll_i
{
    typedef list<epoll_fd_i*> fd_list_t;
public:
    epoll_impl_t();
    ~epoll_impl_t();

    virtual int event_loop();
    virtual int close();
    virtual int register_fd(epoll_fd_i*);
    virtual int unregister_fd(epoll_fd_i*);
    virtual int mod_fd(epoll_fd_i*);
private:
    void trigger_socket_error();
private:
    int                  m_efd;
    fd_list_t          m_wait_del;
};

#endif
