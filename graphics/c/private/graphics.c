#include <fundament/graphics.h>

#include <assert.h>

//==============================================================================
// The following section implements internal utility functions.
//==============================================================================

static void fn__get_device_id(uint32_t id) {

}

static void fn__get_device(struct fn_gfx_device device) {
    return fn__get_device_id(device.id);
}

//==============================================================================
// The following section implements the public API as exposed in 
// "fundament/graphics.c".
//==============================================================================

//--------------------------------- DEVICES ----------------------------------//

struct fn_gfx_device fn_create_gfx_device() {
    return (struct fn_gfx_device) { 0 };
}

void fn_destroy_gfx_device(struct fn_gfx_device device) {
}

//--------------------------------- SURFACES ---------------------------------//

struct fn_gfx_surface fn_create_gfx_surface(
    struct fn_gfx_device device,
    struct fn_gfx_surface_desc* desc) {
    assert(desc && "'desc' must not be NULL.");

    if(desc->window.id == (uint32_t) desc->native_window 
        && desc->native_window == 0) {
        if(desc->error) {
            desc->error->type = fn_gfx_error_invalid_desc;
            desc->error->reason = "You must provide a window to create a"
                                  "surface for.";
        }
        return (struct fn_gfx_surface) { 0, };
    }


    return (struct fn_gfx_surface) { 0, };  
}

void fn_destroy_gfx_surface(struct fn_gfx_surface surface) {
    
}

//---------------------------------- FENCES ----------------------------------//

struct fn_gfx_fence fn_create_gfx_fence(
    struct fn_gfx_device device,
    struct fn_gfx_fence_desc* desc) {
    return (struct fn_gfx_fence) { 0, };
}

void fn_destroy_gfx_fence(struct fn_gfx_fence fence) {
}

//------------------------------ COMMAND QUEUES ------------------------------//

struct fn_gfx_queue fn_create_gfx_queue(
    struct fn_gfx_device device,
    struct fn_gfx_queue_desc* desc) {
    return (struct fn_gfx_queue) { 0, };
}

void fn_destroy_gfx_queue(struct fn_gfx_queue queue) {
}

//----------------------------- COMMAND BUFFERS ------------------------------//

struct fn_gfx_cmd_buffer fn_create_gfx_cmd_buffer(
    struct fn_gfx_queue queue,
    struct fn_gfx_cmd_buffer_desc* desc
) {
    return (struct fn_gfx_cmd_buffer) { 0, };
}

void fn_destroy_gfx_cmd_buffer(struct fn_gfx_cmd_buffer buffer) {
}

//--------------------------------- BUFFERS ----------------------------------//

struct fn_gfx_buffer fn_create_gfx_buffer(
    struct fn_gfx_device device,
    struct fn_gfx_buffer* desc) {
    return (struct fn_gfx_buffer) { 0, };
}

void fn_destroy_gfx_buffer(struct fn_gfx_buffer buffer) {
}

//-------------------------------- PIPELINES ---------------------------------//

struct fn_gfx_pipeline fn_create_gfx_pipeline(
    struct fn_gfx_device device,
    struct fn_gfx_pipeline_desc* desc) {
    return (struct fn_gfx_pipeline) { 0, };
}

void fn_destroy_gfx_pipeline(
    struct fn_gfx_pipeline pipeline
) {
}

