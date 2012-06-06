#include <iostream>
using namespace std;

#include "utility/timer_service.h"
#include "utility/performance_daemon.h"

struct timeval begin;
struct timeval end;

void foo(void*)
{
    AUTO_PERF();
    gettimeofday(&end, NULL);
    long cost_ms = (end.tv_sec - begin.tv_sec)*1000 + (end.tv_usec - begin.tv_usec)/1000;

    for (int i=0; i < cost_ms % 20 + 1;  ++i){
        AUTO_PERF();
        for (int j=0; j < cost_ms % 500 + 1;  ++j)
            cout <<"oh nice:" << cost_ms << "\n";
    }
}
void test(void*)
{
    gettimeofday(&end, NULL);
    long cost_ms = (end.tv_sec - begin.tv_sec)*1000 + (end.tv_usec - begin.tv_usec)/1000;

    for (int i=0; i < cost_ms % 6 + 1;  ++i){
        AUTO_PERF();
        for (int j=0; j < cost_ms % 1000 + 1;  ++j)
            cout <<"oh Ha:" << cost_ms << "\n";
    }
}
void dumy(void*)
{
    gettimeofday(&end, NULL);
    long cost_ms = (end.tv_sec - begin.tv_sec)*1000 + (end.tv_usec - begin.tv_usec)/1000;
    for (int i=0; i < cost_ms % 10 + 1;  ++i){
        AUTO_PERF();
        for (int j=0; j < cost_ms % 300 + 1;  ++j)
            cout <<"oh fuck:" << cost_ms << "\n";
    }
}

int main(int argc, char* argv[])
{
    
    singleton_t<performance_daemon_t>::instance().start("perf.txt", 5); 

    timer_service_t ts(1);
    for (int i = 0; i < 100; ++i)
    {
        gettimeofday(&begin, NULL);
        ts.timer_callback(200, task_t(&foo, NULL));
        ts.timer_callback(300, task_t(&test, NULL));
        ts.timer_callback(500, task_t(&dumy, NULL));
        usleep(1000*100);
    }
    sleep(4);

    singleton_t<performance_daemon_t>::instance().stop();  
    return 0;
}
