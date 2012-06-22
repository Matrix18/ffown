#ifndef _BROKER_SERVICE_H_
#define _BROKER_SERVICE_H_


class broker_service_t: public msg_handler_i
{
public:
    broker_service_t();
    ~broker_service_t();

    int handle_broken(socket_ptr_t sock_);
    int handle_msg(const message_t& msg_, socket_ptr_t sock_);
};
#endif
