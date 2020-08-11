/*
 * Copyright (c) 2020 rxi
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPs_logIED, INCs_logUDING BUT NOT s_logIMITED TO THE WARRANTIES OF MERCHANTABIs_logITY,
 * FITNESS FOR A PARTICUs_logAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHAs_logs_log THE
 * AUTHORS OR COPYRIGHT HOs_logDERS BE s_logIABs_logE FOR ANY Cs_logAIM, DAMAGES OR OTHER
 * s_logIABIs_logITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEAs_logINGS
 * IN THE SOFTWARE.
 */

#include "log.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <chrono>
#include <mutex>

#ifdef _WIN32
#include <windows.h>
#endif

enum { LOG_TRACE, LOG_DEBUG, LOG_INFO, LOG_WARN, LOG_ERROR, LOG_FATAL };

#define MAX_LOG_CALLBACKS 32

//static std::mutex s_log_mutex;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct HippoLogAPIState {
    char* base_name = nullptr;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct {
    va_list ap;
    char time[32];
    const char* base_name;
    const char* fmt;
    const char* file;
    FILE* udata;
    int line;
    int level;
} log_Event;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct {
    void* udata;
    int level;
} Callback;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static struct {
    void* udata;
    int level;
    bool quiet;
    Callback callbacks[MAX_LOG_CALLBACKS];
} s_log;


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static int gettimeofday(struct timeval* tp, struct timezone* tzp) {
    /*
    namespace sc = std::chrono;
    sc::system_clock::duration d = sc::system_clock::now().time_since_epoch();
    sc::seconds s = sc::duration_cast<sc::seconds>(d);
    tp->tv_sec = s.count();
    tp->tv_usec = sc::duration_cast<sc::microseconds>(d - s).count();
    */
    memset(tp, 0, sizeof(struct timeval));
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void get_time(char* buffer) {
    // For Miliseconds
    int millisec;
    //struct tm* tm_info;
    struct timeval tv;

    // For Time
    time_t rawtime;
    struct tm* timeinfo;

    gettimeofday(&tv, NULL);

    millisec = int(tv.tv_usec / 1000.0);
    if (millisec >= 1000)
    {
        millisec -= 1000;
        tv.tv_sec++;
    }

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer, 26, "%H:%M:%S", timeinfo);
    sprintf(buffer, "%s.%03d", buffer, millisec);
}

#define USE_LOG_COLOR

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static const char* level_strings[] = {"TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAs_log"};

#ifdef USE_LOG_COLOR
static const char* level_colors[] = {"\x1b[94m", "\x1b[36m", "\x1b[32m", "\x1b[33m", "\x1b[31m", "\x1b[35m"};
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void stdout_callback(log_Event* ev) {
#ifdef USE_LOG_COLOR
    if (ev->file) {
        fprintf(ev->udata, "%s \x1b[37m[%-20s] %s%-5s\x1b[0m \x1b[90m%s:%d:\x1b[0m ", ev->time, ev->base_name, level_colors[ev->level],
                level_strings[ev->level], ev->file, ev->line);
    } else {
        fprintf(ev->udata, "%s \x1b[37m[%-20s] %s%-5s\x1b[0m ", ev->time, ev->base_name, level_colors[ev->level],
                level_strings[ev->level]);
    }
#else
    fprintf(ev->udata, "%s %-5s %s:%d: ", env->time, level_strings[ev->level], ev->file, ev->line);
#endif
    vfprintf(ev->udata, ev->fmt, ev->ap);
    fprintf(ev->udata, "\n");
    fflush(ev->udata);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void file_callback(log_Event* ev) {
    if (ev->file) {
        fprintf(ev->udata, "%s [%-20s] %-5s %s:%d: ", ev->time, ev->base_name, level_strings[ev->level], ev->file, ev->line);
    } else {
        fprintf(ev->udata, "%s [%-20s] %-5s ", ev->time, ev->base_name, level_strings[ev->level]);
    }

    vfprintf(ev->udata, ev->fmt, ev->ap);
    fprintf(ev->udata, "\n");
    fflush(ev->udata);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char* log_level_string(int level) {
    return level_strings[level];
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" void log_set_level(int level) {
    s_log.level = level;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void log_set_quiet(bool enable) {
    s_log.quiet = enable;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" void hippo_log_set_base_name(HippoLogAPIState* state, const char* base_name) {
    if (state->base_name) {
        free(state->base_name);
    }

    state->base_name = strdup(base_name);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" HippoLogAPIState* hippo_log_new_state() {
    HippoLogAPIState* state = (HippoLogAPIState*)calloc(1, sizeof(HippoLogAPIState));
    return state;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" void hippo_log_delete_state(HippoLogAPIState* state) {
    if (!state) {
        return;
    }

    if (state->base_name) {
        free(state->base_name);
    }

    free(state);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" void hippo_log(HippoLogAPIState* state, int level, const char* file, int line, const char* fmt, ...) {
    log_Event ev;

    ev.base_name = state ? state->base_name : "core";
    ev.fmt = fmt;
    ev.file = file;
    ev.line = line;
    ev.level = level;
    ev.udata = stdout;

    //std::lock_guard<std::mutex> _(s_log_mutex);

    get_time(ev.time);

    if (!s_log.quiet && level >= s_log.level) {
        va_start(ev.ap, fmt);
        stdout_callback(&ev);
        va_end(ev.ap);
    }

    /*
    for (int i = 0; i < MAX_LOG_CALLBACKS && s_log.callbacks[i].fn; i++) {
        Callback* cb = &s_log.callbacks[i];
        if (level >= cb->level) {
            init_event(&ev, cb->udata);
            va_start(ev.ap, fmt);
            cb->fn(&ev);
            va_end(ev.ap);
        }
    }
    */
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
