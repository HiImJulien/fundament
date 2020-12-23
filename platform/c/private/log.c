#include <fundament/log.h>

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define fn_black	"\033[30m"  	
#define fn_red 		"\033[31m"  	
#define fn_green	"\033[32m"  	
#define fn_yellow	"\033[33m"   
#define fn_blue	 	"\033[34m"  
#define fn_magenta	"\033[35m"   
#define fn_cyan	 	"\033[36m"
#define fn_white	"\033[37m"

#define fn_bred		"\033[1;31m"
#define fn_bwhite	"\033[1;37m"

//
// A custom implementation of printf.
//
// This function should be used instead of stdc's printf,
// since it allows for setting the color of the output.
//
static void fn__printf(const char* msg);

static void fn__stdout_callback(
	enum fn_log_severity, 
	const char* file, 
	int line, 
	const char* topic, 
	const char* message
);

static void fn__file_callback(
	enum fn_log_severity severity, 
	const char* file, 
	int line, 
	const char* topic, 
	const char* message
);

#define fn__max_files 8
#define fn__max_callbacks 34

static enum fn_log_severity fn__g_min_severity = fn_log_severity_debug;
static FILE* fn__g_files[fn__max_files] = {NULL, };
static fn_log_callback_t fn__g_log_callbacks[fn__max_callbacks] = {
	fn__stdout_callback,
	fn__file_callback,
	NULL,
};

#if defined(_WIN32)
	
	#include <Windows.h>

	struct fn_vt_sequence_mapping {
		const char* seq;
		size_t seq_len;
		DWORD attr;
	};

	static const struct fn_vt_sequence_mapping mapping[] = {
		{fn_black, 5, 0},													// BLACK
		{fn_red, 5, FOREGROUND_RED},										// RED
		{fn_green, 5, FOREGROUND_GREEN},									// GREEN
		{fn_yellow, 5, FOREGROUND_RED | FOREGROUND_GREEN},					// YELLOW
		{fn_blue, 5, FOREGROUND_BLUE},										// BLUE
		{fn_magenta, 5, FOREGROUND_RED | FOREGROUND_BLUE},					// MAGENTA
		{fn_cyan, 5, FOREGROUND_GREEN | FOREGROUND_BLUE},					// CYAN
		{fn_white, 5, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE},	// WHITE
		{fn_bred, 7, FOREGROUND_RED | FOREGROUND_INTENSITY },				// BOLD RED
		{fn_bwhite, 7, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY}
	};

	//
	// Implementation for Win32; simulate colors using 
	// the WinAPI.
	//
	void fn__printf(const char* msg) {
		static HANDLE std_out;
		if(!std_out)
			std_out = GetStdHandle(STD_OUTPUT_HANDLE);

		CONSOLE_SCREEN_BUFFER_INFO settings_stash;
		GetConsoleScreenBufferInfo(std_out, &settings_stash);

		for(const char* cursor = msg; *cursor != '\0'; ++cursor) {
			if(*cursor == '\033') {
				const char* seq_end = cursor;
				for(seq_end; *seq_end != 'm'; ++seq_end)
					;

				const size_t len = seq_end - cursor + 1;
				for(size_t it = 0; it < 10; ++it) {

					const struct fn_vt_sequence_mapping* m = &mapping[it];
					if(len != m->seq_len)
						continue;

					if(memcmp(cursor, m->seq, len) != 0)
						continue;

					SetConsoleTextAttribute(std_out, m->attr);
				}

				cursor = seq_end;
				continue;
			}

			WriteConsole(std_out, cursor, 1, NULL, NULL);
		}

		SetConsoleTextAttribute(std_out, settings_stash.wAttributes);
	}

#elif defined(__linux__) || (__APPLE__)

	#include <unistd.h>

	void fn__printf(const char* msg) {
		write(STDOUT_FILENO, msg, strlen(msg));	
	} 

#else

	//
	// Default implementation; assume the colors are supported.
	// 
	void fn__printf(const char* msg) {
		printf(msg);
	}

#endif 

