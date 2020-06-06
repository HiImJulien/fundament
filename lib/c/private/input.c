#include <fundament/input.h>
#include "input_common.h"

bool fn_key_is_pressed(enum fn_key key) {
    return fn__get_key_state(key);
}
