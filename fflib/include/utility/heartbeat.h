#ifndef _HEARTBEAT_H_
#define _HEARTBEAT_H_

#include <map>
#include <list>
using namespace std;

namespace ff {

class heartbeat_t
{
    typedef long type_t;
    struct node_info_t
    {
        node_info_t():
            timeout(0)
        {}
        type_t  value;
        long    timeout;
    };
    typedef list<node_info_t>                           soft_timeout_list_t;
    typedef map<type_t, soft_timeout_list_t::iterator>  data_map_t;
public:
    heartbeat_t();
    ~heartbeat_t();
    
    int add(const type_t& v_);
    int del(const type_t& v_);
    int update(const type_t& v_);

private:
    long                        m_timeout;
    data_map_t                  m_data_map;
    soft_timeout_list_t         m_time_sort_list;
};

heartbeat_t::heartbeat_t()
{
    
}

heartbeat_t::~heartbeat_t()
{
    
}

int heartbeat_t::add(const type_t& v_)
{
    node_info_t info;
    info.value   = v_;
    info.timeout = ::time(NULL) + m_timeout;

    m_time_sort_list.push_front(info);

    pair<data_map_t::iterator, bool> ret =  m_data_map.insert(make_pair(v_, m_time_sort_list.begin()));

    if (ret.second == false)
    {
        m_time_sort_list.pop_front();
        return -1;
    }

    return 0;
}

int heartbeat_t::update(const type_t& v_)
{
    data_map_t::iterator it = m_data_map.find(v_);
    if (m_data_map.end() == it)
    {
        return -1;
    }

    node_info_t tmp_info = *(it->second);
    m_time_sort_list.erase(it->second);

    m_time_sort_list.push_front(tmp_info);
    it->second = m_time_sort_list.begin();

    return 0;
}

int heartbeat_t::del(const type_t& v_)
{
    data_map_t::iterator it = m_data_map.find(v_);
    if (m_data_map.end() == it)
    {
        return -1;
    }

    m_time_sort_list.erase(it->second);
    m_data_map.erase(it);
    
    return 0;
}
};
#endif
