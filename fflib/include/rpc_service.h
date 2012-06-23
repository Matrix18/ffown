//!  rpc 异步服务
//! 接口
#ifndef _RPC_SERVICE_H_
#define _RPC_SERVICE_H_

#include <assert.h>
#include <map>
#include <string>
using namespace std;

#include "net_factory.h"
#include "rpc_callback.h"

class rpc_service_t
{
    typedef map<uint32_t, callback_wrapper_i*>    callback_map_t;
    typedef map<string, msg_process_func_i*>      interface_map_t;
public:
    rpc_service_t(uint16_t service_group_id_, uint16_t servie_id_, socket_ptr_t socket_ = NULL);
    ~rpc_service_t();
    uint16_t get_group_id() const;
    uint16_t get_id() const;

    void async_call(msg_i& msg_, callback_wrapper_i* callback_);
    template<typename RET, typename MSGT>
    void async_call(msg_i& msg_, RET (*callback_)(MSGT&));

    template <typename IN_MSG, typename RET, typename OUT_MSG>
    rpc_service_t& reg(RET (*interface_)(IN_MSG&, rpc_callcack_t<OUT_MSG>&));
    template <typename IN_MSG, typename RET, typename T, typename OUT_MSG>
    rpc_service_t& reg(RET (T::*interface_)(IN_MSG&, rpc_callcack_t<OUT_MSG>&), T* obj_);

    rpc_service_t& bind_service(void* p_) { m_bind_service_ptr = p_;  return *this; }
    template <typename IN_MSG, typename RET, typename T, typename OUT_MSG>
    rpc_service_t& reg(RET (T::*interface_)(IN_MSG&, rpc_callcack_t<OUT_MSG>&)) { return reg(interface_, (T*)m_bind_service_ptr); }

    int interface_callback(uint32_t uuid_, const string& buff_);
    int call_interface(const string& interface_name_, const string& msg_buff_, socket_ptr_t sock_);

    void set_socket(socket_ptr_t socket_);
    socket_ptr_t get_socket() const;
private:
    uint16_t        m_service_group_id;
    uint16_t        m_service_id;
    uint32_t        m_uuid;
    socket_ptr_t    m_socket;
    callback_map_t  m_callback_map;
    interface_map_t m_interface_map;
    void*           m_bind_service_ptr;
};

template<typename RET, typename MSGT>
void rpc_service_t::async_call(msg_i& msg_, RET (*callback_)(MSGT&))
{
    this->async_call(msg_, new callback_wrapper_cfunc_impl_t<RET, MSGT>(callback_));
}

template <typename IN_MSG, typename RET, typename OUT_MSG>
rpc_service_t& rpc_service_t::reg(RET (*interface_)(IN_MSG&, rpc_callcack_t<OUT_MSG>&))
{
    IN_MSG msg;
    const string& msg_name = msg.get_name();
    msg_process_func_i* msg_process_func = new msg_process_func_impl_t<IN_MSG, RET, OUT_MSG>(interface_);
    assert(m_interface_map.insert(make_pair(msg_name, msg_process_func)).second == true  && "interface has existed");
    return *this;
}

template <typename IN_MSG, typename RET, typename T, typename OUT_MSG>
rpc_service_t& rpc_service_t::reg(RET (T::*interface_)(IN_MSG&, rpc_callcack_t<OUT_MSG>&), T* obj_)
{
    IN_MSG msg;
    const string& msg_name = msg.get_name();
    msg_process_func_i* msg_process_func = new msg_process_class_func_impl_t<IN_MSG, RET, T, OUT_MSG>(interface_, obj_);
    assert(obj_ && m_interface_map.insert(make_pair(msg_name, msg_process_func)).second == true  && "interface has existed");
    return *this;
}

#endif
