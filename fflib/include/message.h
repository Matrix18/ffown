#ifndef _MESSAGE_H_
#define _MESSAGE_H_

#include <stdint.h>
#include <string>
using namespace std;

class message_t
{
public:
    message_t()
    {
    }

    uint16_t get_cmd() const               { return m_cmd; }
    const string& get_body() const         { return m_body;}
    size_t size() const                    { return m_body.size(); }
    uint16_t get_flag() const              { return m_flag; }

    void append_msg(char* buff, size_t len) { m_body.append(buff, len); }
    void clear()
    {
        m_flag = 0;
        m_cmd  = 0;
        m_body.clear();
    }
private:
    uint16_t      m_flag;
    uint16_t      m_cmd;
    string        m_body;
};
#endif
