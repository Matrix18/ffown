#ifndef _MSG_HANDLER_H_
#define _MSG_HANDLER_H_

#include "message.h"
#include "socket_i.h"

namespace ff {
    
class socket_i;
class message_t;

class msg_handler_i
{
public:
    typedef socket_i* socket_ptr_t;

public:
    virtual ~msg_handler_i() {} ;

    virtual int handle_broken(socket_ptr_t sock_)  = 0;
    virtual int handle_msg(const message_t& msg_, socket_ptr_t sock_) = 0;

};

typedef msg_handler_i* msg_handler_ptr_t;
}

#endif
