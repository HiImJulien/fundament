#include <fundament/fundament.h>

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

//------------------------------------------------------------------------------
// The following section declares internal functions, that must be implemented
// respectively for each supported target platform.
//------------------------------------------------------------------------------

static bool fn_imp_init(void);
static void fn_imp_quit(void);
static fn_native_window_handle_t fn_imp_window_create(uint8_t id);
static void fn_imp_window_destroy(fn_native_window_handle_t handle);
static void fn_imp_window_set_size(fn_native_window_handle_t handle,
	uint32_t width, uint32_t height);
static void fn_imp_window_set_title(fn_native_window_handle_t handle, 
	const char* title);
static void fn_imp_window_set_visibility(fn_native_window_handle_t handle,
	bool visible);
static void fn_imp_pump_events(void);

//------------------------------------------------------------------------------
// The following section defines structures and functions used 
// thoughout the library internally.
//------------------------------------------------------------------------------

static const fn_native_window_handle_t g_imp_null_window =
	(fn_native_window_handle_t) (intptr_t) NULL;

/**
 * @struct Bundles all properties of a window.
 */
struct fn_imp_window {
	fn_native_window_handle_t handle;
	uint32_t width;
	uint32_t height;
	int32_t x;
	int32_t y;
	bool visible;
	bool focused;
	const char* title;	
};

/**
 * @struct Represents the context within which the library operates and bundles
 * platform specific details.
 */
struct fn_imp_window_context {
	struct fn_imp_window windows[255];
	struct fn_event* events;
	size_t events_capacity;
	size_t events_size;
	size_t events_head;
	// TODO: Keys!
	bool key_state[fn_key_count];
	enum fn_button button_state; 
};

static struct fn_imp_window_context g_imp_window_context = {
	.windows = {g_imp_null_window, 0, 0, 0, 0, false, false, NULL},
	.events = NULL,
	.events_capacity = 0,
	.events_size = 0,
	.events_head = 0,
	.key_state = {0},
	.button_state = 0,
};

/**
 * @brief Pushes a new event to the end of the 
 * event queue.
 * Resizes the queue if this operation would exceed 
 * the capacity.
 * @param ev The event to push onto the queue.
 */
static void fn_imp_push_event(struct fn_event* ev) {
	if(g_imp_window_context.events_size == 
		g_imp_window_context.events_capacity) {

		struct fn_event* events = (struct fn_event*) malloc(
			sizeof(struct fn_event) * g_imp_window_context.events_capacity * 2
		);

		for(size_t it = 0; it < g_imp_window_context.events_size; ++it) {
			const size_t idx = 
				(it
				+ g_imp_window_context.events_head)
				% g_imp_window_context.events_capacity;

			events[it] = g_imp_window_context.events[idx];
		}

		free(g_imp_window_context.events);

		g_imp_window_context.events = events;
		g_imp_window_context.events_capacity *= 2;
		g_imp_window_context.events_head = 0;
	}

	const size_t index = (g_imp_window_context.events_head
		+ g_imp_window_context.events_size++) 
		% g_imp_window_context.events_capacity;

	struct fn_event* dst = &g_imp_window_context.events[index];
	memcpy(dst, ev, sizeof(struct fn_event));
}

/**
 * @brief Pops an event from the beginning of the 
 * event queue.
 * @param ev The location where to store event.
 */
static void fn_imp_pop_event(struct fn_event* ev) {
	if(g_imp_window_context.events_size == 0) {
		memset(ev, 0, sizeof(struct fn_event));
		return;
	}

	g_imp_window_context.events_size--;	
	const size_t index = g_imp_window_context.events_head++
		% g_imp_window_context.events_capacity;

	struct fn_event* src = &g_imp_window_context.events[index];
	memcpy(ev, src, sizeof(struct fn_event));
}

static bool fn_imp_check_window(struct fn_window window) {
	if(window.id == 255)
		return false;

	return g_imp_window_context.windows[window.id].handle;
}

