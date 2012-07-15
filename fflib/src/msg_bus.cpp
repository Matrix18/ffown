#include "msg_bus.h"


msg_bus_t::msg_bus_t():
    m_uuid(0),
    m_broker_service(NULL)
{
}

msg_bus_t::~msg_bus_t()
{
    for (service_map_t::iterator it = m_service_map.begin(); it != m_service_map.end(); ++it)
    {
        delete it->second;
    }
    m_service_map.clear();
}

rpc_service_group_t& msg_bus_t::create_service_group(const string& name_)
{
    for (service_map_t::iterator it = m_service_map.begin(); it != m_service_map.end(); ++it)
    {
        if (name_ == it->second->get_name())
        {
            return *(it->second);
        }
    }
    
    rpc_future_t<create_service_group_t::out_t> rpc_future;
    create_service_group_t::in_t in;
    in.service_name = name_;
    const create_service_group_t::out_t& out = rpc_future.call(m_broker_service, in);
    cout << "msg_bus_t::create_service_group out:" << out.service_id <<"\n";

    rpc_service_group_t* rsg = new rpc_service_group_t(this, name_, out.service_id);
    m_service_map[rsg->get_id()] = rsg;
    return *rsg;
}

rpc_service_t& msg_bus_t::create_service(const string& name_, uint16_t id_)
{
    rpc_service_group_t& rsg = create_service_group(name_);
    return rsg.create_service(id_);
}

rpc_service_group_t* msg_bus_t::get_service_group(uint16_t id_)
{
    service_map_t::iterator it = m_service_map.find(id_);
    if (it != m_service_map.end())
    {
        return it->second;
    }
    return NULL;
}

rpc_service_group_t* msg_bus_t::get_service_group(const string& name_)
{
    service_map_t::iterator it = m_service_map.begin();
    for (; it != m_service_map.end(); ++it)
    {
        if (it->second->get_name() == name_)
        {
            return it->second;
        }
    }
    return NULL;
}

int msg_bus_t::handle_broken(socket_ptr_t sock_)
{
    sock_->safe_delete();
    return 0;    
}

int msg_bus_t::handle_msg(const message_t& msg_, socket_ptr_t sock_)
{
    msg_tool_t msg_tool;

    try
    {
        msg_tool.decode(msg_.get_body());
        cout <<"msg_bus_t::handle_msg:" << msg_.get_cmd() << ":" << msg_tool.get_name()<<"gid:"<< msg_tool.get_group_id()  << " id:"<< msg_tool.get_service_id() <<"\n";
        rpc_service_group_t* rsg = get_service_group(msg_tool.get_group_id());
        if (NULL == rsg)
        {
            cout <<msg_tool.get_group_id()<< " ==>msg_bus_t::handle_msg 111...\n";
            m_broker_service->interface_callback(msg_tool.get_uuid(), msg_.get_body());
            //sock_->close();
            return -1;
        }
        rpc_service_t* rs = rsg->get_service(msg_tool.get_service_id());
        if (NULL == rs)
        {
            cout <<__FUNCTION__<< " 222...\n";
            sock_->close();
            return -1;
        }
        if (rpc_msg_cmd_e::CALL_INTERFACE == msg_.get_cmd())
        {
            rs->call_interface(msg_tool.get_name(), msg_.get_body(), sock_);
        }
        else if (rpc_msg_cmd_e::INTREFACE_CALLBACK == msg_.get_cmd())
        {
            rs->interface_callback(msg_tool.get_uuid(), msg_.get_body());
        }
        else
        {
            cout <<__FUNCTION__<< " 333...\n";
            return -1;
        }
    }
    catch (exception& e_)
    {
        cout <<"msg_bus_t::handle_msg excep:" << e_.what() <<"\n";
        sock_->close();
        return -1;
    }
    return 0;
}

int msg_bus_t::open(const string& host_)
{
    m_socket = net_factory_t::connect(host_, this);
    if (NULL == m_socket)
    {
        return -1;
    }
 
    m_broker_service = new rpc_service_t(this, 0, 0);
    return 0;
}

socket_ptr_t msg_bus_t::get_socket(const rpc_service_t* rs_)
{
    return m_socket;
}

int msg_bus_t::register_service(const string& name_, uint16_t gid_, uint16_t id_)
{
    rpc_future_t<create_service_t::out_t> rpc_future;
    create_service_t::in_t in;
    in.new_service_group_id = gid_;
    in.new_service_id = id_;
    const create_service_t::out_t& out = rpc_future.call(m_broker_service, in);
    cout << "msg_bus_t::register_service:" << out.value <<"\n";
    return out.value == true? 0: -1;
}

