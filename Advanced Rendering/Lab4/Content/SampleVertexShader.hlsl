// Per-vertex data used as input to the vertex shader.
struct VertexShaderInput
{
	float3 pos : POSITION;
	float3 color : COLOR0;
};

// Per-pixel color data passed through the pixel shader.
struct GeometryShaderInput
{
	float4 pos : SV_POSITION;
	float3 color : COLOR0;
};

// Simple shader to do vertex processing on the GPU.
GeometryShaderInput main(VertexShaderInput input)
{
	GeometryShaderInput output;
	float4 pos = float4(input.pos, 1.0f);

	// Transform the vertex position into projected space.
	output.pos = pos;

	// Pass the color through without modification.
	output.color = input.color;

	return output;
}
