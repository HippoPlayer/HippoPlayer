#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
//#include <dirent.h>
#include <sys/stat.h>
//#include <unistd.h>
#include <string.h>

#include <unixwalkdir.h>

/*

    Implementation of POSIX directory browsing functions and types for Win32.

    Author:  Kevlin Henney (kevlin@acm.org, kevlin@curbralan.com)
    History: Created March 1997. Updated June 2003 and July 2012.
    Rights:  See end of file.

*/

#ifdef _WIN32

//#include <dirent.h>
#include <errno.h>
#include <io.h> /* _findfirst and _findnext set errno iff they return -1 */
#include <stdlib.h>
#include <string.h>

struct dirent
{
    char *d_name;
};

typedef ptrdiff_t handle_type; /* C99's intptr_t not sufficiently portable */

typedef struct DIR
{
    handle_type         handle; /* -1 for failed rewind */
    struct _finddata_t  info;
    struct dirent       result; /* d_name null iff first time */
    char                *name;  /* null-terminated char string */
} DIR;

DIR *opendir(const char *name)
{
    DIR *dir = 0;

    if(name && name[0])
    {
        size_t base_length = strlen(name);
        const char *all = /* search pattern must end with suitable wildcard */
            strchr("/\\", name[base_length - 1]) ? "*" : "/*";

        if((dir = (DIR *) malloc(sizeof *dir)) != 0 &&
           (dir->name = (char *) malloc(base_length + strlen(all) + 1)) != 0)
        {
            strcat(strcpy(dir->name, name), all);

            if((dir->handle =
                (handle_type) _findfirst(dir->name, &dir->info)) != -1)
            {
                dir->result.d_name = 0;
            }
            else /* rollback */
            {
                free(dir->name);
                free(dir);
                dir = 0;
            }
        }
        else /* rollback */
        {
            free(dir);
            dir   = 0;
            errno = ENOMEM;
        }
    }
    else
    {
        errno = EINVAL;
    }

    return dir;
}

int closedir(DIR *dir)
{
    int result = -1;

    if(dir)
    {
        if(dir->handle != -1)
        {
            result = _findclose(dir->handle);
        }

        free(dir->name);
        free(dir);
    }

    if(result == -1) /* map all errors to EBADF */
    {
        errno = EBADF;
    }

    return result;
}

struct dirent *readdir(DIR *dir)
{
    struct dirent *result = 0;

    if(dir && dir->handle != -1)
    {
        if(!dir->result.d_name || _findnext(dir->handle, &dir->info) != -1)
        {
            result         = &dir->result;
            result->d_name = dir->info.name;
        }
    }
    else
    {
        errno = EBADF;
    }

    return result;
}

void rewinddir(DIR *dir)
{
    if(dir && dir->handle != -1)
    {
        _findclose(dir->handle);
        dir->handle = (handle_type) _findfirst(dir->name, &dir->info);
        dir->result.d_name = 0;
    }
    else
    {
        errno = EBADF;
    }
}

#endif // _WIN32


void *uade_walk_directories(const char *dirname,
			    void *(*fn) (const char *file,
					 enum uade_wtype wtype, void *opaque),
			    void *opaque)
{
	char *dename;
	DIR *dir;
	size_t namelen;
	struct dirent *de;
	void *ret = NULL;
	struct stat st;
	enum uade_wtype wtype;

	namelen = strlen(dirname) + 256 + 2;
	if ((dename = malloc(namelen)) == NULL)
		return NULL;

	if ((dir = opendir(dirname)) == NULL)
		return NULL;

	while ((de = readdir(dir)) != NULL) {

		if (strcmp(de->d_name, ".") == 0
		    || strcmp(de->d_name, "..") == 0)
			continue;

		if (snprintf(dename, namelen, "%s/%s", dirname, de->d_name) >=
		    namelen) {
			fprintf(stderr, "interesting: too long a filename\n");
			continue;
		}

		if (lstat(dename, &st))
			continue;

		if (S_ISREG(st.st_mode))
			wtype = UADE_WALK_REGULAR_FILE;
		else if (S_ISDIR(st.st_mode))
			wtype = UADE_WALK_DIRECTORY;
		else if (S_ISLNK(st.st_mode))
			wtype = UADE_WALK_SYMLINK;
		else
			wtype = UADE_WALK_SPECIAL;

		if ((ret = fn(dename, wtype, opaque)) != NULL)
			break;

		if (wtype == UADE_WALK_DIRECTORY) {
			if ((ret =
			     uade_walk_directories(dename, fn, opaque)) != NULL)
				break;
		}
	}

	closedir(dir);
	free(dename);

	return ret;
}
