#include <iostream>
using namespace std;

#include "utility/timer_service.h"

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
    timer_service_t ts(1);
    gettimeofday(&begin, NULL);
    ts.timer_callback(6000, task_t(&foo, NULL));
    sleep(1);
    ts.timer_callback(2100, task_t(&foo2, NULL));
    sleep(11);
    
    return 0;
}

