#ifndef _SOCKET_I_
#define _SOCKET_I_

class socket_i
{
public:
    virtual ~socket_i(){}

    virtual int handle_epoll_read()  = 0;
    virtual int handle_epoll_write() = 0;
    virtual int handle_epoll_error() = 0;

    virtual void close() = 0;
    virtual void async_send(const string& buff_) = 0;
    virtual void async_recv() = 0;

};

#endif
