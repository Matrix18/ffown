#include "channel.h"

channel_t::channel_t(socket_ptr_t sock_):
    m_socket(sock_)
{
}

channel_t::~channel_t()
{
}
