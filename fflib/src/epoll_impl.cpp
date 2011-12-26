
#include <sys/epoll.h>
#include <errno.h>

#include "epoll_fd_i.h"
#include "detail/epoll_impl.h"

epoll_impl_t::epoll_impl_t(task_queue_group_t* tqg_):
    m_efd(-1),
    m_task_queue(tqg_)
{
    m_efd = ::epoll_create(CREATE_EPOLL_SIZE);
}

epoll_impl_t::~epoll_impl_t()
{
}

static void post_read_event(void* p)
{
    epoll_fd_i* fd_ptr = (epoll_fd_i*)p;
    fd_ptr->handle_epoll_read();
}

static void post_write_event(void* p)
{
    epoll_fd_i* fd_ptr = (epoll_fd_i*)p;
    fd_ptr->handle_epoll_write();
}

static void post_error_event(void* p)
{
    epoll_fd_i* fd_ptr = (epoll_fd_i*)p;
    fd_ptr->handle_epoll_error();
}

int epoll_impl_t::event_loop()
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
                m_task_queue->produce(task_t(post_read_event, fd_ptr));
            }
            else if(cur_ev.events & EPOLLOUT)
            {
                m_task_queue->produce(task_t(post_write_event, fd_ptr));
            }
            else
            {
                fd_ptr->close();
            }
        }

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
    ee.events    = EPOLLIN | EPOLLPRI | EPOLLOUT | EPOLLHUP | EPOLLET;;

    return ::epoll_ctl(m_efd, EPOLL_CTL_ADD, fd_ptr_->socket(), &ee);
}

int epoll_impl_t::unregister_fd(epoll_fd_i* fd_ptr_)
{
    struct epoll_event ee;

    ee.data.ptr  = (void*)0;
    int ret = ::epoll_ctl(m_efd, EPOLL_CTL_DEL, fd_ptr_->socket(), &ee);

    m_task_queue->produce(task_t(post_error_event, fd_ptr_));
    return ret;
}

int epoll_impl_t::mod_fd(epoll_fd_i* fd_ptr_)
{
    struct epoll_event ee = { 0, { 0 } };

    ee.data.ptr  = fd_ptr_;
    ee.events    = EPOLLIN | EPOLLPRI | EPOLLOUT | EPOLLHUP | EPOLLET;;

    return ::epoll_ctl(m_efd, EPOLL_CTL_MOD, fd_ptr_->socket(), &ee);
}

