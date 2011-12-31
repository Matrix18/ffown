#include "detail/socket_controller_impl.h"
#include "socket_i.h"

#include <string.h>
#include <iostream>
using namespace std;

socket_controller_impl_t::socket_controller_impl_t():
    m_head_end_flag(false),
    m_body_size(0)
{
}

int socket_controller_impl_t::handle_error(socket_i* sp_)
{
    //! cout <<"socket_controller_impl_t::handle_error \n";
    delete sp_;
    return 0;
}

int socket_controller_impl_t::handle_read(socket_i* sp_, char* buff, size_t len)
{
    char* buff_begin = buff;
    size_t left = len;
    do
    {
        if (false == m_head_end_flag)
        {
            char* pos = ::strstr(buff, "\r\n");
            if (NULL == pos)
            {
                m_head.append(buff, left);
                return 0;
            }

            m_head.append(buff, pos - buff);
            if (parse_msg_head())
            {
                sp_->async_send("ERROR\r\n");
                return -1;
            }
    
            m_head_end_flag = true;
            left -= (pos + 2 - buff);
            buff_begin = pos + 2;
        }

        append_msg_body(buff_begin, left);
    }
    while (left > 0);
    //! const char* data = "HTTP/1.0 200 OK\r\nConnection: Close\r\nContent-type: text/html\r\nContent-length: 10\r\n\r\nHelloWorld";
    //! string msg(buff, len);
    //! cout <<"socket_controller_impl_t::handle_read:" << msg <<"\n";
    //! sp_->async_send(data);

    return 0;
}

int socket_controller_impl_t::handle_write_completed(socket_i* sp_)
{
    //! cout <<"socket_controller_impl_t::handle_write_completed \n";
    sp_->close();
    return 0;
}

int socket_controller_impl_t::parse_msg_head()
{
    return 0;
}

int socket_controller_impl_t::append_msg_body(char* buff_begin_, size_t& left_)
{
    size_t tmp        = m_body_size > left_ ? left_: m_body_size;
    left_               -= tmp;
    m_body_size  -= tmp;
    m_message.append_msg(buff_begin_, tmp);

    if (m_body_size == 0)
    {
        //! msg dispatcher
        m_head_end_flag = false;
        m_head.clear();
        m_message.clear();
    }
    return 0;
}
