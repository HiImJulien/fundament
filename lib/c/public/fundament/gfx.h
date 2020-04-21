#ifndef FUNDAMENT_GFX_H
#define FUNDAMENT_GFX_H

#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Initializes the library
 * and prepares the backend for processing
 * draw calls.
 */
bool fn_gfx_init(void);

/**
 * @brief Frees the internal state effectively
 * shutting down the library and freeing
 * resources allocated by it.
 */
void fn_gfx_quit(void);

/**
 * @brief Represents a handle to a swap chain.
 */
struct fn_gfx_swap_chain{ uint8_t id; };

/**
 * @enum Enumerates all supported data formats.
 */
enum fn_gfx_data_format {
	fn_gfx_data_format_r32g32b32a32_typeless,
	fn_gfx_data_format_r32g32b32a32_float,
	fn_gfx_data_format_r32g32b32a32_uint,
	fn_gfx_data_format_r32g32b32a32_sint,
	fn_gfx_data_format_r32g32b32_typeless,
	fn_gfx_data_format_r32g32b32_float,
	fn_gfx_data_format_r32g32b32_uint,
	fn_gfx_data_format_r32g32b32_sint,
	fn_gfx_data_format_r8g8b8a8_typeless,
	fn_gfx_data_format_r8g8b8a8_unorm,
	fn_gfx_data_format_r8g8b8a8_unorm_srgb,
	fn_gfx_data_format_r8g8b8a8_uint,
	fn_gfx_data_format_r8g8b8a8_snorm,
	fn_gfx_data_format_r8g8b8a8_sint
};

/**
 * @brief Stores the configuration data used
 * to create a swap chain.
 */
struct fn_gfx_swap_chain_desc {
	uint8_t buffers;
	uint32_t width;
	uint32_t height;
	enum fn_gfx_data_format format;
	const void* window;	//!< Stores the window this swap chain presents on.
	const char*	label;	//!< Stores the debug label.
};

/**
 * @brief Creates a new swap chain.
 */
struct fn_gfx_swap_chain fn_gfx_create_swap_chain(
	const struct fn_gfx_swap_chain_desc* desc);

/**
 * @brief Represents a handle to a shader.
 */
struct fn_gfx_shader { uint8_t id; };

/**
 * @brief Enumerates all supported shader types.
 */
enum fn_gfx_shader_type {
	fn_gfx_shader_type_vertex = 0x1,
	fn_gfx_shader_type_hull,
	fn_gfx_shader_type_domain,
	fn_gfx_shader_type_geometry,
	fn_gfx_shader_type_pixel,
	fn_gfx_shader_type_compute
};

/**
 * @struct Stores the configuration data used
 * to create a shader.
 */
struct fn_gfx_shader_desc {
	enum fn_gfx_shader_type type;	//!< Stores the type of shader.
	uint8_t* byte_code;				//!< Stores a pointer to the shader byte code.
	size_t byte_code_size;			//!< Stores the size of the shader byte code.
};

/**
 * @brief Creates a new shader.
 */
struct fn_gfx_shader fn_gfx_create_shader(struct fn_gfx_shader_desc* desc);

/**
 * @brief Represents a handle to a buffer.
 */
struct fn_gfx_buffer { uint16_t id; };

/**
 * @enum Enumerates all supported
 * buffer types.
 * @note Those can be combined using logical or.
 */
enum fn_gfx_buffer_type {
	fn_gfx_buffer_type_vertex,
	fn_gfx_buffer_type_index,
	fn_gfx_buffer_type_constant,
	fn_gfx_buffer_type_shader,
};

/**
 * @enum Enumerates all supported buffer usages.
 */
enum fn_gfx_buffer_usage {
	fn_gfx_buffer_usage_default,
	fn_gfx_buffer_usage_immutable,
	fn_gfx_buffer_usage_dynamic,
	fn_gfx_buffer_usage_staging
};

