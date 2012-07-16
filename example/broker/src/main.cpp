#include <iostream>
using namespace std;

#include "broker_service.h"
#include "net_factory.h"

#include "broker_application.h"
using namespace ff;

int main(int argc, char* argv[])
{
    broker_application_t::run(argc, argv);

    while(1)
        sleep(100);
    
    return 0;
}
