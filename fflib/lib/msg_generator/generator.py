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
                    val_type = key_and_value[1].strip()

            dest_data[type_struct[len(type_struct) -1]][field_name] = [field_atomic_type, key_type, val_type]

    func_vt.append(find_struct_name)
    func_vt.append(find_struct_begin)
    func_vt.append(find_struct_end)
    func = find_struct_name

    for line in all_line:
        if 0 == len(func_stack):
            func_stack.append(find_struct_name)
        func_stack[len(func_stack)-1](line)

    #print("len func_stack", len(func_stack), dest_data)
    #print(dest_data)
    for type_name in dest_data:
        tmp = "%s include fileds:" % (type_name)
        print(tmp)
        all_fileds = dest_data[type_name]
        for filed in all_fileds:
            tmp = "%10s   ==>> %s " % (filed, all_fileds[filed])
            print(tmp)
    return dest_data


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
    if var == "int8":
        return "IsNumber"
    elif var == "int16": #or var == "int32" or var == "int64":
        return "IsNumber"
    elif var == "string":
        return "IsString"
    elif var == "float":
        return "IsDouble"
    elif var == "array":
        return "IsArray"
    elif var == "dictionary":
        return "IsObject"
    return "IsObject"

def convert_type_to_fetch_op(var):
    if var == "int8": #or var == "int16" or var == "int32" or var == "int64":
        return "GetInt"
    if var == "int16":
        return "GetInt"
    elif var == "string":
        return "GetString"
    elif var == "float":
        return "GetDouble"
    return var

