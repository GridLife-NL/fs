//
//  LLAppDelegate.m
//  SecondLife
//
//  Created by Geenz on 12/16/12.
//
//

#import "llappviewermacosx-delegate.h"

@implementation LLAppDelegate

@synthesize window;

- (void)dealloc
{
    [super dealloc];
}

- (void) applicationDidFinishLaunching:(NSNotification *)notification
{
	frameTimer = nil;
	
	setLLNSWindowRef([self window]);
	
	if (initViewer())
	{
		frameTimer = [NSTimer scheduledTimerWithTimeInterval:0.0 target:self selector:@selector(mainLoop) userInfo:nil repeats:YES];
	} else {
		handleQuit();
	}
}

- (NSApplicationDelegateReply) applicationShouldTerminate:(NSApplication *)sender
{
	if (!runMainLoop())
	{
		handleQuit();
		return NSTerminateCancel;
	} else {
		[frameTimer release];
		cleanupViewer();
		return NSTerminateNow;
	}
}

- (void) mainLoop
{
	bool appExiting = runMainLoop();
	if (appExiting)
	{
		[frameTimer release];
		[[NSApplication sharedApplication] terminate:self];
	}
}

@end
