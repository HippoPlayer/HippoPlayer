
#import <Cocoa/Cocoa.h>

@interface MinimalAppAppDelegate : NSObject <NSApplicationDelegate> {
    NSWindow *window;
	NSButton *button;
}

- (IBAction) buttonClicked:(id)sender;

@property (assign) IBOutlet NSWindow *window;
@property (assign) IBOutlet NSButton *button;

@end
