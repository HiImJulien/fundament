#ifndef FUNDAMENT_INPUT_WIN32_H
#define FUNDAMENT_INPUT_WIN32_H

//==============================================================================
// This module implements the internal functions used to process user input for
// the Win32 system.
//==============================================================================

#include "../input_common.h"

#include <Windows.h>

//
// Processes Win32 events such as 'WM_KEYDOWN', 'WM_SYSKEYDOWN' and their
// counterparts.
//
void fn__imp_process_keyboard_input(UINT msg, WPARAM wParam, LPARAM lParam);

//
// Processes Win32 events such 'WM_LBUTTONDOWN', 'WM_RBUTTONDOWN',
// 'WM_MBUTTONDOWN' and their counterparts.
//
void fn__imp_process_mouse_input(UINT msg, LPARAM lParam);

//
// Processes the Win32 event
//
void fn__imp_process_mouse_move(LPARAM lParam);

//
// Translates 'WM_KEYDOWN' and 'WM_KEYUP' events to
//
//
char fn__imp_translate_to_char(WPARAM wParam, LPARAM lParam);

#endif  // FUNDAMENT_INPUT_WIN32_H
