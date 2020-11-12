#include "wayland_decorations.h"
#include "../window_common.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define STB_TRUETYPE_IMPLEMENTATION
#define STBTT_STATIC
#include "stb_truetype.h"

static const uint32_t fn__g_fg_color = 0xb2bec3;
static const uint32_t fn__g_bg_color = 0x2d3436;

static bool                 fn__g_initialized_font_info = false;
static unsigned char*       fn__g_font_buffer;
static stbtt_fontinfo       fn__g_font_info;
static int                  fn__g_font_height = 50;
static float                fn__g_font_scaling = 0;

static const char* font_path = "/usr/share/fonts/droid/DroidSansMono.ttf";

typedef struct fn__pixel {
    uint8_t b;
    uint8_t g;
    uint8_t r;
    uint8_t x;
} fn__pixel;

void fn__init_wayland_decorations() {
    if(!fn__g_initialized_font_info) {
        FILE* font_file = fopen(font_path, "rb"); 
        fseek(font_file, 0, SEEK_END); 
        const size_t size = ftell(font_file);
        rewind(font_file);

        fn__g_font_buffer = malloc(size);
        
        fread(fn__g_font_buffer, size, 1, font_file);
        fclose(font_file);

        stbtt_InitFont(&fn__g_font_info, fn__g_font_buffer, 0);
        fn__g_font_scaling = stbtt_ScaleForPixelHeight(
            &fn__g_font_info, 
            fn__g_font_height
        );

        fn__g_initialized_font_info = true;
    } 
}

void fn__deinit_wayland_decorations() {
}

void fn__decorate_wayland_window(fn__window* window, uint32_t* image_data) {
    const size_t size = window->width * window->height * 4;
    memset(image_data, fn__g_bg_color, size);

}

