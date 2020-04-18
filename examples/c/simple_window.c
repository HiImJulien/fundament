#include <fundament/fundament.h>

#include <stdlib.h>
#include <stdio.h>

int main() {
	if(!fn_init()) {
		printf("Failed to initialize fundament.\n");
		return EXIT_FAILURE;
	}

	struct fn_window win = fn_window_create();
	fn_window_set_title(win, "Simple Window");
	fn_window_set_size(win, 300, 300);
	fn_window_set_visibility(win, true);

	struct fn_event ev = {0, };
	while(ev.type != fn_event_type_window_closed) {
		if(ev.type == fn_event_type_key_pressed && ev.key == fn_key_a)
			printf("Pressed A.\n");

		if(ev.type == fn_event_type_key_pressed && ev.key == fn_key_enter)
			printf("Pressed Enter.\n");

		if(ev.type == fn_event_type_key_pressed && ev.key == fn_key_caps)
			printf("Pressed caps.\n");

		if(ev.type == fn_event_type_button_pressed 
			&& ev.mouse.button & fn_button_left)
			printf("Pressed left mouse button.\n");

		if(ev.type == fn_event_type_button_pressed
			&& (ev.mouse.button & fn_button_right) != 0)
			printf("Pressed right mouse button.\n");

		if(ev.type == fn_event_type_button_released
			&& (ev.mouse.button & fn_button_left) != 0)
			printf("Released left mouse button.\n");

		if(ev.type == fn_event_type_button_released
			&& ev.mouse.button & fn_button_right)
			printf("Released right mouse button.\n");

		fn_poll_event(&ev);

		if(ev.type == fn_event_type_window_closed)
			printf("Closed window.\n");
	}

	fn_window_destroy(win);
	return EXIT_SUCCESS;
}