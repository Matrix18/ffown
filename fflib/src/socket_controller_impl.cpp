#include "detail/socket_controller_impl.h"
#include "socket_i.h"
#include "utility/strtool.h"

#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>
using namespace std;

socket_controller_impl_t::socket_controller_impl_t(msg_handler_ptr_t msg_handler_):
    m_msg_handler(msg_handler_),
    m_head_end_flag(false),
    m_body_size(0)
{
}

socket_controller_impl_t::~socket_controller_impl_t()
{
}

int socket_controller_impl_t::handle_error(socket_i* sp_)
{
    //! cout <<"socket_controller_impl_t::handle_error \n";
    //! TODO delete sp_;
    m_msg_handler->handle_broken(sp_);
    return 0;
}

int socket_controller_impl_t::handle_read(socket_i* sp_, char* buff, size_t len)
{
    if ('G' == buff[0])
    {
        const char* data = "HTTP/1.0 200 OK\r\nConnection: Close\r\nContent-type: text/html\r\nContent-length: 10\r\n\r\nHelloWorld";
        sp_->async_send(data);
        return 0;
    }

    char* buff_begin = buff;
    size_t left = len;
    do
    {
        if (false == m_head_end_flag)
        {
            
            char* pos = ::strstr(buff_begin, "\r\n");
            if (NULL == pos)
            {
                m_head.append(buff_begin, left);
                return 0;
            }

            m_head.append(buff_begin, pos - buff);
            if (parse_msg_head())
            {
                sp_->async_send("ERROR\r\n");
                return -1;
            }
    
            m_head_end_flag = true;
            left -= (pos + 2 - buff_begin);
            buff_begin = pos + 2;
        }

        int consume = append_msg_body(sp_, buff_begin, left);
        buff_begin += consume;
    }
    while (left > 0);

    return 0;
}

int socket_controller_impl_t::handle_write_completed(socket_i* sp_)
{
    //! cout <<"socket_controller_impl_t::handle_write_completed \n";
    //! sp_->close();
    return 0;
}

int socket_controller_impl_t::parse_msg_head()
{
    vector<string> vt;
    strtool::split(m_head, vt, " ");

    size_t vt_size = vt.size();
    switch (vt_size)
    {
        case 1:
        {
            m_body_size = atoi(vt[0].c_str());;
        }
        break;
        case 2:
        case 3:
        {
            m_body_size = atoi(vt[0].c_str());;
        }
        break;
        default:
        {
            return -1;
        }
        break;
    }
    return 0;
}

int socket_controller_impl_t::append_msg_body(socket_i* sp_, char* buff_begin_, size_t& left_)
{
    size_t tmp        = m_body_size > left_ ? left_: m_body_size;
    left_             -= tmp;
    m_body_size  -= tmp;
    m_message.append_msg(buff_begin_, tmp);

    if (m_body_size == 0)
    {
        //! msg dispatcher
        //! TODO sp_->async_send(m_message.get_body());
        m_msg_handler->handle_msg(m_message, sp_);

        m_head_end_flag = false;
        m_head.clear();
        m_message.clear();
    }
    return tmp;
}

int socket_controller_impl_t::check_pre_send(socket_i*, string& buff_)
{
    ostringstream outstr;
    outstr << buff_.size() <<" \r\n" << buff_;
    buff_ = outstr.str();
    return 0;
}
