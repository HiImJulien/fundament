#include <fundament/window.h>
#include <fundament/graphics.h>

#include <stdlib.h>
#include <stdio.h>

int main(int argc, char** argv) {
    if(!fn_init_graphics()) {
        printf("Failed to initialize graphics.\n");
        return EXIT_FAILURE;
    }

    fn_deinit_graphics();
    return EXIT_SUCCESS;
}