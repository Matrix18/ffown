#include "detail/acceptor_impl.h"
#include "detail/epoll_impl.h"
#include "utility/thread.h"

#include <iostream>
using namespace std;

#include<pthread.h>
#include<stdio.h>

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        cout <<argv[0] <<" useage listen:host port\n";
        return 1;
    }
    char buff[128];
    snprintf(buff, sizeof(buff), "tcp://%s:%s", argv[1], argv[2]);

    int ret = 0;

    task_queue_pool_t tg;
    thread_t thread;
    thread.create_thread(task_queue_pool_t::gen_task(&tg), 200);

    epoll_impl_t epoll(&tg);
    acceptor_impl_t acceptor(&epoll);
    ret = acceptor.open(string(buff));
     if (ret)
    {
        cout <<"acceptor open failed:" << buff <<"\n";
        return 1;
    }
    else
    {
        cout <<"acceptor open ok, wait to listen\n";
    }

    epoll.event_loop();
    thread.join_all();

	return 0;
}

