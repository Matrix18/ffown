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
    if var == "array":
        return "vector"
    if var == "dictionary":
        return "map"
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
        head_str = head_str + "    int %s_dispacher(const json_value_t& jval_, socket_ptr_t sock_)" % (struct_name) + "\n    {\n"
        head_str = head_str + "        return 0;\n    }\n"

    head_str = head_str + "\nprivate:\n    T&                      m_msg_handler;\n"
    head_str = head_str + "    map<string, reg_func_t> m_reg_func;\n};"

    head_str = head_str + "\n" + open("template_end.txt").read()
    #print(head_str)

    f = open("msg_def.h", "w")
    f.write(ret)
    f.write(head_str)
    f.close()
try:
    type_info = analyse_type_info("foo.cpp")
    gen_cpp(type_info)
except:
    print("exception")
