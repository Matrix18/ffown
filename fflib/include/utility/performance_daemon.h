#ifndef _PERFORMANCE_DEAMON_H_
#define _PERFORMANCE_DEAMON_H_

#include <string>
#include <map>
#include <fstream>
#include <iostream>
using namespace std;

#include "detail/task_queue_impl.h"
#include "utility/timer_service.h"

#include "thread.h"
#include "singleton.h"

#define AUTO_PERF() performance_daemon_t::perf_tool_t __tmp__(__FUNCTION__)
#define PERF(m)     performance_daemon_t::perf_tool_t __tmp__(m)

//! 性能监控
class performance_daemon_t
{
public:
    struct perf_tool_t
    {
        perf_tool_t(const char* mod_):
            mod(mod_)
        {
            gettimeofday(&tm, NULL);
        }
        ~perf_tool_t()
        {
            struct timeval now;
            gettimeofday(&now, NULL);
            long cost = (now.tv_sec - tm.tv_sec)*1000000 + (now.tv_usec - tm.tv_usec);
            singleton_t<performance_daemon_t>::instance().post(mod, cost);
        }
        const char*    mod;
        struct timeval tm;
    };
    struct perf_info_t
    {
        perf_info_t():
            max(0),
            min(0x7FFFFFFFFFFFFFFF),
            total(0),
            times(0)
        {}
        long max;
        long min;
        long total;
        long times;
    };

    struct timer_lambda_t
    {
        static void exe(void* p_)
        {
            ((performance_daemon_t*)p_)->handle_timer();
        }
        static void setup_timer(void* p_)
        {
            performance_daemon_t* pd = (performance_daemon_t*)p_;
            pd->get_task_queue().produce(task_t(&timer_lambda_t::exe, pd));
        }
    };
public:
    performance_daemon_t();
    ~performance_daemon_t();

    //! 启动线程，创建文件
    int start(const string& path_, int seconds_);
    //! 关闭线程
    int stop();

    //! 增加性能监控数据
    void post(const string& mod_, long us);

    //! 更新性能日志
    void flush();

    //! 所有队列
    void run();

    task_queue_t& get_task_queue() { return m_task_queue; }
public:
    void add_perf_data(const string& mod_, long us);

private:
    void handle_timer();

private:
    volatile bool               m_started;
    int                         m_timeout_sec;
    map<string, perf_info_t>    m_perf_info;
    task_queue_t                m_task_queue;
    thread_t                    m_thread;
    ofstream                    m_fstream;
    timer_service_t*            m_timer_service;
};

performance_daemon_t::performance_daemon_t():
    m_started(false),
    m_timeout_sec(3600),
    m_timer_service(NULL)
{
}

performance_daemon_t::~performance_daemon_t()
{
    stop();
}


int performance_daemon_t::start(const string& path_, int seconds_)
{
    if (true == m_started) return -1;

    m_timeout_sec = seconds_;

    //! 启动定时器 1 times/seconds
    m_timer_service = new timer_service_t(1000);

    m_started = true; //! timer will start

    struct lambda_t
    {
        static void run(void* p_)
        {
            ((performance_daemon_t*)p_)->run();
        }
    };

    m_fstream.open(path_.c_str());
    m_thread.create_thread(task_t(&lambda_t::run, this), 1);
    return 0;
}

int performance_daemon_t::stop()
{
    if (false == m_started) return -1;

    if (m_timer_service)
    {
        delete m_timer_service;
        m_timer_service = NULL;
    }

    m_started = false; //! timer will stop

    m_task_queue.close();
    m_thread.join();
    m_fstream.close();
    return 0;
}

void performance_daemon_t::post(const string& mod_, long us_)
{
    struct lambda_perf_data_t
    {
        static void add_perf_data(void* p_)
        {
            lambda_perf_data_t* pd_ptr = (lambda_perf_data_t*)p_;
            pd_ptr->pd->add_perf_data(pd_ptr->mod, pd_ptr->cost);
    
            delete pd_ptr;
        }
        lambda_perf_data_t(performance_daemon_t* p_, const string& mod_, long cost_):
            pd(p_),
            mod(mod_),
            cost(cost_)
        {
        }
        performance_daemon_t* pd;
        string                mod;
        long                  cost;
    };

    m_task_queue.produce(task_t(&lambda_perf_data_t::add_perf_data, new lambda_perf_data_t(this, mod_, us_)));
}

void performance_daemon_t::handle_timer()
{
    flush();
    m_perf_info.clear();
    m_timer_service->timer_callback(m_timeout_sec * 1000, task_t(&timer_lambda_t::setup_timer, this));
}

void performance_daemon_t::flush()
{
    map<string, perf_info_t>::iterator it = m_perf_info.begin();

    time_t timep   = time(NULL);
    struct tm *tmp = localtime(&timep);

    char tmp_buff[256];
    sprintf(tmp_buff, "%04d%02d%02d-%02d:%02d:%02d",
                        tmp->tm_year + 1900, tmp->tm_mon + 1, tmp->tm_mday,
                        tmp->tm_hour, tmp->tm_min, tmp->tm_sec);
    char buff[1024] = {0};

    snprintf(buff, sizeof(buff), "time,mod,max_cost[us],min_cost[us],per_cost[us],request_per_second,exe_times\n");
    m_fstream << buff;

    for (; it != m_perf_info.end(); ++it)
    {
        perf_info_t& pinfo = it->second;
        long per = pinfo.total / pinfo.times;
        long rps = (per == 0? -1: 1000000 / per);

        //! -------------------------- time, mod, max, min, per, rps, times
        snprintf(buff, sizeof(buff), "%s,%s,%ld,%ld,%ld,%ld,%ld\n",
                 tmp_buff, it->first.c_str(), pinfo.max, pinfo.min, per, rps, pinfo.times);
        m_fstream << buff;
    }

    m_fstream.flush();
}



void performance_daemon_t::run()
{
    m_timer_service->timer_callback(m_timeout_sec * 1000, task_t(&timer_lambda_t::setup_timer, this));
    m_task_queue.run();
}

void performance_daemon_t::add_perf_data(const string& mod_, long us_)
{
    perf_info_t& pinfo = m_perf_info[mod_];

    pinfo.total += us_;
    pinfo.times += 1;

    if (us_ > pinfo.max)
    {
        pinfo.max = us_;
    }
    else if (us_ < pinfo.min)
    {
        pinfo.min = us_;
    }
}

#endif //PERFORMANCE_DEAMON_H_
