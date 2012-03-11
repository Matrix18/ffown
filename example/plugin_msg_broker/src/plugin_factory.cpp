#include "plugin_factory.h"
#include "plugin_impl/plugin_dll.h"
#include "plugin_impl/plugin_python.h"
#include "plugin_impl/plugin_lua.h"

plugin_ptr_t plugin_factory_t::create_plugin(const string& path_)
{
    if (-1 != path_.find(".so"))
    {
    return new plugin_dll_t(path_);
    }
    else if (-1 != path_.find(".py"))
    {
        return new plugin_python_t(path_);
    }    
}
