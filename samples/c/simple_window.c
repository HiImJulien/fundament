#include <fundament/window.h>
#include <fundament/event.h>

#include <stdio.h>

int main() {
    fn_init_window_module();
    struct fn_window win = fn_create_window();

    fn_window_set_height(win, 600);
    fn_window_set_width(win, 800);
    fn_window_set_title(win, "Hello, fundament!");
    fn_window_set_visibility(win, true);

    printf("Created and configured window.\n");

    struct fn_event ev = {0, };
    while(ev.type != fn_event_type_closed)
        fn_poll_events(&ev);

    fn_deinit_window_module();
    return 0;
}