#include <fundament/math.h>

#include <stdio.h>

int main() {
    for(float it = 1; it <= 10; ++it) {
        struct fn_vec3f ex = {it, 0, 0};
        printf("Len of E_x (vec3f) (%f): %f\n", it, fn_vec3f_len(ex));
    }

    {
        struct fn_vec4f ex = {1, 0, };
        printf("Len of E_x (vec4f): %f\n", fn_vec4f_len(ex));
    }
    return 0;
}