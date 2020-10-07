#include <fundament/graphics.h>

struct fn_gfx_device fn_create_gfx_device() {
    return (struct fn_gfx_device) { 0 };
}

void fn_destroy_gfx_device(struct fn_gfx_device device) {
}

struct fn_gfx_queue fn_create_gfx_queue(
    struct fn_gfx_device device,
    struct fn_gfx_queue_desc* desc) {
    return (struct fn_gfx_queue) { 0, };
}

void fn_destroy_gfx_queue(struct fn_gfx_queue queue) {
}

struct fn_gfx_cmd_buffer fn_create_gfx_cmd_buffer(
    struct fn_gfx_queue queue,
    struct fn_gfx_cmd_buffer_desc* desc
) {
    return (struct fn_gfx_cmd_buffer) { 0, };
}

void fn_destroy_gfx_cmd_buffer(struct fn_gfx_cmd_buffer buffer) {
}

struct fn_gfx_buffer fn_create_gfx_buffer(
    struct fn_gfx_device device,
    struct fn_gfx_buffer* desc) {
    return (struct fn_gfx_buffer) { 0, };
}

void fn_destroy_gfx_buffer(struct fn_gfx_buffer buffer) {
}

struct fn_gfx_pipeline fn_create_gfx_pipeline(
    struct fn_gfx_device device,
    struct fn_gfx_pipeline_desc* desc) {
    return (struct fn_gfx_pipeline) { 0, };
}

void fn_destroy_gfx_pipeline(
    struct fn_gfx_pipeline pipeline
) {
}

