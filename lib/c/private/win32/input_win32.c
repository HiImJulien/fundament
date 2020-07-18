#include "input_win32.h"
#include <fundament/event.h>
#include "../window_common.h"
#include "input_key_map_win32.h"

#include <stdbool.h>

void fn__imp_process_keyboard_input(UINT msg, WPARAM wParam, LPARAM lParam) {
    if((msg != WM_KEYDOWN) && (msg != WM_SYSKEYDOWN) && (msg != WM_KEYUP)
       && (msg != WM_SYSKEYUP))
        return;

    const bool        is_press= (msg == WM_KEYDOWN) || (msg == WM_SYSKEYDOWN);
    const enum fn_key key     = fn__imp_map_win32_key(wParam);
    const char        letter  = fn__imp_translate_to_char(wParam, lParam);
    if(is_press)
        fn__notify_key_pressed(key, letter);
    else
        fn__notify_key_released(key, letter);
}

void fn__imp_process_mouse_input(UINT msg, LPARAM lParam) {
    const bool is_press= (msg == WM_LBUTTONDOWN) || (msg == WM_RBUTTONDOWN)
                         || (msg == WM_MBUTTONDOWN);

    enum fn_button button= 0;

    switch(msg) {
        case WM_LBUTTONDOWN: button= fn_button_left; break;
        case WM_LBUTTONUP: button= fn_button_left; break;
        case WM_RBUTTONDOWN: button= fn_button_right; break;
        case WM_RBUTTONUP: button= fn_button_right; break;
        case WM_MBUTTONDOWN: button= fn_button_middle; break;
        case WM_MBUTTONUP: button= fn_button_middle; break;
    }

    if(is_press)
        fn__notify_button_pressed(button,
                                  (int32_t) LOWORD(lParam),
                                  (int32_t) HIWORD(lParam));
    else
        fn__notify_button_released(button,
                                   (int32_t) LOWORD(lParam),
                                   (int32_t) HIWORD(lParam));
}

void fn__imp_process_mouse_move(LPARAM lParam) {
    fn__notify_mouse_moved((int32_t) LOWORD(lParam), (int32_t) HIWORD(lParam));
}

char fn__imp_translate_to_char(WPARAM wParam, LPARAM lParam) {
    BYTE state[256];
    GetKeyboardState(state);

    DWORD     ascii= 0;
    const int length=
        ToAscii(wParam, (lParam >> 16) & 0x00ff, state, (LPWORD) &ascii, 0);

    return length == 1 ? (char) ascii : 0;
}
