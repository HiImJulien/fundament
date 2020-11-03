#ifndef FUNDAMENT_COCOA_WINDOW_H
#define FUNDAMENT_COCOA_WINDOW_H

//==============================================================================
//                                COCOA WINDOW
//
// Implements windows using the Cocoa/AppKit window.
//
//==============================================================================

#include <stdbool.h>
#include <stdint.h>

struct fn__window;

bool fn__init_cocoa_window();
void fn__deinit_cocoa_window();

bool fn__create_cocoa_window(struct fn__window* window);
void fn__destroy_cocoa_window(struct fn__window* window);

void fn__cocoa_window_set_size(
   struct fn__window* window,
   uint32_t width,
   uint32_t height
);

void fn__cocoa_window_set_title(
    struct fn__window* window,
    const char* title
);

void fn__cocoa_window_set_visible(
    struct fn__window* window,
    bool visible
);

void fn__cocoa_pump_events();

#define fn__init_imp_window         fn__init_cocoa_window
#define fn__deinit_imp_window       fn__deinit_cocoa_window
#define fn__create_imp_window       fn__create_cocoa_window
#define fn__destroy_imp_window      fn__destroy_cocoa_window
#define fn__imp_window_set_size     fn__cocoa_window_set_size
#define fn__imp_window_set_title    fn__cocoa_window_set_title
#define fn__imp_window_set_visible  fn__cocoa_window_set_visible
#define fn__imp_pump_events         fn__cocoa_pump_events
#define FN_NULL_WINDOW              NULL

#endif  // FUNDAMENT_COCOA_WINDOW_H
