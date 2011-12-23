#include <sys/types.h>
#include <sys/socket.h>

#include "detail/acceptor_impl.h"

int acceptor_impl_t::handle_epoll_read()
{
    struct sockaddr_storage addr;
    socklen_t addrlen = sizeof(addr);

    int fd = ::accept(m_listen_fd, (struct sockaddr *)&addr, &addrlen);
    return 0;
}

int acceptor_impl_t::handle_epoll_error()
{
    return 0;
}