//------------------------------------------------------------------------------
// The following section implements the public functions.
//------------------------------------------------------------------------------

bool fn_init(void) {
	const size_t inital_event_queue_capacity = 256;
	g_imp_window_context.events = (struct fn_event*) malloc(
		sizeof(struct fn_event) * inital_event_queue_capacity);
	g_imp_window_context.events_capacity = inital_event_queue_capacity;

	const bool imp_init_result = fn_imp_init();
	if(!imp_init_result) {
		free(g_imp_window_context.events);
		g_imp_window_context.events_capacity = 0;
	}

	return imp_init_result;
}

void fn_quit(void) {
	for(size_t it = 0; it < 255; ++it) {
		struct fn_imp_window* inst = &g_imp_window_context.windows[it];
		if(inst->handle) {
			fn_imp_window_destroy(inst->handle);
			inst->handle = g_imp_null_window;
		}

		if(inst->title) {
			free((char*) inst->title);
			inst->title = NULL;
		}
	}

	free(g_imp_window_context.events);
	g_imp_window_context.events = NULL;
	g_imp_window_context.events_capacity = 0;
	g_imp_window_context.events_size = 0;
	g_imp_window_context.events_head = 0;

	fn_imp_quit();
}

struct fn_window fn_window_create() {
	uint8_t id;
	for(id = 0; id <= 255; ++id) {
		if(id == 255)
			return (struct fn_window) { id };

		if(!g_imp_window_context.windows[id].handle)
			break;
	}

	g_imp_window_context.windows[id].handle = fn_imp_window_create(id);
	return (struct fn_window) {
		g_imp_window_context.windows[id].handle ? id : 255
	};
}

void fn_window_destroy(struct fn_window window) {
	if(!fn_imp_check_window(window))
		return;

	struct fn_imp_window* inst = &g_imp_window_context.windows[window.id];
	fn_imp_window_destroy(inst->handle);
	inst->handle = g_imp_null_window;

	if(inst->title) {
		free((char*) inst->title);
		inst->title = NULL;
	}
}

void fn_window_set_size(struct fn_window window, uint32_t width, 
	uint32_t height) {
	if(!fn_imp_check_window(window))
		return;

	struct fn_imp_window* inst = &g_imp_window_context.windows[window.id];
	inst->width = width;
	inst->height = height;
	fn_imp_window_set_size(inst->handle, width, height);
}

uint32_t fn_window_width(struct fn_window window) {
	if(!fn_imp_check_window(window))
		return 0;

	return g_imp_window_context.windows[window.id].width;
}

uint32_t fn_window_height(struct fn_window window) {
	if(!fn_imp_check_window(window))
		return 0;

	return g_imp_window_context.windows[window.id].height;
}

void fn_window_set_title(struct fn_window window, const char* title) {
	if(!fn_imp_check_window(window))
		return;

	struct fn_imp_window* inst = &g_imp_window_context.windows[window.id];

	if(inst->title)
		free((char*) inst->title);

	const size_t title_len = strlen(title) + 1;
	inst->title = (const char*) malloc(sizeof(char) * title_len);
	strcpy((char*) inst->title, title);

	fn_imp_window_set_title(inst->handle, inst->title);	
}

const char* fn_window_title(struct fn_window window) {
	if(!fn_imp_check_window(window))
		return NULL;

	return g_imp_window_context.windows[window.id].title;
}

void fn_window_set_visibility(struct fn_window window, bool visible) {
	if(!fn_imp_check_window(window))
		return;

	struct fn_imp_window* inst = &g_imp_window_context.windows[window.id];
	if(inst->visible != visible)
		fn_imp_window_set_visibility(inst->handle, visible);
}

bool fn_window_visible(struct fn_window window) {
	if(!fn_imp_check_window(window))
		return false;

	return g_imp_window_context.windows[window.id].visible;
}

