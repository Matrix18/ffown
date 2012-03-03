#include "msg_broker_iterface.h"
#include <iostream>
using namespace std;

extern "C"{

int handle_channel_msg(const message_t& msg_, channel_ptr_t channel_)
{
    cout <<"handle_channel_msg:\n";
    return 0;
}

int handle_channel_broken(channel_ptr_t channel_)
{
    cout <<"handle_channel_broken:\n";
    return 0;
}

}
