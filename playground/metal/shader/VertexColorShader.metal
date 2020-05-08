#include <metal_stdlib>

typedef struct {
	float4 position;
	float4 color;
} pipeline_input;

typedef struct {
	float4 position [[position]];
	float4 color;
} pipeline_output;

vertex pipeline_output vertex_color_vmain(
	device pipeline_input* verts [[buffer(0)]],
	uint vid [[vertex_id]]) {
	pipeline_output out;
	out.position = verts[vid].position;
	out.color = verts[vid].color;
	return out;
}

fragment float4 vertex_color_pmain(
	pipeline_output in [[stage_in]]) {
	return in.color;
}