bool fn_window_focused(struct fn_window window) {
	if(!fn_imp_check_window(window))
		return false;

	return g_imp_window_context.windows[window.id].focused;
}

bool fn_window_open(struct fn_window window) {
	return fn_imp_check_window(window);
}

fn_native_window_handle_t fn_window_handle(struct fn_window window) {
	if(!fn_imp_check_window(window))
		return g_imp_null_window;

	return g_imp_window_context.windows[window.id].handle;
}

bool fn_key_pressed(enum fn_key key) {
	return g_imp_window_context.key_state[key];
}

void fn_poll_event(struct fn_event* ev) {
	assert(ev != NULL);
	fn_imp_pump_events();
	fn_imp_pop_event(ev);
}

//------------------------------------------------------------------------------
// The following section implements "platform specific" functions declared
// at the top of the file using win32 API.
//------------------------------------------------------------------------------

#if defined(_WIN32)

#include <Windows.h>

EXTERN_C IMAGE_DOS_HEADER __ImageBase;
static HINSTANCE g_imp_module_instance = (HINSTANCE) &__ImageBase;
static const char* g_imp_window_class_name = "fundament.WNDCLASSEXA";

static enum fn_key fn_imp_map_key(UINT vkey) {
	
	// A-Z lies in the range [0x41, 0x5A]
	if(vkey >= 0x41 && vkey <= 0x5A)
		return (enum fn_key) vkey - 0x41;	

	// 0-9 lies in the range [0x30, 0x39]
	if(vkey == 0x30)
		return fn_key_0;

	if(vkey >= 0x31 && vkey <= 0x39)
		return (enum fn_key) vkey - 0x31;

	switch(vkey) {
		case VK_RETURN: return fn_key_enter;
		case VK_ESCAPE: return fn_key_esc;
		case VK_BACK: return fn_key_backspace;
		case VK_TAB: return fn_key_tab;
		case VK_SPACE: return fn_key_space;
		case VK_OEM_MINUS: return fn_key_hyphen;
		case VK_OEM_PLUS: return fn_key_eq;
		case VK_OEM_4: return fn_key_lbrace;
		case VK_OEM_6: return fn_key_rbrace;
		case VK_OEM_5: return fn_key_backslash;

		case VK_OEM_1: return fn_key_semicolon;
		case VK_OEM_7: return fn_key_quote;
		case VK_OEM_3: return fn_key_grave;
		case VK_OEM_COMMA: return fn_key_comma;
		case VK_OEM_PERIOD: return fn_key_dot;
		case VK_OEM_2: return fn_key_slash;
		case VK_CAPITAL: return fn_key_caps;

		case VK_F1:	return fn_key_f1;
		case VK_F2:	return fn_key_f2;
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

		case VK_PRINT: return fn_key_print;
		case VK_SCROLL: return fn_key_scroll;
		case VK_PAUSE: return fn_key_pause;
		case VK_INSERT: return fn_key_ins;
		case VK_DELETE: return fn_key_del;
		case VK_END: return fn_key_end;
		case VK_PRIOR: return fn_key_page_up;
		case VK_NEXT: return fn_key_page_down;

		case VK_LEFT: return fn_key_left;
		case VK_RIGHT: return fn_key_right;
		case VK_UP: return fn_key_up;
		case VK_DOWN: return fn_key_down;

		case VK_LCONTROL: return fn_key_lctrl;
		case VK_RCONTROL: return fn_key_rctrl;
		case VK_LSHIFT: return fn_key_lshift;
		case VK_RSHIFT: return fn_key_rshift;
		case VK_LMENU: return fn_key_lalt;
		case VK_RMENU: return fn_key_ralt;
		case VK_LWIN: return fn_key_lsys;
		case VK_RWIN: return fn_key_rsys;

		default: return fn_key_unknown;
	}

}

