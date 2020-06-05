#include "window_common.h"
#include <fundament/event.h>

#include <assert.h>
#include <stdlib.h>
#include <string.h>

struct fn__window_context fn__g_window_context = {
    .windows = NULL,
    .windows_capacity = 0,
    .windows_size = 0,

    .events = NULL,
    .events_capacity = 0,
    .events_size = 0,
    .events_tail = 0
};

void fn__init_window_context() {
    fn__g_window_context.windows = calloc(1, sizeof(struct fn__window) * 256);
    fn__g_window_context.windows_capacity = 256;
    fn__g_window_context.windows_size = 0;

    fn__g_window_context.events = calloc(1, sizeof(struct fn_event) * 512);
    fn__g_window_context.events_capacity = 512;
    fn__g_window_context.events_size = 0;
    fn__g_window_context.events_tail = 0;

    fn__imp_init_window_context();
}

void fn__deinit_window_context() {
    free(fn__g_window_context.windows);
    // TODO: Destroy each window.
    fn__g_window_context.windows_capacity = 0;
    fn__g_window_context.windows_size = 0;

    free(fn__g_window_context.events);
    fn__g_window_context.events_capacity = 0;
    fn__g_window_context.events_size = 0;
    fn__g_window_context.events_tail = 0;
}

void fn__push_event(struct fn_event* ev) {
    assert(ev != NULL && "You must pass an instance of fn_event!");

    if(fn__g_window_context.events_size 
        == fn__g_window_context.events_capacity) {
        struct fn_event* events = malloc(sizeof(struct fn_event)
            * fn__g_window_context.events_capacity
            * 2
        );

        for(size_t it = 0; it < fn__g_window_context.events_size; ++it) {
            const size_t index = (it
                + fn__g_window_context.events_tail)
                % fn__g_window_context.events_capacity;

            events[it] = fn__g_window_context.events[index];
        }

        free(fn__g_window_context.events);
        fn__g_window_context.events = events;
        fn__g_window_context.events_capacity *= 2;
        fn__g_window_context.events_tail = 0;
    }

    const size_t index = (fn__g_window_context.events_tail
        + fn__g_window_context.events_size++)
        & fn__g_window_context.events_capacity;

    memcpy(
        &fn__g_window_context.events[index],
        ev,
        sizeof(struct fn_event)
    );
}

void fn__pop_event(struct fn_event* ev) {
    assert(ev != NULL && "You must pass the address of an instance of \
                          fn_event to write to.");

    if(fn__g_window_context.events_size == 0) {
        memset(ev, 0, sizeof(struct fn_event));
        return;
    }

    fn__g_window_context.events_size--;
    const size_t index = fn__g_window_context.events_tail++
        % fn__g_window_context.events_capacity;

    memcpy(
        ev,
        &fn__g_window_context.events[index],
        sizeof(struct fn_event)
    );
}
