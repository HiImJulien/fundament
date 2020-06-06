#ifndef FUNDAMENT_INPUT_APPKIT_H
#define FUNDAMENT_INPUT_APPKIT_H

//==============================================================================
// This module implements the internal functions used to process user input for
// the AppKit platform.
//==============================================================================

#include "input_common.h"

#include <AppKit/AppKit.h>

//
// Processes AppKit's NSEvents.
//
void fn__imp_process_keyboard_input(NSEvent* ev);

//
//
//
enum fn_key fn__imp_map_virtual_key(unsigned short keyCode);

//
//
//
void fn__imp_process_mouse_input(NSEvent* ev);

#endif  // FUNDAMENT_INPUT_APPKIT_H