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
#include <vector>

#ifdef _WIN32
#include <Windows.h>
#endif

enum { LOG_TRACE, LOG_DEBUG, LOG_INFO, LOG_WARN, LOG_ERROR, LOG_FATAL };

#define MAX_LOG_CALLBACKS 32

static std::mutex s_log_mutex;
static std::vector<const char*> s_log_messages;
static FILE* s_log_file = nullptr;

// index to where we are reading messages
static bool s_enable_send_msgs = false;
static int s_log_read_index = 0;
static int s_total_log_message_count = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct HippoLogAPIState {
    char* base_name = nullptr;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct {
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

static int gettimeofday(struct timeval* tp) {
    namespace sc = std::chrono;
    sc::system_clock::duration d = sc::system_clock::now().time_since_epoch();
    sc::seconds s = sc::duration_cast<sc::seconds>(d);
    tp->tv_sec = s.count();
    tp->tv_usec = sc::duration_cast<sc::microseconds>(d - s).count();

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

    gettimeofday(&tv);

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

static const char* level_colors[] = {"\x1b[94m", "\x1b[36m", "\x1b[32m", "\x1b[33m", "\x1b[31m", "\x1b[35m"};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void stdout_callback(log_Event* ev, va_list ap) {
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
    vfprintf(ev->udata, ev->fmt, ap);
    fprintf(ev->udata, "\n");
    fflush(ev->udata);
}

/*
#define COL_T "<span style=\"color:gainsboro\">"
#define COL_G "<span style=\"color:gray\">"
#define ES "</span>"

static const char* html_colors[] = {
    "<span style=\"color:green\">"
    "<span style=\"color:green\">"
    "<span style=\"color:green\">"
    "<span style=\"color:green\">"
    "<span style=\"color:green\">"
    "<span style=\"color:green\">"
    "<span style=\"color:green\">"
    "<span style=\"color:green\">"
    "<span style=\"color:green\">"
};
        written_chars = sprintf(buffer, "<tt> " COL_T " %s " ES COL_G "[%-40s]" ES " %s%-5s " ES COL_T,
                        ev->time, ev->base_name, html_colors[0], level_strings[ev->level]);
  //<tt> <span style="color:gainsboro"> 15:19:16.000 </span> <span style="color:gray"> [VGM 0.0.1           ] </span> <span style="color:green"> INFO </span> <span>  <span style="color:gainsboro"> Unsupported /home/emoon/Music/dune/dune ii - 00.adl </span> </tt>
*/

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void file_console_out(log_Event* ev, va_list ap) {
    char buffer[8192];
    int written_chars = 0;

    if (ev->file) {
        written_chars = sprintf(buffer, "%s [%-20s] %-5s %s:%d: ", ev->time, ev->base_name, level_strings[ev->level], ev->file, ev->line);
    } else {
        written_chars = sprintf(buffer, "%s [%-20s] %-5s ", ev->time, ev->base_name, level_strings[ev->level]);
    }

    written_chars += vsprintf(&buffer[written_chars], ev->fmt, ap);

    s_log_messages.push_back(strdup(buffer));

    buffer[written_chars + 0] = '\n';
    buffer[written_chars + 1] = 0;

    if (s_log_file) {
        fwrite(buffer, 1, written_chars + 1, s_log_file);
        fflush(s_log_file);
    }

    s_total_log_message_count += 1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
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
*/

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

    std::lock_guard<std::mutex> _(s_log_mutex);

    if (s_log_messages.empty()) {
        // reserve space for 10 000 messages, should hopefully be enough for a while :)
        s_log_messages.reserve(10000);
    }

    get_time(ev.time);

    //stdout_callback(&ev);

    va_list ap;
    va_start(ap, fmt);
    va_list itemcopy;
    va_copy(itemcopy, ap);

    stdout_callback(&ev, ap);
    va_end(ap);

    file_console_out(&ev, itemcopy);
    va_end(itemcopy);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" void hippo_log_clear() {
    std::lock_guard<std::mutex> _(s_log_mutex);

    for (auto& msg : s_log_messages) {
        free((void*)msg);
    }

    s_log_messages.clear();

    s_log_read_index = 0;
    s_total_log_message_count = 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" void hippo_log_send_messages(bool enable) {
    std::lock_guard<std::mutex> _(s_log_mutex);
    s_enable_send_msgs = enable;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" void hippo_log_to_file(const char* filename, bool enable) {
    std::lock_guard<std::mutex> _(s_log_mutex);

    if (s_log_file) {
        fclose(s_log_file);
        s_log_file = nullptr;
    }

    if (filename && enable) {
        s_log_file = fopen(filename, "wb");

        // write all cached messages to the file
        for (const auto& message : s_log_messages) {
            fputs(message, s_log_file);
            fputs("\n", s_log_file);
        }

        fflush(s_log_file);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" const char* hippo_log_get_messages() {
    std::lock_guard<std::mutex> _(s_log_mutex);

    if ((s_log_read_index >= s_total_log_message_count) || !s_enable_send_msgs) {
        return nullptr;
    }

    int index = s_log_read_index++;

    return s_log_messages[index];
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
