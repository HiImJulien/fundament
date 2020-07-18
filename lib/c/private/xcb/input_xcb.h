#ifndef FUNDAMENT_INPUT_XCB_H
#define FUNDAMENT_INPUT_XCB_H

//=============================================================================
// This module implements the internal functions used to process user input for
// the X11 API.
//=============================================================================

#include "../input_common.h"

#include <stdint.h>

#include <X11/Xlib.h>

//
// Translates a keycode to a localized character representing the key.
//
char fn__imp_translate_key(Display* dpy, uint32_t keycode);

#endif  // FUNDAMENT_INPUT_XCB_H
