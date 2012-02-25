#ifndef _CHAT_SERVICE_H_
#define _CHAT_SERVICE_H_

#include "socket_i.h"
#include "msg_def.h"

typedef  uint32_t    uid_t;
typedef socket_i*    socket_ptr_t;

class chat_service_t
{
public:
    chat_service_t();
    ~chat_service_t();
    int handle(shared_ptr_t<login_req_t> req_, socket_ptr_t sock_);
    int handle(shared_ptr_t<logout_req_t> req_, socket_ptr_t sock_);
    int handle(shared_ptr_t<chat_to_some_req_t> req_, socket_ptr_t sock_);

};
#endif
