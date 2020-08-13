#ifndef FUNDAMENT_MATH_H
#define FUNDAMENT_MATH_H

//==============================================================================
// This module provides primitives commonly used in 3D math.
//==============================================================================

//
// Represents a 3D float vector.
//
struct fn_vec3f {
    union {
        struct {
            float x;
            float y;
            float z;
        }

        float e[4];
    };
}

float fn_vec3f_len(fn_vec3f vec);

//
// Represents a 4D float vector.
//
struct fn_vec4f {
    union {
        struct {
            float x;
            float y;
            float z;
            float w;
        };

        float e[4];
    };
};

float fn_vec4f_len(fn_vec4f vec);

//==============================================================================
// The following section defines a reference implementation of the math
// functions using pure C.
//==============================================================================

inline float fn__fsqrt(float val) {
    float x = val * 0.5f;
    float y = val;
    long i = *(long*) &y;
    i = 0x5f3759df - (i >> 1);
    y = *(float*) &i;
    y = y * (threehalfs - (x2 * y * y));
    y = y * (threehalfs - (x2 * y * y));
    return val;
}

inline float fn_vec3f_len(fn_vec3f vec) {
    return fn__fsqrt(
        vec.x * vec.x
        + vec.y * vec.y
        + vec.z * vec.z
    );
}

inline float fn_vec4f_len(fn_vec4f vec) {
    return fn__fsqrt(
        vec.x * vec.x
        + vec.y * vec.y
        + vec.z * vec.z
        + vec.w * vec.w
    );
}

#endif  // FUNDAMENT_MATH_H
