#ifndef _SOCKET_CONTROLLER_IMPL_H_
#define _SOCKET_CONTROLLER_IMPL_H_

#include <string>
using namespace std;

#include "socket_controller_i.h"

class socket_i;

class socket_controller_impl_t: public socket_controller_i
{
public:
    socket_controller_impl_t();
    virtual int handle_error(socket_i*);
    virtual int handle_read(socket_i*, char* buff, size_t len);

private:
};
#endif
