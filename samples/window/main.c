//==============================================================================
//                               WINDOW SAMPLE
//
// This sample shows you have to create and manage windows using the fn_window 
// module.
//==============================================================================

#include <fundament/window.h>
#include <fundament/event.h>

#include <stdlib.h>
#include <stdio.h>

int main(int argc, char** argv) {
    fn_init_window();
    printf("Initialized module.\n");

    struct fn_window window = fn_create_window();
    if(window.id == 0)
        printf("Failed to create window.\n");

    fn_window_set_size(window, 800, 600);
    fn_window_set_title(window, "fundament: Window Sample");
    fn_window_set_visible(window, true);
    printf("Created window.\n");

    struct fn_event ev = {0, };
    while(ev.type != fn_event_type_closed)
        fn_poll_events(&ev);

    fn_deinit_window();
    return EXIT_SUCCESS;
}

