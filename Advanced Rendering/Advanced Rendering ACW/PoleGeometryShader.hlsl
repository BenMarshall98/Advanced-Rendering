// A constant buffer that stores the three basic column-major matrices for composing geometry.
cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
	matrix model;
	matrix inverseModel;
	matrix view;
	matrix projection;
	float4 eyePosition;
};

// Per-pixel color data passed through the pixel shader.
struct GeometryShaderInput
{
	float4 pos : SV_Position;
	float2 texCoord : TEXCOORD0;
	float3 normal : NORMAL0;
};

struct PixelShaderInput
{
	float4 pos : SV_Position;
	float3 fragPos : POSITION0;
	float2 texCoord : TEXCOORD0;
	float3 normal : NORMAL0;
};

float4x4 inverse(float4x4 m);

[maxvertexcount(60)]
void main(triangle GeometryShaderInput input[3], inout TriangleStream<PixelShaderInput> OutputStream)
{
	const int numSculptures = 12;

	const float3 positions[numSculptures] =
	{
		float3(20, 5, 0),
		float3(20, 5, 20),
		float3(20, 5, 40),
		float3(20, 5, 60),
		float3(20, 5, 80),
		float3(20, 5, 100),
		float3(-20, 5, 0),
		float3(-20, 5, 20),
		float3(-20, 5, 40),
		float3(-20, 5, 60),
		float3(-20, 5, 80),
		float3(-20, 5, 100),
	};

	PixelShaderInput output = (PixelShaderInput)0;

	for (int i = 0; i < numSculptures; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			float4 vPos = input[j].pos;
			vPos.xyz += positions[i];

			output.pos = vPos;
			output.fragPos = output.pos.xyz;
			output.pos = mul(output.pos, view);
			output.pos = mul(output.pos, projection);
			output.texCoord = input[j].texCoord;
			output.normal = input[j].normal;

			OutputStream.Append(output);
		}

		OutputStream.RestartStrip();
	}

	//output.pos = vPos + (float4(quadSize * g_positions[1].x * right + g_positions[1].y * float3(0.0f, 1.0f, 0.0f), 0.0));
}