def gen_cpp(type_info):
    print("gen_cpp begin")
    # gen struct code...
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
        ret = ret + tmp

    print(ret)
    #gen  template head
    head_str = open("template_head.txt").read()
    for struct_name in tmp_type_store:
        head_str = head_str + "        m_reg_func[\"" + struct_name + "\"]" + " = &msg_dispather_t<T, R>::%s_dispacher;" % (struct_name) + "\n"
    head_str = head_str + "    }\n" + "    int dispath(const string& json_, socket_ptr_t sock_);\n\nprivate:\n"
    for struct_name in tmp_type_store:
        #head_str = head_str + "    int %s_dispacher(const json_value_t& jval_, socket_ptr_t sock_)" % (struct_name) + "\n    {\n"
        head_str = head_str + "   int %s_dispacher(const json_value_t& jval_, socket_ptr_t sock_)" % (struct_name) + ";\n"

    head_str = head_str + "\nprivate:\n    T&                      m_msg_handler;\n"
    head_str = head_str + "    map<string, reg_func_t> m_reg_func;\n};"

    head_str = head_str + "\n" + open("template_end.txt").read()
    #print(head_str)

    
    # append impl code 
    for struct_name in tmp_type_store:
        # one struct impl code begin
        head_str = head_str + "\ntemplate<typename T, typename R>\nint msg_dispather_t<T, R>::%s_dispacher(const json_value_t& jval_, socket_ptr_t sock_)" % (struct_name) + "\n{\n"

        head_str = head_str + "    " + "%s ret_val;" % (struct_name)
        head_str = head_str + "\n    " + "char buff[512];\n"
        all_fileds = type_info[struct_name]

        for field_name in all_fileds:
            type_str = all_fileds[field_name][0]
            key_type = all_fileds[field_name][1]
            val_type = all_fileds[field_name][2]
            head_str = head_str + "\n    " + "const json_value_t& %10s = jval_[\"%s\"];" % (field_name, field_name)
            head_str = head_str + "\n    " + "if (false == %s.%s())\n    { " % (field_name, convert_type_to_json_type(type_str))
            head_str = head_str + "\n    " + "    snprintf(buff, sizeof(buff), \"%s::%s[%s] field needed\");" % (struct_name, field_name, type_str)
            head_str = head_str + "\n    " + "    throw msg_exception_t(buff);\n    }\n"
            op = convert_type_to_fetch_op(type_str)
            if op != type_str:
                head_str = head_str + "    " + "ret_val.%s = %s.%s();\n " % (field_name, field_name, op)
            elif type_str == "array":
                head_str = head_str + "    " + "for (rapidjson::SizeType i = 0; i < %s.Size(); i++)\n    {\n" % (field_name)
                head_str = head_str + "    " + "    const json_value_t& val = %s[i];\n" % (field_name)
                head_str = head_str + "    " + "    if (false == val.%s())\n        {" % (convert_type_to_json_type(key_type))
                head_str = head_str + "\n    " + "        snprintf(buff, sizeof(buff), \"%s::%s[%s] field at[%%u] must %s\", i);" % (struct_name, field_name, type_str, key_type)
                head_str = head_str + "\n    " + "        throw msg_exception_t(buff);\n        }\n"
                head_str = head_str + "\n    " + "    ret_val.%s.push_back(val.%s());\n    }\n" % (field_name, convert_type_to_fetch_op(key_type))
            elif type_str == "dictionary":
                head_str = head_str + "    " + "rapidjson::Document::ConstMemberIterator it = %s.MemberBegin();\n" % (field_name)
                head_str = head_str + "    " + "for (; it != %s.MemberEnd(); ++it)\n    {\n" % (field_name)
                head_str = head_str + "    " + "    const json_value_t& _name = it->name;\n"
                head_str = head_str + "    " + "    if (false == _name.IsString())\n        {\n"
                head_str = head_str + "    " + "        snprintf(buff, sizeof(buff), \"%s::%s[%s] key must [%s]\");\n" % (struct_name, field_name, type_str, key_type)
                head_str = head_str + "    " + "        throw msg_exception_t(buff);\n        }\n"
                head_str = head_str + "        const  json_value_t& _val = it->value;\n"
                head_str = head_str + "    " + "    if (false == _val.%s())\n        {\n" % (convert_type_to_json_type(key_type))
                head_str = head_str + "    " + "        snprintf(buff, sizeof(buff), \"%s::%s[%s] value must [%s]\");\n" % (struct_name, field_name, type_str, key_type)
                head_str = head_str + "    " + "        throw msg_exception_t(buff);\n        }"
                head_str = head_str + "\n        ret_val.%s[_name.GetString()] = _val.%s();" % (field_name, convert_type_to_fetch_op(val_type))
                head_str = head_str + "\n    }\n"
            else:
                sub_type = struct_name + "::" + op
                sub_all_fileds = type_info[sub_type]
                #print("####", op, sub_type, sub_all_fileds)
                for sub_filled in sub_all_fileds:
                    sub_type_str = sub_all_fileds[sub_filled][0]
                    sub_key_type = convert_type_str(sub_all_fileds[sub_filled][1])
                    sub_val_type = convert_type_str(sub_all_fileds[sub_filled][2])
                    var_name = "_%s_%s" % (field_name, sub_filled)
                    head_str = head_str + "    const  json_value_t& %s = %s[\"%s\"];\n" % (var_name, field_name, sub_filled)
                    #print("###", sub_filled, sub_type_str, convert_type_to_json_type(sub_type_str))
                    head_str = head_str + "    " + "if (false == %s.%s())\n    {\n" % (var_name, convert_type_to_json_type(sub_type_str))
                    head_str = head_str + "    " + "    snprintf(buff, sizeof(buff), \"%s::%s[%s]->%s key must [%s]\");\n" % (struct_name, field_name, type_str, sub_filled, sub_type_str)
                    head_str = head_str + "    " + "    throw msg_exception_t(buff);\n"
                    head_str = head_str + "    }\n"
                    head_str = head_str + "    ret_val.%s.%s = %s.%s();\n" % (field_name, sub_filled, var_name, convert_type_to_fetch_op(sub_type_str))
                



    # one struct impl code end
    head_str = head_str + "    m_msg_handler.handle(ret_val, sock_);"
    head_str = head_str + "\n    return 0;\n}\n"

    f = open("msg_def.h", "w")
    str = open("template_inc.txt").read()#include files code
    f.write(str)
    f.write(ret)
    f.write(head_str)
    f.close()
try:
    type_info = analyse_type_info("foo.cpp")
    gen_cpp(type_info)
except:
    print("exception")
