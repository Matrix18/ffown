#include <iostream>
using namespace std;

#include "utility/timer_service.h"
#include "utility/performance_daemon.h"

struct timeval begin;
struct timeval end;

void foo(void*)
{
    gettimeofday(&end, NULL);
    long cost_ms = (end.tv_sec - begin.tv_sec)*1000 + (end.tv_usec - begin.tv_usec)/1000;
    cout <<"oh nice:" << cost_ms << "\n";
}
void foo2(void*)
{
    gettimeofday(&end, NULL);
    long cost_ms = (end.tv_sec - begin.tv_sec)*1000 + (end.tv_usec - begin.tv_usec)/1000;
    cout <<"oh fuck:" << cost_ms << "\n";
}

int main(int argc, char* argv[])
{
    singleton_t<performance_daemon_t>::instance().start("perf.txt", 1); 
    singleton_t<performance_daemon_t>::instance().post("test", 100);
    singleton_t<performance_daemon_t>::instance().post("test", 200);
    singleton_t<performance_daemon_t>::instance().post("test", 150);

    sleep(3);
    singleton_t<performance_daemon_t>::instance().stop();

    timer_service_t ts(1);
    gettimeofday(&begin, NULL);
    ts.timer_callback(2000, task_t(&foo, NULL));
    sleep(1);
    ts.timer_callback(3500, task_t(&foo2, NULL));
    sleep(4);
    
    return 0;
}
