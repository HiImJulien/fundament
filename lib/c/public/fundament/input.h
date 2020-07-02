#ifndef FUNDAMENT_INPUT_H
#define FUNDAMENT_INPUT_H

//==============================================================================
// This module implements functionality to receive input from keyboard and
// mouse devices.
//==============================================================================

#include <fundament/api.h>

#include <stdbool.h>

//
// Enumerates all supported physical keyboard keys.
//
// Note, that the values are aligned with the
// USB HID's key usage pages, as defined per this specification:
// https://usb.org.10-1-108-210.causewaynow.com/sites/default/files/hut1_12.pdf
//
enum fn_key {
    fn_key_a= 0x04,
    fn_key_b,
    fn_key_c,
    fn_key_d,
    fn_key_e,
    fn_key_f,
    fn_key_g,
    fn_key_h,
    fn_key_i,
    fn_key_j,
    fn_key_k,
    fn_key_l,
    fn_key_m,
    fn_key_n,
    fn_key_o,
    fn_key_p,
    fn_key_q,
    fn_key_r,
    fn_key_s,
    fn_key_t,
    fn_key_u,
    fn_key_v,
    fn_key_w,
    fn_key_x,
    fn_key_y,
    fn_key_z,

    fn_key_1,
    fn_key_2,
    fn_key_3,
    fn_key_4,
    fn_key_5,
    fn_key_6,
    fn_key_7,
    fn_key_8,
    fn_key_9,
    fn_key_0,

    fn_key_enter,
    fn_key_escape,
    fn_key_backspace,
    fn_key_tab,
    fn_key_space,
    fn_key_hyphen,
    fn_key_equal,
    fn_key_left_bracket,
    fn_key_right_bracket,
    fn_key_backslash,
    fn_key_tilde,
    fn_key_semicolon,
    fn_key_tick,
    fn_key_comma,
    fn_key_dot,
    fn_key_slash,
    fn_key_caps,

    fn_key_f1,
    fn_key_f2,
    fn_key_f3,
    fn_key_f4,
    fn_key_f5,
    fn_key_f6,
    fn_key_f7,
    fn_key_f8,
    fn_key_f9,
    fn_key_f10,
    fn_key_f11,
    fn_key_f12,

    fn_key_print,
    fn_key_scroll,
    fn_key_pause,
    fn_key_insert,
    fn_key_home,
    fn_key_page_up,
    fn_key_delete,
    fn_key_end,
    fn_key_page_down,

    fn_key_right,
    fn_key_left,
    fn_key_down,
    fn_key_up,

    fn_key_pad_lock,
    fn_key_pad_div,
    fn_key_pad_mul,
    fn_key_pad_sub,
    fn_key_pad_add,
    fn_key_pad_enter,
    fn_key_pad_1,
    fn_key_pad_2,
    fn_key_pad_3,
    fn_key_pad_4,
    fn_key_pad_5,
    fn_key_pad_6,
    fn_key_pad_7,
    fn_key_pad_8,
    fn_key_pad_9,
    fn_key_pad_0,

    fn_key_mute    = 0x7F,
    fn_key_vol_up  = 0x80,
    fn_key_vol_down= 0x81,

    fn_left_ctrl= 0xE0,
    fn_left_shift,
    fn_left_alt,
    fn_left_system,
    fn_right_ctrl,
    fn_right_shift,
    fn_right_alt,
    fn_right_system,

    fn_key_unknown,
    fn_key_count= 105,
};

//
// Returns whether specified key is pressed.
//
// Note, that this function does NOT query the actual physical state,
// but, instead, looks up its state in the frameworks cache.
//
API bool fn_is_key_pressed(enum fn_key key);

//
//  Enumerates all supported mouse buttons.
//
enum fn_button {
    fn_button_left  = (1u << 0u),
    fn_button_right = (1u << 1u),
    fn_button_middle= (1u << 2u),
};

//
//  Returns whether specified button(s) are pressed.
//
//  Note, that multiple buttons can be queried by OR'ing
//  the button values. E.g.:
//
//      fn_is_button_pressed(fn_button_left | fn_button_right);
//
//  Also this function does NOT query the actual physical state,
//  but, instead, looks up its state in the frameworks cache.
//
API bool fn_is_button_pressed(enum fn_button buttons);

#endif  // FUNDAMENT_INPUT_H
