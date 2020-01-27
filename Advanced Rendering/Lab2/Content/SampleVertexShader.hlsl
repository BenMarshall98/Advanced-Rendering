// A constant buffer that stores the three basic column-major matrices for composing geometry.
cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
	matrix model;
	matrix view;
	matrix projection;
};

// Per-vertex data used as input to the vertex shader.
struct VertexShaderInput
{
	float3 pos : POSITION;
	float3 color : COLOR0;
};

// Per-pixel color data passed through the pixel shader.
struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float2 canvasCoord : TEXCOORD0;
};

// Simple shader to do vertex processing on the GPU.
PixelShaderInput main(VertexShaderInput input)
{
	PixelShaderInput output;
    float4 pos = float4(sign(input.pos.xz), 0.0f, 1.0f);

	output.pos = pos;
    
    float aspectRatio = projection._m11 / projection._m00;

	// Pass the color through without modification.
    output.canvasCoord = sign(input.pos.xz) * float2(aspectRatio, 1.0);

	return output;
}
