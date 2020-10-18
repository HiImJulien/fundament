#ifndef FUNDAMENT_WIN32_WINDOW_H
#define FUNDAMENT_WIN32_WINDOW_H

//==============================================================================
//                                WIN32 WINDOW
//
// Implements windows using the win32 API.
//
//==============================================================================

#include <stdbool.h>
#include <stdint.h>

struct fn__window;

bool fn__init_win32_window();
void fn__deinit_win32_window();

bool fn__create_win32_window(struct fn__window* window);
void fn__destroy_win32_window(struct fn__window* window);

void fn__win32_window_set_size(
    struct fn__window* window,
    uint32_t width,
    uint32_t height
);

void fn__win32_window_set_title(
    struct fn__window* window,
    const char* title 
);

void fn__win32_window_set_visible(
    struct fn__window* window,
    bool visible
);

void fn__win32_pump_events();

#define fn__init_imp_window         fn__init_win32_window
#define fn__deinit_imp_window       fn__deinit_win32_window
#define fn__create_imp_window       fn__create_win32_window
#define fn__destroy_imp_window      fn__destroy_win32_window
#define fn__imp_window_set_size     fn__win32_window_set_size
#define fn__imp_window_set_title    fn__win32_window_set_title
#define fn__imp_window_set_visible  fn__win32_window_set_visible
#define fn__imp_pump_events         fn__win32_pump_events

#endif  // FUNDAMENT_WIN32_WINDOW_H
