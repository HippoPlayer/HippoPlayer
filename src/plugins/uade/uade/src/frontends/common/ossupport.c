#include <uade/ossupport.h>

/* UNIX support tools

   Copyright 2000-2011 (C) Heikki Orsila <heikki.orsila@iki.fi>
   
   This module is licensed under the GNU LGPL.
*/

#include <uade/uadeipc.h>
#include <uade/unixatomic.h>

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#ifdef _WIN32
#include "uade/dirent_win32.h"
#else
#include <unistd.h>
#include <dirent.h>
#include <libgen.h>
#endif


int dumb_socketpair(int socks[2], int make_overlapped);

int uade_filesize(size_t *size, const char *pathname)
{
	struct stat st;
	if (stat(pathname, &st))
		return -1;
	if (size)
		*size = st.st_size;
	return 0;
}

static int uade_amiga_scandir(char *real, char *dirname, char *fake, int ml)
{
	DIR *dir;
	struct dirent *direntry;
	if (!(dir = opendir(dirname))) {
		uade_warning("Can't open dir (%s) (amiga scandir)\n", dirname);
		return 0;
	}
	while ((direntry = readdir(dir))) {
		if (!strcmp(fake, direntry->d_name)) {
			if (((int) strlcpy(real, direntry->d_name, ml)) >= ml) {
				uade_warning("uade: %s does not fit real",
					     direntry->d_name);
				closedir(dir);
				return 0;
			}
			break;
		}
	}
	if (direntry) {
		closedir(dir);
		return 1;
	}
	rewinddir(dir);
	while ((direntry = readdir(dir))) {
		if (!strcasecmp(fake, direntry->d_name)) {
			if (((int) strlcpy(real, direntry->d_name, ml)) >= ml) {
				fprintf(stderr, "uade: %s does not fit real", direntry->d_name);
				closedir(dir);
				return 0;
			}
			break;
		}
	}
	closedir(dir);
	return direntry != NULL;
}

char *uade_dirname(char *dst, char *src, size_t maxlen)
{
#ifdef _WIN32
	char node[4096];
	char dir[4096];
	char fname[4096];
	char ext[4096];
#endif

	char *srctemp = strdup(src);
	if (srctemp == NULL)
		return NULL;

#ifdef _WIN32
	_splitpath(src, node, dir, fname, ext);
	strlcpy(dst, dir, maxlen);
#else
	strlcpy(dst, dirname(srctemp), maxlen);
#endif
	free(srctemp);
	return dst;
}


