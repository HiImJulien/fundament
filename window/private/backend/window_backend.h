#ifndef FUNDAMENT_WINDOW_BACKEND_H
#define FUNDAMENT_WINDOW_BACKEND_H

//==============================================================================
//                               WINDOW BACKEND
//
// Provides functionality to dynamically choose the window backend. Currently
// this only used for builds, that support both X11 and wayland.
//
//==============================================================================

#include <stdbool.h>
#include <stdint.h>

typedef struct fn__window fn__window;

typedef struct fn__window_backend {
    bool(*init_backend)();
    void(*deinit_backend)();
    bool(*create_window)(fn__window*);
    void(*destroy_window)(fn__window*);
    void(*set_size)(fn__window*, uint32_t, uint32_t);
    void(*set_title)(fn__window*, const char*);
    void(*set_visible)(fn__window*, bool);
    void(*pump_events)();
} fn__window_backend;

extern const fn__window_backend* fn__g_window_backend;

bool fn__init_imp_window();

#define fn__deinit_imp_window       fn__g_window_backend->deinit_backend
#define fn__create_imp_window       fn__g_window_backend->create_window
#define fn__destroy_imp_window      fn__g_window_backend->destroy_window
#define fn__imp_window_set_size     fn__g_window_backend->set_size
#define fn__imp_window_set_title    fn__g_window_backend->set_title
#define fn__imp_window_set_visible  fn__g_window_backend->set_visible
#define fn__imp_pump_events         fn__g_window_backend->pump_events

#endif  // FUNDAMENT_WINDOW_BACKEND_H

