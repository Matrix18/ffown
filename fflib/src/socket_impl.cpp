#include <fcntl.h>
#include <errno.h>

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
    this->set_nonblock();
    m_sc->handle_open(this);
    async_recv();
}

void socket_impl_t::close()
{
    if (m_fd > 0)
    {
        m_epoll->unregister_fd(this);
        ::close(m_fd);
    }
    m_fd = -1;
}

int socket_impl_t::set_nonblock()
{
    int flags;
    flags = fcntl(m_fd, F_GETFL, 0);
    if ((flags = fcntl(m_fd, F_SETFL, flags | O_NONBLOCK)) < 0)
    {
        return -1;
    }

    return 0;
}

int socket_impl_t::handle_epoll_read()
{
    if (is_open())
    {
        int len = ::read(m_fd, m_recv_buffer, sizeof(m_recv_buffer));

        if (len > 0)
        {
            m_sc->handle_read(this, m_recv_buffer, size_t(len));
            return 0;
        }
    }

    this->close();
    return -1;
}

int socket_impl_t::handle_epoll_error()
{
    return m_sc->handle_error(this);
}

int socket_impl_t::handle_epoll_write()
{
    int ret = 0;
    string left_buff;

    while (false == m_send_buffer.empty())
    {
        const string& msg = m_send_buffer.front();
        ret = do_send(msg, left_buff);

        if (ret < 0)
        {
            this ->close();
            return -1;
        }
        else if (ret > 0)
        {
            m_send_buffer.pop_front();
            m_send_buffer.push_front(left_buff);
            return 0;
        }
        else
        {
            m_send_buffer.pop_front();
        }
    }

    m_sc->handle_write_completed(this);
    return 0;
}

void socket_impl_t::async_send(const string& buff_)
{
    if (false == is_open() || m_sc->check_pre_send(this, buff_) || buff_.empty())
    {
        return;
    }

    //! socket buff is full, cache the data
    if (false == m_send_buffer.empty())
    {
        m_send_buffer.push_back(buff_);
        return;
    }

    string left_buff;
    int ret = do_send(buff_, left_buff);

    if (ret < 0)
    {
        this ->close();
    }
    else if (ret > 0)
    {
        m_send_buffer.push_back(left_buff);
    }
    else
    {
        //! send ok
        m_sc->handle_write_completed(this);
    }
}

int socket_impl_t::do_send(const string& buff_, string& left_buff_)
{
    size_t nleft             = buff_.size();
    const char* buffer = buff_.data();
    ssize_t nwritten;

    while(nleft > 0)
    {
        if((nwritten = ::write(m_fd, buffer, nleft)) <= 0)
        {
            if (EINTR == errno)
            {
                nwritten = 0;
            }
            else if (EWOULDBLOCK == errno)
            {
                left_buff_.assign(buff_.c_str() + (buff_.size() - nleft), nleft);
                return 1;
            }
            else
            {
                return -1;
            }
        }

        nleft    -= nwritten;
        buffer += nwritten;
    }

    return 0;
}

void socket_impl_t::async_recv()
{
    m_epoll->register_fd(this);
}
