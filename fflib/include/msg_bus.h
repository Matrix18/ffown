//! 消息发送管理
#ifndef _RPC_SERVICE_H_
#define _RPC_SERVICE_H_


class rpc_service_t;
class rpc_service_group_t;

class msg_bus_t
{
public:
    rpc_service_group_t* get_service_group(const string& name_);
};

#endif
