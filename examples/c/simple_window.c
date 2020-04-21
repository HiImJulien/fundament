#include <fundament/fundament.h>
#include <fundament/gfx.h>

#include <stdlib.h>
#include <stdio.h>

struct fn_gfx_shader_desc load_shader(const char* path,
	enum fn_gfx_shader_type type) {
	FILE* file = fopen(path, "r");

	if(file == NULL) {
		printf("Failed to open shader.\n");
		exit(-1);
	}

	struct fn_gfx_shader_desc desc = {0};
	fseek(file, 0L, SEEK_END);
	desc.byte_code_size = ftell(file);

	rewind(file);

	desc.type = type;
	desc.byte_code = malloc(desc.byte_code_size);
	fread(desc.byte_code, 1, desc.byte_code_size, file);
	fclose(file);

	return desc;
}

const float vertices[] = {
	0.0f, 0.5f, 0.0f,
	0.5f, -0.5f, 0.0f,
	-0.5f, -0.5f, 0.0f
};

int main() {
	if(!fn_init()) {
		printf("Failed to initialize fundament.\n");
		return EXIT_FAILURE;
	}

	struct fn_window win = fn_window_create();
	fn_window_set_title(win, "Simple Window");
	fn_window_set_size(win, 300, 300);
	fn_window_set_visibility(win, true);

	printf("....\n");

	if(!fn_gfx_init()) {
		printf("Failed to initialize graphics device.\n");
		return EXIT_FAILURE;
	}

	printf("Here...\n");

	struct fn_gfx_shader_desc vdesc = load_shader("../examples/shaders/triangle.v.cso", fn_gfx_shader_type_vertex);
	struct fn_gfx_shader_desc pdesc = load_shader("../examples/shaders/triangle.p.cso", fn_gfx_shader_type_pixel);

	printf("Loaded shader byte codes.\n");

	struct fn_gfx_shader vertex = fn_gfx_create_shader(&vdesc);
	struct fn_gfx_shader pixel = fn_gfx_create_shader(&pdesc);

	free(vdesc.byte_code);
	free(pdesc.byte_code);

	struct fn_gfx_buffer vbuffer = fn_gfx_create_buffer(&(struct fn_gfx_buffer_desc) {
		.capacity = sizeof(float) * 9,
		.type = fn_gfx_buffer_type_vertex,
		.stride = sizeof(float)
	});

	fn_gfx_update_buffer(vbuffer, (struct fn_gfx_buffer_data_desc) {
		.data = vertices,
		.size = sizeof(float) * 9
	});

	struct fn_gfx_pipeline_desc pipe_desc = {
		.vertex_shader = vertex,
		.pixel_shader = pixel,
		.topology = fn_gfx_primitive_topology_type_triangle_list,
	};

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

	struct fn_gfx_pipeline pipeline = fn_gfx_create_pipeline(
		&(struct fn_gfx_pipeline_desc) {
			.vertex_shader = vertex,
			.pixel_shader = pixel,
			.topology = fn_gfx_primitive_topology_type_triangle_strip,
			.layout.elements[0] = {
				.stride = sizeof(float) * 3,
				.step = fn_gfx_input_element_step_vertex
			},
			.layout.bindings[0] = {
				.buffer_slot = 0,
				.buffer_offset = 0
			},
			.label = "pipeline"
	});

	if(pipeline.id == 65535) {
		printf("Failed to create pipeline.\n");
		return EXIT_FAILURE;
	}

	fn_gfx_apply_pipeline(pipeline);
	
	fn_gfx_apply_bindings( (struct fn_gfx_buffer_binding) {
		.buffers[0] = vbuffer,
		.buffer_count = 1
	});

	fn_gfx_set_canvas(sc);

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
		fn_gfx_draw();
		fn_gfx_swap_chain_present(sc);
		fn_poll_event(&ev);

		if(ev.type == fn_event_type_window_closed)
			printf("Closed window.\n");
	}

	fn_window_destroy(win);
	return EXIT_SUCCESS;
}