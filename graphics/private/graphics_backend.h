#ifndef FUNDAMENT_GRAPHICS_BACKEND_H
#define FUNDAMENT_GRAPHICS_BACKEND_H

//==============================================================================
//                              GRAPHICS BACKEND
//
// Provides a common interface for graphics backends.
//
//==============================================================================

#include <fundament/graphics.h>

#include <stdbool.h>

typedef struct fn__imp_swap_chain   fn__imp_swap_chain_t;
typedef struct fn_swap_chain_desc   fn_swap_chain_desc_t;

typedef struct fn__imp_command_list fn__imp_command_list_t;

typedef bool(*fn__init_backend_ptr)();
typedef void(*fn__deinit_backend_ptr)();

typedef bool(*fn__create_imp_swap_chain_ptr)(
    fn__imp_swap_chain_t*,
    const fn_swap_chain_desc_t*
);

typedef void(*fn__destroy_imp_swap_chain_ptr)(fn__imp_swap_chain_t*);

typedef bool(*fn__create_imp_command_list_ptr)(fn__imp_command_list_t*);
typedef void(*fn__destroy_imp_command_list_ptr)(fn__imp_command_list_t*);
typedef void(*fn__encode_imp_command_list_ptr)(fn__imp_command_list_t*);
typedef void(*fn__commit_imp_command_list_ptr)(fn__imp_command_list_t*);

struct fn__graphics_backend {
    enum fn_graphics_backend_type       type;
    fn__init_backend_ptr                init_backend;
    fn__deinit_backend_ptr              deinit_backend;

    fn__create_imp_swap_chain_ptr       create_swap_chain;
    fn__destroy_imp_swap_chain_ptr      destroy_swap_chain;

    fn__create_imp_command_list_ptr     create_command_list;
    fn__destroy_imp_command_list_ptr    destroy_command_list;
    fn__encode_imp_command_list_ptr     encode_command_list;
    fn__commit_imp_command_list_ptr     commit_command_list;
};


#endif  // FUNDAMENT_GRAPHICS_BACKEND_H
