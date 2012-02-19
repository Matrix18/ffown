
def convert_to_check_op(t):
    if t == 'int8' or t == 'int16' or t == 'int32':
        return 'IsNumber'
    elif t == 'float':
        return 'IsDouble'
    elif t == 'string':
        return 'IsString'
    elif t == 'array':
        return 'IsArray'
    elif t == 'dictionary':
        return 'IsObject'
    else:
        return 'IsObject'

def convert_to_fetch_op(t, var_name = 'tmp_val', j_var = 'val'):
    if t == 'int8' or t == 'int16' or t == 'int32':
        return '%s = %s.GetInt()' % (var_name, j_var)
    elif t == 'float':
        return '%s = %s.GetDouble();' % (var_name, j_var)
    elif t == 'string':
        return '%s = %s.GetString()' % (var_name, j_var)
    else:
        return '%s.parse(%s)' % (var_name, j_var)

class field_def_t:
    def __init__(self, name, type, key_type, val_type_):
        self.name       = name
        self.parent     = None
        self.type       = type
        self.key_type   = key_type
        self.val_type   = val_type_
    def get_name(self):
        return self.name
    def get_parent(self):
        return self.parent
    def set_parent(self, p):
        self.parent = p
    def get_type(self):
        return self.name
    def get_key_type(self):
        return self.key_type
    def get_val_type(self):
        return self.val_type
    def dump(self, prefix = ''):
        print(prefix, self.name, self.type, self.key_type, self.val_type)
    def format_declare_code(self, prefix = ''):
        ret = prefix
        if self.val_type != '':
            ret = ret + 'map<%s, %s> %s;' %(self.key_type, self.val_type, self.name)
        elif self.key_type != '':
            ret = ret + 'vector<%s> %s;' %(self.key_type, self.name)
        else:
            ret = ret + '%s %s;' %(self.type, self.name)
        return ret
    def format_parse_code(self, prefix = ''):
        ret = ''
        ret +='''%s    const json_value_t& %s = jval_["%s"];''' %(prefix, self.name, self.name)
        if 'int8' == self.type or 'int16' == self.type or 'int32' == self.type:
            ret += \
        '''
    %sif (false == %s.IsNumber())
    %s{
        %ssnprintf(buff, sizeof(buff), "%s::%s[%s] field needed");
        %sthrow msg_exception_t(buff);
    %s}
    %sthis->%s = %s.GetInt();
        ''' % (prefix, self.name, prefix, prefix, self.parent.get_name(), self.name, self.type, prefix, prefix, prefix, self.name, self.name)
        elif 'string' == self.type:
            ret += \
        '''
    %sif (false == %s.IsString())
    %s{
        %ssnprintf(buff, sizeof(buff), "%s::%s[%s] field needed");
        %sthrow msg_exception_t(buff);
    %s}
    %sthis->%s = %s.GetString();
        ''' % (prefix, self.name, prefix, prefix, self.parent.get_name(), self.name, self.type, prefix, prefix, prefix, self.name, self.name)
        elif 'float' == self.type:
            ret += \
        '''
    %sif (false == %s.IsDouble())
    %s{
        %ssnprintf(buff, sizeof(buff), "%s::%s[%s] field needed");
        %sthrow msg_exception_t(buff);
    %s}
    %sthis->%s = %s.GetDouble();
        ''' % (prefix, self.name, prefix, prefix, self.parent.get_name(), self.name, self.type, prefix, prefix, prefix, self.name, self.name)
        elif 'array' == self.type:
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
        ''' % (prefix, self.name, \
               prefix, \
               prefix, self.parent.get_name(), self.name, self.type, \
               prefix, \
               prefix, \
               prefix, self.name, \
               prefix, \
               prefix, self.name, \
               prefix, convert_to_check_op(self.key_type), \
               prefix, \
               prefix, self.parent.get_name(), self.name, self.type, \
               prefix, \
               prefix, \
               prefix, self.key_type, \
               prefix, convert_to_fetch_op(self.key_type), \
               prefix, self.name, \
               prefix)
        elif 'dictionary' == self.type:
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
        ''' % (prefix, self.name, \
               prefix, \
               prefix, self.parent.get_name(), self.name, self.type, \
               prefix, \
               prefix, \
               prefix, self.name, \
               prefix, self.name, \
               prefix, \
               prefix, \
               prefix, convert_to_check_op(self.key_type), \
               prefix, \
               prefix, self.parent.get_name(), self.name, \
               prefix, \
               prefix, \
               prefix, \
               prefix, convert_to_check_op(self.val_type), \
               prefix, \
               prefix, self.parent.get_name(), self.name, self.val_type, \
               prefix, \
               prefix, \
               prefix, self.key_type, convert_to_fetch_op(self.key_type, 'key_val', 'name'), \
               prefix, self.val_type, convert_to_fetch_op(self.val_type), \
               prefix, self.name, \
               prefix)
        return ret

class struct_def_t:
    def __init__(self, name, parent = None):
        self.name       = name
        self.parent     = parent
        self.all_fields = {}
        self.sub_struct = []

    def get_name(self):
        return self.name
    def get_parent(self):
        return self.parent
    def set_parent(self, parent):
        self.parent = parent    
    def add_field(self, field_def_):
        self.all_fields[field_def_.get_name()] = field_def_
        field_def_.set_parent(self)
    def add_struct(self, struct_def_):
        self.sub_struct.append(struct_def_)
        struct_def_.set_parent(self)
    def get_all_struct(self):
        return self.sub_struct
    def get_parent(self):
        return self.parent
    def has_field(self, name):
        if None == self.all_fields.get(name):
            return False
        return True
    def dump(self, prefix = ''):
        print(prefix, self.name, 'include struct:')
        for struct in self.sub_struct:
            struct.dump(prefix + "    ")
        print(prefix, self.name, "include fields:")
        for field in self.all_fields:
            self.all_fields[field].dump(prefix + "    ")
    def format_declare_code(self, prefix = ''):
        ret = prefix
        ret = ret + 'struct %s {\n' %(self.name)
        for sub_struct in self.sub_struct:
            ret = ret + sub_struct.format_declare_code(prefix + '    ') + '\n'
        for field_name in self.all_fields:
            ret = ret + self.all_fields[field_name].format_declare_code(prefix + '    ') + '\n'

        ret = ret + prefix + '    int parse(const json_value_t& jval_) {\n'
        ret = ret + prefix + '        char buff[128];\n'
        for field_name in self.all_fields:    
            ret += self.all_fields[field_name].format_parse_code(prefix + '    ') + '\n'
        ret += prefix + '        return 0;\n' + prefix + '    }'

        ret = ret + '\n' + prefix + '};'
        return ret
        
class struct_def_mgr_t:
    def __init__(self):
        self.all_struct = {}
    def get_name(self):
        return ''
    def add_struct(self, struct_def_):
        self.all_struct[struct_def_.get_name()] = struct_def_
        struct_def_.set_parent(None)
    def get_all_struct(self):
        return self.all_struct
    def get_struct(self, name):
        return self.all_struct[name]
    def get_parent(self):
        return ''
    def dump(self):
        for name in self.all_struct:
            self.all_struct[name].dump()
