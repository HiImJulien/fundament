#include <fundament/input.h>
#include "window_common.h"

bool fn_is_key_pressed(enum fn_key key) {
    return fn__g_window_context.pressed_keys[key];
}