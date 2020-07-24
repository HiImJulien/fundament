#ifndef FUNDAMENT_MATH_H
#define FUNDAMENT_MATH_H

#include <xmmintrin.h>
#include <smmintrin.h>

//
// Defines a 4D vector.
//
struct fn_vector {
	union {
		__m128	xyzw;

		struct {
			float x;
			float y;
			float z;
			float w;
		};
	};
};

//
// Calculates the dot product of vectors lhs and rhs.
//
inline float fn_vector_dot(struct fn_vector lhs, struct fn_vector rhs) {
	__m128 mul_res, shuf_reg, sums_reg;
	mul_res = _mm_mul_ps(lhs.xyzw, rhs.xyzw);

	shuf_reg = _mm_movehdup_ps(mul_res);
	sums_reg = _mm_add_ps(mul_res, shuf_reg);
	shuf_reg = _mm_movehl_ps(shuf_reg, sums_reg);
	sums_reg = _mm_add_ss(sums_reg, shuf_reg);

	return _mm_cvtss_f32(sums_reg);
}

//
// Calculates the cross product of vectors lhs and rhs. 
//
inline struct fn_vector fn_vector_cross(
	struct fn_vector lhs, 
	struct fn_vector rhs
) {
	struct fn_vector res;
	res.xyzw =
		_mm_sub_ps(
			_mm_mul_ps(
				_mm_shuffle_ps(
					lhs.xyzw, 
					lhs.xyzw, 
					_MM_SHUFFLE(3, 0, 2, 1)), 
				_mm_shuffle_ps(
					rhs.xyzw, 
					rhs.xyzw, 
					_MM_SHUFFLE(3, 1, 0, 2))
			), 
			_mm_mul_ps(
				_mm_shuffle_ps(
					lhs.xyzw, 
					lhs.xyzw, 
					_MM_SHUFFLE(3, 1, 0, 2)), 
				_mm_shuffle_ps(
					rhs.xyzw, 
					rhs.xyzw, 
					_MM_SHUFFLE(3, 0, 2, 1))
				)
		);

	return res;
}

#endif 	// FUNDAMENT_MATH_H