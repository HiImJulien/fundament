#include "cocoa_window.h"
#include "../window_common.h"
#include "cocoa_window_delegate.h"

#import <AppKit/AppKit.h>
#import <Carbon/Carbon.h>

static enum fn_key fn__cocoa_map_key(unsigned short keyCode);

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
                                     dequeue:YES])) {
        if(ev.type == NSEventTypeKeyUp
           || ev.type == NSEventTypeKeyDown) {
            enum fn_key key = fn__cocoa_map_key(ev.keyCode);
            const bool press = ev.type == NSEventTypeKeyDown;
            fn__notify_key_state_changed(key, press);
        }
        
        [NSApp sendEvent: ev];
    }
        
}

static enum fn_key fn__g_cocoa_key_mappings[] = {
    [kVK_ANSI_A] = fn_key_a, [kVK_ANSI_B] = fn_key_b, [kVK_ANSI_C] = fn_key_c,
    [kVK_ANSI_D] = fn_key_d, [kVK_ANSI_E] = fn_key_e, [kVK_ANSI_F] = fn_key_f,
    [kVK_ANSI_G] = fn_key_g, [kVK_ANSI_H] = fn_key_h, [kVK_ANSI_I] = fn_key_i,
    [kVK_ANSI_J] = fn_key_j, [kVK_ANSI_K] = fn_key_k, [kVK_ANSI_L] = fn_key_l,
    [kVK_ANSI_M] = fn_key_m, [kVK_ANSI_N] = fn_key_n, [kVK_ANSI_O] = fn_key_o,
    [kVK_ANSI_P] = fn_key_p, [kVK_ANSI_Q] = fn_key_q, [kVK_ANSI_R] = fn_key_r,
    [kVK_ANSI_S] = fn_key_s, [kVK_ANSI_T] = fn_key_t, [kVK_ANSI_U] = fn_key_u,
    [kVK_ANSI_V] = fn_key_v, [kVK_ANSI_W] = fn_key_w, [kVK_ANSI_X] = fn_key_x,
    [kVK_ANSI_Y] = fn_key_y, [kVK_ANSI_Z] = fn_key_z, [kVK_ANSI_1] = fn_key_1,
    [kVK_ANSI_2] = fn_key_2, [kVK_ANSI_3] = fn_key_3, [kVK_ANSI_4] = fn_key_4,
    [kVK_ANSI_5] = fn_key_5, [kVK_ANSI_6] = fn_key_6, [kVK_ANSI_7] = fn_key_7,
    [kVK_ANSI_8] = fn_key_8, [kVK_ANSI_9] = fn_key_9, [kVK_ANSI_0] = fn_key_0,
    [kVK_Return] = fn_key_enter, [kVK_Escape] = fn_key_escape,
    [kVK_Delete] = fn_key_backspace, [kVK_Tab] = fn_key_tab,
    [kVK_Space] = fn_key_space, [kVK_ANSI_Minus] = fn_key_hyphen,
    [kVK_ANSI_Equal] = fn_key_equal, [kVK_ANSI_LeftBracket] = fn_key_left_bracket,
    [kVK_ANSI_RightBracket] = fn_key_right_bracket, [kVK_ANSI_Backslash] = fn_key_backslash,
    [kVK_ANSI_Semicolon] = fn_key_semicolon, [kVK_ANSI_Quote] = fn_key_quote,
    [kVK_ANSI_Grave] = fn_key_tilde, [kVK_ANSI_Comma] = fn_key_comma,
    [kVK_ANSI_Period] = fn_key_period, [kVK_ANSI_Slash] = fn_key_slash,
    [kVK_CapsLock] = fn_key_caps, [kVK_F1] = fn_key_f1, [kVK_F2] = fn_key_f2,
    [kVK_F3] = fn_key_f3, [kVK_F4] = fn_key_f4, [kVK_F5] = fn_key_f5,
    [kVK_F6] = fn_key_f6, [kVK_F7] = fn_key_f7, [kVK_F8] = fn_key_f8,
    [kVK_F9] = fn_key_f9, [kVK_F10] = fn_key_f10, [kVK_F11] = fn_key_f11,
    [kVK_F12] = fn_key_f12, [kVK_Help] = fn_key_insert, [kVK_Home] = fn_key_home,
    [kVK_PageUp] = fn_key_page_up, [kVK_ForwardDelete] = fn_key_delete,
    [kVK_End] = fn_key_end, [kVK_PageDown] = fn_key_page_down,
    [kVK_RightArrow] = fn_key_right, [kVK_LeftArrow] = fn_key_left,
    [kVK_UpArrow] = fn_key_down, [kVK_DownArrow] = fn_key_up,
    [kVK_ANSI_KeypadDivide] = fn_key_np_divide,
    [kVK_ANSI_KeypadMultiply] = fn_key_np_multiply, [kVK_ANSI_KeypadMinus] = fn_key_np_minus,
    [kVK_ANSI_Keypad1] = fn_key_np_1, [kVK_ANSI_Keypad2] = fn_key_np_2,
    [kVK_ANSI_Keypad3] = fn_key_np_3, [kVK_ANSI_Keypad4] = fn_key_np_4,
    [kVK_ANSI_Keypad5] = fn_key_np_5, [kVK_ANSI_Keypad6] = fn_key_np_6,
    [kVK_ANSI_Keypad7] = fn_key_np_7, [kVK_ANSI_Keypad8] = fn_key_np_8,
    [kVK_ANSI_Keypad9] = fn_key_np_9, [kVK_ANSI_Keypad0] = fn_key_np_0,
    [kVK_F13] = fn_key_f13, [kVK_F14] = fn_key_f14,
    [kVK_F15] = fn_key_f15, [kVK_Control] = fn_key_left_ctrl,
    [kVK_Shift] = fn_key_left_shift, [kVK_Option] = fn_key_left_alt,
    [kVK_Command] = fn_key_left_system, [kVK_RightControl] = fn_key_right_ctrl,
    [kVK_RightShift] = fn_key_right_shift, [kVK_RightOption] = fn_key_right_alt,
    [kVK_RightCommand] = fn_key_right_system,
};

static const size_t fn__g_cocoa_key_mappings_size =
    sizeof(fn__g_cocoa_key_mappings)
    / sizeof(enum fn_key);

enum fn_key fn__cocoa_map_key(unsigned short keyCode) {
    return keyCode >= fn__g_cocoa_key_mappings_size
        ? fn_key_unknown
        : fn__g_cocoa_key_mappings[keyCode];
}
