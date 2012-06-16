//! rpc callback 声明
#ifndef _RPC_CALLBACK_H_
#define _RPC_CALLBACK_H_

#include <string>
using namespace std;

#include "socket_i.h"
#include "msg_sender.h"

class rpc_callcack_t
{
public:
    rpc_callcack_t();
    void exe(const string& msg_);
    void exe(msg_i& msg_);
    void set_socket(socket_ptr_t socket_);
    void set_cmd(uint16_t cmd_);
private:
    socket_ptr_t m_socket;
    uint16_t     m_cmd;
};

rpc_callcack_t::rpc_callcack_t():
    m_socket(NULL),
    m_cmd(0)
{
}

void rpc_callcack_t::exe(const string& msg_)
{
    msg_sender_t::send(m_socket, 100, msg_);
}

void rpc_callcack_t::exe(msg_i& msg_)
{
    msg_sender_t::send(m_socket, 100, msg_);
}

void rpc_callcack_t::set_socket(socket_ptr_t socket_)
{
    m_socket = socket_;
}
void rpc_callcack_t::set_cmd(uint16_t cmd_)
{
    m_cmd = cmd_;
}

struct msg_process_func_i
{
    virtual ~msg_process_func_i(){}
    virtual void exe(const string& msg_, rpc_callcack_t& cb_) = 0;
};

template <typename IN_MSG, typename RET>
struct msg_process_func_impl_t: public msg_process_func_i
{
    msg_process_func_impl_t(RET (*interface_)(IN_MSG&, rpc_callcack_t&)):
        m_interface(interface_)
    {
    }
    virtual void exe(const string& msg_, rpc_callcack_t& cb_)
    {
        IN_MSG in_msg;
        in_msg.decode(msg_);
        (*m_interface)(in_msg, cb_);
    }
    RET (*m_interface)(IN_MSG&, rpc_callcack_t&);
};

template <typename IN_MSG, typename RET, typename T>
struct msg_process_class_func_impl_t: public msg_process_func_i
{
    msg_process_class_func_impl_t(RET (T::*interface_)(IN_MSG&, rpc_callcack_t&), T* obj_):
        m_interface(interface_),
        m_obj(obj_)
    {
    }
    virtual void exe(const string& msg_, rpc_callcack_t& cb_)
    {
        IN_MSG in_msg;
        in_msg.decode(msg_);
        (*m_interface)(in_msg, cb_);
    }
    RET (T::*m_interface)(IN_MSG&, rpc_callcack_t&);
    T* m_obj;
};

struct callback_wrapper_i
{
    virtual ~callback_wrapper_i() {}
    virtual void callback(const string& msg_buff_) = 0;
};

template <typename RET, typename MSGT>
struct callback_wrapper_cfunc_impl_t
{
    callback_wrapper_cfunc_impl_t(RET (*callback_)(MSGT&)):
        m_callback(callback_)
    {}
    virtual void callback(const string& msg_buff_)
    {
        MSGT msg;
        try
        {
            msg.decode(msg_buff_);
        }
        catch (exception& e_){}
    
        (*m_callback)(msg);
    }
    RET (*m_callback)(MSGT&);
};
#endif
