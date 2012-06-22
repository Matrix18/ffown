#include "broker_service.h"
#include "msg_bus.h"

broker_service_t::broker_service_t()
{
    
}

broker_service_t::~broker_service_t()
{
    
}

int broker_service_t::handle_broken(socket_ptr_t sock_)
{
    sock_->safe_delete();
    return 0;
}
int broker_service_t::handle_msg(const message_t& msg_, socket_ptr_t sock_)
{
    return 0;
}