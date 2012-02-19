
from inc import *
from src_parser import src_parser_t
from code_generator import code_generator_t


if __name__ =='__main__':
    p = src_parser_t('foo.cpp')
    p.exe()
    mgr = p.get_struct_def_mgr()
    mgr.dump()
    cg = code_generator_t(mgr)
    cg.exe()


t = struct_def_t('TT')

f = field_def_t('ok', 'int8', '', '')
t.add_field(f)
s = f.format_parse_code()
print(s)