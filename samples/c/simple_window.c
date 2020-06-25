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

    printf("Created and configured window.\n");

    struct fn_event ev= {
        0,
    };
    while(ev.type != fn_event_type_closed) {
        fn_poll_events(&ev);

        if(ev.type == fn_event_type_key_pressed && ev.key == fn_key_escape)
            break;
        else if(ev.type == fn_event_type_key_pressed)
            printf("Character: %c\n", ev.localized_key);

        if(ev.type == fn_event_type_button_pressed
           && ev.button == fn_button_left) {
            printf("X: %d, Y: %d\n", ev.x, ev.y);
            printf("Pressed left button!\n");
        }

        if(ev.type == fn_event_type_mouse_wheel) {
            printf("delta: %d\n", ev.mouse_wheel);
        }

        if(ev.type == fn_event_type_key_pressed && ev.key == fn_key_pad_enter)
            printf("Keypad enter!\n");

        if(ev.type == fn_event_type_key_pressed && ev.key == fn_key_pad_add)
            printf("Keypad add!\n");

        if(ev.type == fn_event_type_key_pressed && ev.key == fn_key_pad_div)
            printf("Keypad div!\n");

        if(ev.type == fn_event_type_key_pressed && ev.key == fn_key_mute)
            printf("Mute!\n");

        if(ev.type == fn_event_type_key_pressed && ev.key == fn_key_unknown)
            printf("Unknown key.\n");
    }

    fn_deinit_window_module();
    return 0;
}
