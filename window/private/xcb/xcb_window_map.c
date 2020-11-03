#include "xcb_window_map.h"
#include "../window_common.h"
#include <fundament/types.h>

#include <stddef.h>
#include <stdlib.h>

typedef struct fn__xcb_window_map {
    struct fn__window*  window_pointers[FN_WINDOW_CAPACITY];
    size_t              window_capacity;
    size_t              window_size;
} fn__xcb_window_map;

static fn__xcb_window_map fn__g_map = { NULL, };

void fn__xcb_window_map_insert(struct fn__window* window) {
    uint32_t hash = window->native % FN_WINDOW_CAPACITY;
    for(size_t it = 0; it < FN_WINDOW_CAPACITY; ++it) {
        hash = (hash + it) % FN_WINDOW_CAPACITY;

        if(fn__g_map.window_pointers[hash])
            continue;

        fn__g_map.window_pointers[hash] = window;
        break;
    }
}

void fn__xcb_window_map_remove(struct fn__window* window) {
    uint32_t hash = window->native % FN_WINDOW_CAPACITY;
    for(size_t it = 0; it < FN_WINDOW_CAPACITY; ++it) {
        hash = (hash + it) % FN_WINDOW_CAPACITY;

        if(fn__g_map.window_pointers[hash] != window)
            continue;

        fn__g_map.window_pointers[hash] = NULL;
        return;
    }
}

struct fn__window* fn__xcb_window_map_get(uint32_t id) {
    uint32_t hash = id % FN_WINDOW_CAPACITY;

    for(size_t it = 0; it < FN_WINDOW_CAPACITY; ++it) {
        hash = (hash + it) % FN_WINDOW_CAPACITY;

        if(fn__g_map.window_pointers[hash]->native != id)
            continue;

        return fn__g_map.window_pointers[hash];
    }

    return NULL;
}

