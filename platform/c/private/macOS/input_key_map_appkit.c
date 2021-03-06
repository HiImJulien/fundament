#include "input_key_map_appkit.h"
#include <fundament/input.h>

#include <Carbon/Carbon.h>

uint16_t fn__imp_map_appkit_key(uint16_t key_code) {
    switch(key_code) {
        case kVK_ANSI_A: return fn_key_a;
        case kVK_ANSI_B: return fn_key_b;
        case kVK_ANSI_C: return fn_key_c;
        case kVK_ANSI_D: return fn_key_d;
        case kVK_ANSI_E: return fn_key_e;
        case kVK_ANSI_F: return fn_key_f;
        case kVK_ANSI_G: return fn_key_g;
        case kVK_ANSI_H: return fn_key_h;
        case kVK_ANSI_I: return fn_key_i;
        case kVK_ANSI_J: return fn_key_j;
        case kVK_ANSI_K: return fn_key_k;
        case kVK_ANSI_L: return fn_key_l;
        case kVK_ANSI_M: return fn_key_m;
        case kVK_ANSI_N: return fn_key_n;
        case kVK_ANSI_O: return fn_key_o;
        case kVK_ANSI_P: return fn_key_p;
        case kVK_ANSI_Q: return fn_key_q;
        case kVK_ANSI_R: return fn_key_r;
        case kVK_ANSI_S: return fn_key_s;
        case kVK_ANSI_T: return fn_key_t;
        case kVK_ANSI_U: return fn_key_u;
        case kVK_ANSI_V: return fn_key_v;
        case kVK_ANSI_W: return fn_key_w;
        case kVK_ANSI_X: return fn_key_x;
        case kVK_ANSI_Y: return fn_key_y;
        case kVK_ANSI_Z: return fn_key_z;
        case kVK_ANSI_1: return fn_key_1;
        case kVK_ANSI_2: return fn_key_2;
        case kVK_ANSI_3: return fn_key_3;
        case kVK_ANSI_4: return fn_key_4;
        case kVK_ANSI_5: return fn_key_5;
        case kVK_ANSI_6: return fn_key_6;
        case kVK_ANSI_7: return fn_key_7;
        case kVK_ANSI_8: return fn_key_8;
        case kVK_ANSI_9: return fn_key_9;
        case kVK_ANSI_0: return fn_key_0;
        case kVK_Return: return fn_key_enter;
        case kVK_Escape: return fn_key_escape;
        case kVK_Delete: return fn_key_backspace;
        case kVK_Tab: return fn_key_tab;
        case kVK_Space: return fn_key_space;
        case kVK_ANSI_Minus: return fn_key_hyphen;
        case kVK_ANSI_Equal: return fn_key_equal;
        case kVK_ANSI_LeftBracket: return fn_key_left_bracket;
        case kVK_ANSI_RightBracket: return fn_key_right_bracket;
        case kVK_ANSI_Backslash: return fn_key_backslash;
        case kVK_ANSI_Semicolon: return fn_key_semicolon;
        case kVK_ANSI_Quote: return fn_key_quote;
        case kVK_ANSI_Grave: return fn_key_tilde;
        case kVK_ANSI_Comma: return fn_key_comma;
        case kVK_ANSI_Period: return fn_key_period;
        case kVK_ANSI_Slash: return fn_key_slash;
        case kVK_CapsLock: return fn_key_caps;
        case kVK_F1: return fn_key_f1;
        case kVK_F2: return fn_key_f2;
        case kVK_F3: return fn_key_f3;
        case kVK_F4: return fn_key_f4;
        case kVK_F5: return fn_key_f5;
        case kVK_F6: return fn_key_f6;
        case kVK_F7: return fn_key_f7;
        case kVK_F8: return fn_key_f8;
        case kVK_F9: return fn_key_f9;
        case kVK_F10: return fn_key_f10;
        case kVK_F11: return fn_key_f11;
        case kVK_F12: return fn_key_f12;
        // No known mapping for fn_key_pause
        case kVK_Help: return fn_key_insert;
        case kVK_Home: return fn_key_home;
        case kVK_PageUp: return fn_key_page_up;
        case kVK_ForwardDelete: return fn_key_delete;
        case kVK_End: return fn_key_end;
        case kVK_PageDown: return fn_key_page_down;
        case kVK_RightArrow: return fn_key_right;
        case kVK_LeftArrow: return fn_key_left;
        case kVK_UpArrow: return fn_key_down;
        case kVK_DownArrow: return fn_key_up;
        case kVK_ANSI_KeypadDivide: return fn_key_np_divide;
        case kVK_ANSI_KeypadMultiply: return fn_key_np_multiply;
        case kVK_ANSI_KeypadMinus: return fn_key_np_minus;
        case kVK_ANSI_KeypadPlus: return fn_key_np_plus;
        case kVK_ANSI_Keypad1: return fn_key_np_1;
        case kVK_ANSI_Keypad2: return fn_key_np_2;
        case kVK_ANSI_Keypad3: return fn_key_np_3;
        case kVK_ANSI_Keypad4: return fn_key_np_4;
        case kVK_ANSI_Keypad5: return fn_key_np_5;
        case kVK_ANSI_Keypad6: return fn_key_np_6;
        case kVK_ANSI_Keypad7: return fn_key_np_7;
        case kVK_ANSI_Keypad8: return fn_key_np_8;
        case kVK_ANSI_Keypad9: return fn_key_np_9;
        case kVK_ANSI_Keypad0: return fn_key_np_0;
        // No known mapping for fn_key_menu
        case kVK_F13: return fn_key_f13;
        case kVK_F14: return fn_key_f14;
        case kVK_F15: return fn_key_f15;
        case kVK_Control: return fn_key_left_ctrl;
        case kVK_Shift: return fn_key_left_shift;
        case kVK_Option: return fn_key_left_alt;
        case kVK_Command: return fn_key_left_system;
        case kVK_RightControl: return fn_key_right_ctrl;
        case kVK_RightShift: return fn_key_right_shift;
        case kVK_RightOption: return fn_key_right_alt;
        case kVK_RightCommand: return fn_key_right_system;
        default: return fn_key_undefined;
    }
}
