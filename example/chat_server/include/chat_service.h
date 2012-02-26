#ifndef _CHAT_SERVICE_H_
#define _CHAT_SERVICE_H_

#include "socket_i.h"
#include "msg_def.h"
#include "msg_handler_i.h"

typedef  uint32_t    uid_t;
typedef socket_i*    socket_ptr_t;

class chat_service_t: public msg_handler_i
{
public:
    chat_service_t();
    ~chat_service_t();

    int handle_broken(socket_ptr_t sock_);
    int handle_msg(const message_t& sock_);

    int handle(shared_ptr_t<login_req_t> req_, socket_ptr_t sock_);
    int handle(shared_ptr_t<logout_req_t> req_, socket_ptr_t sock_);
    int handle(shared_ptr_t<chat_to_some_req_t> req_, socket_ptr_t sock_);

};
#endif
