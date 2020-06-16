#include "input_Xcb.h"

#include <linux/input-event-codes.h>

void fn__imp_process_keyboard_input(uint32_t keycode, bool pressed) {
    const enum fn_key key = fn__imp_map_virtual_key(keycode);
    fn__set_key_state(key, pressed); 
}

enum fn_key fn__imp_map_virtual_key(uint32_t keycode) {
    const uint32_t sanitized_key = keycode - 8;
    
    switch(sanitized_key) {
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

        case KEY_0: return fn_key_0;
        case KEY_1: return fn_key_1;
        case KEY_2: return fn_key_2;
        case KEY_3: return fn_key_3;
        case KEY_4: return fn_key_4;
        case KEY_5: return fn_key_5;
        case KEY_6: return fn_key_6;
        case KEY_7: return fn_key_7;
        case KEY_8: return fn_key_8;
        case KEY_9: return fn_key_9;

        default:    return fn_key_unknown;
    }
}
