#ifndef FUNDAMENT_GFX_H
#define FUNDAMENT_GFX_H

#include <stdbool.h>
#include <stdint.h>

struct fn_buffer { uint32_t id; };
struct fn_pipeline { uint32_t id; };
struct fn_shader { uint32_t id; };
struct fn_render_device { uint32_t id; };
struct fn_swap_chain { uint32_t id; };

/**
 * @enum Specifies as which type shader byte code
 * should be treated.
 */
enum fn_shader_type {
    fn_shader_type_vertex,
    fn_shader_type_pixel
};

/**
 * @enum Describes a shader.
 */
struct fn_shader_desc {
    enum fn_shader_type type;
    const void*         byte_code;
    size_t              byte_code_size;
};

/**
 * @enum Specifies how to interpret vertices. 
 */
enum fn_topology_type {
    fn_topology_type_none,
    fn_topology_type_point_list,
    fn_topology_type_line_list,
    fn_topology_type_line_strip,
    fn_topology_type_triangle_list,
    fn_topology_type_triangle_strip,
    fn_topology_type_line_list_adj,
    fn_topology_type_line_strip_adj,
    fn_topology_type_triangle_list_adj,
    fn_topology_type_triangle_strip_adj
};

/**
 * @enum Specifies how to interpret set of bytes. 
 */
enum fn_data_format {
    fn_data_format_none,
    fn_data_format_r32g32b32a32_float,
    fn_data_format_r32g32b32a32_uint,
    fn_data_format_r32g32b32_float,
    fn_data_format_r32g32b32_uint,
    fn_data_format_r8g8b8a8_unorm,
    fn_data_format_r8g8b8a8_uint,
};

/**
 * @enum Specifies the resource use.
 */
enum fn_resource_usage {
    fn_resource_usage_default = (1 << 0),
    fn_resource_usage_immutable = (1 << 1),
    fn_resource_usage_dynamic = (1 << 2),
    fn_resource_usage_staging = (1 << 3)
};

/**
 * @enum Specifies how a resource will
 * be bound to the pipeline.
 */
enum fn_resource_bind_flags {
    fn_resource_bind_flags_vertex,
    fn_resource_bind_flags_index,
    fn_resource_bind_flags_constant,
    fn_resource_bind_flags_shader_resource,
    fn_resource_bind_flags_render_target
};

/**
 * @enum Specifies how a resource
 * will be accessed.
 */
enum fn_resource_access_flags {
    fn_resource_access_flags_none,
    fn_resource_access_flags_write = (1 << 0),
    fn_resource_access_flags_read = (1 << 1),
};

/**
 * @struct Describes a buffer.
 */
struct fn_buffer_desc {
    size_t                      size;
    enum fn_resource_usage      usage;
    enum fn_resource_bind_flags bind_flags;
    const void*                 data;
    size_t                      pitch;
    size_t                      slice_pitch;
};

/**
 * @struct Describes the properties of a vertex.
 */
struct fn_vertex_layout {
    enum fn_data_format     format;
    size_t                  byte_offset;
    uint16_t                slot;
    bool                    is_per_instance;
    uint16_t                steps_per_instance;
};


/**
 * @struct Describes a pipeline state.
 */
struct fn_pipeline_desc {
    struct fn_shader        vertex_shader;
    struct fn_shader        pixel_shader;
    enum fn_topology_type   topology;
    struct fn_vertex_layout layout[32];
};

/**
 * @struct Describes a swap chain.
 */
struct fn_swap_chain_desc {
    enum fn_data_format format;
    void*               window;
    uint16_t            width;
    uint16_t            height;
};

/**
 * @struct Describes how to bind buffers to the pipeline.
 */
struct fn_buffer_binding {
    struct {
        size_t slot;
        struct fn_buffer buffer;
        size_t stride;
        size_t offset;
    } vertex_buffers[32];
    size_t vertex_buffer_count;
    struct {
        enum fn_data_format format;
        struct fn_buffer buffer;
        size_t offset;
    } index_buffer;
    struct fn_swap_chain swap_chains[8];
    // TODO: Constants buffers?
    // TODO: What about readable and writeable textures?
};

/**
 * @brief Attempts to create a new device.
 */
struct fn_render_device fn_create_render_device();

/**
 * @brief Destroys given render device and 
 * releases all resources allocated by it.
 */
void fn_destroy_render_device(struct fn_render_device device);

/**
 * @brief Creates a new swap chain tied 
 * to the device.
 */
struct fn_swap_chain fn_create_swap_chain(
    struct fn_render_device device,
    struct fn_swap_chain_desc desc
);

/**
 * @brief Creates a new shader.
 */
struct fn_shader fn_create_shader(
    struct fn_render_device device,
    struct fn_shader_desc desc
);

/**
 * @brief Creates a pipeline.
 */
struct fn_pipeline fn_create_pipeline(
    struct fn_render_device device,
    struct fn_pipeline_desc desc
);

/**
 * @brief Creates a buffer.
 */
struct fn_buffer fn_create_buffer(
    struct fn_render_device device,
    struct fn_buffer_desc desc
);

/**
 * @brief Binds the given buffers to the context.
 * TODO: How to handle command queues?
 */
void fn_apply_bindings(
    struct fn_render_device device,
    struct fn_buffer_binding desc
);

/**
 * @brief Binds the given pipeline to the context.
 */
void fn_apply_pipeline(
    struct fn_render_device device,
    struct fn_pipeline pipeline
);

/**
 * @brief Issues a draw command.
 */
void fn_draw(
    struct fn_render_device device,
    size_t vertex_count,
    size_t vertex_offset
);

/**
 * @brief Clears the contents of the swap chain 
 * to given color.
 */
void fn_clear(
    struct fn_render_device device, 
    struct fn_swap_chain sc,
    float r, float g, float b, float a
);

/**
 * @brief Presents the contents of the swap chain
 * on the window bound to it.
 */
void fn_present(struct fn_render_device device, struct fn_swap_chain sc);

#endif  // FUNDAMENT_GFX_H