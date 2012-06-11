//! rpc 服务组
#ifndef _RPC_SERVICE_GROUP_H_
#define _RPC_SERVICE_GROUP_H_

class rpc_service_t;

class rpc_service_group_t
{
public:
    uint32_t get_id();
    rpc_service_t* get_service(uint32_t id_);
};

#endif