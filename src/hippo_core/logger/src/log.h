#pragma once

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>

#define LOG_VERSION "0.1.0"

//typedef void (*log_LogFn)(log_Event* ev);
//typedef void (*log_LockFn)(bool lock, void* udata);

/*
#define log_trace(...) log_log(LOG_TRACE, __FILE__, __LINE__, __VA_ARGS__)
#define log_debug(...) log_log(LOG_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define log_info(...) log_log(LOG_INFO, __FILE__, __LINE__, __VA_ARGS__)
#define log_warn(...) log_log(LOG_WARN, __FILE__, __LINE__, __VA_ARGS__)
#define log_error(...) log_log(LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define log_fatal(...) log_log(LOG_FATAL, __FILE__, __LINE__, __VA_ARGS__)
*/

struct HippoLogAPIState;

extern "C" {
    struct HippoLogAPIState* hippo_log_alloc_state();
    void hippo_log_set_level(int level);
    void hippo_log_set_base_name(HippoLogAPIState* state, const char* base_name);
    void hippo_log_set_level(int level);
    void hippo_log(HippoLogAPIState* state, int level, const char* file, int line, const char* fmt, ...);
}
