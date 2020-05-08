#include <metal_stdlib>

typedef struct {
	float4 position;
} pipeline_input;

typedef struct {
	float4 position [[position]];
} pipeline_output;

vertex pipeline_output simple_vmain(
	device pipeline_input* verts [[buffer(0)]],
	uint vid [[vertex_id]]) {
	pipeline_output out;
	out.position = verts[vid].position;
	return out;
}

fragment float4 simple_pmain(
	pipeline_output in [[stage_in]]) {
	return float4(1.0, 0.0, 0.0, 1.0);	
}
