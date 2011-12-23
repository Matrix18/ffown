#ifndef _EPOLL_I_
#define _EPOLL_I_

class socket_i;

class epoll_i
{
public:
    virtual ~epoll_i(){}

    virtual int open()  = 0;
    virtual int close() = 0;
    virtual int register_read_event(socket_i*)   = 0;
    virtual int register_write_event(socket_i*)  = 0;

};

#endif
