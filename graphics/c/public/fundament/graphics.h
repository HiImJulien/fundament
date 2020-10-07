#ifndef FUNDAMENT_GRAPHICS_H
#define FUNDAMENT_GRAPHICS_H

//==============================================================================
//                                  GRAPHICS
//
// This module wraps graphics APIs such as Direct3D 12, Metal or Vulkan.
// The API is choosen based on the target system.
//
// All resources are wrapped behind a typesafe numeric handle of the form
//  struct fn_gfx_*resource* { uint32_t id; };
//
// E.g.:
//  struct fn_gfx_pipeline { uint32_t id; };
//  struct fn_gfx_buffer{ uint32_t id; };
//
// If the allocation fails, the `id` equals to zero.
// A handle may incorporate additional data. For instance GPU resources, 
// include the ID of the device they belong to:
//   struct fn_gfx_buffer{
//     struct fn_gfx_device parent;
//     uint32_t             id;
//   };
//
//==============================================================================

#include <stdbool.h>
#include <stdint.h>


//==============================================================================
//                                   ERRORS
//==============================================================================

enum fn_gfx_error_type {
    fn_gfx_error_none
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
// Used to store data such as vertices or indices to be used for rendering.
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
// Describe the entire state of the pipeline.
//=============================================================================

struct fn_gfx_pipeline{ uint32_t device_id; uint32_t id; };

enum fn_gfx_topology_type {
    fn_gfx_topology_type_triangle_list
};

struct fn_gfx_rasterizer_state {
    int foo;
};

struct fn_gfx_pipeline_desc {
    enum fn_gfx_topology_type topology;
    struct fn_gfx_rasterizer_state rasterizer_state;
    bool                    is_compute;
    const char*             label;
    struct fn_gfx_error*    error;
};

struct fn_gfx_pipeline fn_create_gfx_pipeline(
    struct fn_gfx_device device,
    struct fn_gfx_pipeline_desc* desc 
);

void fn_destroy_gfx_pipeline(
    struct fn_gfx_pipeline pipeline
);

#endif  // FUNDAMENT_GRAPHICS_H
