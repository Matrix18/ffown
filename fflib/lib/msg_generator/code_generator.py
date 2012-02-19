from inc import *

inc_str = '''
#include <stdint.h>
#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include <iostream>
using namespace std;

#include "rapidjson/document.h"     // rapidjson's DOM-style API
#include "rapidjson/prettywriter.h" // for stringify JSON
#include "rapidjson/filestream.h"   // wrapper of C stream for prettywriter as output
//! using namespace rapidjson;

typedef runtime_error        msg_exception_t;
typedef rapidjson::Document  json_dom_t;
typedef rapidjson::Value     json_value_t;

typedef int int8 ;
typedef int int16;
typedef int int32;

'''

class code_generator_t:
    def __init__(self, struct_def_mgr):
        self.struct_def_mgr = struct_def_mgr

    def gen_class_def_code(self, f):
        tmp_s = '''

template<typename T, typename R>
class msg_dispather_t
{
    typedef R                    socket_ptr_t;
    typedef int (msg_dispather_t<T, R>::*reg_func_t)(const json_value_t&, socket_ptr_t);
public:
    msg_dispather_t(T& msg_handler_):
        m_msg_handler(msg_handler_)
    {
        '''
        for struct in self.struct_def_mgr.get_all_struct():
            tmp_s += '''
        m_reg_func["%s"] = &msg_dispather_t<T, R>::%s_dispacher;
            ''' % (struct, struct)

        tmp_s += '\n    }\n'

        tmp_s += '''
    int dispath(const string& json_, socket_ptr_t sock_)
    {
        json_dom_t document;  // Default template parameter uses UTF8 and MemoryPoolAllocator.
        if (document.Parse<0>(json_.c_str()).HasParseError())
        {
            throw msg_exception_t("json format not right");
        }
        if (false == document.IsObject() && false == document.Empty())
        {
            throw msg_exception_t("json must has one field");
        }
    
        const json_value_t& val = document.MemberBegin()->name;
        const char* func_name   = val.GetString();
        typename map<string, reg_func_t>::const_iterator it = m_reg_func.find(func_name);
    
        if (it == m_reg_func.end())
        {
            char buff[512];
            snprintf(buff, sizeof(buff), "msg not supported<%s>", func_name);
            throw msg_exception_t(buff);
            return -1;
        }
        reg_func_t func = it->second;
    
        (this->*func)(document.MemberBegin()->value, sock_);
        return 0;
    }
        '''

        for struct in self.struct_def_mgr.get_all_struct():
            tmp_s += '''
    int %s_dispacher(const json_value_t& jval_, socket_ptr_t sock_)
    {
        %s ret_val;
        ret_val.parse(jval_);

        m_msg_handler.handle(ret_val, sock_);
        return 0;
    }
            ''' % (struct, struct)

        tmp_s += '''
private:
    T&                      m_msg_handler;
    map<string, reg_func_t> m_reg_func;
};
        '''
        f.write(tmp_s)
    def gen_declare_code(self, f):
        f.write(inc_str)
        all_struct = self.struct_def_mgr.get_all_struct()
        for struct_name in all_struct:
            cur_struct = all_struct[struct_name]
            all_sub_struct = cur_struct.get_all_struct()
            str = cur_struct.format_declare_code()
            #print(str)
            f.write(str)
    def exe(self):
        f = open("msg_def.h", "w")
        self.gen_declare_code(f)
        self.gen_class_def_code(f)