static void fn__stdout_callback(
	enum fn_log_severity severity, 
	const char* file, 
	int line, 
	const char* topic, 
	const char* message
) {
	const char* dbg_fmt = fn_cyan "DBG  | " fn_white "%s :: " fn_bwhite " %s \n";
	const char* info_fmt = fn_green "INFO | " fn_white "%s :: " fn_bwhite " %s \n";
	const char* warn_fmt = fn_yellow "WARN | " fn_white "%s :: " fn_bwhite " %s \n";
	const char* error_fmt = fn_red "ERR  | " fn_white "%s :: " fn_bwhite " %s \n";
	const char* fatal_fmt = fn_bred "FAT  | " fn_white "%s :: " fn_bwhite " %s \n";
	const char* msg_fmt = NULL;

	if(severity == fn_log_severity_debug)
		msg_fmt = dbg_fmt;
	else if(severity == fn_log_severity_info)
		msg_fmt = info_fmt;
	else if(severity == fn_log_severity_warn)
		msg_fmt = warn_fmt;
	else if(severity == fn_log_severity_error)
		msg_fmt = error_fmt;
	else if(severity == fn_log_severity_fatal)
		msg_fmt = fatal_fmt;

	if(!msg_fmt)
		return;

	const char* msg = NULL;
	const size_t msg_size = snprintf(NULL, 0, msg_fmt, topic, message) + 1;

	msg = malloc(msg_size);
	snprintf((char*) msg, msg_size, msg_fmt, topic, message);

	fn__printf(msg);

	free((char*) msg);
}

static void fn__file_callback(
	enum fn_log_severity severity, 
	const char* file, 
	int line,
	const char* topic, 
	const char* message
) {
	const char* dbg_fmt = "DBG  | %s:%d :: %s :: %s\n";
	const char* info_fmt = "INFO | %s:%d :: %s :: %s\n";
	const char* warn_fmt = "WARN | %s:%d :: %s :: %s\n";
	const char* error_fmt = "ERR  | %s:%d :: %s :: %s\n";
	const char* fatal_fmt = "FAT  | %s:%d :: %s :: %s\n";
	const char* msg_fmt = NULL;

	if(severity == fn_log_severity_debug)
		msg_fmt = dbg_fmt;
	else if(severity == fn_log_severity_info)
		msg_fmt = info_fmt;
	else if(severity == fn_log_severity_warn)
		msg_fmt = warn_fmt;
	else if(severity == fn_log_severity_error)
		msg_fmt = error_fmt;
	else if(severity == fn_log_severity_fatal)
		msg_fmt = fatal_fmt;

	if(!msg_fmt)
		return;

	const char* msg = NULL;
	const size_t msg_size = snprintf(NULL, 0, msg_fmt, file, line, topic, message) + 1;

	msg = malloc(msg_size);
	snprintf((char*) msg, msg_size, msg_fmt, file, line, topic, message);

	for(uint8_t it = 0; it < 8; ++it)
		if(fn__g_files[it] != NULL)
			fputs(msg, fn__g_files[it]);

	free((char*) msg);
}

void fn_log_set_min_severity(enum fn_log_severity severity) {
	fn__g_min_severity = severity;
}

void fn_log_set_quiet(bool quiet) {
	fn__g_log_callbacks[0] = quiet ? NULL : fn__stdout_callback;
}

bool fn_log_add_callback(fn_log_callback_t cb) {
	for(uint8_t it = 2; it < 32 + 2; ++it) {
		if(fn__g_log_callbacks[it] == NULL) {
			fn__g_log_callbacks[it] = cb;
			return true;
		}
	}

	return false;
}

void fn_log_remove_callback(fn_log_callback_t cb) {
	for(uint8_t it = 2; it < 32 + 2; ++it) {
		if(fn__g_log_callbacks[it] == cb)
			fn__g_log_callbacks[it] = NULL;
	}
}

bool fn_log_add_file(FILE* f) {
	for(uint8_t it = 0; it < 8; ++it) {
		if(fn__g_files[it] == NULL) {
			fn__g_files[it] = f;
			return true;
		}
	}

	return false;
}

void fn_log_remove_file(FILE* f) {
	for(uint8_t it = 0; it < fn__max_files; ++it)
		if(fn__g_files[it] == f)
			fn__g_files[it] = NULL;
}

void fn_log(
	enum fn_log_severity severity, 
	const char* file, 
	int line,
	const char* topic, 
	const char* fmt, 
	...
) {
	if(severity < fn__g_min_severity)
		return;

	va_list args;
	va_start(args, fmt);
	const size_t message_size = snprintf(NULL, 0, fmt, args) + 1;
	const char* message = malloc(message_size);
	snprintf((char*) message, message_size, fmt, args);
	va_end(args);

	for(uint8_t it = 0; it < fn__max_callbacks; ++it)
		if(fn__g_log_callbacks[it])
			fn__g_log_callbacks[it](severity, file, line, topic, message);

	free((char*) message);
}
