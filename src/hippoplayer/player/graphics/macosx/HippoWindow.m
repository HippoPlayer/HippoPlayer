
#include "../HippoWindow.h"
#include "core/memory/LinearAllocator.h"
#include <Cocoa/Cocoa.h>

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
	/*
	NSRect rect;
	NSUInteger windowStyle;
	NSBackingStoreType bsType;
	NSOpenGLPixelFormat* pixFormat;

	rect.origin.x = windowRect->x;
	rect.origin.y = windowRect->y; 
	rect.size.width = windowRect->sizeX;
	rect.size.height = windowRect->sizeY;

	window = LinearAllocator_allocZero(allocator, HippoWindow);

	windowStyle = NSTitledWindowMask | NSClosableWindowMask | NSResizableWindowMask;

	bsType = NSBackingStoreBuffered;

	window->wnd = [[NSWindow alloc] initWithContentRect:rect styleMask:windowStyle backing:bsType defer:NO];
	window->rect = *windowRect;

	[window->wnd makeKeyAndOrderFront:nil];

	return window;
	*/

	HippoWindow* window = 0;

	NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];

	[NSApplication sharedApplication];
	[NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
		
	unsigned int styles = NSResizableWindowMask | NSClosableWindowMask | NSTitledWindowMask;
		
	NSRect rectangle = NSMakeRect(100, 100, 640, 480);
	window_ = [[NSWindow alloc] initWithContentRect:rectangle styleMask:styles backing:NSBackingStoreBuffered defer:NO];
	[window_ setTitle:@"(none)"];
	[window_ setReleasedWhenClosed:NO];
	[window_ performSelectorOnMainThread:@selector(makeKeyAndOrderFront:) withObject:nil waitUntilDone:YES];
	[NSApp activateIgnoringOtherApps:YES];
	[NSApp finishLaunching];
		
	[pool release];

	return window;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HippoWindow_destroy(HippoWindow* window)
{
	[window->wnd release];
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int HippoWindow_isClosed()
{
	return ![window_ isVisible];
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
}

