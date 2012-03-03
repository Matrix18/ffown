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

    const string& get_cmd() const               { return m_cmd; }
    const string& get_key() const                { return m_key; }
    const string& get_body() const;
    size_t size() const                                    { return m_body.size(); }
    uint16_t get_flag() const                        { return m_flag; }

    void append_msg(char* buff, size_t len) { m_body.append(buff, len); }
    void clear()
    {
        m_flag = 0;
        m_cmd.clear();
        m_key.clear();
        m_body.clear();
    }
private:
    uint16_t    m_flag;
    string        m_cmd;
    string        m_key;
    string        m_body;
};
#endif
