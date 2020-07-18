#include <fundament/window.h>
#include <fundament/event.h>

int main() {
    fn_init_window_module();

    struct fn_window win = fn_create_window();
    struct fn_event ev = {0, };

    while(ev.type != fn_event_type_closed) {
        fn_poll_events(&ev);
    }

    fn_deinit_window_module();
    return 0;
}
