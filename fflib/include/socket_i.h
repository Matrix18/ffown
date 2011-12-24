#ifndef _SOCKET_I_
#define _SOCKET_I_

#include <string>
using namespace std;

#include "epoll_fd_i.h"

class socket_i: public epoll_fd_i
{
public:
    virtual ~socket_i(){}

    virtual void open() = 0;
    virtual int set_nonblock() = 0;
    virtual void async_send(const string& buff_) = 0;
    virtual void async_recv() = 0;

};

#endif
