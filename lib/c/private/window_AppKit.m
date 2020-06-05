#include "window_AppKit.h"
#include "window_common.h"

#import <AppKit/AppKit.h>

#include <stdio.h>

static bool g_setup_process = false;

@interface fn__window_delegate: NSObject<NSWindowDelegate>
@property (nonatomic, assign) uint8_t index;
-(id) initWithIndex: (uint8_t) index;
@end

@implementation fn__window_delegate 

-(id) initWithIndex: (uint8_t) index {
    self = [super init];
    self.index = index;
    return self;
}

-(void) windowWillClose: (NSNotification*) notification {
    struct fn_event ev = {0};
    ev.type = fn_event_type_closed;
    fn__push_event(&ev);

    struct fn__window* w = &fn__g_window_context.windows[self.index];
    [w->handle release];
    w->handle = NULL;
    // ev.window.id = self.windowId;
}

@end

void fn__imp_init_window_context() {
    if(!g_setup_process) {
        [NSApplication sharedApplication];     

        [NSApp setActivationPolicy: NSApplicationActivationPolicyRegular];
        [NSApp activateIgnoringOtherApps: YES];
        [NSApp finishLaunching];

        g_setup_process = true;
    }
}

fn_native_window_handle_t fn__imp_create_window(uint32_t index) {
    const NSUInteger style_mask = 
        NSWindowStyleMaskTitled
        | NSWindowStyleMaskClosable
        | NSWindowStyleMaskResizable;

    fn_native_window_handle_t handle = [NSWindow alloc];
    [handle initWithContentRect: NSMakeRect(0, 0, 1, 1)
                      styleMask: style_mask
                        backing: NSBackingStoreBuffered
                          defer: NO];

    fn__window_delegate* delegate = [fn__window_delegate alloc];
    [delegate initWithIndex: index];
    [handle setDelegate: delegate];

    return handle;
}

void fn__imp_destroy_window(fn_native_window_handle_t handle) {
    if(handle.delegate)
        [handle.delegate release];

    [handle release];
}

void fn__imp_window_set_size(
    fn_native_window_handle_t handle,
    uint32_t width,
    uint32_t height) {
    NSRect frame = NSMakeRect(0, 0, width, height);    
    frame = [NSWindow frameRectForContentRect: frame
                                    styleMask: handle.styleMask];

    [handle setFrame: frame
             display: YES];
}

void fn__imp_window_set_title(
    fn_native_window_handle_t handle,
    const char* title) {
    NSString* str = [NSString stringWithUTF8String: title];
    [handle setTitle: str];
}

void fn__imp_window_set_visibility(
    fn_native_window_handle_t handle,
    bool visible) {
    if(visible)
        [handle orderFront: nil];
    else
        [handle orderOut: nil];
}

void fn__imp_window_poll_events() {
    NSEvent* ev = nil;

    while((ev = [NSApp nextEventMatchingMask: NSEventMaskAny
                                   untilDate: nil
                                      inMode: NSDefaultRunLoopMode
                                     dequeue: YES])) {
        [NSApp sendEvent: ev];
    }
}