#include "window_win32.h"
#include "input_key_map_win32.h"
#include "../window_common.h"

#include <stdbool.h>

#include <Windowsx.h>

extern IMAGE_DOS_HEADER __ImageBase;
static const HINSTANCE g_instance = (HINSTANCE) &__ImageBase;
static const char* g_window_class_name = "fundament.WNDCLASSEX";

void fn__imp_init_window_context()
{
    WNDCLASSEXA wc = {0,};

    wc.cbSize = sizeof(WNDCLASSEXA);
    wc.hInstance = g_instance;
    wc.lpfnWndProc = fn__imp_callback;
    wc.lpszClassName = g_window_class_name;

    RegisterClassExA(&wc);
}

fn_native_window_handle_t fn__imp_create_window(uint32_t index)
{
    fn_native_window_handle_t handle = CreateWindowExA(
        0,
        g_window_class_name,
        "",
        WS_OVERLAPPEDWINDOW,
        0,
        0,
        200,
        200,
        NULL,
        NULL,
        g_instance,
        (LPVOID) (intptr_t) index
    );

    return handle;
}

void fn__imp_destroy_window(fn_native_window_handle_t handle)
{
    DestroyWindow(handle);
}

void fn__imp_window_set_size(
    fn_native_window_handle_t handle,
    uint32_t width,
    uint32_t height
)
{
    RECT rect = {0, 0, width, height};
    const bool has_menu = GetMenu(handle) != NULL;
    const DWORD style = (DWORD) GetWindowLongPtrA(handle, GWL_STYLE);
    const DWORD ex_style = (DWORD) GetWindowLongPtr(handle, GWL_EXSTYLE);

    AdjustWindowRectEx(&rect, style, has_menu, ex_style);

    SetWindowPos(
        handle,
        NULL,
        0,
        0,
        rect.right - rect.left,
        rect.bottom - rect.top,
        SWP_NOMOVE | SWP_NOOWNERZORDER
    );
}

void fn__imp_window_set_title(
    fn_native_window_handle_t handle, const char* title
)
{
    SetWindowTextA(handle, title);
}

void fn__imp_window_poll_events()
{
    MSG msg;
    while(PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

void fn__imp_window_set_visibility(
    fn_native_window_handle_t handle, bool visible
)
{
    ShowWindow(handle, visible ? SW_SHOWDEFAULT : SW_HIDE);
}

//
// Translates a scan code to its letter/character representation.
//
static char fn__imp_translate_to_letter(WPARAM wParam, LPARAM lParam)
{
    BYTE state[256];
    GetKeyboardState(state);

    DWORD ascii = 0;
    const int length = ToAscii(
        wParam,
        (lParam >> 16) & 0x00ff,
        state,
        (LPWORD) &ascii,
        0
    );

    return length == 1 ? (char) ascii : 0;
}

//
// Processes incoming keyboard events.
//
static void fn__imp_process_keyboard_input(
    UINT msg,
    WPARAM wParam,
    LPARAM lParam
)
{
    if((msg != WM_KEYDOWN)
       && (msg != WM_SYSKEYDOWN)
       && (msg != WM_KEYUP)
       && (msg != WM_SYSKEYUP)
        )
        return;

    const bool is_press = (msg == WM_KEYDOWN) || (msg == WM_KEYUP);
    const enum fn_key key = fn__imp_map_win32_key(wParam);
    const char letter = fn__imp_translate_to_letter(wParam, lParam);
    fn__notify_key_changed(key, letter, is_press);
}

//
// Processes incoming mouse events.
//
static void fn__imp_process_mouse_input(UINT msg, LPARAM lParam) {
    const bool is_press = (msg == WM_LBUTTONDOWN) || (msg == WM_RBUTTONDOWN) ||
                          (msg == WM_MBUTTONDOWN);

    enum fn_button button = 0;

    switch(msg) {
        case WM_LBUTTONDOWN:    button = fn_button_left; break;
        case WM_LBUTTONUP:      button = fn_button_left; break;
        case WM_RBUTTONDOWN:    button = fn_button_right; break;
        case WM_RBUTTONUP:      button = fn_button_right; break;
        case WM_MBUTTONDOWN:    button = fn_button_middle; break;
        case WM_MBUTTONUP:      button = fn_button_middle; break;
    }

    if(is_press)
        fn__notify_button_pressed(
            button,
            (int32_t) LOWORD(lParam),
            (int32_t) HIWORD(lParam)
        );
    else
        fn__notify_button_released(
            button,
            (int32_t) LOWORD(lParam),
            (int32_t) HIWORD(lParam)
        );
}

LRESULT fn__imp_callback(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if(msg == WM_NCCREATE) {
        LPCREATESTRUCTA cs = (LPCREATESTRUCTA) (lParam);
        SetWindowLongPtrA(hWnd, GWLP_USERDATA, (LONG_PTR) cs->lpCreateParams);
    }

    const uint32_t index = (uint32_t) GetWindowLongPtrA(hWnd, GWLP_USERDATA);

    switch(msg) {
        case WM_CLOSE:
        case WM_DESTROY:
            fn__notify_window_destroyed(index);
            return 0;

        case WM_KILLFOCUS:
            fn__notify_window_lost_focus(index);
            return 0;

        case WM_SETFOCUS:
            fn__notify_window_gained_focus(index);
            return 0;

        case WM_SIZE:
            fn__notify_window_resized(
                index,
                (uint32_t) LOWORD(lParam),
                (uint32_t) HIWORD(lParam)
            );

            return 0;

        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYUP:
            fn__imp_process_keyboard_input(msg, wParam, lParam);
            return 0;

        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_RBUTTONUP:
        case WM_MBUTTONUP:
            fn__imp_process_mouse_input(msg, lParam);
            return 0;

        case WM_MOUSEMOVE:
            fn__notify_mouse_moved(
                (int32_t) LOWORD(lParam),
                (int32_t) HIWORD(lParam)
            );
            return 0;

        case WM_MOUSEWHEEL: {
            POINT pt;
            pt.x = GET_X_LPARAM(lParam);
            pt.y = GET_Y_LPARAM(lParam);
            ScreenToClient(hWnd, &pt);

            MOUSEHOOKSTRUCTEX* mhs = (MOUSEHOOKSTRUCTEX*) lParam;
            fn__notify_mouse_wheel_moved(
                (int32_t) GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA,
                (int32_t) pt.x,
                (int32_t) pt.y
            );

            return 0;
        }

        default:
            return DefWindowProcA(hWnd, msg, wParam, lParam);
    }
}
