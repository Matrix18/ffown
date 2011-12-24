#ifndef _EPOLL_IMPL_H_
#define _EPOLL_IMPL_H_

#include "epoll_i.h"

#define CREATE_EPOLL_SIZE  10
#define EPOLL_EVENTS_SIZE  128

class epoll_impl_t: public epoll_i
{
public:
    epoll_impl_t();
    ~epoll_impl_t();

    virtual int open();
    virtual int close();
    virtual int register_fd(epoll_fd_i*);
    virtual int unregister_fd(epoll_fd_i*);
private:
    int m_efd;
};

#endif
