#include "../HippoFile.h"
#include <string.h>
#import <Cocoa/Cocoa.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int HippoFile_openDialog(char* filename, int maxLength)
{
	strcpy(filename, "test0r!12");
	return 1;

	/*
	NSOpenPanel* open = [NSOpenPanel openPanel];

	int result = [open runModal];

	if (result == NSOKButton)
	{
		NSArray* selectedFiles = [open URLs];

		// TODO: Proper handling of multiple files

		for (NSURL* url in selectedFiles)
		{
			const char* temp = [[url path] UTF8String];
			strncpy(filename, temp, maxLength);
		}

		return 1;
	}
	*/

	return 0;
}

