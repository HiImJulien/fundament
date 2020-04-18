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
 * @brief Stores the configuration data used
 * to create a swap chain.
 */
struct fn_gfx_swap_chain_desc {
	uint8_t buffers;
	uint32_t width;
	uint32_t height;
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
	fn_gfx_shader_type_vertex,
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
 * @brief Presents the current contents of the swap chain's
 * back buffer on the window the swap chain's bound to. 
 */
void fn_gfx_swap_chain_present(struct fn_gfx_swap_chain sc);

/**
 * @brief Clears the contents of the render target.
 */
void fn_gfx_clear_target(struct fn_gfx_swap_chain sc, float (*rgba)[4]);

#endif 	// FUNDAMENT_GFX_H