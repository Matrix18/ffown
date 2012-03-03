#include <stdarg.h>
#include <stdio.h>

static int log_impl(const char* mod, const char* fmt, ...)
{
    char buff[256];
    int len = snprintf(buff, sizeof(buff), "%s ", mod);

    va_list vl;
    va_start(vl, fmt);
    vsnprintf(buff + len, sizeof(buff) - len - 1, fmt, vl);
    va_end(vl);
    printf(buff);
    return printf("\n");
}

#define logdebug(content) log_impl content
#define logtrace(content) log_impl content
#define loginfo(content) log_impl content
#define logwarn(content) log_impl content
#define logerror(content) log_impl content
#define logfatal(content) log_impl content


