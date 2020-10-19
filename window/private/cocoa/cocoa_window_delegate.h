#ifndef FUNDAMENT_COCOA_WINDOW_DELEGATE_H
#define FUNDAMENT_COCOA_WINDOW_DELEGATE_H

#import <AppKit/AppKit.h>

struct fn__window;

@interface FNCocoaWindowDelegate: NSObject<NSWindowDelegate>
@property(nonatomic, assign) struct fn__window* Window;

- (id) initWithWindow: (struct fn__window*) Window;

@end

#endif  // FUNDAMENT_COCOA_WINDOW_DELEGATE_H
