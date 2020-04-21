struct input {
	float3 vertex: POSITION;
};

float4 main(input inp): SV_POSITION {
	float4 vs_out = float4(inp.vertex.xyz, 1.0f);
	return vs_out;
}