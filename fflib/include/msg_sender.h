//! 消息发送器

#ifndef _MSG_SENDER_H_
#define _MSG_SENDER_H_

#include "socket_i.h"
#include "message.h"
#include "codec.h"
//! #include "zlib_util.h"

class msg_sender_t
{
public:
    static void send(socket_ptr_t socket_ptr_, uint16_t cmd_, const string& str_)
    {
        if (socket_ptr_)
        {
            message_head_t h(cmd_);
            h.size = str_.size();
            string dest((const char*)&h, sizeof(h));
            dest += str_;
            socket_ptr_->async_send(dest);
        }
    }
    static void send(socket_ptr_t socket_ptr_, uint16_t cmd_, const codec_i& src_msg_)
    {
        if (socket_ptr_)
        {
            socket_ptr_->async_send(src_msg_.encode(cmd_));
        }
    }
};

#endif
