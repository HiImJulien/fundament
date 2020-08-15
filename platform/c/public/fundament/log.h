#ifndef FUNDAMENT_LOG_H
#define FUNDAMENT_LOG_H

//==============================================================================
// This module provides logging functionality.
//==============================================================================

#include <fundament/api.h>

#include <stdarg.h>

//
// Enumerates all supported log severeties.
//
enum fn_log_severity {
	fn_log_severity_info,
	fn_log_severity_warn,
};

API void fn_log(
	enum fn_log_severity severity, 
	const char* file, 
	int line,
	const char* topic, 
	const char* fmt, 
	...
);

#define fn_info(...) fn_log(fn_log_severity_info, __FILE__, __LINE__, __VA_ARGS__)
#define fn_warn(...) fn_log(fn_log_severity_warn, __FILE__, __LINE__, __VA_ARGS__)

#endif 	// FUNDAMENT_LOG_H
