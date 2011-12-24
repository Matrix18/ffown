#ifndef _EPOLL_I_
#define _EPOLL_I_

class epoll_fd_i;

class epoll_i
{
public:
    virtual ~epoll_i(){}

    virtual int open()  = 0;
    virtual int close() = 0;
    virtual int register_read_event(epoll_fd_i*)   = 0;
    virtual int register_write_event(epoll_fd_i*)  = 0;
    virtual int del_fd(epoll_fd_i*)                       = 0;
};

#endif
