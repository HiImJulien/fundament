#ifndef FUNDAMENT_XCB_WINDOW_H
#define FUNDAMENT_XCB_WINDOW_H

//==============================================================================
//                                 XCB WINDOW
//
// Implements windows using the xcb API.
//
//==============================================================================

#include <stdbool.h>
#include <stdint.h>

struct fn__window;

bool fn__init_xcb_window();
void fn__deinit_xcb_window();

bool fn__create_xcb_window(struct fn__window* window);
void fn__destroy_xcb_window(struct fn__window* window);

void fn__xcb_window_set_size(
    struct fn__window* window,
    uint32_t width,
    uint32_t height
);

void fn__xcb_window_set_title(
    struct fn__window* window,
    const char* title
);

void fn__xcb_window_set_visible(
    struct fn__window* window,
    bool visible
);

void fn__xcb_pump_events();

#define fn__init_imp_window         fn__init_xcb_window
#define fn__deinit_imp_window       fn__deinit_xcb_window
#define fn__create_imp_window       fn__create_xcb_window
#define fn__destroy_imp_window      fn__destroy_xcb_window
#define fn__imp_window_set_size     fn__xcb_window_set_size
#define fn__imp_window_set_title    fn__xcb_window_set_title
#define fn__imp_window_set_visible  fn__xcb_window_set_visible
#define fn__imp_pump_events         fn__xcb_pump_events
#define FN_NULL_WINDOW              0

#endif //FUNDAMENT_XCB_WINDOW_H