static void fn_imp_process_keyboard_input(const RAWKEYBOARD* kbd, uint8_t id) {
		UINT vkey = kbd->VKey;
	UINT sc = kbd->MakeCode;
	UINT flags = kbd->Flags;

	if(vkey == 255)
		return;
	else if(vkey == VK_SHIFT)
		vkey = MapVirtualKeyA(sc, MAPVK_VSC_TO_VK_EX);
	else if(vkey == VK_NUMLOCK)
		sc = MapVirtualKeyA(vkey, MAPVK_VK_TO_VSC) | 0x100;

	const bool is_e0 = (flags & RI_KEY_E0) != 0;
	const bool is_e1 = (flags % RI_KEY_E1) != 0;

	if(is_e1) {
		if(vkey == VK_PAUSE)
			sc = 0x45;
		else 
			sc = MapVirtualKeyA(vkey, MAPVK_VK_TO_VSC);
	}

	switch(vkey) {
		case VK_CONTROL:
			if(is_e0) vkey = VK_RCONTROL;
			else vkey = VK_LCONTROL;
			break;

		case VK_MENU:
			if(is_e0) vkey = VK_LMENU;
			else vkey = VK_RMENU;
			break;

		// TODO: Numpad enter

		case VK_DELETE:
			if(!is_e0) vkey = VK_DECIMAL;
			break;

		case VK_INSERT:
			if(!is_e0) vkey = VK_NUMPAD0;
			break;

		case VK_END:
			if(!is_e0) vkey = VK_NUMPAD1;
			break;

		case VK_DOWN:
			if(!is_e0) vkey = VK_NUMPAD1;
			break;

		case VK_NEXT:
			if(!is_e0) vkey = VK_NUMPAD3;
			break;

		case VK_LEFT:
			if(!is_e0) vkey = VK_NUMPAD4;
			break;

		case VK_CLEAR:
			if(!is_e0) vkey = VK_NUMPAD5;
			break;

		case VK_RIGHT:
			if(!is_e0) vkey = VK_NUMPAD6;
			break;

		case VK_HOME:
			if(!is_e0) vkey = VK_NUMPAD7;
			break;

		case VK_UP:
			if(!is_e0) vkey = VK_NUMPAD8;
			break;

		case VK_PRIOR:
			if(!is_e0) vkey = VK_NUMPAD9;
			break;
	}

	const enum fn_key key = fn_imp_map_key(vkey);
	const bool key_pressed = (flags & RI_KEY_BREAK) == 0;

	g_imp_window_context.key_state[key] = key_pressed;

	struct fn_event ev = {0, };
	ev.type = key_pressed ? 
				fn_event_type_key_pressed
				: fn_event_type_key_released;

	ev.key = key;
	ev.window.id = id;

	fn_imp_push_event(&ev);
}

static void fn_imp_process_mouse_input(const RAWMOUSE* mb, uint8_t id) {
	struct fn_event ev = {0};
	ev.window.id = id;

	enum fn_button state = g_imp_window_context.button_state;

	if(mb->usButtonFlags & RI_MOUSE_LEFT_BUTTON_DOWN)
		state |= fn_button_left;

	if(mb->usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_DOWN)
		state |= fn_button_middle;

	if(mb->usButtonFlags & RI_MOUSE_RIGHT_BUTTON_DOWN)
		state |= fn_button_right;

	if(mb->usButtonFlags & RI_MOUSE_LEFT_BUTTON_UP)
		state &= ~fn_button_left;

	if(mb->usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_UP)
		state &= ~fn_button_middle;

	if(mb->usButtonFlags & RI_MOUSE_RIGHT_BUTTON_UP)
		state &= ~fn_button_right;

	// Isolate all pressed keys from the just released keys.
	// While unlikely two events CAN be registered at the same time,
	// i.e. releasing left button while pressing the right button.
	enum fn_button up = g_imp_window_context.button_state & ~state;
	enum fn_button down = (g_imp_window_context.button_state ^ state) & ~up;
	g_imp_window_context.button_state = state;

	if(up != 0) {
		ev.type = fn_event_type_button_released;
		ev.mouse.button = up;
		ev.mouse.dx = mb->lLastX;
		ev.mouse.dy = mb->lLastY;
		fn_imp_push_event(&ev);
	}

	if(down != 0) {
		ev.type = fn_event_type_button_pressed;
		ev.mouse.button = down;
		ev.mouse.dx = mb->lLastX;
		ev.mouse.dy = mb->lLastY;
		fn_imp_push_event(&ev);
	}

	if(up == 0 && down == 0) {
		ev.type = fn_event_type_mouse_moved;
		ev.mouse.button = g_imp_window_context.button_state;
		ev.mouse.dx = mb->lLastX;
		ev.mouse.dy = mb->lLastY;
		fn_imp_push_event(&ev);
	}

	if(mb->usButtonFlags & RI_MOUSE_WHEEL) {
		ev.type = fn_event_type_mouse_wheel;
		ev.mouse.button = g_imp_window_context.button_state;
		ev.mouse.wheel = mb->usButtonData;
		ev.mouse.dx = mb->lLastX;
		ev.mouse.dy = mb->lLastY;
		fn_imp_push_event(&ev);
	}
}

