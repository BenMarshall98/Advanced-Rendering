// A constant buffer that stores the three basic column-major matrices for composing geometry.
cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
	matrix model;
	matrix view;
	matrix projection;
};

// Per-pixel color data passed through the pixel shader.
struct VertexShaderOutput
{
	float4 pos : SV_POSITION;
};

// Simple shader to do vertex processing on the GPU.
VertexShaderOutput main()
{
	VertexShaderOutput output;
    
    output.pos = float4(0.0f, 0.0f, 0.0f, 1.0f);

	return output;
}
