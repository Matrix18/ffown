#include "rpc_service.h"
#include "msg_bus.h"

using namespace ff;

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

void rpc_service_t::async_call(msg_i& msg_, uint16_t msg_id_, callback_wrapper_i* callback_)
{
    cout <<"rpc_service_t::async_call msg_id:"<< msg_id_ <<"\n";
    uint32_t uuid = ++m_uuid;
    msg_.set(m_service_group_id, m_service_id, uuid, msg_id_);
    
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
    else
    {
        cout <<"rpc_service_t::interface_callback\n";
    }
    return -1;
}

int rpc_service_t::call_interface(uint32_t interface_name_, const string& msg_buff_, socket_ptr_t socket_)
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

        cout << "rpc_service_t::call_interface none msg id:" << interface_name_ <<"\n";
        rcb.exe("interface not existed");
    }
    catch (exception& e_)
    {
        rcb.exe(e_.what());
    }
    return -1;
}

int rpc_service_t::add_interface(const string& in_name_, const string& out_name_, msg_process_func_i* func_)
{
    int alloc_id = m_msg_bus->register_interface(in_name_, out_name_, get_group_id(), get_id());

    assert(alloc_id > 0 && func_ && m_interface_map.insert(make_pair(alloc_id, func_)).second == true  && "interface has existed");

    m_name_to_id[in_name_] = alloc_id;
    return 0; 
}
