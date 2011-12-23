
#include <sys/epoll.h>
#include "socket_i.h"
#include "detail/epoll_impl.h"

epoll_impl_t::epoll_impl_t():
    m_efd(0)
{
}

epoll_impl_t::~epoll_impl_t()
{
}

int epoll_impl_t::open()
{
    int i = 0, nfds = 0;
    struct epoll_event ev_set[EPOLL_EVENTS_SIZE];

    m_efd = epoll_create(CREATE_EPOLL_SIZE);
    
    nfds  = epoll_wait(m_efd, ev_set, EPOLL_EVENTS_SIZE, -1);
    for (i = 0; i < nfds; ++i)
    {
        epoll_event& cur_ev = ev_set[i];
        socket_i* socket_ptr = (socket_i*)cur_ev.data.ptr;

        if (cur_ev.events & EPOLLIN)
        {
            socket_ptr->handle_epoll_read();
        }
        else if(cur_ev.events & EPOLLOUT)
        {
            socket_ptr->handle_epoll_write();
        }
        else if(cur_ev.events & EPOLLERR)
        {
            socket_ptr->handle_epoll_error();
        }
    }
    return 0;
}

int epoll_impl_t::close()
{
    return 0;
}

int epoll_impl_t::register_read_event(socket_i* socket_ptr_)
{
    struct epoll_event ee;

    ee.data.ptr  = socket_ptr_;
    ee.data.fd   = socket_ptr_->socket();
    ee.events    = EPOLLIN | EPOLLET;

    //! EPOLL_CTL_ADD more efficient than EPOLL_CTL_MOD
    int ret = epoll_ctl(m_efd, EPOLL_CTL_ADD, ee.data.fd, &ee);
    return ret;
}

int epoll_impl_t::register_write_event(socket_i* socket_ptr_)
{
    struct epoll_event ee;

    ee.data.ptr  = socket_ptr_;
    ee.data.fd   = socket_ptr_->socket();
    ee.events    = EPOLLOUT | EPOLLET;

    int ret = epoll_ctl(m_efd, EPOLL_CTL_ADD, ee.data.fd, &ee);
    return ret;
}
