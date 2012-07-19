#include "broker_service.h"
#include "log/log.h"
#include "utility/timer_service.h"
#include "utility/performance_daemon.h"

using namespace ff;

broker_service_t::broker_service_t():
    m_uuid(0)
{
}

broker_service_t::~broker_service_t()
{
    
}

int broker_service_t::handle_broken(socket_ptr_t sock_)
{
    logwarn((BROKER, "broker_service_t::handle_broken bagin soket_ptr<%p>", sock_));
    
    vector<uint32_t> del_sgid;
    vector<uint32_t> del_sid;

    service_obj_map_t::iterator it = m_service_obj_mgr.begin();
    for (; it != m_service_obj_mgr.end(); ++it)
    {
        map<uint16_t, service_obj_t>::iterator it2 = it->second.service_objs.begin();
        for (; it2 != it->second.service_objs.end(); ++it2)
        {
            if (it2->second.socket_ptr == sock_)
            {
                del_sgid.push_back(it->first);
                del_sid.push_back(it2->first);
            }
        }
    }
    
    for (size_t i = 0; i < del_sgid.size(); ++i)
    {
        logwarn((BROKER, "broker_service_t::handle_broken del sgid[%u], sid[%u]", del_sgid[i], del_sid[i]));
        m_service_obj_mgr[del_sgid[i]].service_objs.erase(del_sid[i]);
        if (m_service_obj_mgr[del_sgid[i]].service_objs.empty())
        {
            m_service_obj_mgr.erase(del_sgid[i]);
            logwarn((BROKER, "broker_service_t::handle_broken del sgid[%u]", del_sgid[i]));
        }
    }
    
    sock_->safe_delete();
    logwarn((BROKER, "broker_service_t::handle_broken en ok"));
    return 0;
}

int broker_service_t::handle_msg(const message_t& msg_, socket_ptr_t sock_)
{
    
    msg_tool_t msg_tool;
    try{
        msg_tool.decode(msg_.get_body());
    }catch(exception& e_)
    {
        logerror((BROKER, "broker_service_t::handle_msg except<%s>", e_.what()));
        return -1;
    }

    logtrace((BROKER, "broker_service_t::handle_msg begin... cmd[%u], name[%s], sgid[%u], sid[%u]",
                      msg_.get_cmd(), msg_tool.get_name().c_str(), msg_tool.get_group_id(), msg_tool.get_service_id()));
    
    if (msg_tool.get_group_id() == 0 && msg_tool.get_service_id() == 0)
    {
        if (msg_tool.get_name() == "create_service_group_t::in")
        {
                create_service_group_t::in_t in;
                in.decode(msg_.get_body());
                rpc_callcack_t<create_service_group_t::out_t> rcb;
                rcb.set_cmd(rpc_msg_cmd_e::INTREFACE_CALLBACK);
                rcb.set_socket(sock_);
                create_service_group(in, rcb);
        }
        else if (msg_tool.get_name() == "create_service_t::in")
        {
                create_service_t::in_t in;
                in.decode(msg_.get_body());
                rpc_callcack_t<create_service_t::out_t> rcb;
                rcb.set_cmd(rpc_msg_cmd_e::INTREFACE_CALLBACK);
                rcb.set_socket(sock_);
                create_service(in, rcb);
        }
        else if (msg_tool.get_name() == "reg_interface_t::in")
        {
            reg_interface_t::in_t in;
            in.decode(msg_.get_body());
            
            rpc_callcack_t<reg_interface_t::out_t> rcb;
            rcb.set_cmd(rpc_msg_cmd_e::INTREFACE_CALLBACK);
            rcb.set_socket(sock_);

            reg_interface(in, rcb);
        }
    }
    else
    {
        switch (msg_.get_cmd())
        {
            case rpc_msg_cmd_e::CALL_INTERFACE:
            {
                service_obj_t& sobj = m_service_obj_mgr[msg_tool.get_group_id()].service_objs[msg_tool.get_service_id()];

                sobj.async_call(msg_tool, msg_.get_body());
            }break;
            case rpc_msg_cmd_e::INTREFACE_CALLBACK:
            {
                service_obj_t& sobj = m_service_obj_mgr[msg_tool.get_group_id()].service_objs[msg_tool.get_service_id()];
                
                sobj.interface_callback(msg_tool, msg_.get_body());
            }break;
        }
    }

    return 0;
}

