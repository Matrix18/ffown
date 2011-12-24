#include "detail/acceptor_impl.h"
#include "detail/epoll_impl.h"

#include <iostream>
using namespace std;

#include<pthread.h>

void *thr_fn(void *arg)
{
    epoll_impl_t* p = (epoll_impl_t*)arg;
    p->open();
    sleep(10);
    return NULL;
}

int main(int argc, char* argv[])
{
    pthread_t ntid;

    int ret = 0;
    epoll_impl_t epoll;

    ret = pthread_create(&ntid, NULL, thr_fn, &epoll);
    
    acceptor_impl_t acceptor(&epoll);
    ret = acceptor.open("tcp://*:10241");
    cout <<"acceptor open:" << ret <<"\n";

    pthread_join(ntid, NULL);
    
	return 0;
}

