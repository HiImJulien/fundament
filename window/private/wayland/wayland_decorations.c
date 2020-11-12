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
static int                  fn__g_font_height = 25;
static float                fn__g_font_scaling = 0;

static const char* font_path = "/usr/share/fonts/droid/DroidSansMono.ttf";

typedef struct fn__pixel {
    uint8_t b;
    uint8_t g;
    uint8_t r;
    uint8_t x;
} fn__pixel;

static void fn__g_draw_rectangle(
    fn__window* window,
    uint32_t*   image_data,
    uint32_t    x,
    uint32_t    y,
    uint32_t    width,
    uint32_t    height,
    uint32_t    color
) {
    for(uint32_t iy = 0; iy < height; ++iy)
        for(uint32_t ix = 0; ix < width; ++ix)
            image_data[(y + iy) * window->width + x + ix] = color;
}

static uint32_t fn__g_rgba_to_rgb(uint32_t rgb, unsigned char a) {
    uint8_t* _rgb = (uint8_t*) &rgb; 
    float b = _rgb[0] / 255.f;
    float g = _rgb[1] / 255.f;
    float r = _rgb[2] / 255.f;

    float _a = a / 255.f; 
    // float _a1 = 1.f - a;
    float _a1 = a;

    b = _a1 * b + _a * b;
    g = _a1 * g + _a * g;
    r = _a1 * r + _a * r;
    
    _rgb[0] = roundf(b * 255.f);
    _rgb[1] = roundf(g * 255.f);
    _rgb[2] = roundf(r * 255.f);

    rgb = 0;
    _rgb[0] = a;
    _rgb[3] = 0;

    return rgb;
}

static void fn__g_draw_text(
    fn__window* window,
    uint32_t*   image_data,
    uint32_t    x,
    uint32_t    y,
    const char* text
) {
    const int text_height = fn__g_font_height;
    int text_width = 0;
    for(int it = 0; it < strlen(text); ++it) {
        int width, bearing;
        stbtt_GetCodepointHMetrics(
            &fn__g_font_info, 
            text[it], 
            &width, 
            &bearing
        );

        int kern = stbtt_GetCodepointKernAdvance(
            &fn__g_font_info,
            text[it],
            text[it+1]
        );

        text_width += roundf(width * fn__g_font_scaling);
        text_width += roundf(kern * fn__g_font_scaling);
    }

    int ascent, descent, line_gap;
    stbtt_GetFontVMetrics(
        &fn__g_font_info,
        &ascent,
        &descent,
        &line_gap
    );

    ascent = roundf(ascent * fn__g_font_scaling);
    descent = roundf(descent * fn__g_font_scaling);

    // VLA causes a segfault for whatever reasons.
    // unsigned char bitmap[text_width * text_height];
    // memset(bitmap, 0, text_width * text_height * sizeof(unsigned char));
    unsigned char* bitmap = calloc(
        text_width * text_height, 
        sizeof(unsigned char)
    );

    int cx = 0;
    for(int it = 0; it < strlen(text); ++it) {
        int width, bearing;
        stbtt_GetCodepointHMetrics(
            &fn__g_font_info,
            text[it],
            &width,
            &bearing
        );

        int x1, y1, x2, y2;
        stbtt_GetCodepointBitmapBox(
            &fn__g_font_info,
            text[it],
            fn__g_font_scaling,
            fn__g_font_scaling,
            &x1, &y1,
            &x2, &y2
        );

        const int y = ascent + y1;
        int byte_offset = cx 
            + roundf(width * fn__g_font_scaling) 
            + (y * text_width);

        stbtt_MakeCodepointBitmap(
            &fn__g_font_info,
            bitmap + byte_offset,
            x2 - x1,
            y2 - y1,
            text_width,
            fn__g_font_scaling,
            fn__g_font_scaling,
            text[it]
        );

        int kern = stbtt_GetCodepointKernAdvance(
            &fn__g_font_info,
            text[it],
            text[it+1]
        );

        cx += roundf(width * fn__g_font_scaling);
        cx += roundf(kern * fn__g_font_scaling);
    }

    // Time to blit that shit.
    for(int yt = 0; yt < text_height; ++yt) {
        for(int xt = 0; xt < text_width; ++xt) {
            printf("X: %i Y: %i\n", xt, yt);
            unsigned char pixel = bitmap[yt * text_width + xt];

            if(pixel != 0)
                printf("%i\n", (int) pixel);

            uint32_t* out_pixel = &image_data[(y + yt) * window->width + x + xt];
            *out_pixel = fn__g_rgba_to_rgb(fn__g_fg_color, pixel);
        }
    }

    printf("W: %i H: %i\n", text_width, text_height);

    free(bitmap);
}

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

    fn__g_draw_rectangle(
        window, 
        image_data, 
        window->width - 25 - 10, 
        10, 
        25, 
        25, 
        fn__g_fg_color
    );

    fn__g_draw_rectangle(
        window, 
        image_data, 
        window->width - 50 - 20, 
        10, 
        25, 
        25, 
        fn__g_fg_color
    );

    fn__g_draw_rectangle(
        window, 
        image_data, 
        window->width - 75 - 30, 
        10, 
        25, 
        25, 
        fn__g_fg_color
    );

    fn__g_draw_text(
        window,
        image_data,
        10, 
        10,
        window->title
    );
}

