
#include "detail/socket_impl.h"
#include "epoll_i.h"
#include "socket_controller_i.h"

socket_impl_t::socket_impl_t(epoll_i* e_, socket_controller_i* seh_, int fd_):
    m_epoll(e_),
    m_sc(seh_),
    m_fd(fd_)
{
}

socket_impl_t::~socket_impl_t()
{
    this->close();
}

void socket_impl_t::open()
{
    m_sc->handle_open(this);
    async_recv();
}

void socket_impl_t::close()
{
    ::close(m_fd);
}

int socket_impl_t::handle_epoll_read()
{
    int len = ::read(m_fd, m_recv_buffer, sizeof(m_recv_buffer));

    m_sc->handle_read(this, m_recv_buffer, size_t(len));
    return 0;
}

int socket_impl_t::handle_epoll_write()
{
    send_buffer_t::iterator it = m_send_buffer.begin();
    for(; it != m_send_buffer.end(); ++it)
    {
        //! TODO check ret error code
        ::write(m_fd, it->c_str(), it->size());
    }

    m_send_buffer.clear();
    return 0;
}

int socket_impl_t::handle_epoll_error()
{
    m_sc->handle_error(this);
    return 0;
}

void socket_impl_t::async_send(const string& buff_)
{
    if (m_sc->check_pre_send(this, buff_))
    {
        return;
    }

    if (m_send_buffer.empty())
    {
        m_epoll->register_write_event(this);
    }

    m_send_buffer.push_back(buff_);
}

void socket_impl_t::async_recv()
{
    m_epoll->register_read_event(this);
}
