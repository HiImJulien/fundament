#include "window_appkit.h"
#include "input_key_map_appkit.h"
#include "../window_common.h"

#include <AppKit/AppKit.h>

//==============================================================================
// The following section defines utilities used in the implementation.
//==============================================================================

//
// Processes events of type 'NSEventTypeKey*'.
//
static void fn__imp_on_keyboard(NSEvent* ev) {
    const bool press = ev.type == NSEventTypeKeyDown;
    const enum fn_key key = fn__imp_map_appkit_key((uint16_t) ev.keyCode);
    const char letter = ev.characters.length == 1 
        ? [ev.characters UTF8String][0]
        : 0;

    fn__notify_key_changed(key, letter, press);
}

static void fn__imp_on_mouse(NSEvent* ev) {
    const bool press = (ev.type == NSEventTypeLeftMouseDown)
        || (ev.type == NSEventTypeRightMouseDown)
        || (ev.type == NSEventTypeOtherMouseDown);

    enum fn_button button = 0;
    
    switch(ev.type) {
        case NSEventTypeLeftMouseDown:
        case NSEventTypeLeftMouseUp:
            button = fn_button_left;
            break;

        case NSEventTypeRightMouseDown:
        case NSEventTypeRightMouseUp:
            button = fn_button_right;
            break;

        case NSEventTypeOtherMouseDown:
        case NSEventTypeOtherMouseUp:
            button = fn_button_middle;
            break;
    }

    const NSRect frame = ev.window.contentView.frame;
    const NSPoint loc = ev.locationInWindow;

    const int32_t x = loc.x;
    const int32_t y = frame.size.height - loc.y; 

    fn__notify_button_changed(button, x, y, press);
}

static void fn__imp_on_mouse_wheel(NSEvent* ev) {
    NSRect frame = ev.window.contentView.frame;
    NSPoint loc = ev.locationInWindow;
    
    fn__notify_mouse_wheel_moved(
        (int32_t) ev.deltaZ,
        (int32_t) loc.x,
        (int32_t) frame.size.height - loc.y
    );
}

//
// Implements the NSWindowDelegate protocol, used as callback
// (hence the name delegate), to inform the user about events.
//
@interface fn__window_delegate : NSObject <NSWindowDelegate>

@property(nonatomic, assign) uint32_t index;

- (id) initWithIndex: (uint32_t) index;
@end

@implementation fn__window_delegate

- (id) initWithIndex: (uint32_t) index {
    self = [super init];
    self.index = index;
    return self;
}

- (void) windowWillClose: (NSNotification*) notification {
    fn__notify_window_destroyed(self.index);
}

- (void) windowDidResize: (NSNotification*) notification {
    NSWindow* w = [notification object];
    NSSize size = w.contentView.frame.size;
    fn__notify_window_resized(
        self.index, (uint32_t) size.width, (uint32_t) size.height
    );
}

- (void) windowDidBecomeKey: (NSNotification*) notification {
    fn__notify_window_gained_focus(self.index);
}

- (void) windowDidResignKey: (NSNotification*) notification {
    fn__notify_window_lost_focus(self.index);
}

@end

//==============================================================================
// The following section implements the window functions.
//==============================================================================

void fn__imp_init_window_context()
{
    if(!fn__g_window_context.setup_process) {
        [NSApplication sharedApplication];

        [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
        [NSApp activateIgnoringOtherApps:YES];
        [NSApp finishLaunching];

        fn__g_window_context.setup_process = true;
    }
}

fn_native_window_handle_t fn__imp_create_window(uint32_t index) {
    const NSUInteger style_mask =
        NSWindowStyleMaskTitled 
        | NSWindowStyleMaskClosable 
        | NSWindowStyleMaskResizable
    ;

    fn_native_window_handle_t handle = [NSWindow alloc];
    [handle initWithContentRect: NSMakeRect(0, 0, 1, 1)
                      styleMask: style_mask 
                        backing: NSBackingStoreBuffered 
                          defer: NO
    ];

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
    uint32_t height
) {
    NSRect frame = NSMakeRect(0, 0, width, height);
    frame = [NSWindow frameRectForContentRect:frame styleMask:handle.styleMask];

    [handle setFrame:frame display:YES];
}

void fn__imp_window_set_title(
    fn_native_window_handle_t handle, 
    const char* title
) {
    NSString* str = [NSString stringWithUTF8String:title];
    [handle setTitle:str];
}

void fn__imp_window_set_visibility(
    fn_native_window_handle_t handle, 
    bool visible
) {
    if(visible)
        [handle orderFront:nil];
    else
        [handle orderOut:nil];
}

void fn__imp_window_poll_events() {
    NSEvent* ev = nil;

    while((ev = [NSApp nextEventMatchingMask: NSEventMaskAny 
                                   untilDate: nil 
                                      inMode: NSDefaultRunLoopMode 
                                     dequeue: YES])) {
        switch(ev.type) {
            case NSEventTypeKeyUp:
            case NSEventTypeKeyDown:
                fn__imp_on_keyboard(ev);
                break;

            case NSEventTypeLeftMouseDown:
            case NSEventTypeLeftMouseUp:
            case NSEventTypeRightMouseDown:
            case NSEventTypeRightMouseUp:
            case NSEventTypeOtherMouseDown:
            case NSEventTypeOtherMouseUp:
                fn__imp_on_mouse(ev);
                break;

            case NSEventTypeScrollWheel:
                fn__imp_on_mouse_wheel(ev);
                break;
        }

        [NSApp sendEvent:ev];
    }
}
