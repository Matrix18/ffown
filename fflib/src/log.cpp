
#include "log.h"

#include "string.h"

using namespace ff;

str_format_t::str_format_t(const char* fmt_):
	m_fmt(fmt_),
	cur_format_index(0)
{
	m_fmt_len = ::strlen(fmt_);
}

str_format_t::~str_format_t()
{

}

bool str_format_t::move_to_next_wildcard()
{
	m_fmt_type.clear();
	char tmp = '\0';

	for (; cur_format_index < m_fmt_len; ++ cur_format_index)
	{
		tmp = m_fmt[cur_format_index];
		if (tmp != '%')
		{
			m_result += tmp;
			continue;
		}

		char next = m_fmt[cur_format_index + 1];
		if (next == '%')
		{
			cur_format_index += 1;
			m_result += next;
			continue;
		}

		//! 支持多种格式化 %c %s, %d, %ld, %u, %lu, %x, %X, 找到格式化的类型
		//for (++cur_format_index; cur_format_index < m_fmt_len; ++ cur_format_index)
		for (unsigned int i = 1 ; i <= 5; ++i)
		{
			char cur = m_fmt[cur_format_index + i];

			if (cur == '\0' || cur == '%')
			{
				break;
			}
			else if (cur == 'c' || cur == 'd' || cur == 'u' || cur == 'x' ||
					 cur == 'f' || cur == 's')
			{
				m_fmt_type.type    = cur;
				m_fmt_type.min_len = ::atoi(m_fmt + cur_format_index + 1);
				cur_format_index   = cur_format_index + i + 1;
				return true;
			}
			else if (cur == 'l')
			{
				char c_num = m_fmt[cur_format_index + i + 1];
				if ('d' == c_num || 'u' == c_num)
				{
					m_fmt_type.type    = c_num;
					m_fmt_type.min_len = ::atoi(m_fmt + cur_format_index + 1);
					cur_format_index   = cur_format_index + i + 2;
					return true;
				}
			}
		}
		m_result += tmp;

	}
	return false;
}

void str_format_t::append(const char* str_)
{
	if (move_to_next_wildcard())
	{
		unsigned int len = ::strlen(str_);
		int width = m_fmt_type.min_len > len? m_fmt_type.min_len -len: 0;
		for (; width > 0; -- width)
		{
			m_result += ' ';
		}
	}

	m_result += str_;
}
void str_format_t::append(const string& str_)
{
	if (move_to_next_wildcard())
	{
		int width = m_fmt_type.min_len > str_.length()? m_fmt_type.min_len -str_.length(): 0;
		for (; width > 0; -- width)
		{
			m_result += ' ';
		}
	}

	m_result += str_;
}

const string& str_format_t::gen_result()
{
	if (cur_format_index < m_fmt_len)
	{
		m_result.append(m_fmt + cur_format_index);
	}
	return m_result;
}


log_t::log_t()
{

}

log_t::~log_t()
{

}

bool log_t::enable_level(int level_)
{
	return m_enabled_level & level_;
}

bool log_t::enable_class(const char* class_)
{
	return false;//!TODO
}

