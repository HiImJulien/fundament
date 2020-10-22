#include "win32_window.h"
#include "../window_common.h"

#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

extern IMAGE_DOS_HEADER __ImageBase;
static const HINSTANCE fn__g_instance = (HINSTANCE) &__ImageBase;
static const char* fn__g_window_class_name = "fundament.WNDCLASSEX";

static enum fn_key fn__win32_map_key(WPARAM vkey);

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
            fn__notify_window_closed(ptr);
            return 0;

        case WM_KILLFOCUS:
            fn__notify_window_lost_focus(ptr);
            return 0;

        case WM_SETFOCUS:
            fn__notify_window_gained_focus(ptr);
            return 0;

        case WM_SIZE:
            fn__notify_window_resized(
                ptr,
                (uint32_t) LOWORD(lParam),
                (uint32_t) HIWORD(lParam)
            );
            return 0;

        case WM_KEYUP:
        case WM_KEYDOWN: {
            enum fn_key key     = fn__win32_map_key(wParam);
            const bool pressed  = (lParam & (1 << 31));
            fn__notify_key_state_changed(key, pressed);

            return 0;
        }

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

static enum fn_key fn__g_win32_key_mappings[] = {
    ['A'] =	fn_key_a,   ['B'] =	fn_key_b,   ['C'] =	fn_key_c, ['D'] =	fn_key_d,
    ['E'] =	fn_key_e,   ['F'] =	fn_key_f,   ['G'] =	fn_key_g, ['H'] =	fn_key_h,
    ['I'] =	fn_key_i,   ['J'] =	fn_key_j,   ['K'] =	fn_key_k, ['L'] =	fn_key_l,
    ['M'] =	fn_key_m,   ['N'] =	fn_key_n,   ['O'] =	fn_key_o, ['P'] =	fn_key_p,
    ['Q'] =	fn_key_q,   ['R'] =	fn_key_r,   ['S'] =	fn_key_s, ['T'] =	fn_key_t,
    ['U'] =	fn_key_u,   ['V'] =	fn_key_v,   ['W'] =	fn_key_w, ['X'] =	fn_key_x,
    ['Y'] =	fn_key_y,   ['Z'] =	fn_key_z,   ['1'] =	fn_key_1, ['2'] =	fn_key_2,
    ['3'] =	fn_key_3,   ['4'] =	fn_key_4,   ['5'] =	fn_key_5, ['6'] =	fn_key_6,
    ['7'] =	fn_key_7,   ['8'] =	fn_key_8,   ['9'] =	fn_key_9, ['0'] =	fn_key_0,
    [VK_RETURN] = fn_key_enter, [VK_ESCAPE] = fn_key_escape,
    [VK_BACK] =	fn_key_backspace, [VK_TAB] = fn_key_tab,
    [VK_SPACE] = fn_key_space, [VK_OEM_MINUS] = fn_key_hyphen,
    [VK_OEM_PLUS] =	fn_key_equal, [VK_OEM_4] = fn_key_left_bracket,
    [VK_OEM_6] = fn_key_right_bracket, [VK_OEM_5] = fn_key_backslash,
    [VK_OEM_1] = fn_key_semicolon, [VK_OEM_7] = fn_key_quote,
    [VK_OEM_3] = fn_key_tilde, [VK_OEM_COMMA] = fn_key_comma,
    [VK_OEM_PERIOD] = fn_key_period, [VK_OEM_2] = fn_key_slash,
    [VK_CAPITAL] = fn_key_caps,
    [VK_F1] = fn_key_f1, [VK_F2] = fn_key_f2, [VK_F3] = fn_key_f3,
    [VK_F4] = fn_key_f4, [VK_F5] = fn_key_f5, [VK_F6] = fn_key_f6,
    [VK_F7] = fn_key_f7, [VK_F8] = fn_key_f8, [VK_F9] = fn_key_f9,
    [VK_F10] = fn_key_f10, [VK_F11] = fn_key_f11, [VK_F12] = fn_key_f12,
    [VK_PAUSE] = fn_key_pause, [VK_INSERT] = fn_key_insert,
    [VK_HOME] =	fn_key_home, [VK_PRIOR] = fn_key_page_up,
    [VK_DELETE] = fn_key_delete, [VK_END] = fn_key_end,
    [VK_NEXT] =	fn_key_page_down, [VK_RIGHT] = fn_key_right,
    [VK_LEFT] =	fn_key_left, [VK_DOWN] = fn_key_down,
    [VK_UP] = fn_key_up, [VK_DIVIDE] = fn_key_np_divide,
    [VK_MULTIPLY] =	fn_key_np_multiply, [VK_SUBTRACT] =	fn_key_np_minus,
    [VK_ADD] = fn_key_np_plus, [VK_NUMPAD1] = fn_key_np_1,
    [VK_NUMPAD2] = fn_key_np_2, [VK_NUMPAD3] = fn_key_np_3,
    [VK_NUMPAD4] = fn_key_np_4, [VK_NUMPAD5] = fn_key_np_5,
    [VK_NUMPAD6] = fn_key_np_6, [VK_NUMPAD7] = fn_key_np_7,
    [VK_NUMPAD8] = fn_key_np_8, [VK_NUMPAD9] = fn_key_np_9,
    [VK_NUMPAD0] = fn_key_np_0, [VK_APPS] =	fn_key_menu,
    [VK_F13] = fn_key_f13, [VK_F14] = fn_key_f14, [VK_F15] = fn_key_f15,
    [VK_LCONTROL] =	fn_key_left_ctrl, [VK_LSHIFT] = fn_key_left_shift,
    [VK_LMENU] = fn_key_left_alt, [VK_LWIN] = fn_key_left_system,
    [VK_RCONTROL] =	fn_key_right_ctrl, [VK_RSHIFT] = fn_key_right_shift,
    [VK_RMENU] = fn_key_right_alt, [VK_RWIN] = fn_key_right_system,
};

static const size_t fn__g_win32_key_mappings_size =
    sizeof(fn__g_win32_key_mappings)
    / sizeof(enum fn_key);

enum fn_key fn__win32_map_key(WPARAM vkey) {
    return vkey >= fn__g_win32_key_mappings_size
        ? fn_key_unknown
        : fn__g_win32_key_mappings[vkey];
}