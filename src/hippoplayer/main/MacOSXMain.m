#import <Cocoa/Cocoa.h>

int HippoMain_create();
void HippoMain_destroy();

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
	[NSApplication sharedApplication];
	NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];

	if (!HippoMain_create())
		return 0;

	[NSApp run];

	[pool drain]; 

	HippoMain_destroy();

	return 0;
}
		

