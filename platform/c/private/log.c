#include <fundament/log.h>

#include <stdio.h>
#include <string.h>

static const char* levels[] = {
	"INFO",
	"WARN"
};

static const char* levels_colours[] = {
	"\033[7;",	// FG_WHITE
	"\033[33;"	// FG_YELLOW
};



//
// A custom implementation of printf.
//
// This function should be used instead of stdc's printf,
// since it allows for setting the color of the output.
//
static void fn__printf(const char* fmt, ...);

#if !defined(_WIN32)

	//
	// Default implementation; assume the colors are supported.
	// 
	void fn__printf(const char* fmt, ...) {
		va_list args;
		va_start(args, fmt);
		printf(fmt, args);
		va_end(args);
	}

#else

	#include <Windows.h>

	//
	// Implementation for Win32; simulate colors using 
	// the WinAPI.
	//
	void fn__printf(const char* fmt, ...) {
		static HANDLE std_out;
		if(!std_out)
			std_out = GetStdHandle(STD_OUTPUT_HANDLE);

		CONSOLE_SCREEN_BUFFER_INFO settings_stash;
		// settings_stash.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFO);
		GetConsoleScreenBufferInfo(std_out, &settings_stash);

		va_list args;
		va_start(args, fmt);

		for(const char* cursor = fmt; *cursor != '\0'; ++cursor) {
			// Search for an escape sequence.
			if(*cursor == '\033') {
				const char* seq_end = cursor;
				for(seq_end; *seq_end != ';'; ++seq_end)
					;

				if(memcmp(cursor, "\033[0", strlen("\033[0")) == 0)
					SetConsoleTextAttribute(std_out, settings_stash.wAttributes);

				if(memcmp(cursor, levels_colours[1], strlen(levels_colours[1])) == 0)
					SetConsoleTextAttribute(std_out, FOREGROUND_BLUE | FOREGROUND_GREEN);

				cursor = seq_end;
				continue;
			}

			if(*cursor != '%') {
				WriteConsole(std_out, cursor, 1, NULL, NULL);
				continue;
			}

			if(*cursor == '%' && *(cursor + 1) == '\0')
				break;

			if(*cursor == '%' && (*(cursor+1) == 'd' || *(cursor+1) == 'i')) {
				const int dec = va_arg(args, int);
				size_t buf_size = snprintf(NULL, 0, "%d", dec);
				const char* buf = malloc(buf_size);
				snprintf((char*) buf, buf_size, "%d", dec);

				WriteConsole(std_out, buf, buf_size, NULL, NULL);
				free((void*) buf);

				cursor += 2;
			}



		}

		va_end(args);

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
	if(severity == fn_log_severity_warn)
	fn__printf("\033[33;\nWARN \033[0;%d", line);
}