#include "../HippoIo.h"
#include "core/memory/LinearAllocator.h"
#import <Cocoa/Cocoa.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char** HippoIo_scanDirectory(struct LinearAllocator* allocator, const char* directory, const char* ext, bool recurse)
{
	// TODO: Support recursion

	NSArray* files = [[NSFileManager defaultManager] contentsOfDirectoryAtPath: [NSString stringWithUTF8String:directory] error: NULL];
	uint32_t count = [files count];

	char** outFiles = LinearAllocator_allocArray(allocator, char*, count + 1);
	NSString* nsSuffix = [NSString stringWithUTF8String:ext];
	uint32_t realCount = 0;

	for (NSString* file in files)
	{
		if ([file hasSuffix:nsSuffix])
			outFiles[realCount++] = LinearAllocator_allocString(allocator, [file UTF8String]); 
	}

	outFiles[realCount] = 0;
	return (const char**)outFiles;
}

