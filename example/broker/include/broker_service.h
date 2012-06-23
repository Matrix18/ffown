#ifndef _BROKER_SERVICE_H_
#define _BROKER_SERVICE_H_

#include "msg_bus.h"

class broker_service_t: public msg_handler_i
{
public:
    broker_service_t();
    ~broker_service_t();

    int handle_broken(socket_ptr_t sock_);
    int handle_msg(const message_t& msg_, socket_ptr_t sock_);

    void create_service_group(create_service_group_t::in_t& in_msg_, rpc_callcack_t<create_service_group_t::out_t>& cb_);
    void create_service(create_service_t::in_t& in_msg_, rpc_callcack_t<create_service_t::out_t>& cb_);
    void reg_interface(reg_interface_t::in_t& in_msg_, rpc_callcack_t<reg_interface_t::out_t>& cb_);
private:
    msg_bus_t   m_msg_bus;
};
#endif
