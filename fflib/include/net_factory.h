//! 接口
#ifndef _NET_FACTORY_H_
#define _NET_FACTORY_H_

#include "detail/acceptor_impl.h"
#include "detail/epoll_impl.h"
#include "connector.h"
#include "utility/singleton.h"
#include "utility/performance_daemon.h"
#include "thread.h"
#include "codec.h"
#include "msg_sender.h"

class net_factory_t
{
public:
    struct global_data_t
    {
        volatile bool     started_flag;
        task_queue_pool_t tg;
        thread_t          thread;
        epoll_impl_t      epoll;
        global_data_t():
            started_flag(false),
            epoll()
        {
        }
        ~ global_data_t()
        {
            stop();
        }
        static void run_epoll(void* e_)
        {
            global_data_t* p = (global_data_t*)e_;
            p->epoll.event_loop();
        }
        void start(int thread_num_ = 2)
        {
            if (false == started_flag)
            {
                assert(thread_num_ > 0);
                started_flag = true;
                thread.create_thread(task_t(&run_epoll, this), 1);
                thread.create_thread(task_queue_pool_t::gen_task(&tg), thread_num_);
                while (tg.size() != (size_t)thread_num_){}
            }
        }
        void stop()
        {
            if (true == started_flag)
            {
                tg.close();
                epoll.close();
                thread.join();
            }
        }
    };

    static acceptor_i* listen(const string& host_, msg_handler_i* msg_handler_);
    static socket_ptr_t connect(const string& host_, msg_handler_i* msg_handler_);
};

acceptor_i* net_factory_t::listen(const string& host_, msg_handler_i* msg_handler_)
{
    singleton_t<global_data_t>::instance().start();
    acceptor_impl_t* ret = new acceptor_impl_t(&(singleton_t<global_data_t>::instance().epoll),
                                               msg_handler_, 
                                               &(singleton_t<global_data_t>::instance().tg));

    if (ret->open(host_))
    {
        delete ret;
        return NULL;
    }
    return ret;
}

socket_ptr_t net_factory_t::connect(const string& host_, msg_handler_i* msg_handler_)
{
    singleton_t<global_data_t>::instance().start();
    return connector_t::connect(host_, &(singleton_t<global_data_t>::instance().epoll), msg_handler_,
                                &(singleton_t<global_data_t>::instance().tg));
}
#endif
