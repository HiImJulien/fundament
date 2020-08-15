#include <fundament/log.h>

#include <stdio.h>
#include <string.h>

static const char* levels[] = {
	"INFO",
	"WARN"
};

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

#if !defined(_WIN32)

	//
	// Default implementation; assume the colors are supported.
	// 
	void fn__printf(const char* msg) {
		printf(msg);
	}

#else

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

#endif 

void fn_log(
	enum fn_log_severity severity, 
	const char* file, 
	int line,
	const char* topic, 
	const char* fmt, 
	...
) {
	const char* sub_msg = NULL;

	{
		va_list args;
		va_start(args, fmt);
		const size_t msg_size = snprintf(NULL, 0, fmt, args) + 1;
		sub_msg = malloc(msg_size);
		snprintf((char*) sub_msg, msg_size, fmt, args);
		va_end(args);
	}

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
	const size_t msg_size = snprintf(NULL, 0, msg_fmt, topic, sub_msg) + 1;

	msg = malloc(msg_size);
	snprintf((char*) msg, msg_size, msg_fmt, topic, sub_msg);

	fn__printf(msg);

	free((char*) msg);
	free((char*) sub_msg);
}