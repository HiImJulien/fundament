#ifndef FUNDAMENT_INPUT_H
#define FUNDAMENT_INPUT_H

//==============================================================================
// This module implements functionality to receive input from keyboard and
// mouse devices.
//==============================================================================

#include <fundament/api.h>

#include <stdbool.h>

//
// Enumerates all supported keys.
//
enum fn_key {
    fn_key_undefined,
    fn_key_a,
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
    fn_key_semicolon,
    fn_key_quote,
    fn_key_tilde,
    fn_key_comma,
    fn_key_period,
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
    fn_key_np_divide,
    fn_key_np_multiply,
    fn_key_np_minus,
    fn_key_np_plus,
    fn_key_np_1,
    fn_key_np_2,
    fn_key_np_3,
    fn_key_np_4,
    fn_key_np_5,
    fn_key_np_6,
    fn_key_np_7,
    fn_key_np_8,
    fn_key_np_9,
    fn_key_np_0,
    fn_key_menu,
    fn_key_f13,
    fn_key_f14,
    fn_key_f15,
    fn_key_left_ctrl,
    fn_key_left_shift,
    fn_key_left_alt,
    fn_key_left_system,
    fn_key_right_ctrl,
    fn_key_right_shift,
    fn_key_right_alt,
    fn_key_right_system,
    fn_key_count
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
