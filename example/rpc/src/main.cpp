#include <iostream>
using namespace std;

#include "net_factory.h"

class test_service_t: public msg_handler_i
{
public:
    int m_a;
    test_service_t(int a):m_a(a){}
    ~test_service_t(){}
    int handle_broken(socket_ptr_t sock_)
    {
        cout <<m_a <<" handle broken\n";
        delete sock_;
        return 0;
    }
    int handle_msg(const message_t& msg_, socket_ptr_t sock_)
    {
        cout <<m_a << " handle msg:" << msg_.get_body() <<"\n";
        return 0;
    }
};
test_service_t test_service(1), test_service2(2);
int main(int argc, char* argv[])
{
    
    char buff[128];
    snprintf(buff, sizeof(buff), "tcp://%s:%s", "127.0.0.1", "10241");

    acceptor_i* acceptor = net_factory_t::listen(buff, &test_service);
    cout <<"acceptor:" << acceptor <<"\n";
    //test_service2.handle_broken(NULL);

    socket_ptr_t skt = net_factory_t::connect("tcp://127.0.0.1:10241", (msg_handler_i*)&test_service2);
    cout << "skt:" << skt <<"\n";
    
    skt->async_send("Ohnice");
    //sleep(1);
    skt->close();
    sleep(1);
    return 0;
}
