#ifndef FUNDAMENT_INPUT_XCB_H
#define FUNDAMENT_INPUT_XCB_H

//=============================================================================
// This module implements the internal functions used to process user input for 
// the X11 API.
//=============================================================================

#include "input_common.h"

#include <stdint.h>

#include <X11/Xlib.h>
#include <linux/input-event-codes.h>

//
// Processes X11/Xinput key press/release events.
//
void fn__imp_process_keyboard_input(uint32_t keycode, bool pressed);

//
// Maps the X11/XInput virtual keycodes to a fundament key code.
// Note, that I have no real idea how this mapping works. However,
// it looks like these values are offset by 8 when comparing to 
// 'linux/input-event-codes.h'.
//
enum fn_key fn__imp_map_virtual_key(uint32_t keycode);

//
// Translates a keycode to a localized character representing the key.
//
char fn__imp_translate_key(Display* dpy, uint32_t keycode);

#endif  // FUNDAMENT_INPUT_XCB_H
