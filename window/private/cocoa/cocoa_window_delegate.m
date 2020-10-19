#include "cocoa_window_delegate.h"
#include "../window_common.h"

@implementation FNCocoaWindowDelegate

- (id) initWithWindow: (struct fn__window *) Window {
    self = [super init];
    self.Window = Window;
    return self;
}

- (void) windowWillClose: (NSNotification *)notification {
    fn__notify_window_closed(self.Window);
}

- (void) windowDidResize: (NSNotification *)notification {
    NSWindow* window = [notification object];
    NSSize size = window.contentView.frame.size;
    fn__notify_window_resized(self.Window, size.width, size.height);
}

- (void) windowDidBecomeKey: (NSNotification *)notification {
    fn__notify_window_gained_focus(self.Window);
}

- (void) windowDidResignKey: (NSNotification *)notification {
    fn__notify_window_lost_focus(self.Window);
}

@end
