
#ifndef HIPPOIO_H
#define HIPPOIO_H

#include "core/Types.h"

struct LinearAllocator;

const char** HippoIo_scanDirectory(struct LinearAllocator* allocator, const char* directory, const char* ext, bool recurse);

#endif
