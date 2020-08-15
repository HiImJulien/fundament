#ifndef FUNDAMENT_LOG_H
#define FUNDAMENT_LOG_H

//==============================================================================
// This module provides logging functionality.
//==============================================================================

#include <fundament/api.h>

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>

//
// Enumerates all supported log severities.
//
enum fn_log_severity {
	fn_log_severity_debug,
	fn_log_severity_info,
	fn_log_severity_warn,
	fn_log_severity_error,
	fn_log_severity_fatal
};

//
// Sets the minimum severity.
//
// Causes logs with a severity lower than specified to be ignored.
//
API void fn_log_set_min_severity(
	enum fn_log_severity severity
);

//
// Enables/Disables output to stdout.
//
API void fn_log_set_quiet(bool queit);

//
// Typedef for log callbacks.
//
// Parameters:
//   1 -> Severity
//	 2 -> File
//	 3 -> Line
// 	 4 -> Topic
//   5 -> Message
//
typedef void(*fn_log_callback_t)(enum fn_log_severity, const char*, int, const char*, const char*);

//
// Adds an callback, which is called for every log.
//
// Up to 16 callbacks are supported.
//
// Returns true, if a callback could be added. If no callback slot is available,
// returns false.
//
API bool fn_log_add_callback(fn_log_callback_t cb);

//
// Removes an callback.
//
API void fn_log_remove_callback(fn_log_callback_t cb);

//
// Adds a filestream to output logs to.
//
// Up to 8 files are supported.
//
// Returns true, if a stream could be added. If no stream slot is available,
// returns false.  
//
API bool fn_log_add_file(FILE* f);

//
// Removes a filestream.
//
API void fn_log_remove_file(FILE* f);

#define fn_dbg(...) fn_log(fn_log_severity_debug, __FILE__, __LINE__, __VA_ARGS__)
#define fn_info(...) fn_log(fn_log_severity_info, __FILE__, __LINE__, __VA_ARGS__)
#define fn_warn(...) fn_log(fn_log_severity_warn, __FILE__, __LINE__, __VA_ARGS__)
#define fn_err(...) fn_log(fn_log_severity_error, __FILE__, __LINE__, __VA_ARGS__)
#define fn_fatal(...) fn_log(fn_log_severity_fatal, __FILE__, __LINE__, __VA_ARGS__)

//
// Underlying function used to generate the logs.
//
// Note:
// - Do not use this directly! Instead use the macros.
//
API void fn_log(
	enum fn_log_severity severity, 
	const char* file, 
	int line,
	const char* topic, 
	const char* fmt, 
	...
);

#endif 	// FUNDAMENT_LOG_H
