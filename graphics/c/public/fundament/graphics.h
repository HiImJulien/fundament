#ifndef FUNDAMENT_GRAPHICS_H
#define FUNDAMENT_GRAPHICS_H

//==============================================================================
//                                  GRAPHICS
//
// This module wraps graphics APIs such as Direct3D 12, Metal or Vulkan.
// The API is choosen based on the target system, for which the library
// is being compiled for.
// 
//                              OBJECT LIFETIME
// Objects are created using `fn_create_*object*` functions, which accept
// the parent device (i.e. the GPU to use) and a description of the object
// to create.
//
// If the parent device of an object is being released, all child objects are
// invalidated.
//
//==============================================================================

#include <stdbool.h>
#include <stdint.h>

#include <fundament/window.h>

//==============================================================================
//                                   ERRORS
//==============================================================================

enum fn_gfx_error_type {
    fn_gfx_error_none,
    fn_gfx_error_invalid_device,
    fn_gfx_error_invalid_desc
};

struct fn_gfx_error {
    enum fn_gfx_error_type  type;
    const char*             reason;
};

//==============================================================================
//                                  DEVICES
//
// An instance of `fn_gfx_device` represents a physical and logical device.
// Think of it as a context to your GPU.
//==============================================================================

struct fn_gfx_device{ uint32_t id; };

struct fn_gfx_device fn_create_gfx_device();
void fn_destroy_gfx_device(struct fn_gfx_device device);

//==============================================================================
//                                  SURFACES
// 
// Represents the surface of a window to render onto.
//==============================================================================

struct fn_gfx_surface{ uint32_t device_id; uint32_t id; };

//
// Describes the surface to create.
//
// Note:
// - Only `window` or `native_window` must be specified.
//
struct fn_gfx_surface_desc {
    struct fn_window            window;
    fn_native_window_handle_t   native_window;
    const char*                 label; 
    struct fn_gfx_error*        error; 
};

struct fn_gfx_surface fn_create_gfx_surface(
    struct fn_gfx_device device,
    struct fn_gfx_surface_desc* desc
);

void fn_destroy_gfx_surface(struct fn_gfx_surface surface);

//==============================================================================
//                                   FENCES
//
// The GPU works in parallel to the CPU, requiring you to synchronise the two,
// when updating resources (i.e. constants buffers, etc.).
//==============================================================================

struct fn_gfx_fence{ uint32_t device_id; uint32_t id; };

struct fn_gfx_fence_desc {
    const char*             label;
    struct fn_gfx_error*    error;
};

struct fn_gfx_fence fn_create_gfx_fence(
    struct fn_gfx_device device,
    struct fn_gfx_fence_desc* desc
);

void fn_destroy_gfx_fence(struct fn_gfx_fence fence);

//==============================================================================
//                               COMMAND QUEUES
//
// **Command queues** are used to submit **command buffers** to the GPU to 
// process.
//==============================================================================

struct fn_gfx_queue{ uint32_t device_id; uint32_t id; };

struct fn_gfx_queue_desc {
    const char*             label;
    struct fn_gfx_error*    error;
};

struct fn_gfx_queue fn_create_gfx_queue(
    struct fn_gfx_device device,
    struct fn_gfx_queue_desc* desc
);

void fn_destroy_gfx_queue(struct fn_gfx_queue queue);

//==============================================================================
//                              COMMAND BUFFERS
//
// **Command buffers** are used to record one or more commands such as 
// "draw the vertices" or "perform this computation".
// Command buffers can be reused multipe times; i.e. once every frame.  
//==============================================================================

struct fn_gfx_cmd_buffer { uint32_t device_id; uint32_t id; };

struct fn_gfx_cmd_buffer_desc {
    const char*             label;
    struct fn_gfx_error*    error;
};

struct fn_gfx_cmd_buffer fn_create_gfx_cmd_buffer(
    struct fn_gfx_queue queue,
    struct fn_gfx_cmd_buffer_desc* desc
);

void fn_destroy_gfx_cmd_buffer(struct fn_gfx_cmd_buffer buffer);

//==============================================================================
//                                  BUFFERS
//
// Represents vertex-, index- and constants-buffers, which are later bound to
// pipeline(s).
//
// Note:
// - Constant buffers are also known as uniform buffers.
//==============================================================================

struct fn_gfx_buffer{ uint32_t device_id; uint32_t id; };

struct fn_gfx_buffer_desc {
    const char*             label;  
    struct fn_gfx_error*    error;
};

struct fn_gfx_buffer fn_create_gfx_buffer(
    struct fn_gfx_device device,
    struct fn_gfx_buffer* desc
);

void fn_destroy_gfx_buffer(struct fn_gfx_buffer buffer);

//=============================================================================
//                                 PIPELINES
//
// Represents the entire state of the pipeline, allowing to validate the pipe-
// line state upfront.
//
// Stores informations such as:
// - current shader program (vertex-, pixel- and/or compute shader)
// - currently bound vertex buffer(s) and index buffer
// - topology type
// - ...
//
//=============================================================================

struct fn_gfx_pipeline{ uint32_t device_id; uint32_t id; };

enum fn_gfx_topology_type {
    fn_gfx_topology_type_none,
    fn_gfx_topology_type_point,
    fn_gfx_topology_type_line,
    fn_gfx_topology_type_triangle
};

struct fn_gfx_pipeline_desc {
    enum fn_gfx_topology_type       topology;
    bool                            is_compute;
    const char*                     label;
    struct fn_gfx_error*            error;
};

struct fn_gfx_pipeline fn_create_gfx_pipeline(
    struct fn_gfx_device device,
    struct fn_gfx_pipeline_desc* desc 
);

void fn_destroy_gfx_pipeline(
    struct fn_gfx_pipeline pipeline
);

#endif  // FUNDAMENT_GRAPHICS_H
