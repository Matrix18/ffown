#include <iostream>
using namespace std;

#include "broker_service.h"

int main(int argc, char* argv[])
{
    char buff[128];
    snprintf(buff, sizeof(buff), "tcp://%s:%s", "127.0.0.1", "10241");
    
    return 0;
}
