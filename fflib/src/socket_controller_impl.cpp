#include "detail/socket_controller_impl.h"
#include "socket_i.h"

#include <iostream>
using namespace std;

socket_controller_impl_t::socket_controller_impl_t()
{
}

int socket_controller_impl_t::handle_error(socket_i* sp_)
{
    //! cout <<"socket_controller_impl_t::handle_error \n";
    delete sp_;
    return 0;
}

int socket_controller_impl_t::handle_read(socket_i* sp_, char* buff, size_t len)
{
    const char* data = "HTTP/1.0 200 OK\r\nConnection: Close\r\nContent-type: text/html\r\nContent-length: 10\r\n\r\nHelloWorld";
    //! string msg(buff, len);
    //! cout <<"socket_controller_impl_t::handle_read:" << msg <<"\n";
    sp_->async_send(data);

    return 0;
}

int socket_controller_impl_t::handle_write_completed(socket_i* sp_)
{
    //! cout <<"socket_controller_impl_t::handle_write_completed \n";
    sp_->close();
    return 0;
}
