#include <sys/epoll.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <assert.h>

#include "utility/socket_op.h"
#include "epoll_fd_i.h"
#include "detail/epoll_impl.h"

using namespace ff;

epoll_impl_t::epoll_impl_t():
    m_running(true),
    m_efd(-1)
{
    m_efd = ::epoll_create(CREATE_EPOLL_SIZE);
    m_interunpt_sockets[0] = -1;
    m_interunpt_sockets[1] = -1;
}

epoll_impl_t::~epoll_impl_t()
{
    ::close(m_interunpt_sockets[0]);
    ::close(m_interunpt_sockets[1]);
    ::close(m_efd);
    m_efd = -1;
}

int epoll_impl_t::event_loop()
{
    int i = 0, nfds = 0;
    struct epoll_event ev_set[EPOLL_EVENTS_SIZE];

    do
    {
        nfds  = ::epoll_wait(m_efd, ev_set, EPOLL_EVENTS_SIZE, EPOLL_WAIT_TIME);

        if (false == m_running) return 0;

        if (nfds < 0 && EINTR == errno)
        {
            nfds = 0;
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

            if(cur_ev.events & EPOLLOUT)
            {
                fd_ptr->handle_epoll_write();
            }

            if (cur_ev.events & (EPOLLERR | EPOLLHUP))
            {
                fd_ptr->close();
            }
        }
        
        //! 删除那些已经出现error的socket 对象
        destory_error_fd();
    }while(nfds >= 0);

    return 0;
}

int epoll_impl_t::close()
{
    m_running = false;

    assert( 0 == ::socketpair(AF_LOCAL, SOCK_STREAM, 0, m_interunpt_sockets));
    ::write(m_interunpt_sockets[1], "0", 1);

    struct epoll_event ee = { 0, { 0 } };
    ee.data.ptr  = this;
    ee.events    = EPOLLIN | EPOLLPRI | EPOLLOUT | EPOLLHUP | EPOLLET;;
    ::epoll_ctl(m_efd, EPOLL_CTL_ADD, m_interunpt_sockets[0], &ee);
    
    return 0;
}

int epoll_impl_t::register_fd(epoll_fd_i* fd_ptr_)
{
    struct epoll_event ee = { 0, { 0 } };

    ee.data.ptr  = fd_ptr_;
    ee.events    = EPOLLIN | EPOLLPRI | EPOLLOUT | EPOLLHUP | EPOLLET;;

    return ::epoll_ctl(m_efd, EPOLL_CTL_ADD, fd_ptr_->socket(), &ee);
}

int epoll_impl_t::unregister_fd(epoll_fd_i* fd_ptr_)
{
    struct epoll_event ee;

    ee.data.ptr  = (void*)0;
    int ret = ::epoll_ctl(m_efd, EPOLL_CTL_DEL, fd_ptr_->socket(), &ee);

    lock_guard_t lock(m_mutex);
    m_error_fd_set.push_back(fd_ptr_);
    
    return ret;
}

int epoll_impl_t::mod_fd(epoll_fd_i* fd_ptr_)
{
    struct epoll_event ee = { 0, { 0 } };

    ee.data.ptr  = fd_ptr_;
    ee.events    = EPOLLIN | EPOLLPRI | EPOLLOUT | EPOLLHUP | EPOLLET;;

    return ::epoll_ctl(m_efd, EPOLL_CTL_MOD, fd_ptr_->socket(), &ee);
}

void epoll_impl_t::destory_error_fd()
{
    lock_guard_t lock(m_mutex);
    list<epoll_fd_i*>::iterator it = m_error_fd_set.begin();
    for (; it != m_error_fd_set.end(); ++it)
    {
        (*it)->handle_epoll_error();
    }
    m_error_fd_set.clear();
}
