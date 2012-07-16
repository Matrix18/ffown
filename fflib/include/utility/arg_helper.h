#ifndef _ARG_HELPER_H_
#define _ARG_HELPER_H_

#include <string>
using namespace std;

class arg_helper_t
{
public:
	arg_helper_t(int argc, char* argv[]):
		m_argc(argc),
		m_argv(argv)
	{
	}
	string get_option(int idx_)
	{   
		if (idx_ >= m_argc)
		{   
			return ""; 
		}   
		return m_argv[idx_];
	}   
	bool is_enable_option(string opt_)
	{
		for (int i = 0; i < m_argc; ++i)
		{
			if (opt_ == m_argv[i])
			{
				return true;
			}
		}
		return false;
	}

	string get_option_value(string opt_)
	{
		string ret;
		for (int i = 0; i < m_argc; ++i)
		{   
			if (opt_ == m_argv[i])
			{   
				int value_idx = ++ i;
				if (value_idx >= m_argc)
				{
					return ret;
				}
				ret = m_argv[value_idx];
				return ret;
			}   
		}	
		return ret;
	}
private:
	int    m_argc;
	char** m_argv;
};

#endif

