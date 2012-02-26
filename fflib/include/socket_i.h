#ifndef _SOCKET_I_
#define _SOCKET_I_

#include <string>
using namespace std;

#include "epoll_fd_i.h"

class socket_i: public epoll_fd_i
{
public:
    socket_i():
        m_data(NULL) {}
    virtual ~socket_i(){}

    virtual void open() = 0;
    virtual void async_send(const string& buff_) = 0;
    virtual void async_recv() = 0;

    virtual void set_data(void* p) { m_data = p; }
    template<typename T>
    T* get_data() const { return (T*)m_data; }
    
private:
    void*   m_data;
};

#endif
