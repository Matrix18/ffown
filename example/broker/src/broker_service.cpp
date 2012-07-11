#include "broker_service.h"

broker_service_t::broker_service_t():
    m_uuid(0)
{
    /*
    m_msg_bus.create_service("broker_service", 1)
             .bind_service(this)
             .reg(&broker_service_t::create_service_group)
             .reg(&broker_service_t::create_service)
             .reg(&broker_service_t::reg_interface);
    */
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
    msg_tool_t msg_tool;
    msg_tool.decode(msg_.get_body());
    cout <<"handle:" << msg_.get_cmd() << ":" << msg_tool.get_name()<<"\n";
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
            //rcb.set_cmd(rpc_msg_cmd_e::CREATE_SERVICE_GROUP);
            rcb.set_socket(sock_);
            create_service(in, rcb);
    }
    /*
        case rpc_msg_cmd_e::REG_INTERFACE:
        {
            reg_interface_t::in_t in;
            in.decode(msg_.get_body());
        }break;
        case rpc_msg_cmd_e::CALL_INTERFACE:
        {
            msg_tool_t msg_tool;
            msg_tool.decode(msg_.get_body());
            service_obj_t& sobj = m_service_obj_mgr[msg_tool.get_group_id()].service_objs[msg_tool.get_service_id()];

            msg_sender_t::send(sobj.socket_ptr, rpc_msg_cmd_e::CALL_INTERFACE, msg_.get_body());
        }break;
        case rpc_msg_cmd_e::INTREFACE_CALLBACK:
        {
            msg_tool_t msg_tool;
            msg_tool.decode(msg_.get_body());
            service_obj_t& sobj = m_service_obj_mgr[msg_tool.get_group_id()].service_objs[msg_tool.get_service_id()];
            
            msg_sender_t::send(sobj.socket_ptr, rpc_msg_cmd_e::INTREFACE_CALLBACK, msg_.get_body());
        }break;
    }
     */
    return 0;
}

void broker_service_t::create_service_group(create_service_group_t::in_t& in_msg_, rpc_callcack_t<create_service_group_t::out_t>& cb_)
{
    create_service_group_t::out_t ret;
    cout <<"broker_service_t::create_service_group name:" << in_msg_.service_name <<"\n";
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
    }
    else
    {
        service_obj_mgr_t obj_mgr;
        obj_mgr.id = ++m_uuid;
        obj_mgr.name = in_msg_.service_name;
        obj_mgr.socket_ptr = cb_.get_socket();
        ret.service_id = obj_mgr.id;
        m_service_obj_mgr.insert(make_pair(obj_mgr.id, obj_mgr));
    }

    ret.set_uuid(in_msg_.get_uuid());
    cb_(ret);
}

void broker_service_t::create_service(create_service_t::in_t& in_msg_, rpc_callcack_t<create_service_t::out_t>& cb_)
{
    create_service_t::out_t ret;
    service_obj_mgr_t& som = m_service_obj_mgr[in_msg_.new_service_group_id];
    map<uint16_t, service_obj_t>::iterator it = som.service_objs.find(in_msg_.new_service_id);
    
    cout <<"broker_service_t::create_service gid:" << in_msg_.new_service_group_id << ":" << in_msg_.new_service_id <<"\n";
    if (it != som.service_objs.end())
    {
        ret.value = false;
    }
    else
    {
        service_obj_t obj;
        obj.name = som.name;
        obj.group_id = som.id;
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
