#ifndef FUNDAMENT_IMAGE_H
#define FUNDAMENT_IMAGE_H

//==============================================================================
// This module provides functions to load images.
//==============================================================================

#include <fundament/api.h>

#include <stddef.h>
#include <stdint.h>

//
// Enumerates all known image formats.
//
// TODO:
// - This will be required by the graphics API as well; should image borrow?
//   Or should I have a shared header; i.e. common_types.h? 
//
enum fn_image_format {
    fn_image_format_none,
    fn_image_format_r8g8b8_unorm,
    fn_image_format_r8g8b8a8_unorm,
};

//
// Represents an image.
//
struct fn_image {
    uint8_t*                data;
    size_t                  width;
    size_t                  height;

    enum fn_image_format    format;
};

//
//
//
API struct fn_image fn_load_tga_image_from_file(const char* path);

//
// Loads an TGA image from memory.
//
API struct fn_image fn_load_tga_image(const uint8_t* memory, size_t size);

#endif  // FUNDAMENT_IMAGE_H
