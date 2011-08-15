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

//
// mouseDown:
//
// Handles mouse clicks in our frame. Two actions:
//	- click in the resize box should resize the window
//	- click anywhere else will drag the window.
//
- (void)mouseDown:(NSEvent *)event
{
	NSPoint pointInView = [self convertPoint:[event locationInWindow] fromView:nil];
	
	BOOL resize = NO;
	if (NSPointInRect(pointInView, [self resizeRect]))
	{
		resize = YES;
	}
	
	NSWindow *window = [self window];
	NSPoint originalMouseLocation = [window convertBaseToScreen:[event locationInWindow]];
	NSRect originalFrame = [window frame];
	
    while (YES)
	{
		//
		// Lock focus and take all the dragged and mouse up events until we
		// receive a mouse up.
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
		
		if (!resize)
		{
			//
			// Alter the frame for a drag
			//
			newFrame.origin.x += delta.x;
			newFrame.origin.y += delta.y;
		}
		else
		{
			//
			// Alter the frame for a resize
			//
			newFrame.size.width += delta.x;
			newFrame.size.height -= delta.y;
			newFrame.origin.y += delta.y;
			
			//
			// Constrain to the window's min and max size
			//
			NSRect newContentRect = [window contentRectForFrameRect:newFrame];
			NSSize maxSize = [window maxSize];
			NSSize minSize = [window minSize];
			if (newContentRect.size.width > maxSize.width)
			{
				newFrame.size.width -= newContentRect.size.width - maxSize.width;
			}
			else if (newContentRect.size.width < minSize.width)
			{
				newFrame.size.width += minSize.width - newContentRect.size.width;
			}
			if (newContentRect.size.height > maxSize.height)
			{
				newFrame.size.height -= newContentRect.size.height - maxSize.height;
				newFrame.origin.y += newContentRect.size.height - maxSize.height;
			}
			else if (newContentRect.size.height < minSize.height)
			{
				newFrame.size.height += minSize.height - newContentRect.size.height;
				newFrame.origin.y -= minSize.height - newContentRect.size.height;
			}
		}
		
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
	/*
	[[NSColor clearColor] set];
	NSRectFill(rect);
	
	NSBezierPath *circlePath = [NSBezierPath bezierPathWithOvalInRect:[self bounds]];
	
	NSGradient* aGradient =
		[[[NSGradient alloc]
			initWithColorsAndLocations:
				[NSColor whiteColor], (CGFloat)0.0,
				[NSColor lightGrayColor], (CGFloat)1.0,
				nil]
		autorelease];
	[aGradient drawInBezierPath:circlePath angle:90];

	[[NSColor whiteColor] set];
	[circlePath stroke];
	
	NSRect resizeRect = [self resizeRect];
	NSBezierPath *resizePath = [NSBezierPath bezierPathWithRect:resizeRect];

	[[NSColor lightGrayColor] set];
	[resizePath fill];

	[[NSColor darkGrayColor] set];
	[resizePath stroke];
	
	[[NSColor blackColor] set];
	NSString *windowTitle = [[self window] title];
	NSRect titleRect = [self bounds];
	titleRect.origin.y = titleRect.size.height - (WINDOW_FRAME_PADDING - 7);
	titleRect.size.height = (WINDOW_FRAME_PADDING - 7);
	NSMutableParagraphStyle *paragraphStyle =
		[[[NSMutableParagraphStyle alloc] init] autorelease];
	[paragraphStyle setAlignment:NSCenterTextAlignment];
	[windowTitle
		drawWithRect:titleRect
		options:0
		attributes:[NSDictionary
			dictionaryWithObjectsAndKeys:
				paragraphStyle, NSParagraphStyleAttributeName,
				[NSFont systemFontOfSize:14], NSFontAttributeName,
			nil]];
	*/

	CGContextRef myContext = [[NSGraphicsContext // 1
                                currentContext] graphicsPort];
   // ********** Your drawing code here ********** // 2
    CGContextSetRGBFillColor (myContext, 1, 0, 0, 1);// 3
    CGContextFillRect (myContext, CGRectMake (0, 0, 200, 100 ));// 4
    CGContextSetRGBFillColor (myContext, 0, 0, 1, .5);// 5
    CGContextFillRect (myContext, CGRectMake (0, 0, 100, 200));// 6
}

@end



