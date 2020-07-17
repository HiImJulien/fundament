#include "window_appkit.h"
#include "../window_common.h"
#include "input_appkit.h"

#include <AppKit/AppKit.h>

static bool g_setup_process= false;

@interface fn__window_delegate: NSObject<NSWindowDelegate>
@property(nonatomic, assign) uint8_t index;
- (id)initWithIndex:(uint8_t)index;
@end

@implementation fn__window_delegate

- (id)initWithIndex:(uint8_t)index {
    self      = [super init];
    self.index= index;
    return self;
}

- (void)windowWillClose:(NSNotification*)notification {
    fn__notify_window_destroyed(self.index);
}

- (void)windowDidResize:(NSNotification*)notification {
    NSWindow* w   = [notification object];
    NSSize    size= w.contentView.frame.size;
    fn__notify_window_resized(self.index,
                              (uint32_t) size.width,
                              (uint32_t) size.height);
}

- (void)windowDidBecomeKey:(NSNotification*)notification {
    fn__notify_window_gained_focus(self.index);
}

- (void)windowDidResignKey:(NSNotification*)notification {
    fn__notify_window_lost_focus(self.index);
}

@end

void fn__imp_init_window_context() {
    if(!g_setup_process) {
        [NSApplication sharedApplication];

        [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
        [NSApp activateIgnoringOtherApps:YES];
        [NSApp finishLaunching];

        g_setup_process= true;
    }
}

fn_native_window_handle_t fn__imp_create_window(uint32_t index) {
    const NSUInteger style_mask= NSWindowStyleMaskTitled
                                 | NSWindowStyleMaskClosable
                                 | NSWindowStyleMaskResizable;

    fn_native_window_handle_t handle= [NSWindow alloc];
    [handle initWithContentRect:NSMakeRect(0, 0, 1, 1)
                      styleMask:style_mask
                        backing:NSBackingStoreBuffered
                          defer:NO];

    fn__window_delegate* delegate= [fn__window_delegate alloc];
    [delegate initWithIndex:index];
    [handle setDelegate:delegate];

    return handle;
}

void fn__imp_destroy_window(fn_native_window_handle_t handle) {
    if(handle.delegate)
        [handle.delegate release];

    [handle release];
}

void fn__imp_window_set_size(fn_native_window_handle_t handle,
                             uint32_t                  width,
                             uint32_t                  height) {
    NSRect frame= NSMakeRect(0, 0, width, height);
    frame= [NSWindow frameRectForContentRect:frame styleMask:handle.styleMask];

    [handle setFrame:frame display:YES];
}

void fn__imp_window_set_title(fn_native_window_handle_t handle,
                              const char*               title) {
    NSString* str= [NSString stringWithUTF8String:title];
    [handle setTitle:str];
}

void fn__imp_window_set_visibility(fn_native_window_handle_t handle,
                                   bool                      visible) {
    if(visible)
        [handle orderFront:nil];
    else
        [handle orderOut:nil];
}

void fn__imp_window_poll_events() {
    NSEvent* ev= nil;

    while((ev= [NSApp nextEventMatchingMask:NSEventMaskAny
                                  untilDate:nil
                                     inMode:NSDefaultRunLoopMode
                                    dequeue:YES])) {
        switch(ev.type) {
            case NSEventTypeKeyUp:
            case NSEventTypeKeyDown: fn__imp_process_keyboard_input(ev); break;

            case NSEventTypeLeftMouseDown:
            case NSEventTypeLeftMouseUp:
            case NSEventTypeRightMouseDown:
            case NSEventTypeRightMouseUp:
            case NSEventTypeOtherMouseDown:
            case NSEventTypeOtherMouseUp:
                fn__imp_process_mouse_input(ev);
                break;

            case NSEventTypeScrollWheel: fn__imp_process_mouse_wheel(ev); break;

            default: break;
        }

        [NSApp sendEvent:ev];
    }
}
