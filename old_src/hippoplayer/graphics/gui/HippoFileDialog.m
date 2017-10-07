#include "graphics/gui/HippoGui.h"
#include "core/memory/LinearAllocator.h"
#import <Cocoa/Cocoa.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char** HippoGui_fileOpenDialog(struct LinearAllocator* allocator, int* outCount)
{
	const char** files = 0;

	NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
	NSOpenPanel* open = [NSOpenPanel openPanel];
	[open setAllowsMultipleSelection:YES];

	int result = [open runModal];

	if (result == NSOKButton)
	{
		NSArray* selectedFiles = [open URLs];
		uint32_t count = [selectedFiles count];
		*outCount = 0;

		char** returnFiles = LinearAllocator_allocArray(allocator, char*, count);
		*outCount = (int)count;

		for (int i = 0; i < count; ++i) 
		{
			NSURL* url = [selectedFiles objectAtIndex:i];
			const char* temp = [[url path] UTF8String];
			returnFiles[i] = LinearAllocator_allocString(allocator, temp);
		}

		files = (const char**)returnFiles;
	}

	[pool drain];
	return files;
}



