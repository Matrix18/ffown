#include "chat_service.h"

chat_service_t::chat_service_t()
{
}

chat_service_t::~chat_service_t()
{
}

int chat_service_t::handle(shared_ptr_t<login_req_t> req_, socket_ptr_t sock_)
{
    return 0;
}

int chat_service_t::handle(shared_ptr_t<logout_req_t> req_, socket_ptr_t sock_)
{
    return 0;
}

int chat_service_t::handle(shared_ptr_t<chat_to_some_req_t> req_, socket_ptr_t sock_)
{
    return 0;
}