static void fn_imp_process_input(LPARAM lParam, uint8_t id) {
	RAWINPUT rinput = {0, };
	UINT size = sizeof(RAWINPUT);

	GetRawInputData(
		(HRAWINPUT) lParam, 
		RID_INPUT, 
		&rinput, 
		&size, 
		sizeof(RAWINPUTHEADER)
	);

	if (rinput.header.dwType == RIM_TYPEKEYBOARD) {
		const RAWKEYBOARD* kbd = &rinput.data.keyboard;
		fn_imp_process_keyboard_input(kbd, id);
		return;
	}

	if (rinput.header.dwType == RIM_TYPEMOUSE) {
		const RAWMOUSE* mb = &rinput.data.mouse;
		fn_imp_process_mouse_input(mb, id);
		return;
	}

}

static LRESULT CALLBACK fn_imp_window_proc(
	HWND hWnd, 
	UINT msg, 
	WPARAM wParam, 
	LPARAM lParam) {

	if(msg == WM_NCCREATE) {
		LPCREATESTRUCT cs = (LPCREATESTRUCT) (lParam);
		SetWindowLongPtrA(hWnd, GWLP_USERDATA, (LONG_PTR) cs->lpCreateParams);

		RAWINPUTDEVICE devices[2] = {
			{
				.usUsagePage = 0x01,
				.usUsage = 0x06,
				.dwFlags = 0,
				.hwndTarget = hWnd
			},
			{
				.usUsagePage = 0x01,
				.usUsage = 0x02,
				.dwFlags = 0,
				.hwndTarget = hWnd
			}
		};

		RegisterRawInputDevices(devices, 2, sizeof(RAWINPUTDEVICE));
	}

	const uint8_t id = (uint8_t) GetWindowLongPtr(hWnd, GWLP_USERDATA);
	struct fn_imp_window* inst = &g_imp_window_context.windows[id];
	struct fn_event ev = {0, };

	switch(msg) {
		case WM_DESTROY:
			ev.type = fn_event_type_window_closed;
			ev.window.id = id;
			fn_window_destroy((struct fn_window){ id });
			fn_imp_push_event(&ev);
			return 0;

		case WM_INPUT:
			fn_imp_process_input(lParam, id);
			return 0;

		case WM_SIZE:
			ev.type = fn_event_type_window_resized;
			ev.window.id = id;
			ev.width = LOWORD(lParam);
			ev.height = HIWORD(lParam);
			inst->width = LOWORD(lParam);
			inst->height = HIWORD(lParam);
			fn_imp_push_event(&ev);
			return 0;

		case WM_MOVE:
			ev.type = fn_event_type_window_moved;
			ev.window.id = id;
			ev.x = LOWORD(lParam);
			ev.y = HIWORD(lParam);
			inst->x = LOWORD(lParam);
			inst->y = HIWORD(lParam);
			fn_imp_push_event(&ev);
			return 0;

		case WM_SETFOCUS:
			ev.type = fn_event_type_window_focused;
			ev.window.id = id;
			inst->focused = true;
			fn_imp_push_event(&ev);
			return 0;

		case WM_KILLFOCUS:
			ev.type = fn_event_type_window_unfocused;
			ev.window.id = id;
			inst->focused = false;
			fn_imp_push_event(&ev);
			return 0;

		default:
			return DefWindowProcA(hWnd, msg, wParam, lParam);
	}
}

