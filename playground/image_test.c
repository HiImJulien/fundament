#include <fundament/image.h>

#include <stdlib.h>
#include <stdio.h>

int main(int argc, char** argv) {
    if(argc < 2) {
        printf("Usage: ./image_test <path-to-tga-image>\n");
        return EXIT_FAILURE;
    } 

    struct fn_image image = fn_load_tga_image_from_file(argv[1]); 
    printf("Width: %zu\nHeight: %zu\n", image.width, image.height);

    switch(image.format) {
        default:
        case fn_image_format_none:          
            printf("Format: <Unknown>\n"); break;
        case fn_image_format_r8g8b8_unorm:  
            printf("Format: r8g8b8_unorm\n"); break;
        case fn_image_format_r8g8b8a8_unorm:
            printf("Format: r8g8b8a8_unorm\n"); break;
    }

    free(image.data);

    return EXIT_SUCCESS;
}
