// Per-vertex data used as input to the vertex shader.
struct VertexShaderInput
{
	float3 pos : POSITION;
	float3 color : COLOR0;
};

// Per-pixel color data passed through the pixel shader.
struct GeometryShaderInput
{
	float4 pos : SV_Position;
	float id : POSITION0;
};

GeometryShaderInput main(VertexShaderInput input, uint ID : SV_VertexID)
{
	GeometryShaderInput output;

	output.pos = float4(input.pos, 1.0f);
	output.id = ID;

	return output;
}