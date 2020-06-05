#include "window_win32.h"
#include "window_common.h"
#include <fundament/event.h>

#include <stdbool.h>
#include <string.h>
#include <stdio.h>

extern IMAGE_DOS_HEADER __ImageBase;
static const HINSTANCE g_instance = (HINSTANCE) &__ImageBase; 
static const char* g_window_class_name = "fundament.WNDCLASSEX";

void fn__imp_init_window_context() {
    WNDCLASSEXA wc = {0, };
    wc.cbSize = sizeof(WNDCLASSEXA);
    wc.hInstance = g_instance;
    wc.lpfnWndProc = fn__imp_callback;
    wc.lpszClassName = g_window_class_name;

    RegisterClassExA(&wc);
}

fn_native_window_handle_t fn__imp_create_window(uint32_t index) {
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

void fn__imp_destroy_window(fn_native_window_handle_t handle) {
    DestroyWindow(handle);
}

void fn__imp_window_set_size(
    fn_native_window_handle_t handle, 
    uint32_t width, 
    uint32_t height) {
    RECT rect = {0, 0, width, height};
    const bool has_menu = GetMenu(handle) != NULL;
    const DWORD style = (DWORD) GetWindowLongPtrA(handle, GWL_STYLE);
    const DWORD ex_style = (DWORD) GetWindowLongPtr(handle, GWL_EXSTYLE);

    AdjustWindowRectEx(
        &rect,
        style,
        has_menu,
        ex_style
    );

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
    fn_native_window_handle_t handle,
    const char* title) {
    SetWindowTextA(handle, title);
}

void fn__imp_window_poll_events() {
    MSG msg;
    while(PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

void fn__imp_window_set_visibility(
    fn_native_window_handle_t handle,
    bool visible) {
    ShowWindow(handle, visible ? SW_SHOWDEFAULT : SW_HIDE);
}

LRESULT fn__imp_callback(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if(msg == WM_NCCREATE) {
        LPCREATESTRUCTA cs = (LPCREATESTRUCTA) (lParam);
        SetWindowLongPtrA(hWnd, GWLP_USERDATA, (LONG_PTR) cs->lpCreateParams);
    }

    const uint32_t id = (uint32_t) GetWindowLongPtrA(hWnd, GWLP_USERDATA);
    struct fn__window* w = &fn__g_window_context.windows[id];
    struct fn_event ev;

    switch(msg) {
        case WM_CLOSE:
        case WM_DESTROY:
            ev.type = fn_event_type_closed;
            fn__push_event(&ev);

            return 0;

        default:
            return DefWindowProcA(hWnd, msg, wParam, lParam);
    }
}
