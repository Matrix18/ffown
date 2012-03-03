
#include <stdarg.h>
#include <stdio.h>
#include <iostream>
using namespace std;

#include "msg_broker_service.h"
#include "channel.h"
#include "plugin_factory.h"

static int log_impl(const char* mod, const char* fmt, ...)
{
    char buff[256];
    int len = snprintf(buff, sizeof(buff), "%s ", mod);

    va_list vl;
    va_start(vl, fmt);
    vsnprintf(buff + len, sizeof(buff) - len - 1, fmt, vl);
    va_end(vl);
    printf(buff);
    return printf("\n");
}

#define logtrace(content) log_impl content
#define BROKER_SERVICE "CHAT_SERVICE"

msg_broker_service_t::msg_broker_service_t()
{
}

msg_broker_service_t::~msg_broker_service_t()
{
    if (m_plugin)
    {
        delete m_plugin;
    }
}

int msg_broker_service_t::handle_broken(socket_ptr_t sock_)
{
    return 0;
}

int msg_broker_service_t::handle_msg(const message_t& msg_, socket_ptr_t sock_)
{
    try
    {
        channel_t* c = sock_->get_data<channel_t>();
        if (NULL == c)
        {
            c = new channel_t(sock_);
            sock_->set_data(c);
        }
    }
    catch(exception& e)
    {
        logtrace((BROKER_SERVICE, "chat_service_t::handle_msg exception<%s>", e.what()));
        sock_->close();
    }
    return 0;
}

int msg_broker_service_t::start(const string& path_)
{
    m_plugin = plugin_factory_t::create_plugin(path_);

    if (NULL == m_plugin)
    {
        return -1;
    }
    return m_plugin->start();
}

int msg_broker_service_t::stop()
{
    m_plugin->stop();
    return 0;
}
