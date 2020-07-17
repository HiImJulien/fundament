#include "input_key_map_win32.h"
#include <fundament/input.h>



uint16_t fn__imp_map_win32_key(WPARAM key_code) {
    switch(key_code) {
        case 'A': return fn_key_a;
        case 'B': return fn_key_b;
        case 'C': return fn_key_c;
        case 'D': return fn_key_d;
        case 'E': return fn_key_e;
        case 'F': return fn_key_f;
        case 'G': return fn_key_g;
        case 'H': return fn_key_h;
        case 'I': return fn_key_i;
        case 'J': return fn_key_j;
        case 'K': return fn_key_k;
        case 'L': return fn_key_l;
        case 'M': return fn_key_m;
        case 'N': return fn_key_n;
        case 'O': return fn_key_o;
        case 'P': return fn_key_p;
        case 'Q': return fn_key_q;
        case 'R': return fn_key_r;
        case 'S': return fn_key_s;
        case 'T': return fn_key_t;
        case 'U': return fn_key_u;
        case 'V': return fn_key_v;
        case 'W': return fn_key_w;
        case 'X': return fn_key_x;
        case 'Y': return fn_key_y;
        case 'Z': return fn_key_z;
        case '1': return fn_key_1;
        case '2': return fn_key_2;
        case '3': return fn_key_3;
        case '4': return fn_key_4;
        case '5': return fn_key_5;
        case '6': return fn_key_6;
        case '7': return fn_key_7;
        case '8': return fn_key_8;
        case '9': return fn_key_9;
        case '0': return fn_key_0;
        case VK_RETURN: return fn_key_enter;
        case VK_ESCAPE: return fn_key_escape;
        case VK_BACK: return fn_key_backspace;
        case VK_TAB: return fn_key_tab;
        case VK_SPACE: return fn_key_space;
        case VK_OEM_MINUS: return fn_key_hyphen;
        case VK_OEM_PLUS: return fn_key_equal;
        case VK_OEM_4: return fn_key_left_bracket;
        case VK_OEM_6: return fn_key_right_bracket;
        case VK_OEM_5: return fn_key_backslash;
        case VK_OEM_1: return fn_key_semicolon;
        case VK_OEM_7: return fn_key_quote;
        case VK_OEM_3: return fn_key_tilde;
        case VK_OEM_COMMA: return fn_key_comma;
        case VK_OEM_PERIOD: return fn_key_period;
        case VK_OEM_2: return fn_key_slash;
        case VK_F1: return fn_key_f1;
        case VK_F2: return fn_key_f2;
        case VK_F3: return fn_key_f3;
        case VK_F4: return fn_key_f4;
        case VK_F5: return fn_key_f5;
        case VK_F6: return fn_key_f6;
        case VK_F7: return fn_key_f7;
        case VK_F8: return fn_key_f8;
        case VK_F9: return fn_key_f9;
        case VK_F10: return fn_key_f10;
        case VK_F11: return fn_key_f11;
        case VK_F12: return fn_key_f12;
        case VK_PAUSE: return fn_key_pause;
        case VK_INSERT: return fn_key_insert;
        case VK_HOME: return fn_key_home;
        case VK_PRIOR: return fn_key_page_up;
        case VK_DELETE: return fn_key_delete;
        case VK_END: return fn_key_end;
        case VK_NEXT: return fn_key_page_down;
        case VK_RIGHT: return fn_key_right;
        case VK_LEFT: return fn_key_left;
        case VK_DOWN: return fn_key_down;
        case VK_UP: return fn_key_up;
        case VK_DIVIDE: return fn_key_np_divide;
        case VK_MULTIPLY: return fn_key_np_multiply;
        case VK_SUBTRACT: return fn_key_np_minus;
        case VK_ADD: return fn_key_np_plus;
        case VK_NUMPAD1: return fn_key_np_1;
        case VK_NUMPAD2: return fn_key_np_2;
        case VK_NUMPAD3: return fn_key_np_3;
        case VK_NUMPAD4: return fn_key_np_4;
        case VK_NUMPAD5: return fn_key_np_5;
        case VK_NUMPAD6: return fn_key_np_6;
        case VK_NUMPAD7: return fn_key_np_7;
        case VK_NUMPAD8: return fn_key_np_8;
        case VK_NUMPAD9: return fn_key_np_9;
        case VK_NUMPAD0: return fn_key_np_0;
        case VK_APPS: return fn_key_menu;
        case VK_F13: return fn_key_f13;
        case VK_F14: return fn_key_f14;
        case VK_F15: return fn_key_f15;
        case VK_LCONTROL: return fn_key_left_ctrl;
        case VK_LSHIFT: return fn_key_left_shift;
        case VK_LMENU: return fn_key_left_alt;
        case VK_LWIN: return fn_key_left_system;
        case VK_RCONTROL: return fn_key_right_ctrl;
        case VK_RSHIFT: return fn_key_right_shift;
        case VK_RMENU: return fn_key_right_alt;
        case VK_RWIN: return fn_key_right_system;
        default: return fn_key_undefined;
    }
}
