#ifndef _MSG_HANDLER_H_
#define _MSG_HANDLER_H_

class socket_i;
class message_t;

#include "message.h"
#include "socket_i.h"

class msg_handler_i
{
public:
    typedef socket_i* socket_ptr_t;

public:
    msg_handler_t();
    ~msg_handler_t();

    int handle_broken(socket_ptr_t sock_);
    int handle_msg(const message_t& sock_);

};

typedef msg_handler_i* msg_handler_ptr_t;
#endif
