#ifndef _SOCKET_IMPL_H_
#define _SOCKET_IMPL_H_

#include <list>
#include <string>
using namespace std;

#include "socket_i.h"

class epoll_i;
class socket_event_handler_i;

#define  RECV_BUFFER_SIZE 8096

class socket_impl_t: public socket_i
{
public:
    typedef list<string>    send_buffer_t;

public:
    socket_impl_t(epoll_i*, socket_controller_i*, int fd = -1);
    ~socket_impl_t();

    virtual int socket() { return m_fd; }
    virtual void close();
    virtual void open();

    virtual int handle_epoll_read();
    virtual int handle_epoll_write();
    virtual int handle_epoll_error();

   virtual void async_send(const string& buff_);
   virtual void async_recv();

private:
    epoll_i*                            m_epoll;
    socket_controller_i*         m_sc;
    int                                   m_fd;

    char                                m_recv_buffer[RECV_BUFFER_SIZE];
    send_buffer_t                  m_send_buffer;
};
#endif
