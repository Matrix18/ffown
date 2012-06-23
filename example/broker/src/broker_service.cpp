#include "broker_service.h"

broker_service_t::broker_service_t()
{
    m_msg_bus.create_service("broker_service", 1)
             .bind_service(this)
             .reg(&broker_service_t::create_service_group)
             .reg(&broker_service_t::create_service)
             .reg(&broker_service_t::reg_interface);
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
    switch (msg_.get_cmd())
    {
        case rpc_msg_cmd_e::CREATE_SERVICE_GROUP:
        {
        }break;
        case rpc_msg_cmd_e::CREATE_SERVICE:
        {
            
        }break;
        case rpc_msg_cmd_e::REG_INTERFACE:
        {
            
        }break;
        case rpc_msg_cmd_e::CALL_INTERFACE:
        {
            
        }break;
        case rpc_msg_cmd_e::INTREFACE_CALLBACK:
        {
            
        }break;
    }
    return 0;
}

void broker_service_t::create_service_group(create_service_group_t::in_t& in_msg_, rpc_callcack_t<create_service_group_t::out_t>& cb_)
{
    rpc_service_group_t& rsg = m_msg_bus.create_service_group(in_msg_.service_name);

    create_service_group_t::out_t ret;
    ret.service_id = rsg.get_id();

    cb_(ret);
}

void broker_service_t::create_service(create_service_t::in_t& in_msg_, rpc_callcack_t<create_service_t::out_t>& cb_)
{
    rpc_service_group_t* rsg = m_msg_bus.get_service_group(in_msg_.new_service_group_id);

    create_service_t::out_t ret;
    
    if (NULL != rsg)
    {
        rpc_service_t& rs = rsg->create_service(in_msg_.new_service_id);
        rs.set_socket(cb_.get_socket());
        ret.value = true;
    }
    else
    {
        ret.value = false;
    }

    cb_(ret);
}

void broker_service_t::reg_interface(reg_interface_t::in_t& in_msg_, rpc_callcack_t<reg_interface_t::out_t>& cb_)
{
    reg_interface_t::out_t ret;

    rpc_service_group_t* rsg = m_msg_bus.get_service_group(in_msg_.new_service_group_id);
    if (NULL != rsg)
    {
        rpc_service_t* rs = rsg->get_service(in_msg_.new_service_id);
        if (NULL != rs)
        {
            //! ok
            ret.value = true;
        }
    }


    cb_(ret);
}
