//
//  HippoOSXWindowFrameView.m
//  HippoOSXWindow
//
//  Created by Matt Gallagher on 12/12/08.
//  Copyright 2008 Matt Gallagher. All rights reserved.
//
//  Permission is given to use this source code file without charge in any
//  project, commercial or otherwise, entirely at your risk, with the condition
//  that any redistribution (in part or whole) of source code must retain
//  this copyright and permission notice. Attribution in compiled projects is
//  appreciated but not required.
//

#import "HippoOSXWindowFrameView.h"
#include "graphics/HippoImageLoader.h"
#include "graphics/gui/HippoGui.h"
#include "core/debug/Assert.h"
#include <ApplicationServices/ApplicationServices.h>

@implementation HippoOSXWindowFrameView

//
// resizeRect
//
// Returns the bounds of the resize box.
//
- (NSRect)resizeRect
{
	const CGFloat resizeBoxSize = 16.0;
	const CGFloat contentViewPadding = 5.5;
	
	NSRect contentViewRect = [[self window] contentRectForFrameRect:[[self window] frame]];
	NSRect resizeRect = NSMakeRect(
		NSMaxX(contentViewRect) + contentViewPadding,
		NSMinY(contentViewRect) - resizeBoxSize - contentViewPadding,
		resizeBoxSize,
		resizeBoxSize);
	
	return resizeRect;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

-(void) viewWillMoveToWindow:(NSWindow *)newWindow 
{
    // Setup a new tracking area when the view is added to the window.
    NSTrackingArea* trackingArea = [[NSTrackingArea alloc] initWithRect:[self frame] 
    	options: (NSTrackingMouseMoved | NSTrackingActiveAlways) owner:self userInfo:nil];
    [self addTrackingArea:trackingArea];
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

- (void)mouseMoved:(NSEvent *)event
{
	NSWindow* window = [self window];
	//NSPoint originalMouseLocation = [window convertBaseToScreen:[event locationInWindow]];
	NSRect originalFrame = [window frame];
	NSPoint location = [window mouseLocationOutsideOfEventStream];

	g_hippoGuiState.mousex = (int)location.x; 
	g_hippoGuiState.mousey = (int)originalFrame.size.height - (int)location.y; 

	printf("%d %d\n", g_hippoGuiState.mousex, g_hippoGuiState.mousey);  
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

- (void)mouseUp:(NSEvent *)event
{
	printf("mouse up\n");
	g_hippoGuiState.mouseDown = 0;
}

//
// mouseDown:
//
// Handles mouse clicks in our frame. Two actions:
//	- click in the resize box should resize the window
//	- click anywhere else will drag the window.
//
- (void)mouseDown:(NSEvent *)event
{
	NSWindow *window = [self window];
	NSPoint originalMouseLocation = [window convertBaseToScreen:[event locationInWindow]];
	NSRect originalFrame = [window frame];
	NSPoint location = [window mouseLocationOutsideOfEventStream];

	g_hippoGuiState.mousex = (int)location.x; 
	g_hippoGuiState.mousey = (int)originalFrame.size.height - (int)location.y; 
	
	printf("%f %f\n", originalMouseLocation.x, originalMouseLocation.y); 
	printf("mouse down\n");

	g_hippoGuiState.mouseDown = 1;

	/*
    //while (YES)
	{
		//
		// Lock focus and take all the dragged and mouse up events until we
		// receive a mouse up.
		//
		//
        NSEvent *newEvent = [window
			nextEventMatchingMask:(NSLeftMouseDraggedMask | NSLeftMouseUpMask)];
		
        if ([newEvent type] == NSLeftMouseUp)
		{
			g_hippoGuiState.mouseDown = 0;
			break;
		}
		
		//
		// Work out how much the mouse has moved
		//

		NSPoint newMouseLocation = [window convertBaseToScreen:[newEvent locationInWindow]];
		NSPoint delta = NSMakePoint(
			newMouseLocation.x - originalMouseLocation.x,
			newMouseLocation.y - originalMouseLocation.y);
		
		NSRect newFrame = originalFrame;
		
		newFrame.origin.x += delta.x;
		newFrame.origin.y += delta.y;
		
		[window setFrame:newFrame display:YES animate:NO];
	}
	*/
}

//
// drawRect:
//
// Draws the frame of the window.
//
- (void)drawRect:(NSRect)rect
{
	CGContextRef context = [[NSGraphicsContext currentContext] graphicsPort];
	NSWindow *window = [self window];
	NSRect originalFrame = [window frame];

	uint32_t controlCount = s_controlId; 
	HippoControlInfo* controls = (HippoControlInfo*)&g_controls;

	for (uint i = 0; i < controlCount; ++i)
	{
		HippoControlInfo* control = &g_controls[i]; 

		switch (controls[i].type)
		{
			case DRAWTYPE_NONE :
				break;

			case DRAWTYPE_FILL :
				break;

			case DRAWTYPE_IMAGE :
			{
				HIPPO_ASSERT(control->imageData);

				// if we have no userData, we need to initialize it with CimageDataRef

				if (!control->imageData->userData)
				{
					CGColorSpaceRef space = CGColorSpaceCreateDeviceRGB();
					CGDataProviderRef provider = CGDataProviderCreateWithData(
						NULL, control->imageData->data, control->imageData->width * control->imageData->height * 3, NULL);

					CGImageRef img = CGImageCreate(
						control->imageData->width, control->imageData->height, 8, 24,
						control->imageData->width * 3, space, kCGImageAlphaNoneSkipFirst,
						provider, NULL, false, kCGRenderingIntentDefault);

					control->imageData->userData = (void*)img; 
					CGColorSpaceRelease(space);
					CGDataProviderRelease(provider);
				}

				CGContextDrawImage(context, CGRectMake(control->x, (originalFrame.size.height - control->y) - control->height, control->width, control->height), 
								  (CGImageRef)control->imageData->userData);
				
				break;	
			}
		}
	}
}

@end



