#include <math.h>

#include <stdio.h>

int main()
{
    for (float it = 1; it <= 10; ++it)
    {
        struct fn_float3 ex = {it, 0, 0};
        printf("Len of E_x (vec3f) (%f): %f\n", it, fn_float3_len(ex));
    }

    // {
    //     struct fn_float4 ex = {
    //         1,
    //         0,
    //     };
    //     printf("Len of E_x (vec4f): %f\n", fn_float4_len(ex));
    // }
    return 0;
}