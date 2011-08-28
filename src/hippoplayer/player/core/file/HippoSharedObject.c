#include "HippoSharedObject.h"

#if defined(HIPPO_MACOSX)
#include <dlfcn.h>
#elif defined(HIPPO_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#error "Unsupported platform"
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

HippoHandle HippoSharedObject_open(const char* filename)
{
#if defined(HIPPO_MACOSX)
	return dlopen(filename, RTLD_LOCAL | RTLD_LAZY);
#elif defined(HIPPO_WINN32)
	return (HippoHandle)LoadLibrary(filename);
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HippoSharedObject_close(HippoHandle handle)
{
#if defined(HIPPO_MACOSX)
	dlclose(handle);
#elif defined(HIPPO_WINN32)
	return (HippoHandle)LoadLibrary((HMODULE)handle);
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void* HippoSharedObject_getSym(HippoHandle handle, const char* name)
{
#if defined(HIPPO_MACOSX)
	return dlsym(handle, name);
#elif defined(HIPPO_WINN32)
	return (void*)GetProcAddress(handle, name);
#endif
}

