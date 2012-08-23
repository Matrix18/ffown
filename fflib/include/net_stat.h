#ifndef _BASE_HEARTBEAT_H_
#define _BASE_HEARTBEAT_H_

#include "socket_i.h"

#include <string>
using namespace std;

namespace ff {

class net_stat_t
{
public:
    net_stat_t();
    ~net_stat_t();

    int start(arg_helper_t& arg_helper_);
    int stop();

    //! 最大消息包大小
    int get_max_packet_size() const { return m_max_packet_size; }

    //! 获取心跳模块引用
    base_heartbeat_t<socket_ptr_t>& get_heartbeat() { return m_heartbeat;}

    //! 
private:
    timer_service_t*                m_timer_service;
    int                             m_max_packet_size;
    base_heartbeat_t<socket_ptr_t>  m_heartbeat;
};

net_stat_t::net_stat_t():
    m_timer_service(NULL),
    m_max_packet_size(1000)
{
}
    
net_stat_t::~net_stat_t()
{
    if (m_timer_service)
    {
        delete m_timer_service;
        m_timer_service = NULL;
    }
}

static void timer_close(socket_ptr_t s_)
{
    s_->close();
}

int net_stat_t::start(arg_helper_t& arg_helper_)
{
    if (arg_helper_.is_enable_option("-max_packet_size"))
    {
        m_max_packet_size = ::atoi(arg_helper_.get_option_value("-max_packet_size").c_str());
    }
    
    m_heartbeat.start_option(arg_helper_, timer_close);
    
    m_timer_service = new timer_service_t(1000); //! 1s tick
    return 0;
}

int net_stat_t::stop()
{
    return 0;
}

}
#endif
