#include "plugin_factory.h"
#include "plugin_impl/plugin_dll.h"
#include "plugin_impl/plugin_python.h"
#include "plugin_impl/plugin_lua.h"

plugin_ptr_t plugin_factory_t::create_plugin(const string& path_)
{
    return new plugin_dll_t(path_);
}
