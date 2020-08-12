#include "window_common.h"
#include <fundament/event.h>
#include "input_common.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

struct fn__window_context fn__g_window_context = {0,};

void fn__init_window_context()
{
    fn__g_window_context.windows = calloc(1, sizeof(struct fn__window) * 256);
    fn__g_window_context.windows_capacity = 256;
    fn__g_window_context.windows_size = 0;

    fn__g_window_context.events = calloc(1, sizeof(struct fn_event) * 512);
    fn__g_window_context.events_capacity = 512;
    fn__g_window_context.events_size = 0;
    fn__g_window_context.events_tail = 0;

    fn__g_window_context.focused_window.id = 0;

    fn__imp_init_window_context();
}

void fn__deinit_window_context()
{
    free(fn__g_window_context.windows);
    // TODO: Destroy each window.
    fn__g_window_context.windows_capacity = 0;
    fn__g_window_context.windows_size = 0;

    free(fn__g_window_context.events);
    fn__g_window_context.events_capacity = 0;
    fn__g_window_context.events_size = 0;
    fn__g_window_context.events_tail = 0;

    fn__imp_deinit_window_context();
}

void fn__push_event(struct fn_event* ev)
{
    assert(ev != NULL && "You must pass an instance of fn_event!");

    if((fn__g_window_context.events_size + 1) ==
       fn__g_window_context.events_capacity) {
        struct fn_event* events = calloc(
            1,
            sizeof(struct fn_event) * 2 * fn__g_window_context.events_capacity
        );

        for(size_t it = 0; it < fn__g_window_context.events_size; ++it) {
            const size_t idx = it % fn__g_window_context.windows_capacity;
            events[it] = fn__g_window_context.events[idx];
        }

        free(fn__g_window_context.events);
        fn__g_window_context.events = events;
        fn__g_window_context.events_tail = 0;
        fn__g_window_context.events_capacity *= 2;
    }

    const size_t idx =
        (fn__g_window_context.events_tail + fn__g_window_context.events_size) %
        fn__g_window_context.windows_capacity;

    fn__g_window_context.events_size++;

    memcpy(&fn__g_window_context.events[idx], ev, sizeof(struct fn_event));
}

void fn__pop_event(struct fn_event* ev)
{
    assert(ev != NULL && "You must pass the address of an instance of \
                          fn_event to write to.");

    if(fn__g_window_context.events_size == 0) {
        memset(ev, 0, sizeof(struct fn_event));
        return;
    }

    const size_t idx = fn__g_window_context.events_tail %
                       fn__g_window_context.windows_capacity;

    fn__g_window_context.events_size -= 1;
    fn__g_window_context.events_tail += 1;

    memcpy(ev, &fn__g_window_context.events[idx], sizeof(struct fn_event));
}

//==============================================================================
// The following section implements the callback functions declared in
// 'window_common.h'.
//==============================================================================

void fn__notify_window_destroyed(uint32_t idx)
{
    struct fn_event ev = {0,};
    ev.type = fn_event_type_closed;
    ev.window.id = idx + 1;
    fn__push_event(&ev);

    struct fn__window* w = &fn__g_window_context.windows[idx];
    fn__imp_destroy_window(w->handle);
    w->handle = fn__g_null_wnd;
    w->width = 0;
    w->height = 0;

    free((char*) w->title);
    w->title = NULL;

    w->visible = false;
    w->focused = false;
}

void fn__notify_window_resized(uint32_t idx, uint32_t width, uint32_t height)
{
    struct fn_event ev = {0,};
    ev.type = fn_event_type_resized;
    ev.window.id = idx + 1;
    ev.size.width = width;
    ev.size.height = height;

    fn__push_event(&ev);

    struct fn__window* w = &fn__g_window_context.windows[idx];
    w->width = width;
    w->height = height;
}

void fn__notify_window_gained_focus(uint32_t idx)
{
    struct fn_event ev = {0,};
    ev.type = fn_event_type_focus_gained;
    ev.window.id = idx + 1;
    fn__push_event(&ev);

    struct fn__window* w = &fn__g_window_context.windows[idx];
    w->focused = true;

    fn__g_window_context.focused_window.id = idx + 1;
}

void fn__notify_window_lost_focus(uint32_t idx)
{
    struct fn_event ev = {0,};
    ev.type = fn_event_type_focus_lost;
    ev.window.id = idx + 1;
    fn__push_event(&ev);

    struct fn__window* w = &fn__g_window_context.windows[idx];
    w->focused = true;

    // NOTE: A window id of 0 is considered an invalid handle,
    // this used to signal that currently no window is focused
    // (until notify_window_resized window signals otherwise).
    fn__g_window_context.focused_window.id = 0;
}

void fn__notify_key_changed(enum fn_key key, char localized_key, bool press)
{
    struct fn_event ev = {0,};
    ev.type = press ? fn_event_type_key_pressed : fn_event_type_key_released;
    ev.window = fn__g_window_context.focused_window;
    ev.key.key = key;
    ev.key.letter = localized_key;

    fn__push_event(&ev);
    fn__set_key_state(key, press);
}

void fn__notify_button_changed(
    enum fn_button button,
    int32_t x,
    int32_t y,
    bool press)
{
    struct fn_event ev = {0,};
    ev.type = press
                ? fn_event_type_button_pressed
                : fn_event_type_button_released;

    ev.window = fn__g_window_context.focused_window;
    ev.button.button = button;
    ev.button.x = x;
    ev.button.y = y;

    fn__push_event(&ev);
    fn__set_button_state(button, press);
}

void fn__notify_mouse_moved(int32_t x, int32_t y)
{
    struct fn_event ev = {0,};
    ev.type = fn_event_type_mouse_moved;
    ev.window = fn__g_window_context.focused_window;
    ev.mouse_move.x = x;
    ev.mouse_move.y = y;

    fn__push_event(&ev);
}

void fn__notify_mouse_wheel_moved(int32_t x, int32_t y, int32_t dt)
{
    struct fn_event ev = {0,};
    ev.type = fn_event_type_mouse_wheel;
    ev.window = fn__g_window_context.focused_window;
    ev.mouse_wheel.dt = dt;
    ev.mouse_wheel.x = x;
    ev.mouse_wheel.y = y;

    fn__push_event(&ev);
}