/* Find file in amiga namespace */
int uade_find_amiga_file(char *realname, size_t maxlen, const char *aname,
			 const char *playerdir)
{
	char *separator;
	char *ptr;
	char copy[PATH_MAX];
	char dirname[PATH_MAX];
	char fake[PATH_MAX];
	char real[PATH_MAX];
	int len;
	DIR *dir;
	FILE *file;
	size_t strip_offset;

	if (strlcpy(copy, aname, sizeof(copy)) >= sizeof(copy)) {
		uade_warning("error: amiga tried to open a very long "
			     "filename.\nPlease REPORT THIS!\n");
		return -1;
	}
	ptr = copy;
	// Deal with windows paths
	if(ptr[1] == ':' && ptr[2] == '/') {
		memcpy(dirname, ptr, 3);
		dirname[3] = 0;
		ptr += 3;
	} else
	if ((separator = strchr(ptr, (int) ':'))) {
		len = (int) (separator - ptr);
		memcpy(dirname, ptr, len);
		dirname[len] = 0;
		if (!strcasecmp(dirname, "ENV")) {
			snprintf(dirname, sizeof(dirname), "%s/ENV/", playerdir);
		} else if (!strcasecmp(dirname, "S")) {
			snprintf(dirname, sizeof(dirname), "%s/S/", playerdir);
		} else {
			uade_warning("open_amiga_file: unknown amiga volume "
				     "(%s)\n", aname);
			return -1;
		}
		if (!(dir = opendir(dirname))) {
			uade_warning("Can't open dir (%s) (volume parsing)\n",
				     dirname);
			return -1;
		}
		closedir(dir);
		ptr = separator + 1;
	} else {
		if (*ptr == '/') {
			/* absolute path */
			strlcpy(dirname, "/", sizeof(dirname));
			ptr++;
		} else {
			/* relative path */
			strlcpy(dirname, "./", sizeof(dirname));
		}
	}

	while ((separator = strchr(ptr, (int) '/'))) {
		len = (int) (separator - ptr);
		if (!len) {
			ptr++;
			continue;
		}
		memcpy(fake, ptr, len);
		fake[len] = 0;
		if (uade_amiga_scandir(real, dirname, fake, sizeof(real))) {
			/* found matching entry */
			if (strlcat(dirname, real, sizeof(dirname)) >= sizeof(dirname)) {
				uade_warning("Too long dir path (%s + %s)\n",
					     dirname, real);
				return -1;
			}
			if (strlcat(dirname, "/", sizeof(dirname)) >= sizeof(dirname)) {
				uade_warning("Too long dir path (%s + %s)\n",
					     dirname, "/");
				return -1;
			}
		} else {
			/* didn't find entry */
			return -1;
		}
		ptr = separator + 1;
	}

	if (!(dir = opendir(dirname))) {
		uade_warning("Can't open dir (%s) after scanning\n", dirname);
		return -1;
	}
	closedir(dir);

	if (uade_amiga_scandir(real, dirname, ptr, sizeof(real))) {
		/* found matching entry */
		if (strlcat(dirname, real, sizeof(dirname)) >= sizeof(dirname)) {
			uade_warning("Too long dir path (%s + %s)\n",
				     dirname, real);
			return -1;
		}
	} else {
		/* didn't find entry */
		return -1;
	}

	file = fopen(dirname, "rb");
	if (file == NULL) {
		uade_warning("Couldn't open file (%s) induced by (%s)\n",
			     dirname, aname);
		return -1;
	}
	fclose(file);

	/* Strip leading "./" from the real path name when copying */
	strip_offset = (strncmp(dirname, "./", 2) == 0) ? 2 : 0;

	strlcpy(realname, dirname + strip_offset, maxlen);

	return 0;
}

int uadecore_main (int argc, char **argv);
void uade_run_thread(void (*f)(void*), void *data);
void uade_wait_thread();

void uade_arch_kill_and_wait_uadecore(struct uade_ipc *ipc, pid_t *uadepid)
{
	uade_atomic_close(ipc->in_fd);
	uade_atomic_close(ipc->out_fd);

	uade_wait_thread();
}

static void thread_func(void *data)
{
	int *fds = (int*)data;
	char input[32], output[32];


	/* give in/out fds as command line parameters to uadecore */
	snprintf(input, sizeof input, "%d", fds[1]);
	snprintf(output, sizeof output, "%d", fds[1]);

	char *args[] = { "uadecore", "-i", input, "-o", output };
	uadecore_main(5, args);
}

static int spawn_fds[2];

int uade_arch_spawn(struct uade_ipc *ipc, pid_t *uadepid, const char *uadename)
{
	if (dumb_socketpair(spawn_fds, 0)) {
		uade_warning("Can not create socketpair: %s\n",
			     strerror(errno));
		return -1;
	}
	
	uade_run_thread(&thread_func, (void*)spawn_fds);

	uade_set_peer(ipc, 1, spawn_fds[0], spawn_fds[0]);
	return 0;
}
#include <limits.h>
#include <stdlib.h>

char *canonicalize_file_name(const char *path)
{
	char *s = malloc(PATH_MAX);
	if (s == NULL)
		return NULL;

#ifdef _WIN32
	if(!_fullpath(s, path, PATH_MAX)) {
#else
	if (realpath(path, s) == NULL) {
#endif
		free(s);
		return NULL;
	}

	return s;
}

