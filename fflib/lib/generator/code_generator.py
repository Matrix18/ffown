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


    def format_field_declare_code(self, field_def, prefix = ''):
        type_str   = field_def.get_type()
        field_name = field_def.get_name()
        key_type = field_def.get_key_type()
        val_type = field_def.get_val_type()
        ret = prefix
        if val_type != '':
            ret = ret + 'map<%s, %s> %s;' %(key_type, val_type, field_name)
        elif key_type != '':
            ret = ret + 'vector<%s> %s;' %(key_type, field_name)
        else:
            ret = ret + '%s %s;' %(type_str, field_name)
        return ret
    def format_field_parse_code(self, field_def, prefix = ''):
        type_str   = self.get_type()
        field_name = self.get_name()
        key_type = self.get_key_type()
        val_type = field_def.get_val_type()

        ret = ''
        ret +='''%s    const json_value_t& %s = jval_["%s"];''' %(prefix, field_name, field_name)
        if 'int8' == type_str or 'int16' == type_str or 'int32' == type_str:
            ret += \
        '''
    %sif (false == %s.IsNumber())
    %s{
        %ssnprintf(buff, sizeof(buff), "%s::%s[%s] field needed");
        %sthrow msg_exception_t(buff);
    %s}
    %sthis->%s = %s.GetInt();
        ''' % (prefix, field_name, prefix, prefix, field_def.get_parent().get_name(), field_name, type_str, prefix, prefix, prefix, field_name, field_name)
        elif 'string' == type_str:
            ret += \
        '''
    %sif (false == %s.IsString())
    %s{
        %ssnprintf(buff, sizeof(buff), "%s::%s[%s] field needed");
        %sthrow msg_exception_t(buff);
    %s}
    %sthis->%s = %s.GetString();
        ''' % (prefix, field_name, prefix, prefix, field_def.get_parent().get_name(), field_name, type_str, prefix, prefix, prefix, field_name, field_name)
        elif 'float' == type_str:
            ret += \
        '''
    %sif (false == %s.IsDouble())
    %s{
        %ssnprintf(buff, sizeof(buff), "%s::%s[%s] field needed");
        %sthrow msg_exception_t(buff);
    %s}
    %sthis->%s = %s.GetDouble();
        ''' % (prefix, field_name, prefix, prefix, field_def.get_parent().get_name(), field_name, type_str, prefix, prefix, prefix, field_name, field_name)
        elif 'array' == type_str:
            ret += \
        '''
    %sif (false == %s.IsArray())
    %s{
        %ssnprintf(buff, sizeof(buff), "%s::%s[%s] field needed");
        %sthrow msg_exception_t(buff);
    %s}
    %sfor (rapidjson::SizeType i = 0; i < %s.Size(); i++)
    %s{
        %sconst json_value_t& val = %s[i];
        %sif (false == val.%s())
        %s{
            %ssnprintf(buff, sizeof(buff), "%s::%s field at[%%u] must %s", i);
            %sthrow msg_exception_t(buff);
        %s}
        %s%s tmp_val;
        %s%s;
		%sthis->%s.push_back(tmp_val);
    %s}
        ''' % (prefix, field_name, \
               prefix, \
               prefix, field_def.get_parent().get_name(), field_name, type_str, \
               prefix, \
               prefix, \
               prefix, field_name, \
               prefix, \
               prefix, field_name, \
               prefix, convert_to_check_op(key_type), \
               prefix, \
               prefix, field_def.get_parent().get_name(), field_name, type_str, \
               prefix, \
               prefix, \
               prefix, key_type, \
               prefix, convert_to_fetch_op(key_type), \
               prefix, field_name, \
               prefix)
        elif 'dictionary' == type_str:
            ret += \
        '''
    %sif (false == %s.IsObject())
    %s{
        %ssnprintf(buff, sizeof(buff), "%s::%s[%s] field needed");
        %sthrow msg_exception_t(buff);
    %s}
    %srapidjson::Document::ConstMemberIterator it = %s.MemberBegin();
    %sfor (; it != %s.MemberEnd(); ++it)
    %s{
    %s    const  json_value_t& name = it->name;
    %s    if (false == name.%s())
    %s    {
    %s        snprintf(buff, sizeof(buff), "%s::%s[Object] key must [string]");
    %s        throw msg_exception_t(buff);
    %s    }
    %s    const  json_value_t& val = it->value;
    %s    if (false == val.%s())
    %s    {
    %s        snprintf(buff, sizeof(buff), "%s::%s[Object] val must [%s]");
    %s        throw msg_exception_t(buff);
    %s    }
    %s    %s key_val; %s;
    %s    %s tmp_val; %s;
    %s    this->%s[key_val] = tmp_val;
    %s}
        ''' % (prefix, field_name, \
               prefix, \
               prefix, field_def.get_parent().get_name(), field_name, type_str, \
               prefix, \
               prefix, \
               prefix, field_name, \
               prefix, field_name, \
               prefix, \
               prefix, \
               prefix, convert_to_check_op(key_type), \
               prefix, \
               prefix, field_def.get_parent().get_name(), field_name, \
               prefix, \
               prefix, \
               prefix, \
               prefix, convert_to_check_op(val_type), \
               prefix, \
               prefix, field_def.get_parent().get_name(), field_name, val_type, \
               prefix, \
               prefix, \
               prefix, key_type, convert_to_fetch_op(key_type, 'key_val', 'name'), \
               prefix, val_type, convert_to_fetch_op(val_type), \
               prefix, field_name, \
               prefix)
        else:
            ret += \
        '''
    %sif (false == %s.IsObject())
    %s{
        %ssnprintf(buff, sizeof(buff), "%s::%s[%s] field needed");
        %sthrow msg_exception_t(buff);
    %s}
    %s%s;
        ''' % (prefix, field_name, \
               prefix, \
               prefix, field_def.get_parent().get_name(), field_name, type_str, \
               prefix, \
               prefix, \
               prefix, convert_to_fetch_op(key_type, 'this->%s' %(field_name), field_name))
        return ret

    def format_struct_declare_code(self, struct_def, prefix = ''):
        struct_name = struct_def.get_name()
        all_sub_struct = struct_def.get_all_struct()
        all_fields = struct_def.get_all_field()
        ret = '' + prefix
        ret += 'struct %s {\n' %(struct_name)
        for sub_struct in all_sub_struct:
            #ret = ret + sub_struct.format_declare_code(prefix + '    ') + '\n'
            ret = ret + self.format_struct_declare_code(sub_struct, prefix + '    ') + '\n'

        for field_name in all_fields:
            #ret = ret + all_fields[field_name].format_declare_code(prefix + '    ') + '\n'
            ret = ret +  self.format_field_declare_code(all_fields[field_name], prefix + '    ') + '\n'
            

        ret = ret + prefix + '    int parse(const json_value_t& jval_) {\n'
        ret = ret + prefix + '        char buff[128];\n'
        for field_name in all_fields:    
            ret += all_fields[field_name].format_parse_code(prefix + '    ') + '\n'
        ret += prefix + '        return 0;\n' + prefix + '    }'

        ret = ret + '\n' + prefix + '};'
        return ret

    def gen_declare_code(self, f):
        f.write(inc_str)
        all_struct = self.struct_def_mgr.get_all_struct()
        for struct_name in all_struct:
            cur_struct = all_struct[struct_name]
            all_sub_struct = cur_struct.get_all_struct()
            ret_str = self.format_struct_declare_code(cur_struct)
            #print(ret_str)
            f.write(ret_str)
    def exe(self):
        f = open("msg_def.h", "w")
        self.gen_declare_code(f)
        self.gen_class_def_code(f)

    