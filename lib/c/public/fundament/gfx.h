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
 * @brief Presents the current contents of the swap chain's
 * back buffer on the window the swap chain's bound to. 
 */
void fn_gfx_swap_chain_present(struct fn_gfx_swap_chain sc);

/**
 * @brief Clears the contents of the render target.
 */
void fn_gfx_clear_target(struct fn_gfx_swap_chain sc, float (*rgba)[4]);

#endif 	// FUNDAMENT_GFX_H