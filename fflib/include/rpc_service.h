//!  rpc 异步服务
//! 接口
#ifndef _RPC_SERVICE_H_
#define _RPC_SERVICE_H_

#include <assert.h>
#include <map>
using namespace std;

#include "net_factory.h"
#include "rpc_callback.h"

class rpc_service_t
{
    enum rpc_msg_cmd_e
    {
        CALL_INTERFACE     = 1,
        INTREFACE_CALLBACK = 2
    };
    typedef map<uint32_t, callback_wrapper_i*>    callback_map_t;
    typedef map<string, msg_process_func_i*>  interface_map_t;
public:
    rpc_service_t();
    uint32_t get_id();

    void async_call(msg_i& msg_, callback_wrapper_i* callback_);
    template<typename RET, typename MSGT>
    void async_call(msg_i& msg_, RET (*callback_)(MSGT&));

    rpc_service_t& bind_service(void* p_) { m_bind_service_ptr = p_;  return *this; }
    template <typename IN_MSG, typename RET>
    rpc_service_t& reg(RET (*interface_)(IN_MSG&, rpc_callcack_t&));
    template <typename IN_MSG, typename RET, typename T>
    rpc_service_t& reg(RET (T::*interface_)(IN_MSG&, rpc_callcack_t&), T* obj_);
    template <typename IN_MSG, typename RET, typename T>
    rpc_service_t& reg(RET (T::*interface_)(IN_MSG&, rpc_callcack_t&)) { return reg(interface_, (T*)m_bind_service_ptr); }

    int interface_callback(uint32_t uuid_, const string& buff_);
    int call_interface(const string& interface_name_, const string& msg_buff_, socket_ptr_t sock_);

private:
    uint16_t        m_service_group_id;
    uint16_t        m_service_id;
    uint32_t        m_uuid;
    socket_ptr_t    m_socket;
    callback_map_t  m_callback_map;
    interface_map_t m_interface_map;
    void*           m_bind_service_ptr;
};

rpc_service_t::rpc_service_t():
    m_service_group_id(0),
    m_service_id(0),
    m_uuid(0),
    m_socket(NULL),
    m_bind_service_ptr(NULL)
{
}

void rpc_service_t::async_call(msg_i& msg_, callback_wrapper_i* callback_)
{
    uint32_t uuid = ++m_uuid;
    msg_.set(m_service_group_id, m_service_id, uuid);
    
    m_callback_map[uuid] = callback_;
    msg_sender_t::send(m_socket, CALL_INTERFACE, msg_);
}

template<typename RET, typename MSGT>
void rpc_service_t::async_call(msg_i& msg_, RET (*callback_)(MSGT&))
{
    this->async_call(msg_, new callback_wrapper_cfunc_impl_t<RET, MSGT>(callback_));
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

template <typename IN_MSG, typename RET>
rpc_service_t& rpc_service_t::reg(RET (*interface_)(IN_MSG&, rpc_callcack_t&))
{
    IN_MSG msg;
    const string& msg_name = msg.get_name();
    msg_process_func_i* msg_process_func = new msg_process_func_impl_t<IN_MSG, RET>(interface_);
    assert(m_interface_map.insert(make_pair(msg_name, msg_process_func)).second == true  && "interface has existed");
    return *this;
}

template <typename IN_MSG, typename RET, typename T>
rpc_service_t& rpc_service_t::reg(RET (T::*interface_)(IN_MSG&, rpc_callcack_t&), T* obj_)
{
    IN_MSG msg;
    const string& msg_name = msg.get_name();
    msg_process_func_i* msg_process_func = new msg_process_class_func_impl_t<IN_MSG, RET, T>(interface_, obj_);
    assert(obj_ && m_interface_map.insert(make_pair(msg_name, msg_process_func)).second == true  && "interface has existed");
    return *this;
}

int rpc_service_t::call_interface(const string& interface_name_, const string& msg_buff_, socket_ptr_t socket_)
{
    rpc_callcack_t rcb;
    rcb.set_cmd(INTREFACE_CALLBACK);
    rcb.set_socket(socket_);

    try
    {
        interface_map_t::const_iterator it = m_interface_map.find(interface_name_);
        if (it != m_interface_map.end())
        {
            it->second->exe(msg_buff_, rcb);
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
#endif
