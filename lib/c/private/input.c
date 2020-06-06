#include <fundament/input.h>
#include "input_common.h"

bool fn_is_key_pressed(enum fn_key key) {
    return fn__get_key_state(key);
}

bool fn_is_button_pressed(enum fn_button buttons) {
    return fn__get_button_state(buttons);
}
