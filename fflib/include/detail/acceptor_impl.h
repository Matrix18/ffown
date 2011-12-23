#ifndef _ACCEPTOR_IMPL_H_
#define _ACCEPTOR_IMPL_H_

#include "socket_i.h"

class acceptor_impl_t: public socket_i
{
public:
    int socket() {return m_listen_fd;}
    int handle_epoll_write(){ return -1; }

    int handle_epoll_read();
    int handle_epoll_error();

    void close(){};
    void async_send(const string& buff_){}
    void async_recv(){}

private:
    int m_listen_fd;
};
#endif

