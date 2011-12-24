#ifndef _EPOLL_I_
#define _EPOLL_I_

class epoll_fd_i;

class epoll_i
{
public:
    virtual ~epoll_i(){}

    virtual int open()  = 0;
    virtual int close() = 0;
    virtual int register_fd(epoll_fd_i*)   = 0;
    virtual int unregister_fd(epoll_fd_i*)                       = 0;
};

#endif
