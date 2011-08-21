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
#include <graphics/gui/HippoGui.h>

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

- (void)mouseUp:(NSEvent *)event
{
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
	//NSPoint location = [window mouseLocationOutsideOfEventStream];

	g_hippoGuiState.mouseDown = 1;
	
    while (YES)
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
}

//
// drawRect:
//
// Draws the frame of the window.
//
- (void)drawRect:(NSRect)rect
{
	CGContextRef myContext = [[NSGraphicsContext // 1
                                currentContext] graphicsPort];
   // ********** Your drawing code here ********** // 2
    CGContextSetRGBFillColor (myContext, 1, 0, 0, 1);// 3
    CGContextFillRect (myContext, CGRectMake (0, 0, 200, 100 ));// 4
    CGContextSetRGBFillColor (myContext, 0, 0, 1, .5);// 5
    CGContextFillRect (myContext, CGRectMake (0, 0, 100, 200));// 6
}

@end



