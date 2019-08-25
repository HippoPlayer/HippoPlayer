#include <stdio.h>
#include "hippo_core_loader.h"
#include "../hippo_core/native/hippo_core.h"

HippoCoreNew hippo_core_new;
HippoCoreDrop hippo_core_drop;
HippoPlayFile hippo_play_file;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Wrap the API calls for Windows here as macOS and Linux uses dlopen/dlclose/etc API so we implement the same
// API for windows
//

#ifndef _WIN32

#include <dlfcn.h>

#else

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

static struct {
    long lasterror;
    const char* err_rutin;
} s_dll_err = {
    0,
    NULL
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void* dlopen(const char* filename, int flags) {
	(void)flags;
    HINSTANCE inst = LoadLibrary(filename);
    if (inst == NULL) {
        s_dll_err.lasterror = GetLastError();
        s_dll_err.err_rutin = "dlopen";
    }

    return (void*)inst;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int dlclose(void* handle) {
    int rc = 0;
    int ok = FreeLibrary((HINSTANCE)handle);

    if (!ok) {
        s_dll_err.lasterror = GetLastError();
        s_dll_err.err_rutin = "dlclose";
        rc = -1;
    }

    return rc;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void* dlsym(void* handle, const char* name) {
    FARPROC fp = GetProcAddress ((HINSTANCE)handle, name);
    if (!fp) {
        s_dll_err.lasterror = GetLastError ();
        s_dll_err.err_rutin = "dlsym";
    }
    return (void *)(intptr_t)fp;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char* dlerror() {
	static char errstr[256];

    if (s_dll_err.lasterror) {
        sprintf (errstr, "%s error #%ld", var.err_rutin, var.lasterror);
        return errstr;
    } else {
        return NULL;
    }
}

#endif

/// Loads the hippo_player core shared object and resolve the symbols
extern int HippoCore_load() {
#if defined(HIPPO_MAC)
	const char* core_name = "libhippo_core.dylib";
#elif defined(HIPPO_WIN)
	const char* core_name = "hippo_core.dll";
#else
	// temp temp
	const char* core_name = "/home/emoon/code/projects/hippo_player/t2-output/linux-gcc-debug-default/libhippo_core.so";
#endif

	void* core_handle = dlopen(core_name, RTLD_NOW);

	if (!core_handle) {
		printf("Unable to open %s: %s", core_name, dlerror());
		return 0;
	}

	hippo_core_new = (HippoCoreNew)dlsym(core_handle, "hippo_core_new");
	hippo_core_drop = (HippoCoreDrop)dlsym(core_handle, "hippo_core_drop");
	hippo_play_file = (HippoPlayFile)dlsym(core_handle, "hippo_play_file");

	return 1;
}


