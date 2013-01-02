
#ifndef _FF_LOG_H_
#define _FF_LOG_H_

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sstream>
using namespace std;

namespace ff
{

class str_format_t
{
	struct fmt_type_t
	{
		fmt_type_t():
			type('\0'),
			min_len(0)
		{}
		void clear()
		{
			type = '\0';
			min_len = 0;
		}
		char 			type;//! % d,x,f,s,
		unsigned int 	min_len;
	};
public:
	//! fmt_ like "xxx%d,xx%s"
	str_format_t(const char* fmt_ = "");
	virtual ~str_format_t();

	template<typename T>
	void append(T content_)
	{
		if (move_to_next_wildcard())
		{
			if (m_fmt_type.type == 'x')
			{
				char buff[64];
				snprintf(buff, sizeof(buff), "0x%x", (unsigned int)content_);
				m_num_buff = buff;
			}
			else
			{
				m_strstream << content_;
				m_strstream >> m_num_buff;
			}
			int width = m_fmt_type.min_len > m_num_buff.length()? m_fmt_type.min_len - m_num_buff.length(): 0;
			for (; width > 0; -- width)
			{
				m_result += ' ';
			}
		}
		else
		{
			m_strstream << content_;
			m_strstream >> m_num_buff;
		}

		m_result += m_num_buff;
		m_strstream.clear();//! clear error bit,not content
		m_num_buff.clear();
	}
	void append(const char* str_);
	void append(const string& str_);
	const string& gen_result();
private:
	bool move_to_next_wildcard();

protected:
	const char* 	m_fmt;
	unsigned int    cur_format_index;
	unsigned int 	m_fmt_len;
	fmt_type_t      m_fmt_type;
	string      	m_result;
	stringstream    m_strstream;
	string          m_num_buff;
};

class log_t
{
public:
static int log_impl(const char* mod, const char* fmt, ...)
{
    char buff[256];
    int len = snprintf(buff, sizeof(buff), "%s ", mod);

    va_list vl;
    va_start(vl, fmt);
    vsnprintf(buff + len, sizeof(buff) - len - 1, fmt, vl);
    va_end(vl);
    printf("%s", buff);
    return 0;
}

public:
	log_t();
	virtual ~log_t();

	inline bool enable_level(int level_);
	inline bool enable_class(const char* class_);

	void log_content(const char* class_, const string& content_);

protected:
	int 	m_enabled_level;
};

#define BROKER  "BROKER"
#define RPC     "RPC"
#define FF      "FF"
#define MSG_BUS "MSG_BUS"

#define logdebug(content) printf("\033[1;33mDEBUG "); log_t::log_impl content ; printf("\033[0m\n")
#define logtrace(content) printf("TRACE "); log_t::log_impl content ; printf("\n")
#define loginfo(content) printf("\033[1;32mINFO  "); log_t::log_impl content ; printf("\033[0m\n")
#define logwarn(content) printf("\033[1;34mWARN  "); log_t::log_impl content ; printf("\033[0m\n")
#define logerror(content) printf("\033[0;31mERROR "); log_t::log_impl content ; printf("\033[0m\n")
#define logfatal(content) printf("\033[0;35mFATAL "); log_t::log_impl content ; printf("\033[0m")


}

#endif


