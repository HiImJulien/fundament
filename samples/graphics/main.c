#include <fundament/window.h>
#include <fundament/event.h>
#include <fundament/graphics.h>

#include <stdlib.h>
#include <stdio.h>

int main(int argc, char** argv) {
    fn_init_window();
    fn_init_graphics();

    struct fn_window window = fn_create_window();
    fn_window_set_size(window, 800, 600);
    fn_window_set_title(window, "Graphics Sample");
    fn_window_set_visible(window, true);

    struct fn_surface surface = fn_create_surface(fn_window_handle(window));
    if(surface.id == 0) {
        printf("Failed to create window surface!\n");
        goto quit;
    }

    struct fn_event ev = {0};
    while(ev.type != fn_event_type_closed) {
        fn_poll_events(&ev);
    }

quit:
    fn_deinit_graphics();
    fn_deinit_window();
    return EXIT_SUCCESS;
}