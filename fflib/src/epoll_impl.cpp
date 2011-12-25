
#include <sys/epoll.h>
#include <errno.h>

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
        nfds  = epoll_wait(m_efd, ev_set, EPOLL_EVENTS_SIZE, EPOLL_WAIT_TIME);
        if (nfds < 0 && EINTR == errno)
        {
            continue;
        }

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
                //! how to trigger this event! !!! headache!!!!
                fd_ptr->close();
            }
        }

        trigger_socket_error();
    }while(nfds >= 0);

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
    ee.events    = EPOLLIN | EPOLLPRI | EPOLLOUT | EPOLLRDHUP | EPOLLHUP | EPOLLET;;

    return ::epoll_ctl(m_efd, EPOLL_CTL_ADD, fd_ptr_->socket(), &ee);
}

int epoll_impl_t::unregister_fd(epoll_fd_i* fd_ptr_)
{
    struct epoll_event ee;

    ee.data.ptr  = (void*)0;
    int ret = ::epoll_ctl(m_efd, EPOLL_CTL_DEL, fd_ptr_->socket(), &ee);

    //! trigger socket error handle logic, when post epoll err, epoll will not post any other events
    //! fd_ptr_->handle_epoll_error();
    m_wait_del.push_back(fd_ptr_);
    return ret;
}

int epoll_impl_t::mod_fd(epoll_fd_i* fd_ptr_)
{
    struct epoll_event ee = { 0, { 0 } };

    ee.data.ptr  = fd_ptr_;
    ee.events    = EPOLLIN | EPOLLPRI | EPOLLOUT | EPOLLRDHUP | EPOLLHUP | EPOLLET;;

    return ::epoll_ctl(m_efd, EPOLL_CTL_MOD, fd_ptr_->socket(), &ee);
}

void epoll_impl_t::trigger_socket_error()
{
    fd_list_t::iterator it = m_wait_del.begin();
    for (; it != m_wait_del.end(); ++it)
    {
        (*it)->handle_epoll_error();
    }
    m_wait_del.clear();
}
