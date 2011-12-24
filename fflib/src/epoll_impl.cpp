
#include <sys/epoll.h>

#include "epoll_fd_i.h"
#include "detail/epoll_impl.h"

epoll_impl_t::epoll_impl_t():
    m_efd(-1)
{
    m_efd = ::epoll_create(CREATE_EPOLL_SIZE);
}

epoll_impl_t::~epoll_impl_t()
{
}

int epoll_impl_t::open()
{
    int i = 0, nfds = 0;
    struct epoll_event ev_set[EPOLL_EVENTS_SIZE];

    do
    {
        nfds  = epoll_wait(m_efd, ev_set, EPOLL_EVENTS_SIZE, -1);
        for (i = 0; i < nfds; ++i)
        {
            epoll_event& cur_ev = ev_set[i];
            epoll_fd_i* fd_ptr = (epoll_fd_i*)cur_ev.data.ptr;
    
            if (cur_ev.events & (EPOLLIN | EPOLLPRI))
            {    
                fd_ptr->handle_epoll_read();
            }
            else if(cur_ev.events & EPOLLOUT)
            {
                fd_ptr->handle_epoll_write();
            }
            else
            {
                // ! just ignore this, read event can handle socket error 
                //!  fd_ptr->handle_epoll_error();
            }
        }
    }while(nfds > 0);

    return 0;
}

int epoll_impl_t::close()
{
    return 0;
}

int epoll_impl_t::register_fd(epoll_fd_i* fd_ptr_)
{
    struct epoll_event ee = { 0, { 0 } };

    ee.data.ptr  = fd_ptr_;
    ee.events    = EPOLLIN | EPOLLPRI | EPOLLOUT | EPOLLET;;

    int ret = ::epoll_ctl(m_efd, EPOLL_CTL_ADD, fd_ptr_->socket(), &ee);
    return ret;
}

int epoll_impl_t::unregister_fd(epoll_fd_i* fd_ptr_)
{
    struct epoll_event ee;

    ee.data.ptr  = (void*)0;
    int ret = ::epoll_ctl(m_efd, EPOLL_CTL_DEL, fd_ptr_->socket(), &ee);
    return ret;
}
