def filter_comment(content_):
    return content_
    
def analyse_type_info(file_):
    f_content = open(file_, "r").read()
    f_content = filter_comment(f_content)

    all_line = f_content.split("\n")

    dest_data  = {}
    func_vt    = []
    type_struct= []
    func_stack = []

    def find_struct_name(line):
        #print("find_struct_name")
        pos = line.find("struct")
        if -1 != pos:
            line_data = [line[0:pos+len("struct")], line[pos+len("struct")+1:]]
            struct_name = line_data[1].split("(")[0]
            if len(type_struct) > 0:
                struct_name = type_struct[len(type_struct) - 1] + "::" + struct_name
            #print("find struct", struct_name)            
            dest_data[struct_name] = {}
            type_struct.append(struct_name)
            func_stack.append(func_vt[1])
            return
        return
    def find_struct_begin(line):
        #print("find_struct_begin")
        pos = line.find("{")
        if -1 != pos:
            func_stack.pop()
            func_stack.append(func_vt[2])
            return
        return
    def find_struct_end(line):
        #print("find_struct_end")
        pos = line.find("}")
        if -1 != pos:
            func_stack.pop()#self
            func_stack.pop()#find_struct_name
            type_struct.pop()
            return

        pos = line.find("struct")
        if -1 != pos:
            func_stack.append(func_vt[0])
            func_vt[0](line)
            return

        data = line.split("(")[0]
        data = data.split(";")[0]
        pos = data.rfind(" ");
        if pos == -1 and data.strip() != "":
            raise Exception ("filed define invalid", line)
        field_name = data[pos:].strip()
        field_type = data[0:pos].strip()

        if "" != field_name and "" != field_type:
            field_type_tmp = field_type.split("<")

            field_atomic_type = field_type_tmp[0]
            key_type = ""
            val_type = ""

            if len(field_type_tmp) > 1:
                sub_str = field_type_tmp[1].split(">")[0]
                key_and_value  = sub_str.split(",")
                key_type = key_and_value[0]
                if len(key_and_value) > 1:
                    val_type = key_and_value[1]

            dest_data[type_struct[len(type_struct) -1]][field_name] = [field_atomic_type, key_type, val_type.strip()]

    func_vt.append(find_struct_name)
    func_vt.append(find_struct_begin)
    func_vt.append(find_struct_end)
    func = find_struct_name

    for line in all_line:
        if 0 == len(func_stack):
            func_stack.append(find_struct_name)
        func_stack[len(func_stack)-1](line)

    for type_name in dest_data:
        tmp = "%s include fileds:" % (type_name)
        print(tmp)
        all_fileds = dest_data[type_name]
        for filed in all_fileds:
            tmp = "%10s   ==>> %s " % (filed, all_fileds[filed])
            print(tmp)
    return dest_data

def convert_idl_type_to_check_method(field_type):
    if field_type == "int8":
        return 'IsNumber'
    elif field_type == "int16":
        return 'IsNumber'
    elif field_type == "int32":
        return 'IsNumber'
    elif field_type == "float":
        return 'IsDouble'
    elif field_type == "string":
        return 'IsString'
    elif field_type == "array":
        return 'IsArray'
    return 'IsObject'


def convert_type_to_func_prefix(field_type, key_type, val_type):
        ret = None
        if field_type == "int8":
            return ret
        elif field_type == "int16":
            return ret
        elif field_type == "int32":
            return ret
        elif field_type == "float":
            return ret
        elif field_type == "string":
            return ret
        elif field_type == "array":
            ret = ['array_%s_t' % (key_type), 'vector<%s>' % (key_type)]
        elif field_type == "dictionary":
            ret = ['dictionary_%s_%s_t' % (key_type, val_type), 'map<%s, %s>' % (key_type, val_type)]
        return ret

def fetch_field_val(prefix, field_type_info, field):

    field_type = field_type_info[0]
    key_type   = field_type_info[1]
    val_type   = field_type_info[1]
    ret = '%s_parse(ret_val.%s, %s)' % (field_type, field, field)
    if field_type == "int8":
        ret = 'ret_val.%s = %s.GetInt()' % (field, field)
    elif field_type == "int16":
        ret = 'ret_val.%s = %s.GetInt()' % (field, field)
    elif field_type == "int32":
        ret = 'ret_val.%s = %s.GetInt()' % (field, field)
    elif field_type == "float":
        ret = 'ret_val.%s = %s.GetDouble()' % (field, field)
    elif field_type == "string":
        ret = 'ret_val.%s = %s.GetString()' % (field, field)
    elif field_type == "array":
        ret = 'msg_dispather_t<T, R>::array_%s_t_parse(ret_val.%s, %s)' % (key_type, field, field)
    elif field_type == "dictionary":
        ret = 'msg_dispather_t<T, R>::dictionary_%s_%s_t_parse(ret_val.%s, %s)' % (key_type, val_type, field, field)
    return ret

