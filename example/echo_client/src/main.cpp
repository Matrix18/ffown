#include <iostream>
using namespace std;

#include "msg_def.h"
#include "msg_bus.h"
#include "log/log.h"

using namespace ff;

int g_count = 0;
int main(int argc, char* argv[])
{
    
    char buff[128];
    snprintf(buff, sizeof(buff), "tcp://%s:%s", "127.0.0.1", "10241");

    assert(0 == singleton_t<msg_bus_t>::instance().open(buff));

    assert(singleton_t<msg_bus_t>::instance().get_service_group("echo") && "echo service group not exist");

    assert(singleton_t<msg_bus_t>::instance().get_service_group("echo")->get_service(1) && "echo servie 1 not exist");

    sleep(1);
    echo_t::in_t in;
    in.value = "XXX_echo_test_XXX";

    
    struct lambda_t
    {
        static void callback(echo_t::out_t& msg_)
        {
            if (g_count % 5000 == 0)
            {
                loginfo((FF, "index[%d] echo callback msg value<%s>", g_count, msg_.value.c_str()));
            }

            if (++g_count > 500000) {
                return;
            }
            
            echo_t::in_t in;
            in.value = "XXX_echo_test_XXX";
            
            singleton_t<msg_bus_t>::instance().get_service_group("echo")->get_service(1)->async_call(in, &lambda_t::callback);
        }
        
    };
    singleton_t<msg_bus_t>::instance().get_service_group("echo")->get_service(1)->async_call(in, &lambda_t::callback);

    signal_helper_t::wait();
    singleton_t<msg_bus_t>::instance().close();
    cout <<"\noh end\n";
    
    return 0;
}
