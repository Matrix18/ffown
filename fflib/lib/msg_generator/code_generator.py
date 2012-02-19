from inc import *

class code_generator_t:
    def __init__(self, struct_def_mgr):
        self.struct_def_mgr = struct_def_mgr
    def gen_declare_code(self):
        all_struct = self.struct_def_mgr.get_all_struct()
        for struct_name in all_struct:
            cur_struct = all_struct[struct_name]
            all_sub_struct = cur_struct.get_all_struct()
            str = cur_struct.format_declare_code()
            print(str)
    def exe(self):
        self.gen_declare_code()