#ifndef _LOG_H_
#define _LOG_H_

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
    return 0;
}

#define BROKER "BROKER"
#define RPC    "RPC"
#define FF     "FF"

#define logdebug(content) printf("\033[1;33mDEBUG "); log_impl content ; printf("\033[0m\n")
#define logtrace(content) printf("TRACE "); log_impl content ; printf("\n");
#define loginfo(content) printf("\033[1;32mINFO  "); log_impl content ; printf("\033[0m\n")
#define logwarn(content) printf("\033[1;34mWARN  "); log_impl content ; printf("\033[0m\n")
#define logerror(content) printf("\033[0;31mERROR "); log_impl content ; printf("\033[0m\n")
#define logfatal(content) printf("\033[0;35mFATAL "); log_impl content ; printf("\033[0m")

#endif

