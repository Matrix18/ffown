#include <iostream>
using namespace std;

#include "broker_service.h"
#include "net_factory.h"

using namespace ff;

int main(int argc, char* argv[])
{
    singleton_t<performance_daemon_t>::instance().start("perf.txt", 1);

    char buff[128];
    snprintf(buff, sizeof(buff), "tcp://%s:%s", "127.0.0.1", "10241");
    
    broker_service_t broker_service;
    acceptor_i* m_acceptor = net_factory_t::listen(buff, &broker_service);
    assert(m_acceptor && "can not listen this address");
    
    while(1)
        sleep(100);
    
    singleton_t<performance_daemon_t>::instance().stop();
    return 0;
}
