#ifndef FUNDAMENT_H
#define FUNDAMENT_H

#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Initializes the libraries internal state.
 * @return Returns true on success; otherwise false.
 */
bool fn_init(void);

/**
 * @brief Frees the internal state effectively shutting
 * down the library.
 */
void fn_quit(void);

/**
 * @struct Represents a handle to a window.
 * @note Up to 255 windows are supported.
 * The id 255 is used to mark an handle invalid.
 */
struct fn_window { uint8_t id; };

/**
 * @brief Creates a new window.
 * The window is initially invisible,
 * an empty title and is sized down to (0, 0).
 * @return If the creation succeeds, the return value is 
 * a valid handle to window. Otherwise the return value is an 
 * invalid handle.
 */
struct fn_window fn_window_create();

/**
 * @brief Destroys a window.
 * @param window The window to destroy.
 * @detail Does nothing, if the handle passed is invalid.
 */
void fn_window_destroy(struct fn_window window);

/**
 * @brief Sets the *CLIENT* size a window.
 * @param window The window, whose size needs to be changed.
 * @param width The new width to apply.
 * @param height The new height to apply.
 */
void fn_window_set_size(struct fn_window window, uint32_t width, 
	uint32_t height);

/**
 * @brief Retrieves the current client width of the window.
 * @param window The window to retrieve the width for.
 * @return If the handle is valid, the return value is the current
 * client width of the window. Otherwise the return value is zero.
 */
uint32_t fn_window_width(struct fn_window window);

/**
 * @brief Retrieves the current client height of the window.
 * @param window The window to retrieve the height for.
 * @return If the handle is valid, the return value is the current
 * client height of the window. Otherwise the return value is zero.
 */
uint32_t fn_window_height(struct fn_window window);

/**
 * @brief Sets the title of a window.
 * @param window The window, whose title needs to be changed.
 * @param title The title to set.
 */
void fn_window_set_title(struct fn_window window, const char* title);

/**
 * @brief Returns the current title of a window.
 * @param window The window to retrieve the title for.
 * @return If the handle is valid, the return value is the current
 * title of the window. Otherwise the return value is NULL.
 */
const char* fn_window_title(struct fn_window windw);

/**
 * @brief Sets the visibility of a window, effectively 
 * show or hiding it.
 * @param window The window, whose visiblity needs to be changed.
 * @param visible The visiblity of the window.
 */
void fn_window_set_visibility(struct fn_window window, bool visible);

/**
 * @brief Retrieves the visibility of a window.
 * @param window The window, to retrieve the visiblity for.
 * @return If the window is set to be visible and exists,
 * the return value is true. Otherwise return value is false.
 */
bool fn_window_visible(struct fn_window window);

/**
 * @brief Retrieves the current focus state of a window.
 * @param window The window, to retrieve the state for.
 * @return If the window is focused returns true.
 * Otherwise the return value is false.
 */
bool fn_window_focused(struct fn_window window);

/**
 * @brief Checks whether the window exists in the underlying
 * operating system.
 * @param window The window, to retrieve the state for.
 * @return If the window is open (i.e. exist, can be hidden though),
 * the return value is true. Otherwise the return value is false.
 */
bool fn_window_open(struct fn_window window);

#if defined(_WIN32)
	typedef struct HWND__* fn_native_window_handle_t;
#else
	#error "Attempting to build for unknown target!"
#endif

/**
 * @enum Enumerates physical key codes.
 */
enum fn_key {
	fn_key_a,
	fn_key_b, fn_key_c, fn_key_d, fn_key_e, fn_key_f, 
	fn_key_g, fn_key_h, fn_key_i, fn_key_j, fn_key_k,
	fn_key_l, fn_key_m, fn_key_n, fn_key_o, fn_key_p,
	fn_key_q, fn_key_r, fn_key_s, fn_key_t, fn_key_u, 
	fn_key_v, fn_key_w, fn_key_x, fn_key_y, fn_key_z,

	fn_key_1, fn_key_2, fn_key_3, fn_key_4, fn_key_5,
	fn_key_6, fn_key_7, fn_key_8, fn_key_9, fn_key_0,

	fn_key_enter, fn_key_esc, fn_key_backspace, fn_key_tab, fn_key_space,
	fn_key_hyphen, fn_key_eq, fn_key_lbrace, fn_key_rbrace, fn_key_backslash,
	// None US? # and ~

	fn_key_semicolon, fn_key_quote, fn_key_grave,
	fn_key_comma, fn_key_dot, fn_key_slash, fn_key_caps,

	fn_key_f1, fn_key_f2, fn_key_f3, fn_key_f4, fn_key_f5,
	fn_key_f6, fn_key_f7, fn_key_f8, fn_key_f9, fn_key_f10,
	fn_key_f11, fn_key_f12,

	fn_key_print, fn_key_scroll, fn_key_pause, fn_key_ins, fn_key_del,
	fn_key_end, fn_key_page_up, fn_key_page_down, 

	fn_key_left, fn_key_right, fn_key_up, fn_key_down,

	fn_key_lctrl, fn_key_rctrl, fn_key_lshift, fn_key_rshift, 
	fn_key_lalt, fn_key_ralt, fn_key_lsys, fn_key_rsys,

	fn_key_unknown, fn_key_count
};

/**
 * @brief Determines whether given key is pressed or not.
 */
bool fn_key_pressed(enum fn_key key);

/**
 * @enum Enumerates mouse button codes. 
 */
enum fn_button {
	fn_button_left = (1u << 0),
	fn_button_middle = (1u << 1),
	fn_button_right = (1u << 2),
};

/**
 * @enum Enumerates all recognized event types.
 */
enum fn_event_type {
	fn_event_type_none,
	fn_event_type_window_closed,
	fn_event_type_window_resized,
	fn_event_type_window_moved,
	fn_event_type_window_focused,
	fn_event_type_window_unfocused,
    fn_event_type_key_pressed,
    fn_event_type_key_released,
    fn_event_type_button_pressed,
    fn_event_type_button_released,
    fn_event_type_mouse_moved,
    fn_event_type_mouse_wheel
};

/**
 * @struct Represents an event issued by the underlying
 * operating system.
 */
struct fn_event {
	enum fn_event_type type;
	struct fn_window window;

	union {
		struct {
			uint32_t width;
			uint32_t height;
		};

		struct {
			int32_t x;
			int32_t y;
		};

		struct {
			int32_t dx;
			int32_t dy;
			int32_t wheel;
        	enum fn_button button;
		} mouse;

		enum fn_key key;
	};
};

/**
 * @brief Polls the latest event from the internal
 * event queue.
 * @param ev A pointer to an instance of 'struct fn_event'
 * to store the polled event in.
 * @detail If no event was present in the event queue,
 * 'ev->type' will be set to 'fn_event_type_none'.
 */
void fn_poll_event(struct fn_event* ev);

#endif 	// FUNDAMENT_H