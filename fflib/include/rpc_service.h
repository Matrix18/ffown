//!  rpc 异步服务
//! 接口
#ifndef _RPC_SERVICE_H_
#define _RPC_SERVICE_H_

#include <map>
using namespace std;

#include "net_factory.h"
#include "rpc_callback.h"

class rpc_service_t
{
    typedef map<uint32_t, rpc_callcack_t*>    callback_map_t;
public:
    rpc_service_t();
    uint32_t get_id();
    void async_call(msg_i& codec_, rpc_callcack_t* callback_);

    void callback(uint32_t uuid_, const string& buff_);

private:
    uint16_t        m_service_group_id;
    uint16_t        m_service_id;
    uint32_t        m_uuid;
    socket_ptr_t    m_socket;
    callback_map_t  m_callback_map;
};

rpc_service_t::rpc_service_t():
    m_service_group_id(0),
    m_service_id(0),
    m_uuid(1),
    m_socket(NULL)
{
}

void rpc_service_t::async_call(msg_i& msg_, rpc_callcack_t* callback_)
{
    uint32_t uuid = m_uuid ++;
    msg_.set(m_service_group_id, m_service_id, uuid);
    m_callback_map[uuid] = callback_;
    msg_sender_t::send(m_socket, 100, msg_);
}

void rpc_service_t::callback(uint32_t uuid_, const string& buff_)
{
    callback_map_t::iterator it = m_callback_map.find(uuid_);
    if (it != m_callback_map.end())
    {
        it->second->exe(buff_);
        delete it->second;
        m_callback_map.erase(it);
    }
}
#endif
