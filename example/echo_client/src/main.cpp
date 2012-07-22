#include <iostream>
using namespace std;

#include "msg_def.h"
#include "msg_bus.h"

using namespace ff;

int g_count = 0;
int main(int argc, char* argv[])
{
    
    char buff[128];
    snprintf(buff, sizeof(buff), "tcp://%s:%s", "127.0.0.1", "10241");

    assert(0 == singleton_t<msg_bus_t>::instance().open(buff));

    struct lambda_t
    {
        static void callback(echo_t::out_t& msg_)
        {
            cout <<g_count << " echo callback:" << msg_.value <<"\n";

            if (++g_count > 1000) {
                return;
            }

            echo_t::in_t in;
            in.value = "XXX_echo_test_XXX";

            singleton_t<msg_bus_t>::instance().get_service_group("echo")->get_service(1)->async_call(in, &lambda_t::callback);
        }

    };

    assert(singleton_t<msg_bus_t>::instance().get_service_group("echo") && "echo service group not exist");

    assert(singleton_t<msg_bus_t>::instance().get_service_group("echo")->get_service(1) && "echo servie 1 not exist");

    sleep(1);
    echo_t::in_t in;
    in.value = "XXX_echo_test_XXX";

    singleton_t<msg_bus_t>::instance().get_service_group("echo")->get_service(1)->async_call(in, &lambda_t::callback);

    while(1)
        sleep(10);

    cout <<"oh end\n";
    
    return 0;
}
