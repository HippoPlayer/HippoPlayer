#include "graphics/gui/HippoGui.h"
#include "core/memory/LinearAllocator.h"
#import <Cocoa/Cocoa.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char** HippoGui_openFileDialog(LinearAllocator* allocator)
{
	const char** files = 0;

	NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
	NSOpenPanel* open = [NSOpenPanel openPanel];

	int result = [open runModal];

	if (result == NSOKButton)
	{
		NSArray* selectedFiles = [open URLs];
		uint32_t count = [selectedFiles count];

		char** returnFiles = LinearAllocator_allocArray(allocator, char*, count);
		
		for(int i = 0; i < count; ++i) 
		{
			NSURL* url = [selectedFiles objectAtIndex:i];
			const char* temp = [[url path] UTF8String];
			returnFiles[i] = LinearAllocator_allocString(allocator, temp);
		}

		files = (const char**)returnFiles;
	}

	[pool release];
	return files;
}