bool fn_imp_init() {
	WNDCLASSEXA wc = {0, };
	wc.cbSize = sizeof(WNDCLASSEXA);
	wc.hInstance = g_imp_module_instance;
	wc.lpfnWndProc = fn_imp_window_proc;
	wc.lpszClassName = g_imp_window_class_name;

	return RegisterClassExA(&wc);
}

void fn_imp_quit() {
	UnregisterClassA(g_imp_window_class_name, g_imp_module_instance);
}

fn_native_window_handle_t fn_imp_window_create(uint8_t id) {
	struct fn_imp_window* inst = &g_imp_window_context.windows[id];

	fn_native_window_handle_t handle = CreateWindowExA(
		0,
		g_imp_window_class_name,
		"",
		WS_OVERLAPPEDWINDOW,
		0,
		0,
		0,
		0,
		NULL,
		NULL,
		g_imp_module_instance,
		(LPVOID) (intptr_t) (id)
	);

	return handle;
}

void fn_imp_window_destroy(fn_native_window_handle_t handle) {
	DestroyWindow(handle);
}

void fn_imp_window_set_size(fn_native_window_handle_t handle, uint32_t width,
	uint32_t height) {
	RECT client_area = {0, 0, width, height};
	const DWORD style = (DWORD) GetWindowLong(handle, GWL_STYLE);
	const bool has_menu = GetMenu(handle) != NULL;
	AdjustWindowRect(&client_area, style, has_menu);

	SetWindowPos(
		handle,
		NULL,
		0,
		0,
		client_area.right,
		client_area.bottom,
		SWP_NOMOVE | SWP_NOREPOSITION | SWP_NOZORDER
	);
}

void fn_imp_window_set_title(fn_native_window_handle_t handle, 
	const char* title) {
	SetWindowTextA(handle, title);
}

void fn_imp_window_set_visibility(fn_native_window_handle_t handle, 
	bool visible) {
	if(visible)
		ShowWindow(handle, SW_SHOWDEFAULT);
	else
		ShowWindow(handle, SW_HIDE);
}

