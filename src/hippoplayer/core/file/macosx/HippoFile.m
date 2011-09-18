#include "../HippoFile.h"
#include <string.h>
#import <Cocoa/Cocoa.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int HippoFile_openDialog(char* filename, int maxLength)
{
	NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];

	NSOpenPanel* open = [NSOpenPanel openPanel];

	int result = [open runModal];

	if (result == NSOKButton)
	{
		/*
		// TODO: Proper handling of multiple files
		NSArray* selectedFiles = [open URLs];
		uint32_t count = [selectedFiles count];
		//NSURL* url;
		
		for(int i = 0; i < count; ++i) 
		{
			//url = [selectedFiles objectAtIndex:i];
			//(void)url;
			//const char* temp = [[url path] UTF8String];
			//printf("%s\n", temp);
			strcpy(filename, "eunthoeustoehusnotehuseothusoenhuso");
		}
		*/

		//[open relese];
		[pool release];
		return 0;
	}

	[pool release];
	return 0;
}

