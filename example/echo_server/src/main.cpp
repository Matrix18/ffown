#include <iostream>
using namespace std;

#include "msg_def.h"
#include "msg_bus.h"

using namespace ff;

struct echo_service_t
{
public:
    void echo(echo_t::in_t& in_msg_, rpc_callcack_t<echo_t::out_t>& cb_)
    {
        cout << "echo_service_t::echo done:" << in_msg_.value << "\n";
        echo_t::out_t out;
        out.value = in_msg_.value;
        cb_(out);
    }
};

rpc_service_t* g_rpc_service = NULL;
int main(int argc, char* argv[])
{
    
    char buff[128];
    snprintf(buff, sizeof(buff), "tcp://%s:%s", "127.0.0.1", "10241");
    
    msg_bus_t msg_bus;
    assert(0 == msg_bus.open("tcp://127.0.0.1:10241") && "can't connnect to broker");

    echo_service_t f;

    msg_bus.create_service_group("echo");
    msg_bus.create_service("echo", 1)
            .bind_service(&f)
            .reg(&echo_service_t::echo);
    
    signal_helper_t::wait();

    cout <<"\noh end\n";
    return 0;
}
