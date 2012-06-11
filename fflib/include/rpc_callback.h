//! rpc callback 声明
#ifndef _RPC_CALLBACK_H_
#define _RPC_CALLBACK_H_

#include <string>
using namespace std;

class rpc_callcack_t
{
public:
    int exe(const string& msg_);
};

int rpc_callcack_t::exe(const string& msg_)
{
    return 0;
}
#endif