void broker_service_t::create_service_group(create_service_group_t::in_t& in_msg_, rpc_callcack_t<create_service_group_t::out_t>& cb_)
{
    logtrace((BROKER, "broker_service_t::create_service_group begin... service_name<%s>", in_msg_.service_name.c_str()));
 
    create_service_group_t::out_t ret;
    service_obj_map_t::iterator it = m_service_obj_mgr.begin();
    
    for (; it != m_service_obj_mgr.end(); ++it)
    {
        if (it->second.name == in_msg_.service_name)
        {
            break;
        }
    }

    if (it != m_service_obj_mgr.end())
    {
        ret.service_id = it->second.id;
        loginfo((BROKER, "broker_service_t::create_service_group begin... service_name<%s> has exist", in_msg_.service_name.c_str()));
    }
    else
    {
        service_obj_mgr_t obj_mgr;
        obj_mgr.id = ++m_uuid;
        obj_mgr.name = in_msg_.service_name;
        //obj_mgr.socket_ptr = cb_.get_socket();
        ret.service_id = obj_mgr.id;
        m_service_obj_mgr.insert(make_pair(obj_mgr.id, obj_mgr));
    }

    ret.set_uuid(in_msg_.get_uuid());
    cb_(ret);
}

void broker_service_t::create_service(create_service_t::in_t& in_msg_, rpc_callcack_t<create_service_t::out_t>& cb_)
{
    logtrace((BROKER, "broker_service_t::create_service begin... sgid<%u>, sid[%u]", in_msg_.new_service_group_id, in_msg_.new_service_id));

    create_service_t::out_t ret;
    service_obj_mgr_t& som = m_service_obj_mgr[in_msg_.new_service_group_id];
    map<uint16_t, service_obj_t>::iterator it = som.service_objs.find(in_msg_.new_service_id);

    if (it != som.service_objs.end())
    {
        ret.value = false;

        logerror((BROKER, "broker_service_t::create_service failed... sgid<%u>, sid[%u] exist", in_msg_.new_service_group_id, in_msg_.new_service_id));
    }
    else
    {
        service_obj_t obj;
        obj.name = som.name;
        obj.group_id = in_msg_.new_service_group_id;
        obj.id       = in_msg_.new_service_id;
        obj.socket_ptr = cb_.get_socket();

        som.service_objs[in_msg_.new_service_id] = obj;
        
        ret.value = true;
    }
    ret.set_uuid(in_msg_.get_uuid());
    cb_(ret);
}

void broker_service_t::reg_interface(reg_interface_t::in_t& in_msg_, rpc_callcack_t<reg_interface_t::out_t>& cb_)
{
    reg_interface_t::out_t ret;
    ret.alloc_id = -1;
    ret.set_uuid(in_msg_.get_uuid());

    logtrace((BROKER, "broker_service_t::reg_interface sgid[%u], sid[%u]", in_msg_.sgid, in_msg_.sid));

    service_obj_mgr_t& som = m_service_obj_mgr[in_msg_.sgid];
    map<uint16_t, service_obj_t>::iterator it = som.service_objs.find(in_msg_.sid);
    
    if (it != som.service_objs.end())
    {
        ret.alloc_id = ++ m_uuid;
        
        logtrace((BROKER, "broker_service_t::reg_interface sgid[%u], sid[%u] alloc_id[%u]", in_msg_.sgid, in_msg_.sid, ret.alloc_id));
    }

    cb_(ret);
}

void broker_service_t::service_obj_t::async_call(msg_i& msg_, const string& body_)
{
    proc_stack_t stack;
    struct timeval now;
    gettimeofday(&now, NULL);
    
    stack.start_time = now.tv_sec*1000000 + now.tv_usec;

    stack.uuid    = msg_.get_uuid();
    stack.req_msg = msg_.get_name(); 

    uint32_t uuid = ++m_uuid;

    //! 直接修改消息包内的uuid
    string dest = body_;
    *((uint32_t*)dest.data()) = uuid;

    m_callback_map[uuid] = stack;
    msg_sender_t::send(socket_ptr, rpc_msg_cmd_e::CALL_INTERFACE , dest);
    logtrace((BROKER, "broker_service_t::service_obj_t::async_call socket<%p>", socket_ptr));
    
}

int broker_service_t::service_obj_t::interface_callback(msg_i& msg_, const string& body_)
{
    callback_map_t::iterator it = m_callback_map.find(msg_.get_uuid());
    if (it != m_callback_map.end())
    {
        string dest = body_;
        *((uint32_t*)dest.data()) = it->second.uuid;

        msg_sender_t::send(socket_ptr, rpc_msg_cmd_e::INTREFACE_CALLBACK, dest);
        
        struct timeval now;
        gettimeofday(&now, NULL);

        long cost = now.tv_sec*1000000 + now.tv_usec - it->second.start_time;

        singleton_t<performance_daemon_t>::instance().post(it->second.req_msg, cost);
        m_callback_map.erase(it);
        return 0;
    }
    else
    {
        logerror((BROKER, "broker_service_t::service_obj_t::interface_callback none uuid"));
    }

    return -1;
}
