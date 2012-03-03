#ifndef _CHANNEL_H_
#define _CHANNEL_H_

#include "socket_i.h"

class channel_t
{
public:
    channel_t(socket_ptr_t sock_);
    ~channel_t();

private:
    socket_ptr_t    m_socket;
};

typedef channel_t* channel_ptr_t;
#endif
