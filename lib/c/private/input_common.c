#include "input_common.h"

#include <stddef.h>

static bool g_key_state_cache[fn_key_unknown - fn_key_a] = {false, };
static enum fn_button g_button_state_cache = 0;

void fn__set_key_state(enum fn_key key, bool pressed) {
    // The enum values are offset by 0x04
    // to accomodate the usage pages presented
    // in the USB HID specifications.
    const size_t idx = key - fn_key_a;
    g_key_state_cache[idx] = pressed;
}

bool fn__get_key_state(enum fn_key key) {
    const size_t idx = key - fn_key_a;
    return g_key_state_cache[idx];
}

void fn__set_button_state(enum fn_button button, bool pressed) {
    if(pressed)
        g_button_state_cache |= button;
    else
        g_button_state_cache &= ~button;
}

bool fn__get_button_state(enum fn_button buttons) {
    return (g_button_state_cache & buttons) != 0; 
}

enum fn_button fn__get_pressed_buttons() {
    return g_button_state_cache;
}
