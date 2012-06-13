#include <iostream>
using namespace std;

#include "net_factory.h"
#include "rpc_service.h"
#include "msg_def.h"

class test_service_t: public msg_handler_i
{
public:
    int m_a;
    test_service_t(int a):m_a(a){}
    ~test_service_t(){}
    int handle_broken(socket_ptr_t sock_)
    {
        cout <<m_a <<" handle broken\n";
	sock_->safe_delete();
        return 0;
    }
    int handle_msg(const message_t& msg_, socket_ptr_t sock_)
    {
        test_msg_t test_msg;
        test_msg.decode(msg_.get_body());
        cout <<m_a << " handle msg:" << test_msg.val <<"\n";
        test_msg.val = m_a;
        if (m_a == 1) {
            msg_sender_t::send(sock_, 100, test_msg);
        }
        else
            msg_sender_t::send(sock_, 100, test_msg);
        return 0;
    }
};
test_service_t test_service(1), test_service2(2);

void foo(){}
int foo1() {return 0;}

struct foo_t
{
public:
    void dumy(){}
};
int main(int argc, char* argv[])
{
    
    char buff[128];
    snprintf(buff, sizeof(buff), "tcp://%s:%s", "127.0.0.1", "10241");

    acceptor_i* acceptor = net_factory_t::listen(buff, &test_service);
    cout <<"acceptor:" << acceptor <<"\n";
    //test_service2.handle_broken(NULL);

    socket_ptr_t skt = net_factory_t::connect("tcp://127.0.0.1:10241", (msg_handler_i*)&test_service2);
    cout << "skt:" << skt <<"\n";

    test_msg_t test_msg;
    test_msg.val  = 10241;
    msg_sender_t::send(skt, 100, test_msg);
    sleep(1);
    skt->close();
    sleep(1);
    cout <<"oh end\n";

    task_binder_t::gen(&foo);
    task_binder_t::gen(&foo1);
    
    foo_t f;
    task_binder_t::gen(&foo_t::dumy, &f);
    return 0;
}
