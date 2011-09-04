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
#include "graphics/gui/HippoFont.h"
#include "core/debug/Assert.h"
#include "core/HippoLua.h"
#include <ApplicationServices/ApplicationServices.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

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

	HippoLua_updateScript();
}

static NSPoint s_prevDragPos;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

- (void)mouseDragged:(NSEvent *)event
{
	NSWindow* window = [self window];
	NSRect originalFrame = [window frame];
	NSPoint location = [window mouseLocationOutsideOfEventStream];
	g_hippoGuiState.mousex = (int)location.x; 
	g_hippoGuiState.mousey = (int)originalFrame.size.height - (int)location.y; 
	
	if (g_hippoGuiState.activeItem != -1)
	{
		HippoLua_updateScript();
		return;
	}

	//NSWindow *window = [self window];
	//NSRect originalFrame = [window frame];
	NSPoint newMouseLocation = [window convertBaseToScreen:[event locationInWindow]];
	NSPoint delta = NSMakePoint(newMouseLocation.x - s_prevDragPos.x,
								newMouseLocation.y - s_prevDragPos.y);

	NSRect newFrame = originalFrame;
	
	newFrame.origin.x += delta.x;
	newFrame.origin.y += delta.y;

	s_prevDragPos = newMouseLocation;
	
	[window setFrame:newFrame display:YES animate:NO];
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

- (void)mouseUp:(NSEvent *)event
{
	printf("mouse up\n");
	g_hippoGuiState.mouseDown = 0;
	HippoLua_updateScript();
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
	s_prevDragPos = [window convertBaseToScreen:[event locationInWindow]];
	NSRect originalFrame = [window frame];
	NSPoint location = [window mouseLocationOutsideOfEventStream];

	g_hippoGuiState.mousex = (int)location.x; 
	g_hippoGuiState.mousey = (int)originalFrame.size.height - (int)location.y; 
	
	//printf("%f %f\n", originalMouseLocation.x, originalMouseLocation.y); 
	printf("mouse down\n");

	g_hippoGuiState.mouseDown = 1;

	HippoLua_updateScript();
}

extern HippoBitmapFont g_microknightFont;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void drawText(CGContextRef context, HippoControlInfo* control, int y_pos)
{
	// this will be a bit hacky

	if (!g_microknightFont.userData)
	{
		uint32_t* tempColorData;
		uint32_t* colorData = tempColorData = (uint32_t*)malloc(128 * 128 * 4);
		const uint8_t* data = g_microknightFont.fontData; 

		// Build new texture

		const uint32_t fontColor = 0xff000000;

		for (uint32_t i = 0; i < 128 * 128; ++i)
		{
			uint8_t color = *data++;
			// font data is packed as 1 bit per pixel
			*tempColorData++ = ((color >> 7) & 1) ? fontColor : 0;
			*tempColorData++ = ((color >> 6) & 1) ? fontColor : 0;
			*tempColorData++ = ((color >> 5) & 1) ? fontColor : 0;
			*tempColorData++ = ((color >> 4) & 1) ? fontColor : 0;
			*tempColorData++ = ((color >> 3) & 1) ? fontColor : 0;
			*tempColorData++ = ((color >> 2) & 1) ? fontColor : 0;
			*tempColorData++ = ((color >> 1) & 1) ? fontColor : 0;
			*tempColorData++ = ((color >> 0) & 1) ? fontColor : 0;
		}

		CGColorSpaceRef space = CGColorSpaceCreateDeviceRGB();
		CGDataProviderRef provider = CGDataProviderCreateWithData(NULL, colorData, 128 * 128 * 4, NULL);

		CGImageRef img = CGImageCreate(128, 128, 8, 32, 128 * 4, space, kCGImageAlphaPremultipliedFirst | kCGBitmapByteOrder32Host, 
									   provider, NULL, false, kCGRenderingIntentDefault);

		g_microknightFont.userData = (void*)img; 
		CGColorSpaceRelease(space);
		CGDataProviderRelease(provider);
	}

	CGImageRef img = g_microknightFont.userData;
	const HippoMonoFontlayout* fontLayout = g_microknightFont.layOut;
	const int charOffset = g_microknightFont.firstCharOffset; 

	const char* text = control->text; 
	char c = *text++;
	int x = control->x;

	while (c != 0)
	{
		int offset = c - charOffset; 
		int xo = fontLayout[offset].x;
		int yo = fontLayout[offset].y;

		CGImageRef letter = CGImageCreateWithImageInRect(img, CGRectMake(xo, yo, 8, 8));

		CGContextDrawImage(context, CGRectMake(x, y_pos, 8, 8), letter); 
		c = *text++;
		x += 8;
	}

	if (control->color != 0)
	{
		CGContextSetRGBFillColor(context, 1.0f, 1.0f, 1.0f, 0.5f);
		CGContextFillRect(context, CGRectMake(control->x, y_pos, control->width, control->height));
	}
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
		int y_pos = (originalFrame.size.height - control->y) - control->height;

		switch (controls[i].type)
		{
			case DRAWTYPE_NONE :
				break;

			case DRAWTYPE_SLIDER :
			{
				CGContextSetRGBFillColor(context, 0.0f, 0.0f, 0.0f, 1.0f);
				CGContextFillRect(context, CGRectMake(control->x, y_pos, control->width, control->height));
				y_pos = (originalFrame.size.height - control->sliderThumbY) - control->sliderThumbHeight;
				CGContextSetRGBFillColor(context, 1.0f, 1.0f, 1.0f, 1.0f);
				CGContextFillRect(context, CGRectMake(control->sliderThumbX, y_pos, control->sliderThumbWidth, control->sliderThumbHeight));
				break;
			}

			case DRAWTYPE_FILL :
			{
				uint32_t color = control->color;
				CGContextSetRGBFillColor(context, 
										((color >> 16) & 0xff) * 1.0f / 255.f, 
										((color >> 8) & 0xff) * 1.0f / 255.f, 
										((color >> 0) & 0xff) * 1.0f / 255.f, 
										1.0f - ((color >> 24) & 0xff) * 1.0f / 255.f);
				CGContextFillRect(context, CGRectMake(control->x, y_pos, control->width, control->height));
				break;
			}

			// this will be hacky

			case DRAWTYPE_TEXT :
			{
				drawText(context, control, y_pos);
				break;
			}

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

				CGContextDrawImage(context, CGRectMake(control->x, y_pos, control->width, control->height), 
								  (CGImageRef)control->imageData->userData);
				
				break;	
			}
		}
	}

	/*
    CGContextSelectFont(context, "Monaco", 10, kCGEncodingMacRoman);
    CGContextSetCharacterSpacing (context, 1); 
    CGContextSetTextDrawingMode (context, kCGTextFill); 
    CGContextSetRGBFillColor(context, 0, 0, 0, 1); 
    CGContextShowTextAtPoint(context, 10, 110, "HippoPlayer 3.00 by Daniel Collin", 33); 
    */
}

@end



