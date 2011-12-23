#ifndef _EPOLL_IMPL_H_
#define _EPOLL_IMPL_H_

#include "epoll_i.h"

#define CREATE_EPOLL_SIZE  10
#define EPOLL_EVENTS_SIZE  10

class epoll_impl_t: public epoll_i
{
public:
    epoll_impl_t();
    ~epoll_impl_t();

    int open();
    int close();
    int register_read_event(socket_i*);
    int register_write_event(socket_i*);

private:
    int m_efd;
};

#endif
