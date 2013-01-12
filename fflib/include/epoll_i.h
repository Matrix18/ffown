#ifndef _EPOLL_I_
#define _EPOLL_I_

#include "netbase.h"
namespace ff {


class epoll_i
{
public:
    virtual ~epoll_i(){}

    virtual int event_loop()  = 0;
    virtual int close() = 0;
    virtual int register_fd(fd_i*)      = 0;
    virtual int unregister_fd(fd_i*)  = 0;
    virtual int mod_fd(fd_i*)           = 0;
};

}
#endif