def gen_child_struct_parse(prefix, field_type, field, all_child_fields):
    ret = '''
template<typename T, typename R>
//! %%s replace int msg_dispather_t<T, R>::student_t_book_t_parse(book_t& ret_val, const json_value_t& jval_)
int msg_dispather_t<T, R>::%s_%s_parse(%s& ret_val, const json_value_t& jval_)
{
    const  json_value_t& book_pages   = jval_["pages"];
    const  json_value_t& book_contens = jval_["contents"];
    if (false == book_pages.IsNumber())
    {
        snprintf(buff, sizeof(buff), "student::books::pages[Number] field needed");
        throw msg_exception_t(buff);
    }
    ret_val.pages    = book_pages.GetInt();
    if (false == book_contens.IsString())
    {
        snprintf(buff, sizeof(buff), "student::books::book_contens[String] field needed");
        throw msg_exception_t(buff);
    }
    ret_val.contents = book_contens.GetString();
}
    ''' % (prefix, field, field)
    return ret

def gen_includes():
    ret = '''
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
    '''
    return ret

def gen_class_def(all_struct_name):
    typedef_dict = {}
    s_reg = ''

    for struct_name in all_struct_name:
        if -1 == struct_name.find("::"):
            s_reg = s_reg + '''
        m_reg_func["%s"] = &msg_dispather_t<T, R>::%s_dispacher;''' % (struct_name, struct_name)

    s_func_def = ''
    for struct_name in all_struct_name:
        all_child_fields = all_struct_name[struct_name]
        for field in all_child_fields:
            type_str = all_child_fields[field][0]
            key_str = all_child_fields[field][1]
            val_str = all_child_fields[field][2]
            func_dict = convert_type_to_func_prefix(type_str, key_str, val_str)
            if func_dict != None:
                func_name = func_dict[0]
                if None == typedef_dict.get(func_name):
                    typedef_dict[func_name] = True
                    s_func_def = s_func_def + '''
    typedef %s %s;''' % (func_dict[1], func_name)

                s_func_def = s_func_def + '''
    int %s_parse(%s& ret_val, const json_value_t& jval_);''' % (func_name, func_name)

        if -1 != struct_name.find("::"):
            s_func_def = s_func_def + '''
    int %s_parse(%s& ret_val, const json_value_t& jval_);''' % (struct_name.split("::")[1], struct_name)
            continue
        s_func_def = s_func_def + '''
    int %s_parse(%s& ret_val, const json_value_t& jval_);''' % (struct_name, struct_name)
        s_func_def = s_func_def + '''
    int %s_dispacher(const json_value_t& jval_, socket_ptr_t sock_);''' % (struct_name)

    ret = '''
template<typename T, typename R>
class msg_dispather_t
{
    typedef runtime_error        msg_exception_t;
    typedef rapidjson::Document  json_dom_t;
    typedef rapidjson::Value     json_value_t;
    typedef R                    socket_ptr_t;
    typedef int (msg_dispather_t<T, R>::*reg_func_t)(const json_value_t&, socket_ptr_t);
public:
    msg_dispather_t(T& msg_handler_):
        m_msg_handler(msg_handler_)
    {
        //! %%s replace m_reg_func["student_t"] = &msg_dispather_t<T, R>::student_t_dispacher;
        %s
    }
    int dispath(const string& json_, socket_ptr_t sock_);

private:
    //! %%s replace int student_t_dispacher(const json_value_t& jval_, socket_ptr_t sock_);
    %s

private:
    T&                      m_msg_handler;
    map<string, reg_func_t> m_reg_func;
};

template<typename T, typename R>
int msg_dispather_t<T, R>::dispath(const string& json_, socket_ptr_t sock_)
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
        snprintf(buff, sizeof(buff), "msg not supported<%%s>", func_name);
        throw msg_exception_t(buff);
        return -1;
    }
    reg_func_t func = it->second;

    (this->*func)(document.MemberBegin()->value, sock_);
    return 0;
}
    ''' % (s_reg, s_func_def)

    return ret
    

