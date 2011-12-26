#ifndef _ACCEPTOR_IMPL_H_
#define _ACCEPTOR_IMPL_H_

#include "acceptor_i.h"

#define LISTEN_BACKLOG 5

class epoll_i;
class socket_i;

class acceptor_impl_t: public acceptor_i
{
public:
    acceptor_impl_t(epoll_i*);
    ~acceptor_impl_t();
    int   open(const string& address_);
    void close();

    int socket() {return m_listen_fd;}
    int handle_epoll_read();
    int handle_epoll_error() ;
protected:
    virtual socket_i* create_socket(int);

private:
    int                 m_listen_fd;
    epoll_i*          m_epoll;
};
#endif

