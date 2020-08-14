#ifndef FUNDAMENT_MATH_H
#define FUNDAMENT_MATH_H

//==============================================================================
// This module provides primitives commonly used in 3D math.
//==============================================================================

#include <stdint.h>

//
// Represents a 3D float vector.
//
struct fn_vec3f {
    union {
        struct {
            float x;
            float y;
            float z;
        };

        float e[4];
    };
};

float fn_vec3f_len(struct fn_vec3f vec);
struct fn_vec3f fn_vec3f_norm(struct fn_vec3f vec);
struct fn_vec3f fn_vec3f_add(struct fn_vec3f lhs, struct fn_vec3f rhs);
struct fn_vec3f fn_vec3f_sub(struct fn_vec3f lhs, struct fn_vec3f rhs);
float fn_vec3f_dot(struct fn_vec3f lhs, struct fn_vec3f rhs);

struct fn_vec3f fn_vec3f_cross(struct fn_vec3f lhs, struct fn_vec3f rhs);

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

float fn_vec4f_len(struct fn_vec4f vec);
struct fn_vec4f fn_vec4f_norm(struct fn_vec4f vec);
struct fn_vec4f fn_vec4f_add(struct fn_vec4f lhs, struct fn_vec4f rhs);
struct fn_vec4f fn_vec4f_sub(struct fn_vec4f lhs, struct fn_vec4f rhs);
float fn_vec4f_dot(struct fn_vec4f lhs, struct fn_vec4f rhs);

//
// Represents a 4x4 float matrix.
//
struct fn_float4x4 {
    union {
        struct {
            float e11, e21, e31, e41;
            float e12, e22, e32, e42;
            float e13, e23, e33, e43;
            float e14, e24, e34, e44;
        };

        struct fn_vec4f vec[4];
        float e[16];
    };
};

struct fn_float4x4 fn_float4x4_identity(void);
struct fn_float4x4 fn_float4x4_transpose(struct fn_float4x4 mat);

//==============================================================================
// The following section defines a reference implementation of the math
// functions using pure C.
//==============================================================================

static inline float fn__fsqrt(float val) {
    float x = val * 0.5f;
    float y = val;
    long i = *(long*) &y;
    i = 0x5f3759df - (i >> 1);
    y = *(float*) &i;

    const float threehalfs = 1.5f;

    y = y * (threehalfs - (x * y * y));
    y = y * (threehalfs - (x * y * y));
    return y;
}

inline float fn_vec3f_len(struct fn_vec3f vec) {
    return 1.f / fn__fsqrt(
        vec.x * vec.x
        + vec.y * vec.y
        + vec.z * vec.z
    );
}

inline struct fn_vec3f fn_vec3f_norm(struct fn_vec3f vec) {
    const float len = fn_vec3f_len(vec);

    for(uint8_t it = 0; it < 3; ++it)
        vec.e[it] /= len;

    return vec;
}

inline struct fn_vec3f fn_vec3f_add(struct fn_vec3f lhs, struct fn_vec3f rhs) {
    return (struct fn_vec3f) {
        lhs.x + rhs.x,
        lhs.y + rhs.y,
        lhs.z + rhs.z
    };
}

inline struct fn_vec3f fn_vec3f_sub(struct fn_vec3f lhs, struct fn_vec3f rhs) {
    return (struct fn_vec3f) {
        lhs.x - rhs.x,
        lhs.y - rhs.y,
        lhs.z - rhs.z
    };
}

inline float fn_vec3f_dot(struct fn_vec3f lhs, struct fn_vec3f rhs) {
    return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}

inline struct fn_vec3f fn_vec3f_cross(struct fn_vec3f lhs, struct fn_vec3f rhs) {
    return (struct fn_vec3f) {
        lhs.y * rhs.z - lhs.z * rhs.y,
        lhs.z * rhs.x - lhs.x * rhs.z,
        lhs.x * rhs.y - lhs.y * rhs.x
    };
}

inline float fn_vec4f_len(struct fn_vec4f vec) {
    return 1.f / fn__fsqrt(
        vec.x * vec.x
        + vec.y * vec.y
        + vec.z * vec.z
        + vec.w * vec.w
    );
}

inline struct fn_vec4f fn_vec4f_norm(struct fn_vec4f vec) {
    const float len = fn_vec4f_len(vec);

    for(uint8_t it = 0; it < 4; ++it)
        vec.e[it] /= len;

    return vec;
}

inline struct fn_vec4f fn_vec4f_add(struct fn_vec4f lhs, struct fn_vec4f rhs) {
    return (struct fn_vec4f) {
        lhs.x + rhs.x,
        lhs.y + rhs.y,
        lhs.z + rhs.z,
        lhs.w + rhs.w
    };
}

inline struct fn_vec4f fn_vec4f_sub(struct fn_vec4f lhs, struct fn_vec4f rhs) {
    return (struct fn_vec4f) {
        lhs.x - rhs.x,
        lhs.y - rhs.y,
        lhs.z - rhs.z,
        rhs.w - rhs.w
    };
}

inline float fn_vec4f_dot(struct fn_vec4f lhs, struct fn_vec4f rhs) {
    return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z + lhs.w * rhs.w;
}

inline struct fn_float4x4 fn_float4x4_identity() {
    struct fn_float4x4 identity = {0, };

    for(uint8_t it = 0; it < 4; ++it)
        identity.vec[it].e[it] = 1;

    return identity;
}

inline struct fn_float4x4 fn_float4x4_transpose(struct fn_float4x4 mat) {
    for(uint8_t it = 0; it < 4; ++it)
        for(uint8_t jt = 0; jt < 4; ++jt) {
            const float temp = mat.vec[it].e[jt];
            mat.vec[it].e[jt] = mat.vec[jt].e[it];
            mat.vec[jt].e[it] = temp;
        }

    return mat;
}

#endif  // FUNDAMENT_MATH_H
