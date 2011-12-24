#include "detail/socket_controller_impl.h"
#include "socket_i.h"

#include <iostream>
using namespace std;

socket_controller_impl_t::socket_controller_impl_t()
{
}

int socket_controller_impl_t::handle_error(socket_i* sp_)
{
    cout <<"socket_controller_impl_t::handle_error begin\n";
    delete sp_;
    return 0;
}

int socket_controller_impl_t::handle_read(socket_i* sp_, char* buff, size_t len)
{
    string msg(buff, len);
    cout <<"socket_controller_impl_t::handle_read TODO:"<<msg<<"\n";
    sp_->async_send("HelloWorld!\r\n");

    return 0;
}
