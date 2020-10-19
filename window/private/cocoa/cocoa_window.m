#include "cocoa_window.h"
#include "../window_common.h"
#include "cocoa_window_delegate.h"

#import <AppKit/AppKit.h>

bool fn__init_cocoa_window() {
    [NSApplication sharedApplication];
    [NSApp setActivationPolicy: NSApplicationActivationPolicyRegular];
    [NSApp activateIgnoringOtherApps: YES];
    [NSApp finishLaunching];
    
    return true;
}

void fn__deinit_cocoa_window() {}

bool fn__create_cocoa_window(struct fn__window* window) {
    const NSUInteger style_mask =
        NSWindowStyleMaskTitled
        | NSWindowStyleMaskClosable
        | NSWindowStyleMaskResizable;
    
    window->native = [NSWindow alloc];
    [window->native initWithContentRect: NSMakeRect(0, 0, 1, 1)
                              styleMask: style_mask
                                backing: NSBackingStoreBuffered
                                  defer: NO];
    
    FNCocoaWindowDelegate* delegate = [FNCocoaWindowDelegate alloc];
    [delegate initWithWindow: window];
    [window->native setDelegate: delegate];
    
    return window->native != nil;
}

void fn__destroy_cocoa_window(struct fn__window* window) {
    [window->native release];
}

void fn__cocoa_window_set_size(
    struct fn__window* window,
    uint32_t width,
    uint32_t height
) {
    const NSUInteger style_mask = window->native.styleMask;
    NSRect  frame = NSMakeRect(0, 0, width, height);
            frame = [NSWindow frameRectForContentRect: frame
                                            styleMask: style_mask];
    
    [window->native setFrame: frame
                     display: YES];
}

void fn__cocoa_window_set_title(
    struct fn__window* window,
    const char* title
) {
    [window->native setTitle: @(title)];
}

void fn__cocoa_window_set_visible(
    struct fn__window* window,
    bool visible
) {
    if(visible) [window->native orderFront: nil];
    else [window->native orderOut: nil];
}

void fn__cocoa_pump_events() {
    NSEvent* ev = nil;
    while((ev = [NSApp nextEventMatchingMask: NSEventMaskAny
                                   untilDate: nil
                                      inMode: NSDefaultRunLoopMode
                                     dequeue:YES]))
        [NSApp sendEvent: ev];
}
