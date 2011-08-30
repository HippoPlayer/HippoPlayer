
#ifndef HIPPOSHAREDOBJECT_H
#define HIPPOSHAREDOBJECT_H

typedef void* HippoHandle;

HippoHandle HippoSharedObject_open(const char* filename);
void HippoSharedObject_close(HippoHandle handle);
void* HippoSharedObject_getSym(HippoHandle handle, const char* name);

#endif 