void fn_imp_pump_events() {
	MSG msg;
	while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

#endif // _WIN32

//------------------------------------------------------------------------------
// The following section implements "platform specific" functions declared
// at the top of the file using AppKit API.
//------------------------------------------------------------------------------

#if defined(__APPLE__)

#import <AppKit/AppKit.h>

static bool g_imp_setup_process = false;

@interface fn_imp_window_delegate: NSObject<NSWindowDelegate>
@property (nonatomic, assign) uint8_t window_id;
-(id) initWithWindowId: (uint8_t) id;
@end

@implementation fn_imp_window_delegate

- (id) initWithWindowId: (uint8_t) id {
	self = [super init];
	self.window_id = id;
	return self;
}

- (void) windowWillClose: (NSNotification*) notification {
	struct fn_event ev = {0};
	ev.type = fn_event_type_window_closed;
	ev.window.id = self.window_id;
	fn_imp_push_event(&ev);
	fn_window_destroy((struct fn_window) { .id = self.window_id} );
}

- (void) windowDidResize: (NSNotification*) notification {
	struct fn_event ev = {0};
	ev.type = fn_event_type_window_resized;
	ev.window.id = self.window_id;

	NSWindow* wind = [notification object];
	NSSize client_area = [[wind contentView] frame].size;
	ev.width = client_area.width;
	ev.height = client_area.height;
	fn_imp_push_event(&ev);

	struct fn_imp_window* window = 
		&g_imp_window_context.windows[self.window_id];

	window->width = client_area.width;
	window->height = client_area.height;
}

- (void) windowDidMove: (NSNotification*) notification {
	struct fn_event ev = {0};
	ev.type = fn_event_type_window_moved;
	ev.window.id = self.window_id;

	NSWindow* wind = [notification object];
	NSPoint client_pos = [[wind contentView] frame].origin;
	client_pos = [wind convertRectToScreen: (NSRect) {.origin = client_pos}]
		.origin;

	ev.x = client_pos.x;
	ev.y = client_pos.y;

	struct fn_imp_window* window =
		&g_imp_window_context.windows[self.window_id];

	window->x = client_pos.x;
	window->y = client_pos.y;
}

- (void) windowDidBecomeKey: (NSNotification*) notification {
	struct fn_event ev = {0};
	ev.type = fn_event_type_window_focused;
	ev.window.id = self.window_id;

	fn_imp_push_event(&ev);

	struct fn_imp_window* window = 
		&g_imp_window_context.windows[self.window_id];

	window->focused = true;
}

- (void) windowDidResignKey: (NSNotification*) notification {
	struct fn_event ev = {0};
	ev.type = fn_event_type_window_unfocused;
	ev.window.id = self.window_id;

	fn_imp_push_event(&ev);

	struct fn_imp_window* window = 
		&g_imp_window_context.windows[self.window_id];

	window->focused = false;
}

@end

bool fn_imp_init() {
	if(!g_imp_setup_process) {
		[NSApplication sharedApplication];

		[NSApp setActivationPolicy: NSApplicationActivationPolicyRegular];
		[NSApp activateIgnoringOtherApps: YES];
		[NSApp finishLaunching];

		g_imp_setup_process = true;
	}

	return true;
}

void fn_imp_quit()
{}

fn_native_window_handle_t fn_imp_window_create(uint8_t id) {
	NSRect frame = NSMakeRect(0, 0, 1, 1);
	const NSUInteger styleMask =
		NSWindowStyleMaskTitled
		| NSWindowStyleMaskClosable
		| NSWindowStyleMaskResizable;	

	fn_native_window_handle_t handle = [NSWindow alloc];
	[handle initWithContentRect: frame
					  styleMask: styleMask
					    backing: NSBackingStoreBuffered
					      defer: NO];

	fn_imp_window_delegate* delegate = [[fn_imp_window_delegate alloc]
		initWithWindowId: id];

	[handle setDelegate: delegate];

	return handle;
}

void fn_imp_window_destroy(fn_native_window_handle_t handle) {
	[handle release];
}

void fn_imp_window_set_size(fn_native_window_handle_t handle, uint32_t width,
	uint32_t height) {

	NSRect frame = NSMakeRect(0, 0, width, height);
	frame = [NSWindow frameRectForContentRect: frame
									styleMask: handle.styleMask];

	[handle setFrame: frame
			 display: YES];
}

void fn_imp_window_set_title(fn_native_window_handle_t handle, 
	const char* title) {
	NSString* str = [NSString stringWithUTF8String: title];
	[handle setTitle: str];
}

void fn_imp_window_set_visibility(fn_native_window_handle_t handle, bool visible) {
	if(visible) 
		[handle orderFront: nil];
	else
		[handle orderOut: nil];
}

void fn_imp_pump_events() {
	NSEvent* ev = nil;

	while((ev = [NSApp nextEventMatchingMask: NSEventMaskAny
								   untilDate: nil
								  	  inMode: NSDefaultRunLoopMode
								  	 dequeue: YES])) {
		[NSApp sendEvent: ev];
	}

}

#endif // __APPLE__
