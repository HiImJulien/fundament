#include <fundament/fundament.h>
#include <fundament/gfx.h>

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

	if(!fn_gfx_init()) {
		printf("Failed to initialize graphics device.\n");
		return EXIT_FAILURE;
	}

	struct fn_gfx_swap_chain sc = 
		fn_gfx_create_swap_chain(&(struct fn_gfx_swap_chain_desc) {
			.buffers = 1,
			.width = 300,
			.height = 300,
			.window = fn_window_handle(win),
			.label = "swap_chain"
	});

	if(sc.id == 255) {
		printf("Failed to create swap chain.\n");
		return EXIT_FAILURE;
	}

	float clear_color[4] = {0.4f, 0.6f, 0.9f, 0.0f};
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

		fn_gfx_clear_target(sc, &clear_color);
		fn_gfx_swap_chain_present(sc);
		fn_poll_event(&ev);

		if(ev.type == fn_event_type_window_closed)
			printf("Closed window.\n");
	}

	fn_window_destroy(win);
	return EXIT_SUCCESS;
}