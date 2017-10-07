
#include "HippoWindow.h"
#include "core/memory/LinearAllocator.h"
#include "HippoOSXWindow.h"
#include <Cocoa/Cocoa.h>
#include <unistd.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct HippoWindow
{
	uint32_t* frameBuffer;
	NSWindow* wnd;
	HippoWindowRect rect;

} HippoWindow;

NSWindow* window_;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

HippoWindow* HippoWindow_create(struct LinearAllocator* allocator, const char* name, const HippoWindow* parent, 
								const HippoWindowRect* windowRect)
{
	HippoWindow* window = 0;

	NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];

	[NSApplication sharedApplication];
	[NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
		
	unsigned int styles = NSResizableWindowMask | NSClosableWindowMask | NSTitledWindowMask;
		
	NSRect rectangle = NSMakeRect(0, 0, 264, 136);
	window_ = [[HippoOSXWindow alloc] initWithContentRect:rectangle styleMask:styles backing:NSBackingStoreBuffered defer:NO];
	[window_ setTitle:@"(none)"];
	[window_ setReleasedWhenClosed:NO];
	[window_ performSelectorOnMainThread:@selector(makeKeyAndOrderFront:) withObject:nil waitUntilDone:YES];

	[pool drain];

	return window;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HippoWindow_destroy(HippoWindow* window)
{
	[window->wnd release];
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HippoWindow_refresh()
{
	[[window_ contentView] setNeedsDisplay:YES];
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int HippoWindow_isClosed()
{
	return ![window_ isVisible];
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// A bit hacky to have here so should move it

void Hippo_quit()
{
	[[NSApplication sharedApplication] terminate:nil];
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HippoWindow_updateEvents()
{
	NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
	NSEvent* event = [NSApp nextEventMatchingMask:NSAnyEventMask untilDate:[NSDate distantPast] inMode:NSDefaultRunLoopMode dequeue:YES];
	if (event) 
	{
		[NSApp sendEvent:event];
		[NSApp updateWindows];
	}
	[pool release];

	usleep(500);
}

