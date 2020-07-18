#include "input_key_map_xcb.h"
#include <fundament/input.h>

#include <linux/input-event-codes.h>

uint16_t fn__imp_map_xcb_key(uint32_t key_code) {
    switch(key_code) {
        case KEY_A: return fn_key_a;
        case KEY_B: return fn_key_b;
        case KEY_C: return fn_key_c;
        case KEY_D: return fn_key_d;
        case KEY_E: return fn_key_e;
        case KEY_F: return fn_key_f;
        case KEY_G: return fn_key_g;
        case KEY_H: return fn_key_h;
        case KEY_I: return fn_key_i;
        case KEY_J: return fn_key_j;
        case KEY_K: return fn_key_k;
        case KEY_L: return fn_key_l;
        case KEY_M: return fn_key_m;
        case KEY_N: return fn_key_n;
        case KEY_O: return fn_key_o;
        case KEY_P: return fn_key_p;
        case KEY_Q: return fn_key_q;
        case KEY_R: return fn_key_r;
        case KEY_S: return fn_key_s;
        case KEY_T: return fn_key_t;
        case KEY_U: return fn_key_u;
        case KEY_V: return fn_key_v;
        case KEY_W: return fn_key_w;
        case KEY_X: return fn_key_x;
        case KEY_Y: return fn_key_y;
        case KEY_Z: return fn_key_z;
        case KEY_1: return fn_key_1;
        case KEY_2: return fn_key_2;
        case KEY_3: return fn_key_3;
        case KEY_4: return fn_key_4;
        case KEY_5: return fn_key_5;
        case KEY_6: return fn_key_6;
        case KEY_7: return fn_key_7;
        case KEY_8: return fn_key_8;
        case KEY_9: return fn_key_9;
        case KEY_0: return fn_key_0;
        case KEY_ENTER: return fn_key_enter;
        case KEY_ESC: return fn_key_escape;
        case KEY_BACKSPACE: return fn_key_backspace;
        case KEY_TAB: return fn_key_tab;
        case KEY_SPACE: return fn_key_space;
        case KEY_MINUS: return fn_key_hyphen;
        case KEY_EQUAL: return fn_key_equal;
        case KEY_LEFTBRACE: return fn_key_left_bracket;
        case KEY_RIGHTBRACE: return fn_key_right_bracket;
        case KEY_BACKSLASH: return fn_key_backslash;
        case KEY_SEMICOLON: return fn_key_semicolon;
        case KEY_APOSTROPHE: return fn_key_quote;
        case KEY_GRAVE: return fn_key_tilde;
        case KEY_COMMA: return fn_key_comma;
        case KEY_DOT: return fn_key_period;
        case KEY_SLASH: return fn_key_slash;
        case KEY_CAPSLOCK: return fn_key_caps;
        case KEY_F1: return fn_key_f1;
        case KEY_F2: return fn_key_f2;
        case KEY_F3: return fn_key_f3;
        case KEY_F4: return fn_key_f4;
        case KEY_F5: return fn_key_f5;
        case KEY_F6: return fn_key_f6;
        case KEY_F7: return fn_key_f7;
        case KEY_F8: return fn_key_f8;
        case KEY_F9: return fn_key_f9;
        case KEY_F10: return fn_key_f10;
        case KEY_F11: return fn_key_f11;
        case KEY_F12: return fn_key_f12;
        case KEY_PAUSE: return fn_key_pause;
        case KEY_INSERT: return fn_key_insert;
        case KEY_HOME: return fn_key_home;
        case KEY_PAGEUP: return fn_key_page_up;
        case KEY_DELETE: return fn_key_delete;
        case KEY_END: return fn_key_end;
        case KEY_PAGEDOWN: return fn_key_page_down;
        case KEY_RIGHT: return fn_key_right;
        case KEY_LEFT: return fn_key_left;
        case KEY_DOWN: return fn_key_down;
        case KEY_UP: return fn_key_up;
        case KEY_KPSLASH: return fn_key_np_divide;
        case KEY_KPASTERISK: return fn_key_np_multiply;
        case KEY_KPMINUS: return fn_key_np_minus;
        case KEY_KPPLUS: return fn_key_np_plus;
        case KEY_KP1: return fn_key_np_1;
        case KEY_KP2: return fn_key_np_2;
        case KEY_KP3: return fn_key_np_3;
        case KEY_KP4: return fn_key_np_4;
        case KEY_KP5: return fn_key_np_5;
        case KEY_KP6: return fn_key_np_6;
        case KEY_KP7: return fn_key_np_7;
        case KEY_KP8: return fn_key_np_8;
        case KEY_KP9: return fn_key_np_9;
        case KEY_KP0: return fn_key_np_0;
        case KEY_MENU: return fn_key_menu;
        case KEY_F13: return fn_key_f13;
        case KEY_F14: return fn_key_f14;
        case KEY_F15: return fn_key_f15;
        case KEY_LEFTCTRL: return fn_key_left_ctrl;
        case KEY_LEFTSHIFT: return fn_key_left_shift;
        case KEY_LEFTALT: return fn_key_left_alt;
        case KEY_LEFTMETA: return fn_key_left_system;
        case KEY_RIGHTCTRL: return fn_key_right_ctrl;
        case KEY_RIGHTSHIFT: return fn_key_right_shift;
        case KEY_RIGHTALT: return fn_key_right_alt;
        case KEY_RIGHTMETA: return fn_key_right_system;
        default: return fn_key_undefined;
    }
}
