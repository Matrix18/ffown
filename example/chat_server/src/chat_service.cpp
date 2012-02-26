#include "chat_service.h"
#include <iostream>
using namespace std;

int chat_service_t::handle_broken(socket_ptr_t sock_)
{
    return 0;
}

int chat_service_t::handle_msg(const message_t& msg_, socket_ptr_t sock_)
{
    cout <<"chat_service_t::handle_msg begin..." << msg_.get_body() <<"\n";
    return 0;
}

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
