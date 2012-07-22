#include "msg_bus.h"
#include "log/log.h"

using namespace ff;

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
    in.set_msg_id(rpc_msg_cmd_e::CREATE_SERVICE_GROUP);
    in.service_name = name_;
    const create_service_group_t::out_t& out = rpc_future.call(m_broker_service, in);
    
    logtrace((MSG_BUS, "msg_bus_t::create_service_group out sgid[%u]", out.service_id));

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
    if (m_socket == sock_)
    {
        m_socket = NULL;
    }

    sock_->safe_delete();
    return 0;    
}

int msg_bus_t::handle_msg(const message_t& msg_, socket_ptr_t sock_)
{
    msg_tool_t msg_tool;

    try
    {
        msg_tool.decode(msg_.get_body());

        logdebug((MSG_BUS, "msg_bus_t::handle_msg: cmd[%u], msg_name<%s>, sgid[%u], sig[%u]",
                            msg_.get_cmd(), msg_tool.get_name().c_str(),
                            msg_tool.get_group_id(), msg_tool.get_service_id()));

        if (msg_tool.get_group_id() == 0 && msg_tool.get_service_id() == 0)
        {
            m_broker_service->interface_callback(msg_tool.get_uuid(), msg_.get_body());
            return 0;
        }

        rpc_service_group_t* rsg = get_service_group(msg_tool.get_group_id());
        if (NULL == rsg)
        {
            logerror((MSG_BUS, "msg_bus_t::handle_msg: none sgid cmd[%u], msg_name<%s>, sgid[%u], sig[%u]",
                      msg_.get_cmd(), msg_tool.get_name().c_str(),
                      msg_tool.get_group_id(), msg_tool.get_service_id()));
            return -1;
        }

        rpc_service_t* rs = rsg->get_service(msg_tool.get_service_id());
        if (NULL == rs)
        {
            logerror((MSG_BUS, "msg_bus_t::handle_msg: none sid cmd[%u], msg_name<%s>, sgid[%u], sig[%u]",
                      msg_.get_cmd(), msg_tool.get_name().c_str(),
                      msg_tool.get_group_id(), msg_tool.get_service_id()));
            return -1;
        }

        if (rpc_msg_cmd_e::CALL_INTERFACE == msg_.get_cmd())
        {
            rs->call_interface(msg_tool.get_msg_id(), msg_.get_body(), sock_);
        }
        else if (rpc_msg_cmd_e::INTREFACE_CALLBACK == msg_.get_cmd())
        {
            rs->interface_callback(msg_tool.get_uuid(), msg_.get_body());
        }
        else
        {
            logerror((MSG_BUS, "msg_bus_t::handle_msg: none cmd==>cmd[%u], msg_name<%s>, sgid[%u], sig[%u]",
                      msg_.get_cmd(), msg_tool.get_name().c_str(),
                      msg_tool.get_group_id(), msg_tool.get_service_id()));
            return -1;
        }
    }
    catch (exception& e_)
    {
        logerror((MSG_BUS, "msg_bus_t::handle_msg: exception<%s> cmd[%u], msg_name<%s>, sgid[%u], sig[%u]",
                  e_.what(), msg_.get_cmd(), msg_tool.get_name().c_str(),
                  msg_tool.get_group_id(), msg_tool.get_service_id()));

        return -1;
    }
    return 0;
}

int msg_bus_t::open(const string& host_)
{
    m_socket = net_factory_t::connect(host_, this);
    if (NULL == m_socket)
    {
        logerror((MSG_BUS, "msg_bus_t::open failed, can't connect to remote broker<%s>", host_.c_str()));
        return -1;
    }
 
    m_broker_service = new rpc_service_t(this, 0, 0);
    
    rpc_future_t<sync_all_service_t::out_t> rpc_future;
    sync_all_service_t::in_t in;

    const sync_all_service_t::out_t& out = rpc_future.call(m_broker_service, in);

    for (size_t i = 0; i< out.group_name_vt.size(); ++i)
    {
        rpc_service_group_t* rsg = new rpc_service_group_t(this, out.group_name_vt[i], out.group_id_vt[i]);
        m_service_map[rsg->get_id()] = rsg;
        
        logtrace((MSG_BUS, "msg_bus_t::open add service<%s>", out.group_name_vt[i].c_str()));
    }

    for (size_t i = 0; i< out.id_info_vt.size(); ++i)
    {
        rpc_service_t* rs = new rpc_service_t(this, out.id_info_vt[i].sgid, out.id_info_vt[i].sid);
        get_service_group(out.id_info_vt[i].sgid)->add_service(out.id_info_vt[i].sid, rs);
    }
    
    for (size_t i = 0; i< out.msg_name_vt.size(); ++i)
    {
        singleton_t<msg_name_store_t>::instance().add_msg(out.msg_name_vt[i], out.msg_id_vt[i]);
        logtrace((MSG_BUS, "msg_bus_t::open add interface<%s>, msgid[%u]", out.msg_name_vt[i].c_str(), out.msg_id_vt[i]));
    }
    return 0;
}

int msg_bus_t::close()
{
    if (m_socket)
    {
        m_socket->close();
    }
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

    logtrace((MSG_BUS, "msg_bus_t::register_service ret value[%d]", out.value));
    return out.value == true? 0: -1;
}

int msg_bus_t::register_interface(const string& in_name_, const string& out_name_, uint16_t gid_, uint16_t id_, uint16_t& in_alloc_id_, uint16_t& out_alloc_id_)
{
    rpc_future_t<reg_interface_t::out_t> rpc_future;
    reg_interface_t::in_t in;

    in.sgid = gid_;
    in.sid  = id_;
    in.in_msg_name  = in_name_;
    in.out_msg_name = out_name_;

    const reg_interface_t::out_t& out = rpc_future.call(m_broker_service, in);

    in_alloc_id_  = out.alloc_id;
    out_alloc_id_ = out.out_alloc_id;
    logtrace((MSG_BUS, "msg_bus_t::register_interface ret in<%s>==>[%d], out<%s>==>[%u]", in_name_.c_str(), out.alloc_id, out_name_.c_str(), out_alloc_id_));

    return 0;
}
