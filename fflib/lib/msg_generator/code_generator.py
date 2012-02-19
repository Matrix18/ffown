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
    def gen_declare_code(self):
        f = open("msg_def.h", "w")
        f.write(inc_str)
        all_struct = self.struct_def_mgr.get_all_struct()
        for struct_name in all_struct:
            cur_struct = all_struct[struct_name]
            all_sub_struct = cur_struct.get_all_struct()
            str = cur_struct.format_declare_code()
            #print(str)
            f.write(str)
    def exe(self):
        self.gen_declare_code()