/**
 * @struct Stores the configuration data used
 * to create a buffer.
 * @note Currently all data created is mutable and read-/writable
 * by the CPU.
 */
struct fn_gfx_buffer_desc {
	size_t capacity;
	enum fn_gfx_buffer_type type;
	enum fn_gfx_buffer_usage usage;
	size_t stride;
};

/**
 * @brief Creates a new buffer with matching description.
 */
struct fn_gfx_buffer fn_gfx_create_buffer(const struct fn_gfx_buffer_desc* desc);

/**
 * @struct Stores data to update
 * a buffer with.
 */
struct fn_gfx_buffer_data_desc {
	const void* data;
	size_t size;
	// Pitch, SlicePitch?
};

/**
 * @brief Populates the given buffer with data.
 */
void fn_gfx_update_buffer(struct fn_gfx_buffer buffer, 
	struct fn_gfx_buffer_data_desc data);

/**
 * @struct Represents a handle to a pipeline state.
 */
struct fn_gfx_pipeline{ uint16_t id; };

/**
 * @enum Enumerates all supported 
 * primitive topology types.
 */
enum fn_gfx_primitive_topology_type {
	fn_gfx_primitive_topology_type_triangle_strip,
	fn_gfx_primitive_topology_type_triangle_list
};

/**
 * @enum Enumerates all step options.
 */
enum fn_gfx_input_element_step {
	fn_gfx_input_element_step_none,
	fn_gfx_input_element_step_vertex,
	fn_gfx_input_element_step_instance
};

/**
 * @struct Describes an element for a input layout.
 */
struct fn_gfx_input_element {
	size_t stride;
	enum fn_gfx_data_format format;
	enum fn_gfx_input_element_step step;
};

/**
 * @struct Describes how a buffer is bound to an input element.
 */
struct fn_gfx_input_binding {
	size_t buffer_slot;
	size_t buffer_offset;
	// TODO: Data format!
};

/**
 * @struct Describes the input layout
 * used by the pipeline.
 */
struct fn_gfx_input_layout {
	struct fn_gfx_input_element elements[256];
	struct fn_gfx_input_binding bindings[256];
};

/**
 * @struct Stores the configuration used to create 
 * a pipeline.
 * Note, missing pieces:
 * - blend state,
 * - rasterizer state,
 */
struct fn_gfx_pipeline_desc {
	struct fn_gfx_shader 				vertex_shader;
	struct fn_gfx_shader 				hull_shader;
	struct fn_gfx_shader 				domain_shader;
	struct fn_gfx_shader 				geometry_shader;
	struct fn_gfx_shader 				pixel_shader;
	enum fn_gfx_primitive_topology_type topology;
	struct fn_gfx_input_layout 			layout;
	const char* 						label;
};

struct fn_gfx_pipeline fn_gfx_create_pipeline(struct fn_gfx_pipeline_desc* desc);

/**
 * @struct Describes which buffer to bind which way.
 */
struct fn_gfx_buffer_binding {
	struct fn_gfx_buffer buffers[256];
	size_t strides[256];
	size_t buffer_count;
};

/**
 * @brief Presents the current contents of the swap chain's
 * back buffer on the window the swap chain's bound to. 
 */
void fn_gfx_swap_chain_present(struct fn_gfx_swap_chain sc);

/**
 * @brief Clears the contents of the render target.
 */
void fn_gfx_clear_target(struct fn_gfx_swap_chain sc, float (*rgba)[4]);

/**
 * @brief Applies the pipeline to the context.
 */
void fn_gfx_apply_pipeline(struct fn_gfx_pipeline pipeline);

/**
 * @brief Binds the buffers to the context.
 */
void fn_gfx_apply_bindings(struct fn_gfx_buffer_binding bindings);

/**
 * @brief Draws.
 */
void fn_gfx_draw();

void fn_gfx_set_canvas(struct fn_gfx_swap_chain sc);

#endif 	// FUNDAMENT_GFX_H