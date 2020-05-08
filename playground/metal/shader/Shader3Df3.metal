#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

typedef struct {
	float3 position;
} pipeline_input;

typedef struct {
	float4 position [[position]];
} pipeline_output;

typedef struct {
	float4x4 projection;
} MyUniform;

vertex pipeline_output s3df3_vmain(
	device pipeline_input* verts [[buffer(0)]],
	constant MyUniform& u [[buffer(1)]],
	uint vid [[vertex_id]]) {
	pipeline_output out;
	out.position = u.projection * float4(verts[vid].position, 1.0);
	return out;
}

fragment float4 s3df3_pmain(
	pipeline_output in [[stage_in]]) {
	return float4(1.0, 0.0, 0.0, 1.0);	
}
