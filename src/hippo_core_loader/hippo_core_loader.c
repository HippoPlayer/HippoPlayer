#include <stdio.h>
#include "hippo_core_loader.h"
#include "../hippo_core/native/hippo_core.h"

HippoCoreNew hippo_core_new;
HippoCoreDrop hippo_core_drop;
HippoPlayFile hippo_play_file;
HippoServiceApiNew hippo_service_api_new;
HippoMessageApiNew hippo_message_api_new;
HippoUpdateMessages hippo_update_messages;
HippoPlaylistRemoveEntry hippo_playlist_remove_entry;

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

// not used on Win32 so can be anything
#define RTLD_NOW 0

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

WCHAR* convert_utf8_to_wide(const char* input) {
	int len = MultiByteToWideChar(CP_UTF8, 0, input, -1, NULL, 0);
	len *= 4;
	WCHAR* output = (WCHAR*)malloc(len);
	MultiByteToWideChar(CP_UTF8, 0, input, -1, output, len);

	return output;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void* dlopen(const char* filename, int flags) {
	(void)flags;

	WCHAR* wide_filename = convert_utf8_to_wide(filename);

    HINSTANCE inst = LoadLibrary(wide_filename);
    if (inst == NULL) {
        s_dll_err.lasterror = GetLastError();
        s_dll_err.err_rutin = "dlopen";
    }

    free(wide_filename);

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
	WCHAR* wide_name = convert_utf8_to_wide(name);

    FARPROC fp = GetProcAddress ((HINSTANCE)handle, name);
    if (!fp) {
        s_dll_err.lasterror = GetLastError ();
        s_dll_err.err_rutin = "dlsym";
    }
	free(wide_name);

    return (void *)(intptr_t)fp;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char* dlerror() {
	static char errstr[256];

    if (s_dll_err.lasterror) {
        sprintf (errstr, "%s error #%ld", s_dll_err.err_rutin, s_dll_err.lasterror);
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
#elif defined(_WIN32)
	const char* core_name = "hippo_core.dll";
#else
	// temp temp
	const char* core_name = "t2-output/linux-gcc-debug-default/libhippo_core.so";
#endif

	void* core_handle = dlopen(core_name, RTLD_NOW);

	if (!core_handle) {
		printf("Unable to open %s: %s", core_name, dlerror());
		return 0;
	}

	hippo_core_new = (HippoCoreNew)dlsym(core_handle, "hippo_core_new");
	hippo_core_drop = (HippoCoreDrop)dlsym(core_handle, "hippo_core_drop");
	hippo_play_file = (HippoPlayFile)dlsym(core_handle, "hippo_play_file");
	hippo_service_api_new = (HippoServiceApiNew)dlsym(core_handle, "hippo_service_api_new");
	hippo_message_api_new = (HippoMessageApiNew)dlsym(core_handle, "hippo_message_api_new");
	hippo_update_messages = (HippoUpdateMessages)dlsym(core_handle, "hippo_update_messages");
	hippo_playlist_remove_entry = (HippoPlaylistRemoveEntry)dlsym(core_handle, "hippo_playlist_remove_entry");

	return 1;
}


