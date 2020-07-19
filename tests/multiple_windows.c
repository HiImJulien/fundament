#include <fundament/window.h>
#include <fundament/event.h>

#include <inttypes.h>
#include <stdio.h>

int main() {
    fn_init_window_module();

    struct fn_window w1 = fn_create_window();
    struct fn_window w2 = fn_create_window();
    struct fn_window w3 = fn_create_window();

    struct fn_window windows[] = {
        w1,
        w2,
        w3
    };

    for(size_t it = 0; it < 3; ++it) {
        fn_window_set_height(windows[it], 400);
        fn_window_set_width(windows[it], 400);

        char buf[1024];
        snprintf(buf, 1024, "Window %zu", it+1);
        fn_window_set_title(windows[it], buf);
        fn_window_set_visibility(windows[it], true); 
    }

    struct fn_event ev = {0, };
    uint8_t open_windows = 3;    

    while(open_windows > 0) {
        if(ev.type == fn_event_type_closed) {
            open_windows--;
            printf("Closed window.\n");
        }

        if(ev.type == fn_event_type_resized) {
            printf("Resized window %"PRIu32 " : (%"PRIu32 ", %"PRIu32")\n",
                ev.window.id,
                ev.size.width,
                ev.size.height
            );
        } 

        if(ev.type == fn_event_type_focus_gained)
            printf("Focused window %"PRIu32"\n", ev.window.id);

        if(ev.type == fn_event_type_focus_lost)
            printf("Unfocused window %"PRIu32"\n", ev.window.id);

        if(ev.type == fn_event_type_key_pressed)
            printf("Pressed key (w: %"PRIu32"): %c\n", 
                    ev.window.id,
                    ev.key.letter
            );
            
        if(ev.type == fn_event_type_key_released)
            printf("Released key (w: %"PRIu32"): %c\n", 
                    ev.window.id,
                    ev.key.letter
            );

        if(ev.type == fn_event_type_button_pressed)
            printf("Pressed button with window (w: %"PRIu32").\n", ev.window.id); 

        if(ev.type == fn_event_type_button_released)
            printf("Released button with window (w: %"PRIu32").\n", ev.window.id); 

        if(ev.type == fn_event_type_mouse_moved)
            printf("Moved mouse (w: %"PRIu32"): (%"PRId32 ", %"PRId32")\n",
                ev.window.id,
                ev.mouse_move.x,
                ev.mouse_move.y
            );

        if(ev.type == fn_event_type_mouse_wheel)
            printf("Moved wheel (w: %"PRIu32"): (%"PRId32 ", %"PRId32", %"PRId32")\n",
                ev.window.id,
                ev.mouse_wheel.x,
                ev.mouse_wheel.y,
                ev.mouse_wheel.dt
            );

        fn_poll_events(&ev);
    }

    fn_deinit_window_module();

    return 0;
}
