float4 main(float3 vertex: POSITION): SV_POSITION {
	float4 vs_out = float4(vertex.xyz, 1.0f);
	return vs_out;
}