def gen_class_impl(all_struct_info):
    ret = ''
    for struct_name  in all_struct_info:
        if struct_name.find("::") != -1:
            continue
        fields_var = ''
        all_fileds = all_struct_info[struct_name]
        for field in all_fileds:
            field_type = all_fileds[field][0]
            fields_var = fields_var + '''
    const json_value_t& %s = jval_["%s"];
    //! %%s replace if (false == age.IsNumber())
    if (false == %s.%s())
    {
        //! replace snprintf(buff, sizeof(buff), "student::age[int] field needed");
        snprintf(buff, sizeof(buff), "%s::%s[%s] field needed");
        throw msg_exception_t(buff);
    }
    //! %%s replace ret_val.age = age.GetInt();
    %s;
            ''' % (field, field, field, convert_idl_type_to_check_method(field_type), struct_name, field, field_type, \
                   fetch_field_val(struct_name, all_fileds[field], field))

        single_parse_func_impl = '''
template<typename T, typename R>
//! %%s replace int msg_dispather_t<T, R>::student_t_parse(student_t& ret_val, const json_value_t& jval_)
int msg_dispather_t<T, R>::%s_parse(student_t& ret_val, const json_value_t& jval_)
{
    char buff[128];
    //! %%s replace const json_value_t& age      = jval_["age"];
    %s
    return 0;
}
        ''' % (struct_name, fields_var)
        ret = ret + single_parse_func_impl

    for struct_name  in all_struct_info:
        if struct_name.find("::") != -1:
            continue
        single_dispath_func_impl = '''
template<typename T, typename R>
//! int msg_dispather_t<T, R>::student_t_dispacher(const json_value_t& jval_, socket_ptr_t sock_)
int msg_dispather_t<T, R>::%s_dispacher(const json_value_t& jval_, socket_ptr_t sock_)
{
    student_t ret_val;
    char buff[128];
    if (false == jval_.IsObject())
    {
        //! %%s replace snprintf(buff, sizeof(buff), "student::books[Object] field needed");
        snprintf(buff, sizeof(buff), "%s[Object] must");
        throw msg_exception_t(buff);
    }
    //! %%s replace student_t_parse(ret_val, jval_);
    %s_parse(ret_val, jval_);
    m_msg_handler.handle(ret_val, sock_);
    return 0;
}
        ''' % (struct_name, struct_name, struct_name)
        ret = ret + single_dispath_func_impl
    return ret

type_info = analyse_type_info("foo.cpp")
print(type_info)

def convert_type_str(var):
    if var == "int8":
        return "int8_t"
    if var == "int16":
        return "int16_t"
    if var == "int32":
        return "int32_t"
    if var == "array":
        return "vector"
    if var == "dictionary":
        return "map"
    return var

def convert_type_to_json_type(var):
    if var == "int8": #or var == "int16" or var == "int32" or var == "int64":
        return "IsNumber"
    elif var == "string":
        return "IsString"
    elif var == "float":
        return "IsDouble"
    elif var == "array":
        return "IsArray"
    elif var == "dictionary":
        return "IsObject"

def convert_type_to_fetch_op(var):
    if var == "int8": #or var == "int16" or var == "int32" or var == "int64":
        return "GetInt"
    elif var == "string":
        return "GetString"
    elif var == "float":
        return "GetDouble"
    return None

def gen_cpp(type_info):

    tmp_type_store = {}
    for struct_name in type_info:
        struct_name_pair = struct_name.split("::")
        if len(struct_name_pair) > 1:
            if None == tmp_type_store.get(struct_name_pair[0]):
                tmp_type_store[struct_name_pair[0]] = []
            tmp_type_store[struct_name_pair[0]].append(struct_name_pair[1])

    def format_sub_type(struct_name_, sub_type_name_):
        tmp = "    struct %s \n    {\n" % (sub_type_name_)
        all_fileds = type_info[struct_name_ + "::" + sub_type_name_]
        for field_name in all_fileds:
            type_str = all_fileds[field_name][0]
            if type_str == "array":
                print(convert_type_str(type_str))
                type_str = convert_type_str(type_str) + "<" + all_fileds[field_name][1] + "> "
            elif type_str == "dictionary":
                type_str = convert_type_str(type_str) + "<" + all_fileds[field_name][1] + ", " + all_fileds[field_name][2] + "> "
            tmp = tmp + "        " + convert_type_str(type_str) + " " + field_name + ";\n"
        tmp = tmp + "\n    };\n"
        return tmp
    ret = ""
    for struct_name in tmp_type_store:
        tmp = "struct %s \n{\n" % (struct_name)
        for sub_type in tmp_type_store[struct_name]:
            tmp = tmp + format_sub_type(struct_name, sub_type)

        all_fileds = type_info[struct_name]
        for field_name in all_fileds:
            type_str = all_fileds[field_name][0]
            if type_str == "array":
                type_str = convert_type_str(type_str) + "<" + all_fileds[field_name][1] + "> "
            elif type_str == "dictionary":
                type_str = convert_type_str(type_str) + "<" + all_fileds[field_name][1] + ", " + all_fileds[field_name][2] + "> "
            tmp = tmp + "    " + convert_type_str(type_str) + " " + field_name + ";\n"
        tmp = tmp + "\n};\n"
        ret = ret + tmp + "\n"

    return ret

dest_file = open("msg_def.h", "w")

ret = gen_includes()
dest_file.write(ret)

ret = gen_cpp(type_info)
dest_file.write(ret)

ret = gen_class_def(type_info)
dest_file.write(ret)
all_struct_info = {'student_t':{'a':['a', 'int'],'b':['b', 'string']}, 'teacher_t':{'c':['c', 'foo_t'],'b':['d', 'ok_t']}}
ret = gen_class_impl(type_info)
dest_file.write(ret)
dest_file.write("\n")