#include <fundament/log.h>

#include <stdio.h>

int main() {
	FILE* log = fopen("./log.txt", "w");
	fn_log_add_file(log);

	fn_dbg("playground.log_test", "debug message!");
	fn_info("playground.log_test", "info message!");
	fn_warn("playground.log_test", "warn message!");
	fn_err("playground.log_test", "err message!");
	fn_fatal("playground.log_test", "fatal message!");

	fclose(log);

	char t;
	scanf("%c", &t);
	return 0;
}