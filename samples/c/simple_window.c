#include <fundament/event.h>
#include <fundament/window.h>

#include <stdio.h>

int main() {
    fn_init_window_module();
    struct fn_window win= fn_create_window();

    fn_window_set_height(win, 600);
    fn_window_set_width(win, 800);
    fn_window_set_title(win, "Hello, fundament!");
    fn_window_set_visibility(win, true);

    struct fn_event ev = {0, };
    while(ev.type != fn_event_type_closed) {
        if(ev.type == fn_event_type_resized)
            printf("Resized window to {%ud, %ud}\n", ev.size.width, ev.size.height);  

        fn_poll_events(&ev);
    }

    fn_deinit_window_module();
    return 0;
}
