#ifndef _UADE_UNIXSUPPORT_H_
#define _UADE_UNIXSUPPORT_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

#include <uade/uadeipc.h>

#ifdef _WIN32
#define pid_t int
#endif

#define uade_debug(state, fmt, ...) do { if ((state) == NULL || uade_is_verbose(state)) { fprintf(stderr, fmt, ## __VA_ARGS__); } } while (0)
#define uade_die(fmt, ...) do { fprintf(stderr, "uade: " fmt, ## __VA_ARGS__); exit(1); } while(0)
#define uade_die_error(fmt, ...) do { fprintf(stderr, "uade: " fmt ": %s\n", ## __VA_ARGS__, strerror(errno)); exit(1); } while(0)
#define uade_error(fmt, ...) do { \
    fprintf(stderr, "%s:%d: %s: " fmt, __FILE__, __LINE__, __func__, ## __VA_ARGS__); \
    abort(); \
  } while (0)
#define uade_info(fmt, ...) do { fprintf(stderr, "uade info: " fmt, ## __VA_ARGS__); } while(0)
#define uade_warning(fmt, ...) do { fprintf(stderr, "uade warning: " fmt, ## __VA_ARGS__); } while(0)

char *uade_dirname(char *dst, char *src, size_t maxlen);
int uade_find_amiga_file(char *realname, size_t maxlen, const char *aname, const char *playerdir);

void uade_arch_kill_and_wait_uadecore(struct uade_ipc *ipc, pid_t *uadepid);
int uade_arch_spawn(struct uade_ipc *ipc, pid_t *uadepid, const char *uadename);

int uade_filesize(size_t *size, const char *pathname);

#endif
