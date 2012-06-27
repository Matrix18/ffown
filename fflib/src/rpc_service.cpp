#include "rpc_service.h"
#include "msg_bus.h"

rpc_service_t::rpc_service_t(msg_bus_t* mb_, uint16_t service_group_id_, uint16_t servie_id_):
    m_service_group_id(service_group_id_),
    m_service_id(servie_id_),
    m_uuid(0),
    m_bind_service_ptr(NULL),
    m_msg_bus(mb_)
{
}

rpc_service_t::~rpc_service_t()
{
    for (interface_map_t::iterator it = m_interface_map.begin(); it != m_interface_map.end(); ++it)
    {
        delete it->second;
    }
    m_interface_map.clear();
}

uint16_t rpc_service_t::get_group_id() const
{
    return m_service_group_id;
}

uint16_t rpc_service_t::get_id() const
{
    return m_service_id;
}

socket_ptr_t rpc_service_t::get_socket() const
{
    return m_msg_bus->get_socket(this);
}

void rpc_service_t::async_call(msg_i& msg_, callback_wrapper_i* callback_)
{
    uint32_t uuid = ++m_uuid;
    msg_.set(m_service_group_id, m_service_id, uuid);
    
    m_callback_map[uuid] = callback_;
    msg_sender_t::send(get_socket(), rpc_msg_cmd_e::CALL_INTERFACE, msg_);
}

int rpc_service_t::interface_callback(uint32_t uuid_, const string& buff_)
{
    callback_map_t::iterator it = m_callback_map.find(uuid_);
    if (it != m_callback_map.end())
    {
        it->second->callback(buff_);
        
        delete it->second;
        m_callback_map.erase(it);
        return 0;
    }
    return -1;
}

int rpc_service_t::call_interface(const string& interface_name_, const string& msg_buff_, socket_ptr_t socket_)
{
    rpc_callcack_base_t rcb;
    rcb.set_cmd(rpc_msg_cmd_e::INTREFACE_CALLBACK);
    rcb.set_socket(socket_);

    try
    {
        interface_map_t::const_iterator it = m_interface_map.find(interface_name_);
        if (it != m_interface_map.end())
        {
            it->second->exe(msg_buff_, rpc_msg_cmd_e::INTREFACE_CALLBACK, socket_);
            return 0;
        }

        rcb.exe("interface not existed");
    }
    catch (exception& e_)
    {
        rcb.exe(e_.what());
    }
    return -1;
}
