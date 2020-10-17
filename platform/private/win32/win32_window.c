#include "win32_window.h"
#include "../window_common.h"

#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

extern IMAGE_DOS_HEADER __ImageBase;
static const HINSTANCE fn__g_instance = (HINSTANCE) &__ImageBase;
static const char* fn__g_window_class_name = "fundament.WNDCLASSEX";

static LRESULT CALLBACK fn__win32_wnd_proc(
    HWND hWnd, 
    UINT msg, 
    WPARAM wParam, 
    LPARAM lParam
) {
    if(msg == WM_NCCREATE) {
        LPCREATESTRUCTA cs = (LPCREATESTRUCTA) lParam;
        SetWindowLongPtrA(hWnd, GWLP_USERDATA, (LONG_PTR) cs->lpCreateParams);
    }

    struct fn__window* ptr = (struct fn__window*) GetWindowLongPtr(
        hWnd,
        GWLP_USERDATA
    );

    switch(msg) {
        case WM_CLOSE:
            return 0;

        default:
            return DefWindowProcA(hWnd, msg, wParam, lParam);
    }
}

bool fn__init_win32_window() {
    WNDCLASSEXA wc = {
        .cbSize         = sizeof(WNDCLASSEXA),
        .style          = CS_HREDRAW | CS_VREDRAW | CS_OWNDC,
        .hInstance      = fn__g_instance,
        .lpfnWndProc    = fn__win32_wnd_proc,
        .lpszClassName  = fn__g_window_class_name
    };

    return RegisterClassExA(&wc);
}

void fn__deinit_win32_window() {
    UnregisterClassA(fn__g_window_class_name, fn__g_instance);
}

bool fn__create_win32_window(struct fn__window* window) {
    window->native = CreateWindowExA(
        0,
        fn__g_window_class_name,
        "",
        WS_OVERLAPPEDWINDOW,
        0,
        0,
        0,
        0,
        NULL,
        NULL,
        GetModuleHandle(NULL),
        window
    );  

    return window->native != NULL;
}

void fn__destroy_win32_window(struct fn__window* window) {
    DestroyWindow(window->native);
}

void fn__win32_window_set_size(
    struct fn__window* window,
    uint32_t width,
    uint32_t height
) {
    RECT rect = {0, 0, width, height};
    const bool has_menu = GetMenu(window->native) != NULL;
    const DWORD style = (DWORD) GetWindowLongPtr(window->native, GWL_STYLE);
    const DWORD ex_style = (DWORD) GetWindowLongPtr(window->native, GWL_EXSTYLE);

    AdjustWindowRectEx(&rect, style, has_menu, ex_style);

    SetWindowPos(
        window->native,
        NULL,
        0,
        0,
        rect.right - rect.left,
        rect.bottom - rect.top,
        SWP_NOMOVE | SWP_NOOWNERZORDER
    ); 
}

void fn__win32_window_set_title(
    struct fn__window* window,
    const char* title
) {
    SetWindowTextA(window->native, title);
}

void fn__win32_window_set_visible(
    struct fn__window* window,
    bool visible
) {
    ShowWindow(window->native, visible ? SW_SHOWDEFAULT : SW_HIDE);
}

void fn__win32_pump_events() {
    MSG msg;
    while(PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

