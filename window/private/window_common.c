#include "window_common.h"
#include <fundament/event.h>

#include <assert.h>
#include <stdlib.h>

struct fn__window_context fn__g_window_context = {0, };

void fn__push_event(struct fn_event* in_ev) {
    assert(in_ev);

    const bool exhausted = fn__g_window_context.events_capacity
        == fn__g_window_context.events_size;

    if (exhausted) {
        const size_t next_cap = fn__g_window_context.events_capacity
            ? fn__g_window_context.events_capacity * 2
            : FN_INITIAL_EVENT_CAPACITY;

        struct fn_event* ev = malloc(sizeof(struct fn_event) * next_cap);
        for (size_t it = 0; it < fn__g_window_context.events_size; ++it) {
            const size_t idx =
                (it + fn__g_window_context.events_tail)
                % fn__g_window_context.events_capacity;

            ev[it] = fn__g_window_context.events[idx];
        }

        free(fn__g_window_context.events);
        fn__g_window_context.events = ev;
        fn__g_window_context.events_capacity = next_cap;
        fn__g_window_context.events_head = fn__g_window_context.events_size;
        fn__g_window_context.events_tail = 0;
    }

    ++fn__g_window_context.events_size;
    const size_t idx =
        fn__g_window_context.events_head++
        % fn__g_window_context.events_capacity;

    fn__g_window_context.events[idx] = *in_ev;
}

void fn__pop_event(struct fn_event* out_ev) {
    assert(out_ev);

    if (fn__g_window_context.events_size == 0) {
        *out_ev = (struct fn_event){
            .type = fn_event_type_none,
        };

        return;
    }

    --fn__g_window_context.events_size;
    const size_t idx =
        fn__g_window_context.events_tail++
        % fn__g_window_context.events_capacity;

    *out_ev = fn__g_window_context.events[idx];
}

void fn__notify_window_closed(struct fn__window* window) {
    fn__push_event(&(struct fn_event) {
        .type = fn_event_type_closed,
        .window.id = window->handle
    });

    window->state = fn__window_state_closed;
    free((char*) window->title);
    window->title = NULL;

    fn_dealloc_handle(
        &fn__g_window_context.window_pool,
        window->handle
    );
}

void fn__notify_window_resized(
    struct fn__window* window,
    uint32_t width,
    uint32_t height
) {
    fn__push_event(&(struct fn_event) {
        .type = fn_event_type_resized,
        .window.id = window->handle,
        .size = {
            .width = width,
            .height = height
        }
    });

    window->width = width;
    window->height = height;
}

void fn__notify_window_gained_focus(struct fn__window* window) {
    fn__push_event(&(struct fn_event) {
        .type = fn_event_type_focus_gained,
        .window.id = window->handle
    });

    window->focused = true;
}

void fn__notify_window_lost_focus(struct fn__window* window) {
    fn__push_event(&(struct fn_event) {
        .type = fn_event_type_focus_lost,
        .window.id = window->handle
    });

    window->focused = false;
}

void fn__notify_key_state_changed(
    enum fn_key key,
    bool is_pressed
) {
    fn__push_event(&(struct fn_event) {
        .type = is_pressed
            ? fn_event_type_key_pressed
            : fn_event_type_key_released,
        .key = key,
        .letter = 0
    });
}
