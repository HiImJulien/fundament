#include <fundament/window.h>
#include <stdio.h>

#include "window_common.h"

#define IDX(window) (window.id - 1)
#define BAD_ID(window) (window.id == 0)

void fn_init_window_module() {
    fn__init_window_context();
}

void fn_deinit_window_module() {
    fn__deinit_window_context();
}

struct fn_window fn_create_window() {
    uint32_t index = 0;
    for(; index <= fn__g_window_context.windows_capacity; ++index) {
        if(index == fn__g_window_context.windows_capacity)
            return (struct fn_window) { .id = 0};
        
        if(fn__g_window_context.windows[index].handle == NULL)
            break;
    }

    fn__g_window_context.windows[index].handle = fn__imp_create_window(index);
    return (struct fn_window) { .id = index + 1 };
}

void fn_destroy_window(struct fn_window window) {
    if(BAD_ID(window))
        return;

    struct fn__window* w = &fn__g_window_context.windows[IDX(window)];
    if(w->handle == NULL)
        return;

    fn__imp_destroy_window(w->handle);

    if(w->title) {
        free((char*) w->title);
        w->title = NULL;
    }

    w->height = 0;
    w->width = 0;
}

uint32_t fn_window_width(struct fn_window window) {
    if(BAD_ID(window))
        return 0;

    const struct fn__window* w = &fn__g_window_context.windows[IDX(window)];
    return w->handle == NULL ? 0 : w->width;
}

uint32_t fn_window_height(struct fn_window window) {
    if(BAD_ID(window))
        return 0;

    const struct fn__window* w = &fn__g_window_context.windows[IDX(window)];
    return w->handle == NULL ? 0 : w->height;
}

void fn_window_set_width(struct fn_window window, uint32_t width) {
    if(BAD_ID(window))
        return;

    struct fn__window* w = &fn__g_window_context.windows[IDX(window)];
    if(w->handle == NULL)
        return;

    w->width = width;
    fn__imp_window_set_size(w->handle, width, w->height);
}

void fn_window_set_height(struct fn_window window, uint32_t height) {
    if(BAD_ID(window))
        return;

    struct fn__window* w = &fn__g_window_context.windows[IDX(window)];
    if(w->handle == NULL)
        return;

    w->height = height;
    fn__imp_window_set_size(w->handle, w->width, height);
}

const char* fn_window_title(struct fn_window window) {
    if(BAD_ID(window))
        return NULL;

    const struct fn__window* w = &fn__g_window_context.windows[IDX(window)];
    return w->title;
}

void fn_window_set_title(struct fn_window window, const char* title) {
    if(BAD_ID(window))
        return;

    struct fn__window* w = &fn__g_window_context.windows[IDX(window)];

    if(w->title)
        free((char*) w->title);

    const size_t len = strlen(title) + 1;
    const char* ntitle = malloc(sizeof(char) * len);
    memcpy((char*) ntitle, title, len * sizeof(char));
    w->title = ntitle;

    fn__imp_window_set_title(w->handle, title);
}

bool fn_window_visible(struct fn_window window) {
    if(BAD_ID(window))
        return false;

    const struct fn__window* w = &fn__g_window_context.windows[IDX(window)];
    return w->handle == NULL ? false : w->visible;
}

void fn_window_set_visibility(struct fn_window window, bool visible) {
    if(BAD_ID(window))
        return;

    struct fn__window* w = &fn__g_window_context.windows[IDX(window)];
    if(w->handle == NULL)
        return;

    w->visible = visible;
    fn__imp_window_set_visibility(w->handle, visible);
}

void fn_poll_events(struct fn_event* ev) {
    fn__imp_window_poll_events();
    fn__pop_event(ev);
}
